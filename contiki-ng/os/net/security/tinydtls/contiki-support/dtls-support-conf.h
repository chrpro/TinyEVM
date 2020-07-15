#ifndef DTLS_SUPPORT_CONF_H_
#define DTLS_SUPPORT_CONF_H_

#include "net/ip/uip.h"
#include "sys/ctimer.h"
#include <stdint.h>

/* Use same log level as CoAP as default */
#define LOG_LEVEL_DTLS LOG_LEVEL_COAP

#define DTLS_LOG_CONF_PATH "sys/log.h"

typedef struct {
  struct ctimer retransmit_timer;
} dtls_support_context_state_t;

#define DTLS_SUPPORT_CONF_CONTEXT_STATE dtls_support_context_state_t

typedef struct {
  uip_ipaddr_t addr;
  uint16_t port;
} session_t;

#define DTLS_TICKS_PER_SECOND CLOCK_SECOND

typedef clock_time_t dtls_tick_t;

#define HAVE_ASSERT_H 1

#endif /* DTLS_SUPPORT_CONF_H_ */
