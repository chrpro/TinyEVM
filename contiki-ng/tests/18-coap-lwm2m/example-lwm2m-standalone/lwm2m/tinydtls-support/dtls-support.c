/*
 * Copyright (c) 2017, SICS, Swedish ICT AB.
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
 *         Posix support for TinyDTLS
 * \author
 *         Niclas Finne <nfi@sics.se>
 *         Joakim Eriksson <joakime@sics.se>
 */

#include "tinydtls.h"
#include "coap-timer.h"

/* Log configuration */
#define LOG_MODULE "dtls-support"
#define LOG_LEVEL  LOG_LEVEL_DTLS
#include "dtls-log.h"
/*---------------------------------------------------------------------------*/
#include <pthread.h>
static pthread_mutex_t cipher_context_mutex = PTHREAD_MUTEX_INITIALIZER;
static dtls_cipher_context_t cipher_context;
#define LOCK(P) pthread_mutex_lock(P)
#define UNLOCK(P) pthread_mutex_unlock(P)
/*---------------------------------------------------------------------------*/
dtls_cipher_context_t *
dtls_cipher_context_acquire(void)
{
  LOCK(&cipher_context_mutex);
  return &cipher_context;
}
/*---------------------------------------------------------------------------*/
void
dtls_cipher_context_release(dtls_cipher_context_t *c)
{
  /* just one single context for now */
  UNLOCK(&cipher_context_mutex);
}
/*---------------------------------------------------------------------------*/
dtls_context_t *
dtls_context_acquire(void)
{
  return (dtls_context_t *)malloc(sizeof(dtls_context_t));
}
/*---------------------------------------------------------------------------*/
void
dtls_context_release(dtls_context_t *context)
{
  free(context);
}
/*---------------------------------------------------------------------------*/
/* --------- time support ----------- */
void
dtls_ticks(dtls_tick_t *t)
{
  *t = coap_timer_uptime();
}
/*---------------------------------------------------------------------------*/
int
dtls_fill_random(uint8_t *buf, size_t len)
{
  FILE *urandom = fopen("/dev/urandom", "r");

  if(!urandom) {
    dtls_emerg("cannot initialize PRNG\n");
    return 0;
  }

  if(fread(buf, 1, len, urandom) != len) {
    dtls_emerg("cannot initialize PRNG\n");
    fclose(urandom);
    return 0;
  }

  fclose(urandom);

  return 1;
}
/*---------------------------------------------------------------------------*/
/* message retransmission */
/*---------------------------------------------------------------------------*/
static void
dtls_retransmit_callback(coap_timer_t *timer)
{
  dtls_context_t *ctx;
  uint64_t now;
  uint64_t next;

  ctx = coap_timer_get_user_data(timer);
  now = coap_timer_uptime();
  /* Just one retransmission per timer scheduling */
  dtls_check_retransmit(ctx, &next, 0);

  /* need to set timer to some value even if no nextpdu is available */
  if(next != 0) {
    coap_timer_set(timer, next <= now ? 1 : next - now);
  }
}
/*---------------------------------------------------------------------------*/
void
dtls_set_retransmit_timer(dtls_context_t *ctx, unsigned int timeout)
{
  coap_timer_set_callback(&ctx->support.retransmit_timer,
                          dtls_retransmit_callback);
  coap_timer_set_user_data(&ctx->support.retransmit_timer, ctx);
  coap_timer_set(&ctx->support.retransmit_timer, timeout);
}
/*---------------------------------------------------------------------------*/
/* Implementation of session functions */
void
dtls_session_init(session_t *session)
{
  memset(session, 0, sizeof(session_t));
}
/*---------------------------------------------------------------------------*/
int
dtls_session_equals(const session_t *a, const session_t *b)
{
  coap_endpoint_t *e1 = (coap_endpoint_t *)a;
  coap_endpoint_t *e2 = (coap_endpoint_t *)b;

  if(LOG_DBG_ENABLED) {
    LOG_DBG(" **** EP:");
    LOG_DBG_COAP_EP(e1);
    LOG_DBG_(" =?= ");
    LOG_DBG_COAP_EP(e2);
    LOG_DBG_(" => %d\n", coap_endpoint_cmp(e1, e2));
  }

  return coap_endpoint_cmp(e1, e2);
}
/*---------------------------------------------------------------------------*/
void *
dtls_session_get_address(const session_t *a)
{
  /* improve this to only contain the addressing info */
  return (void *)a;
}
/*---------------------------------------------------------------------------*/
int
dtls_session_get_address_size(const session_t *a)
{
  /* improve this to only contain the addressing info */
  return sizeof(session_t);
}
/*---------------------------------------------------------------------------*/
void
dtls_session_print(const session_t *a)
{
  coap_endpoint_print((const coap_endpoint_t *)a);
}
/*---------------------------------------------------------------------------*/
void
dtls_session_log(const session_t *a)
{
  coap_endpoint_log((const coap_endpoint_t *)a);
}
/*---------------------------------------------------------------------------*/
/* The init */
void
dtls_support_init(void)
{
}
/*---------------------------------------------------------------------------*/
