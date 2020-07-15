/* POSIX support for memb alloc / free and other functions needed to
   run the tinyDTSL applications */

#include "tinydtls.h"
#include "lib/memb.h"
#include "dtls-support.h"
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#ifdef HAVE_ASSERT_H
#include <assert.h>
#endif

#include <arpa/inet.h>
#include <stdarg.h>
#include <stdio.h>

#include <pthread.h>
static pthread_mutex_t cipher_context_mutex = PTHREAD_MUTEX_INITIALIZER;
static dtls_cipher_context_t cipher_context;
#define LOCK(P) pthread_mutex_lock(P)
#define UNLOCK(P) pthread_mutex_unlock(P)

/* Log configuration */
#define LOG_MODULE "dtls-support"
#define LOG_LEVEL  LOG_LEVEL_DTLS
#include "dtls-log.h"

#ifndef MAX
#define MAX(a,b) (((a) < (b)) ? (b) : (a))
#endif /* MAX */

void
dtls_support_log_prefix(int level, const char *level_str, const char *module)
{
  struct timeval now;
  struct tm loctime;
  int n = 0;
  char buf[28];

  if((gettimeofday(&now, 0) == 0) &&
     (localtime_r(&now.tv_sec, &loctime) != NULL) &&
     (n = strftime(buf, sizeof(buf), "%F %T", &loctime)) > 0) {
    buf[n] = '\0';
    LOG_OUTPUT("%s.%03u [%s] %-4s - ", buf, (unsigned)(now.tv_usec / 1000),
               module, level_str);
  } else {
    LOG_OUTPUT("- [%s] %s - ", module, level_str);
  }
}

dtls_cipher_context_t *
dtls_cipher_context_acquire(void)
{
  LOCK(&cipher_context_mutex);
  return &cipher_context;
}

void
dtls_cipher_context_release(dtls_cipher_context_t *c)
{
  /* just one single context for now */
  UNLOCK(&cipher_context_mutex);
}


void
memb_init(struct memb *m)
{
}


void *
memb_alloc(struct memb *m)
{
  return malloc(m->size);
}

char
memb_free(struct memb *m, void *ptr)
{
  free(ptr);
  return 1;
}


dtls_context_t *
dtls_context_acquire(void)
{
  return (dtls_context_t *)malloc(sizeof(dtls_context_t));
}

void
dtls_context_release(dtls_context_t *context)
{
  free(context);
}

/* --------- time support ----------- */

static time_t dtls_clock_offset;

void
dtls_ticks(dtls_tick_t *t)
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  *t = (tv.tv_sec - dtls_clock_offset) * (dtls_tick_t)DTLS_TICKS_PER_SECOND
    + (tv.tv_usec * (dtls_tick_t)DTLS_TICKS_PER_SECOND / 1000000);
}

int
dtls_fill_random(uint8_t *buf, size_t len)
{
  FILE *urandom = fopen("/dev/urandom", "r");

  if(!urandom) {
    dtls_emerg("cannot initialize random\n");
    return 0;
  }

  if(fread(buf, 1, len, urandom) != len) {
    dtls_emerg("cannot fill random\n");
    fclose(urandom);
    return 0;
  }

  fclose(urandom);

  return 1;
}

void
dtls_set_retransmit_timer(dtls_context_t *ctx, unsigned int timeout)
{
  /* Do nothing for now ... */
}

/* Implementation of session functions */
void
dtls_session_init(session_t *sess) {
  assert(sess);
  memset(sess, 0, sizeof(session_t));
  sess->size = sizeof(sess->addr);
}

int
dtls_session_equals(const session_t *a, const session_t *b) {
  assert(a); assert(b);

  if(a->ifindex != b->ifindex ||
     a->size != b->size ||
     a->addr.sa.sa_family != b->addr.sa.sa_family) {
    return 0;
  }

  /* need to compare only relevant parts of sockaddr_in6 */
  switch (a->addr.sa.sa_family) {
  case AF_INET:
    return
     a->addr.sin.sin_port == b->addr.sin.sin_port &&
     memcmp(&a->addr.sin.sin_addr, &b->addr.sin.sin_addr,
	    sizeof(struct in_addr)) == 0;
  case AF_INET6:
    return a->addr.sin6.sin6_port == b->addr.sin6.sin6_port &&
      memcmp(&a->addr.sin6.sin6_addr, &b->addr.sin6.sin6_addr,
             sizeof(struct in6_addr)) == 0;
  default: /* fall through and signal error */
    ;
  }
  return 0;
}

void *
dtls_session_get_address(const session_t *a)
{
  /* improve this to only contain the addressing info */
  return (void *)a;
}

int
dtls_session_get_address_size(const session_t *a)
{
  /* improve this to only contain the addressing info */
  return sizeof(session_t);
}

void
dtls_session_log(const session_t *addr)
{
  char buf[MAX(INET_ADDRSTRLEN, INET6_ADDRSTRLEN)];
  const void *addrptr = NULL;
  in_port_t port;

  switch(addr->addr.sa.sa_family) {
  case AF_INET:
    addrptr = &addr->addr.sin.sin_addr;
    port = ntohs(addr->addr.sin.sin_port);
    break;
  case AF_INET6:
    addrptr = &addr->addr.sin6.sin6_addr;
    port = ntohs(addr->addr.sin6.sin6_port);
    break;
  default:
    LOG_OUTPUT("(unknown address type)");
    return;
  }

  if(inet_ntop(addr->addr.sa.sa_family, addrptr, buf, sizeof(buf)) == 0) {
    perror("dtls_session_log");
    return;
  }


  if(addr->addr.sa.sa_family == AF_INET6) {
    LOG_OUTPUT("[%s]:%d", buf, port);
  } else {
    LOG_OUTPUT("%s:%d", buf, port);
  }
}

void
dtls_session_print(const session_t *addr)
{
  char buf[MAX(INET_ADDRSTRLEN, INET6_ADDRSTRLEN)];
  const void *addrptr = NULL;
  in_port_t port;

  switch(addr->addr.sa.sa_family) {
  case AF_INET:
    addrptr = &addr->addr.sin.sin_addr;
    port = ntohs(addr->addr.sin.sin_port);
    break;
  case AF_INET6:
    addrptr = &addr->addr.sin6.sin6_addr;
    port = ntohs(addr->addr.sin6.sin6_port);
    break;
  default:
    printf("(unknown address type)");
    return;
  }

  if(inet_ntop(addr->addr.sa.sa_family, addrptr, buf, sizeof(buf)) == 0) {
    perror("dtls_session_log");
    return;
  }


  if(addr->addr.sa.sa_family == AF_INET6) {
    printf("[%s]:%d", buf, port);
  } else {
    printf("%s:%d", buf, port);
  }
}

/* The init */
void
dtls_support_init(void)
{
#ifdef HAVE_TIME_H
  dtls_clock_offset = time(NULL);
#else
# ifdef __GNUC__
  /* Issue a warning when using gcc. Other prepropressors do
   *  not seem to have a similar feature. */
#  warning "cannot initialize clock"
# endif
  dtls_clock_offset = 0;
#endif
}
