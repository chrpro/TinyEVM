/*
 * Copyright (c) 2017, George Oikonomou - http://www.spd.gr
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*---------------------------------------------------------------------------*/
#ifndef CC13XX_CC26XX_DEF_H_
#define CC13XX_CC26XX_DEF_H_
/*---------------------------------------------------------------------------*/
#include "cm3/cm3-def.h"
/*---------------------------------------------------------------------------*/
/* TSCH related defines */

/* Delay between GO signal and SFD */
#define RADIO_DELAY_BEFORE_TX ((unsigned)US_TO_RTIMERTICKS(81))
/* Delay between GO signal and start listening.
 * This value is so small because the radio is constantly on within each timeslot. */
#define RADIO_DELAY_BEFORE_RX ((unsigned)US_TO_RTIMERTICKS(15))
/* Delay between the SFD finishes arriving and it is detected in software. */
#define RADIO_DELAY_BEFORE_DETECT ((unsigned)US_TO_RTIMERTICKS(352))

/* Timer conversion; radio is running at 4 MHz */
#define RADIO_TIMER_SECOND   4000000u
#if (RTIMER_SECOND % 256) || (RADIO_TIMER_SECOND % 256)
#error RADIO_TO_RTIMER macro must be fixed!
#endif
#define RADIO_TO_RTIMER(X)   ((uint32_t)(((uint64_t)(X) * (RTIMER_SECOND / 256)) / (RADIO_TIMER_SECOND / 256)))
#define USEC_TO_RADIO(X)     ((X) * 4)

/* The PHY header (preamble + SFD, 4+1 bytes) duration is equivalent to 10 symbols */
#define RADIO_IEEE_802154_PHY_HEADER_DURATION_USEC 160

/* Do not turn off TSCH within a timeslot: not enough time */
#define TSCH_CONF_RADIO_ON_DURING_TIMESLOT 1

/* Disable TSCH frame filtering */
#define TSCH_CONF_HW_FRAME_FILTERING  0

/* Use hardware timestamps */
#ifndef TSCH_CONF_RESYNC_WITH_SFD_TIMESTAMPS
#define TSCH_CONF_RESYNC_WITH_SFD_TIMESTAMPS 1
#define TSCH_CONF_TIMESYNC_REMOVE_JITTER 0
#endif

#ifndef TSCH_CONF_BASE_DRIFT_PPM
/* The drift compared to "true" 10ms slots.
 * Enable adaptive sync to enable compensation for this.
 * Slot length 10000 usec
 *             328 ticks
 * Tick duration 30.517578125 usec
 * Real slot duration 10009.765625 usec
 * Target - real duration = -9.765625 usec
 * TSCH_CONF_BASE_DRIFT_PPM -977
 */
#define TSCH_CONF_BASE_DRIFT_PPM -977
#endif

/* 10 times per second */
#ifndef TSCH_CONF_CHANNEL_SCAN_DURATION
#define TSCH_CONF_CHANNEL_SCAN_DURATION (CLOCK_SECOND / 10)
#endif

/* Slightly reduce the TSCH guard time (from 2200 usec to 1800 usec) to make sure
 * the CC26xx radio has sufficient time to start up. */
#ifndef TSCH_CONF_RX_WAIT
#define TSCH_CONF_RX_WAIT 1800
#endif
/*---------------------------------------------------------------------------*/
#define RTIMER_ARCH_SECOND 65536
/*---------------------------------------------------------------------------*/
/* Path to CMSIS header */
#define CMSIS_CONF_HEADER_PATH               "cc13x0-cc26x0-cm3.h"

/* Path to headers with implementation of mutexes and memory barriers */
#define MUTEX_CONF_ARCH_HEADER_PATH          "mutex-cortex.h"
#define MEMORY_BARRIER_CONF_ARCH_HEADER_PATH "memory-barrier-cortex.h"

#define GPIO_HAL_CONF_ARCH_HDR_PATH          "dev/gpio-hal-arch.h"
/*---------------------------------------------------------------------------*/
#define GPIO_HAL_CONF_ARCH_SW_TOGGLE 0
/*---------------------------------------------------------------------------*/
#endif /* CC13XX_CC26XX_DEF_H_ */
/*---------------------------------------------------------------------------*/
