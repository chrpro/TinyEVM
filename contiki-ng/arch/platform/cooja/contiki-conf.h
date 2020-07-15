/*
 * Copyright (c) 2006, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *
 */

#ifndef CONTIKI_CONF_H_
#define CONTIKI_CONF_H_

/* include the project config */
#ifdef PROJECT_CONF_PATH
#include PROJECT_CONF_PATH
#endif /* PROJECT_CONF_PATH */

#ifdef INCLUDE_SUBPLATFORM_CONF
#include "subplatform-conf.h"
#endif /* INCLUDE_SUBPLATFORM_CONF */

#define LOG_CONF_ENABLED 1

#define COOJA 1

#define LEDS_CONF_LEGACY_API 1

#ifndef EEPROM_CONF_SIZE
#define EEPROM_CONF_SIZE				1024
#endif

#define w_memcpy memcpy

#ifdef NETSTACK_CONF_H

/* These header overrides the below default configuration */
#define NETSTACK__QUOTEME(s) NETSTACK_QUOTEME(s)
#define NETSTACK_QUOTEME(s) #s
#include NETSTACK__QUOTEME(NETSTACK_CONF_H)

#else /* NETSTACK_CONF_H */

/* Default network config */
#define CSMA_CONF_SEND_SOFT_ACK 1
#define CSMA_CONF_ACK_WAIT_TIME                RTIMER_SECOND / 500
#define CSMA_CONF_AFTER_ACK_DETECTED_WAIT_TIME 0

/* Radio setup */
#define NETSTACK_CONF_RADIO cooja_radio_driver

#endif /* NETSTACK_CONF_H */

/* Default network config */
#if NETSTACK_CONF_WITH_IPV6



/* Radio setup */
#define NETSTACK_CONF_RADIO cooja_radio_driver

/* configure network size and density */
#ifndef NETSTACK_MAX_ROUTE_ENTRIES
#define NETSTACK_MAX_ROUTE_ENTRIES   300
#endif /* NETSTACK_MAX_ROUTE_ENTRIES */
#ifndef NBR_TABLE_CONF_MAX_NEIGHBORS
#define NBR_TABLE_CONF_MAX_NEIGHBORS 300
#endif /* NBR_TABLE_CONF_MAX_NEIGHBORS */

#ifndef UIP_CONF_IPV6_QUEUE_PKT
#define UIP_CONF_IPV6_QUEUE_PKT         1
#endif /* UIP_CONF_IPV6_QUEUE_PKT */

#endif /* NETSTACK_CONF_WITH_IPV6 */

#define CC_CONF_REGISTER_ARGS          1
#define CC_CONF_FUNCTION_POINTER_ARGS  1
#define CC_CONF_VA_ARGS                1
#define CC_CONF_INLINE inline

/* These names are deprecated, use C99 names. */
#include <inttypes.h>
typedef uint8_t u8_t;
typedef uint16_t u16_t;
typedef uint32_t u32_t;
typedef int32_t s32_t;

typedef unsigned short uip_stats_t;

#define CLOCK_CONF_SECOND 1000L
typedef unsigned long clock_time_t;
typedef uint64_t rtimer_clock_t;
#define RTIMER_CLOCK_DIFF(a,b)     ((int64_t)((a)-(b)))

#define RADIO_DELAY_BEFORE_TX 0
#define RADIO_DELAY_BEFORE_RX 0
#define RADIO_DELAY_BEFORE_DETECT 0

#define UIP_ARCH_IPCHKSUM        1

#if MAC_CONF_WITH_TSCH
/* A bug in cooja causes many EBs to be missed at scan. Increase EB
   frequency to shorten the join process */
#undef TSCH_CONF_EB_PERIOD
#define TSCH_CONF_EB_PERIOD (4 * CLOCK_SECOND)
#undef TSCH_CONF_MAX_EB_PERIOD
#define TSCH_CONF_MAX_EB_PERIOD (4 * CLOCK_SECOND)
#endif /* MAC_CONF_WITH_TSCH */

#define CFS_CONF_OFFSET_TYPE	long

#define RF_CHANNEL                     26
#define NETSTACK_RADIO_MAX_PAYLOAD_LEN 125

#define PLATFORM_CONF_SUPPORTS_STACK_CHECK  0

#endif /* CONTIKI_CONF_H_ */
