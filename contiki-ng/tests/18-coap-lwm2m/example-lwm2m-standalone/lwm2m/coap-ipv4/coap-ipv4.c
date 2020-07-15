/*
 * Copyright (c) 2016, SICS, Swedish ICT AB.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/**
 * \file
 *         A native IPv4 transport for CoAP
 * \author
 *         Niclas Finne <nfi@sics.se>
 *         Joakim Eriksson <joakime@sics.se>
 */

#include "coap.h"
#include "coap-endpoint.h"
#include "coap-engine.h"
#include "coap-keystore.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <err.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* Log configuration */
#include "coap-log.h"
#define LOG_MODULE "coap-ipv4"
#define LOG_LEVEL  LOG_LEVEL_COAP

#ifdef WITH_DTLS
#include "tinydtls.h"
#include "dtls.h"
#include "dtls-log.h"
#endif /* WITH_DTLS */

#define BUFSIZE 1280

typedef union {
  uint32_t u32[(BUFSIZE + 3) / 4];
  uint8_t u8[BUFSIZE];
} coap_buf_t;

static int coap_ipv4_fd = -1;

static coap_endpoint_t last_source;
static coap_buf_t coap_aligned_buf;
static uint16_t coap_buf_len;

#ifdef WITH_DTLS
static int dtls_ipv4_fd = -1;
static dtls_handler_t cb;
static dtls_context_t *dtls_context = NULL;

static const coap_keystore_t *dtls_keystore = NULL;
#endif /* WITH_DTLS */

