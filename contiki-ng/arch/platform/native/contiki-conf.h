/*
 * Copyright (c) 2005, Swedish Institute of Computer Science
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
 *
 * This file is part of the Contiki operating system.
 *
 */

#ifndef CONTIKI_CONF_H_
#define CONTIKI_CONF_H_

/* include the project config */
#ifdef PROJECT_CONF_PATH
#include PROJECT_CONF_PATH
#endif /* PROJECT_CONF_PATH */

#include <inttypes.h>
#ifndef WIN32_LEAN_AND_MEAN
#include <sys/select.h>
#endif

struct select_callback {
  int  (* set_fd)(fd_set *fdr, fd_set *fdw);
  void (* handle_fd)(fd_set *fdr, fd_set *fdw);
};
int select_set_callback(int fd, const struct select_callback *callback);

#define CC_CONF_REGISTER_ARGS          1
#define CC_CONF_FUNCTION_POINTER_ARGS  1
#define CC_CONF_VA_ARGS                1
/*#define CC_CONF_INLINE                 inline*/

#ifndef EEPROM_CONF_SIZE
#define EEPROM_CONF_SIZE				1024
#endif

/* These names are deprecated, use C99 names. */
typedef uint8_t   u8_t;
typedef uint16_t u16_t;
typedef uint32_t u32_t;
typedef  int32_t s32_t;

typedef unsigned int uip_stats_t;

#define LEDS_CONF_LEGACY_API 1

#ifndef UIP_CONF_BYTE_ORDER
#define UIP_CONF_BYTE_ORDER      UIP_LITTLE_ENDIAN
#endif

#if NETSTACK_CONF_WITH_IPV6

#ifndef NETSTACK_CONF_NETWORK
#define NETSTACK_CONF_NETWORK    tun6_net_driver
#endif

#ifndef NETSTACK_CONF_RADIO
#define NETSTACK_CONF_RADIO   nullradio_driver
#endif /* NETSTACK_CONF_RADIO */

#define NETSTACK_CONF_LINUXRADIO_DEV "wpan0"

#define UIP_CONF_IPV6_QUEUE_PKT  1
#define UIP_ARCH_IPCHKSUM        1

#endif /* NETSTACK_CONF_WITH_IPV6 */

#include <ctype.h>

typedef unsigned long clock_time_t;

#define CLOCK_CONF_SECOND 1000

#define LOG_CONF_ENABLED 1

/* Not part of C99 but actually present */
int strcasecmp(const char*, const char*);

#define PLATFORM_CONF_PROVIDES_MAIN_LOOP 1
#define PLATFORM_CONF_MAIN_ACCEPTS_ARGS  1
#define PLATFORM_CONF_SUPPORTS_STACK_CHECK 0

#endif /* CONTIKI_CONF_H_ */
