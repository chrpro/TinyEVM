#include "dtls-support.h"
#include "lib/random.h"
#include "net/ipv6/uiplib.h"

/* Log configuration */
#define LOG_MODULE "dtls-support"
#define LOG_LEVEL  LOG_LEVEL_DTLS
#include "dtls-log.h"

static dtls_context_t the_dtls_context;
static dtls_cipher_context_t cipher_context;
static uint8_t lock_context = 0;
/*---------------------------------------------------------------------------*/
dtls_context_t *
dtls_context_acquire(void)
{
  if(lock_context) {
    return NULL;
  }
  lock_context = 1;
  return &the_dtls_context;
}
/*---------------------------------------------------------------------------*/
void
dtls_context_release(dtls_context_t *context)
{
  if(context == &the_dtls_context) {
    lock_context = 0;
  }
}
/*---------------------------------------------------------------------------*/
/* In Contiki we know that there should be no threads accessing the
   functions at the same time which means there is no need for locking */
dtls_cipher_context_t *
dtls_cipher_context_acquire(void)
{
  return &cipher_context;
}
/*---------------------------------------------------------------------------*/
void
dtls_cipher_context_release(dtls_cipher_context_t *c)
{
}
/*---------------------------------------------------------------------------*/
void
dtls_session_init(session_t *sess)
{
  memset(sess, 0, sizeof(session_t));
}
/*---------------------------------------------------------------------------*/
int
dtls_session_equals(const session_t *a, const session_t *b)
{
  return a->port == b->port
    && uip_ipaddr_cmp(&((a)->addr),&(b->addr));
}
/*---------------------------------------------------------------------------*/
void *
dtls_session_get_address(const session_t *a)
{
  return (void *)a;
}
/*---------------------------------------------------------------------------*/
int
dtls_session_get_address_size(const session_t *a)
{
  return sizeof(session_t);
}
/*---------------------------------------------------------------------------*/
void
dtls_session_log(const session_t *addr)
{
  LOG_OUTPUT("[");
  log_6addr(&a->addr);
  LOG_OUTPUT("]");
}
/*---------------------------------------------------------------------------*/
void
dtls_session_print(const session_t *addr)
{
  printf("[");
  uiplib_ipaddr_print(&a->addr);
  printf("]");
}
/*---------------------------------------------------------------------------*/
int
dtls_fill_random(uint8_t *buf, size_t len)
{
  int i;
  if(buf) {
    for(i = 0; i < len; i++) {
      buf[i] = random_rand() & 0xff;
    }
    return 1;
  }
  return 0;
}

/*---------------------------------------------------------------------------*/
/* time support */
/*---------------------------------------------------------------------------*/
void
dtls_ticks(dtls_tick_t *t)
{
  *t = clock_time();
}

/*---------------------------------------------------------------------------*/
/* message retransmission */
/*---------------------------------------------------------------------------*/
static void
dtls_retransmit_callback(void *ptr)
{
  dtls_context_t *context;
  clock_time_t now;
  clock_time_t next;

  context = ptr;

  now = clock_time();

  /* Just one retransmission per timer scheduling */
  dtls_check_retransmit(context, &next, 0);

  if(next != 0) {
    ctimer_set(&context->support.retransmit_timer,
               next <= now ? 1 : next - now,
               dtls_retransmit_callback, context);
  }
}
/*---------------------------------------------------------------------------*/
void
dtls_set_retransmit_timer(dtls_context_t *context, unsigned int time)
{
  /* Start the ctimer for this context */
  ctimer_set(&context->support.retransmit_timer, time,
             dtls_retransmit_callback, context);
}
/*---------------------------------------------------------------------------*/
void
dtls_support_init(void)
{
  /* setup whatever */
}
/*---------------------------------------------------------------------------*/