/*---------------------------------------------------------------------------*/
static const coap_endpoint_t *
coap_src_endpoint(void)
{
  return &last_source;
}
/*---------------------------------------------------------------------------*/
int
coap_endpoint_is_secure(const coap_endpoint_t *ep)
{
  return ep->secure;
}
/*---------------------------------------------------------------------------*/
int
coap_endpoint_is_connected(const coap_endpoint_t *ep)
{
  if(ep->secure) {
#ifdef WITH_DTLS
    dtls_peer_t *peer;
    peer = dtls_get_peer(dtls_context, ep);
    if(peer != NULL) {
      /* only if handshake is done! */
      LOG_DBG("DTLS peer state for ");
      LOG_DBG_COAP_EP(ep);
      LOG_DBG_(" is %d %d\n", peer->state, dtls_peer_is_connected(peer));
      return dtls_peer_is_connected(peer);
    } else {
      LOG_DBG("DTLS did not find peer ");
      LOG_DBG_COAP_EP(ep);
      LOG_DBG_("\n");
    }
#endif /* WITH_DTLS */
    return 0;
  }
  /* Assume that the UDP socket is already up... */
  return 1;
}
/*---------------------------------------------------------------------------*/
int
coap_endpoint_connect(coap_endpoint_t *ep)
{
  if(ep->secure == 0) {
    return 1;
  }

#ifdef WITH_DTLS
  LOG_DBG("DTLS connect to ");
  LOG_DBG_COAP_EP(ep);
  LOG_DBG_(" len:%d\n", ep->size);

  /* setup all address info here... should be done to connect */
  if(dtls_context) {
    dtls_connect(dtls_context, ep);
    return 1;
  }
#endif /* WITH_DTLS */

  return 0;
}
/*---------------------------------------------------------------------------*/
void
coap_endpoint_disconnect(coap_endpoint_t *ep)
{
#ifdef WITH_DTLS
  if(ep && ep->secure && dtls_context) {
    dtls_close(dtls_context, ep);
  }
#endif /* WITH_DTLS */
}
/*---------------------------------------------------------------------------*/
void
coap_endpoint_copy(coap_endpoint_t *destination, const coap_endpoint_t *from)
{
  memcpy(destination, from, sizeof(coap_endpoint_t));
}
/*---------------------------------------------------------------------------*/
int
coap_endpoint_cmp(const coap_endpoint_t *e1, const coap_endpoint_t *e2)
{
  /* need to compare only relevant parts of sockaddr */
  switch(e1->addr.sin_family) {
  case AF_INET:
    return e1->addr.sin_port == e2->addr.sin_port &&
      e1->secure == e2->secure &&
      memcmp(&e1->addr.sin_addr, &e2->addr.sin_addr,
             sizeof(struct in_addr)) == 0;
  default:
    return 0;
  }
}
/*---------------------------------------------------------------------------*/
void
coap_endpoint_log(const coap_endpoint_t *ep)
{
  const char *address;
  address = inet_ntoa(ep->addr.sin_addr);
  if(address != NULL) {
    LOG_OUTPUT("coap%s://%s:%u", ep->secure ? "s" : "",
               address, ntohs(ep->addr.sin_port));
  } else {
    LOG_OUTPUT("<#N/A>");
  }
}
/*---------------------------------------------------------------------------*/
void
coap_endpoint_print(const coap_endpoint_t *ep)
{
  const char *address;
  address = inet_ntoa(ep->addr.sin_addr);
  if(address != NULL) {
    printf("coap%s://%s:%u", ep->secure ? "s" : "",
           address, ntohs(ep->addr.sin_port));
  } else {
    printf("<#N/A>");
  }
}
/*---------------------------------------------------------------------------*/
int
coap_endpoint_snprint(char *buf, size_t size, const coap_endpoint_t *ep)
{
  const char *address;
  int n;
  if(size == 0) {
    return 0;
  }
  address = inet_ntoa(ep->addr.sin_addr);
  if(address != NULL) {
    n = snprintf(buf, size - 1,
                 "coap%s://%s:%u", ep->secure ? "s" : "",
                 address, ntohs(ep->addr.sin_port));
  } else {
    n = snprintf(buf, size - 1, "<#N/A>");
  }
  if(n >= size - 1) {
    buf[size - 1] = '\0';
  }
  return n;
}
/*---------------------------------------------------------------------------*/
int
coap_endpoint_parse(const char *text, size_t size, coap_endpoint_t *ep)
{
  /* text = format coap://host:port/... we assume */
  /* will not work for know - on the TODO */
  /* set server and port */
  char host[32];
  uint16_t port;
  int hlen = 0;
  int secure;
  int offset = 0;
  int i;
  LOG_DBG("parsing endpoint %.*s => ", (int)size, text);
  if(strncmp("coap://", text, 7) == 0) {
    secure = 0;
    offset = 7;
  } else if(strncmp("coaps://", text, 8) == 0) {
    secure = 1;
    offset = 8;
    LOG_DBG_("secure ");
  } else {
    secure = 0;
  }

  for(i = offset; i < size && text[i] != ':' && text[i] != '/' &&
        hlen < sizeof(host) - 1; i++) {
    host[hlen++] = text[i];
  }
  host[hlen] = 0;

  port = secure == 0 ? COAP_DEFAULT_PORT : COAP_DEFAULT_SECURE_PORT;
  if(text[i] == ':') {
    /* Parse IPv4 endpoint port */
    port = atoi(&text[i + 1]);
  }

  LOG_DBG_("endpoint at %s:%u\n", host, port);

  ep->addr.sin_family = AF_INET;
  ep->addr.sin_port = htons(port);
  ep->size = sizeof(ep->addr);
  ep->secure = secure;
  if(inet_aton(host, &ep->addr.sin_addr) == 0) {
    /* Failed to parse the address */
    LOG_WARN("Failed to parse endpoint host '%s'\n", host);
    return 0;
  }
  return 1;
}
/*---------------------------------------------------------------------------*/
uint8_t *
coap_databuf(void)
{
  return coap_aligned_buf.u8;
}
/*---------------------------------------------------------------------------*/
static int
read_packet_to_coapbuf(int fd, int is_secure)
{
  int len;

  memset(&last_source, 0, sizeof(last_source));
  last_source.size = sizeof(last_source.addr);
  len = recvfrom(fd, coap_databuf(), BUFSIZE, 0,
                 (struct sockaddr *)&last_source.addr, &last_source.size);
  if(len == -1) {
    if(errno == EAGAIN) {
      return 0;
    }
    err(1, "CoAP-IPv4: recv");
    return 0;
  }

  last_source.secure = is_secure;

  LOG_INFO("RECV from ");
  LOG_INFO_COAP_EP(&last_source);
  LOG_INFO_(" %u bytes\n", len);
  coap_buf_len = len;

#if 0
 if((rand() & 0xffff) < 0x1000) {
    printf("*********---- PACKET LOSS ----********\n");
    return 0;
  }
#endif

  return 1;
}
/*---------------------------------------------------------------------------*/
static int
coap_ipv4_set_fd(fd_set *rset, fd_set *wset)
{
  if(coap_ipv4_fd >= 0) {
    FD_SET(coap_ipv4_fd, rset);
    return 1;
  }
  return 0;
}
/*---------------------------------------------------------------------------*/
static void
coap_ipv4_handle_fd(fd_set *rset, fd_set *wset)
{

  if(coap_ipv4_fd >= 0 && FD_ISSET(coap_ipv4_fd, rset)) {
    if(read_packet_to_coapbuf(coap_ipv4_fd, 0)) {
#if LOG_DBG_ENABLED
      int i;
      uint8_t *data;
      data = coap_databuf();
      LOG_DBG("Received: ");
      for(i = 0; i < coap_buf_len; i++) {
        LOG_DBG_("%02x", data[i]);
      }
      LOG_DBG_("\n");
#endif /* LOG_DBG_ENABLED */
      coap_receive(coap_src_endpoint(), coap_databuf(), coap_buf_len);
    }
  }
}
/*---------------------------------------------------------------------------*/
static const struct select_callback udp_callback = {
  coap_ipv4_set_fd, coap_ipv4_handle_fd
};
/*---------------------------------------------------------------------------*/
#ifdef WITH_DTLS
static int
dtls_ipv4_set_fd(fd_set *rset, fd_set *wset)
{
  if(dtls_ipv4_fd >= 0 && dtls_context) {
    FD_SET(dtls_ipv4_fd, rset);
    return 1;
  }
  return 0;
}
#endif /* WITH_DTLS */
/*---------------------------------------------------------------------------*/
#ifdef WITH_DTLS
static void
dtls_ipv4_handle_fd(fd_set *rset, fd_set *wset)
{
  if(dtls_ipv4_fd >= 0 && FD_ISSET(dtls_ipv4_fd, rset)) {
    if(read_packet_to_coapbuf(dtls_ipv4_fd, 1) && dtls_context) {
      /* DTLS receive */
      dtls_handle_message(dtls_context, &last_source,
                          coap_databuf(), coap_buf_len);
    }
  }
}
#endif /* WITH_DTLS */
/*---------------------------------------------------------------------------*/
#ifdef WITH_DTLS
static const struct select_callback dtls_callback = {
  dtls_ipv4_set_fd, dtls_ipv4_handle_fd
};
#endif /* WITH_DTLS */
/*---------------------------------------------------------------------------*/
void
coap_transport_init(void)
{
  static struct sockaddr_in server;

  coap_ipv4_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if(coap_ipv4_fd == -1) {
    fprintf(stderr, "Could not create CoAP UDP socket\n");
    exit(1);
    return;
  }

  memset((void *)&server, 0, sizeof(server));

  server.sin_family = AF_INET;
  server.sin_port = htons(COAP_SERVER_PORT);
  server.sin_addr.s_addr = htonl(INADDR_ANY);

  if(bind(coap_ipv4_fd, (struct sockaddr *)&server, sizeof(server)) == -1) {
    fprintf(stderr, "Could not bind CoAP UDP port to %u\n", COAP_SERVER_PORT);
    exit(1);
  }

  LOG_INFO("CoAP server listening on port %u\n", COAP_SERVER_PORT);
  select_set_callback(coap_ipv4_fd, &udp_callback);

#ifdef WITH_DTLS
  dtls_init();

  dtls_ipv4_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if(dtls_ipv4_fd == -1) {
    fprintf(stderr, "Could not create CoAP DTLS UDP socket\n");
    exit(1);
    return;
  }

  memset((void *)&server, 0, sizeof(server));
  server.sin_family = AF_INET;
  server.sin_port = htons(COAP_DEFAULT_SECURE_PORT);
  server.sin_addr.s_addr = htonl(INADDR_ANY);

  if(bind(dtls_ipv4_fd, (struct sockaddr *)&server, sizeof(server)) == -1) {
    fprintf(stderr, "Could not bind CoAP DTLS UDP port to %u\n",
           COAP_DEFAULT_SECURE_PORT);
    exit(1);
  }

  LOG_INFO("CoAP DTLS server listening on port %u\n", COAP_DEFAULT_SECURE_PORT);
  select_set_callback(dtls_ipv4_fd, &dtls_callback);

  /* create new contet with app-data */
  dtls_context = dtls_new_context(&dtls_ipv4_fd);
  if(!dtls_context) {
    fprintf(stderr, "DTLS: cannot create context\n");
    exit(-1);
  }

  dtls_set_handler(dtls_context, &cb);
#endif /* WITH_DTLS */
}
/*---------------------------------------------------------------------------*/
int
coap_sendto(const coap_endpoint_t *ep, const uint8_t *data, uint16_t len)
{
  int ret;

  if(!coap_endpoint_is_connected(ep)) {
    LOG_WARN("endpoint ");
    LOG_WARN_COAP_EP(ep);
    LOG_WARN_(" not connected - dropping packet\n");
    return -1;
  }

#ifdef WITH_DTLS
  if(coap_endpoint_is_secure(ep)) {
    if(dtls_context) {
      ret = dtls_write(dtls_context, (session_t *)ep, (uint8_t *)data, len);
      LOG_DBG("SENT DTLS to ");
      LOG_DBG_COAP_EP(ep);
      if(ret < 0) {
        LOG_DBG_(" - error %d\n", ret);
      } else {
        LOG_DBG_(" %d/%u bytes\n", ret, len);
      }
      return ret;
    }
    LOG_WARN("no DTLS context\n");
    return -1;
  }
#endif /* WITH_DTLS */

  if(coap_ipv4_fd >= 0) {
    ret = sendto(coap_ipv4_fd, data, len, 0, (struct sockaddr *)&ep->addr,
                 ep->size);
    LOG_INFO("SENT to ");
    LOG_INFO_COAP_EP(ep);
    if(ret < 0) {
      LOG_INFO_(" - error %d: %s\n", ret, strerror(errno));
    } else {
      LOG_INFO_(" %d/%u bytes\n", ret, len);

      if(LOG_DBG_ENABLED) {
        int i;
        LOG_DBG("Sent: ");
        for(i = 0; i < len; i++) {
          LOG_DBG_("%02x", data[i]);
        }
        LOG_DBG_("\n");
      }
    }
    return ret;
  }

  LOG_DBG("failed to send - no socket\n");
  return -1;
}
/*---------------------------------------------------------------------------*/
/* DTLS */
#ifdef WITH_DTLS

/* This is input coming from the DTLS code - e.g. de-crypted input from
   the other side - peer */
static int
input_from_peer(struct dtls_context_t *ctx,
                session_t *session, uint8_t *data, size_t len)
{
#if LOG_DBG_ENABLED
  size_t i;

  LOG_DBG("DTLS received data: ");
  for(i = 0; i < len; i++) {
    LOG_DBG_("%c", data[i]);
  }
  LOG_DBG_("\n");
  LOG_DBG("Hex: ");
  for(i = 0; i < len; i++) {
    LOG_DBG_("%02x", data[i]);
  }
  LOG_DBG_("\n");
#endif /* LOG_DBG_ENABLED */

  /* Send this into coap-input */
  memmove(coap_databuf(), data, len);
  coap_buf_len = len;

  /* Ensure that the endpoint is tagged as secure */
  session->secure = 1;

  coap_receive(session, coap_databuf(), coap_buf_len);

  return 0;
}
/*---------------------------------------------------------------------------*/
/* This is output from the DTLS code to be sent to peer (encrypted) */
static int
output_to_peer(struct dtls_context_t *ctx,
               session_t *session, uint8_t *data, size_t len)
{
  int fd = *(int *)dtls_get_app_data(ctx);
  LOG_DBG("DTLS output_to_peer len:%d %d (s-size: %d)\n", (int)len, fd,
          session->size);
  return sendto(fd, data, len, MSG_DONTWAIT,
		(struct sockaddr *)&session->addr, session->size);
}
/*---------------------------------------------------------------------------*/
/* This defines the key-store set API since we hookup DTLS here */
void
coap_set_keystore(const coap_keystore_t *keystore)
{
  dtls_keystore = keystore;
}
/*---------------------------------------------------------------------------*/
/* This function is the "key store" for tinyDTLS. It is called to
 * retrieve a key for the given identity within this particular
 * session. */
static int
get_psk_info(struct dtls_context_t *ctx,
             const session_t *session,
             dtls_credentials_type_t type,
             const unsigned char *id, size_t id_len,
             unsigned char *result, size_t result_length)
{
  const coap_keystore_t *keystore;
  coap_keystore_psk_entry_t ks;

  keystore = dtls_keystore;
  if(keystore == NULL) {
    LOG_DBG("--- No key store available ---\n");
    return 0;
  }

  memset(&ks, 0, sizeof(ks));
  LOG_DBG("---===>>> Getting the Key or ID <<<===---\n");
  switch(type) {
  case DTLS_PSK_IDENTITY:
    if(id && id_len) {
      ks.identity_hint = id;
      ks.identity_hint_len = id_len;
      LOG_DBG("got psk_identity_hint: '");
      LOG_DBG_COAP_STRING((const char *)id, id_len);
      LOG_DBG_("'\n");
    }

    if(keystore->coap_get_psk_info) {
      /* we know that session is a coap endpoint */
      keystore->coap_get_psk_info((coap_endpoint_t *)session, &ks);
    }
    if(ks.identity == NULL || ks.identity_len == 0) {
      LOG_DBG("no psk_identity found\n");
      return 0;
    }

    if(result_length < ks.identity_len) {
      LOG_DBG("cannot return psk_identity -- buffer too small\n");
      return dtls_alert_fatal_create(DTLS_ALERT_INTERNAL_ERROR);
    }
    memcpy(result, ks.identity, ks.identity_len);
    LOG_DBG("psk_identity with %u bytes found\n", ks.identity_len);
    return ks.identity_len;

  case DTLS_PSK_KEY:
    if(keystore->coap_get_psk_info) {
      ks.identity = id;
      ks.identity_len = id_len;
      /* we know that session is a coap endpoint */
      keystore->coap_get_psk_info((coap_endpoint_t *)session, &ks);
    }
    if(ks.key == NULL || ks.key_len == 0) {
      LOG_DBG("PSK for unknown id requested, exiting\n");
      return dtls_alert_fatal_create(DTLS_ALERT_ILLEGAL_PARAMETER);
    }

    if(result_length < ks.key_len) {
      LOG_DBG("cannot return psk -- buffer too small\n");
      return dtls_alert_fatal_create(DTLS_ALERT_INTERNAL_ERROR);
    }
    memcpy(result, ks.key, ks.key_len);
    LOG_DBG("psk with %u bytes found\n", ks.key_len);
    return ks.key_len;

  default:
    dtls_warn("unsupported request type: %d\n", type);
  }

  return dtls_alert_fatal_create(DTLS_ALERT_INTERNAL_ERROR);
}
/*---------------------------------------------------------------------------*/
static dtls_handler_t cb = {
  .write = output_to_peer,
  .read  = input_from_peer,
  .event = NULL,
#ifdef DTLS_PSK
  .get_psk_info = get_psk_info,
#endif /* DTLS_PSK */
#ifdef DTLS_ECC
  /* .get_ecdsa_key = get_ecdsa_key, */
  /* .verify_ecdsa_key = verify_ecdsa_key */
#endif /* DTLS_ECC */
};

#endif /* WITH_DTLS */
/*---------------------------------------------------------------------------*/
