/*
 * Copyright (c) 2017, Alex Stanoev
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
/**
 * \addtogroup cc26xx
 * @{
 *
 * \defgroup cc26xx-ccxxware-conf CCxxware-specific configuration
 *
 * @{
 *
 * \file
 *  CCxxware-specific configuration for the cc26xx-cc13xx CPU family
 */
#ifndef CCXXWARE_CONF_H_
#define CCXXWARE_CONF_H_

#include "contiki-conf.h"

/*---------------------------------------------------------------------------*/
/**
 * \brief JTAG interface configuration
 *
 * Those values are not meant to be modified by the user
 * @{
 */
#if CCXXWARE_CONF_JTAG_INTERFACE_ENABLE
#define SET_CCFG_CCFG_TI_OPTIONS_TI_FA_ENABLE           0xC5
#define SET_CCFG_CCFG_TAP_DAP_0_CPU_DAP_ENABLE          0xC5
#define SET_CCFG_CCFG_TAP_DAP_0_PRCM_TAP_ENABLE         0xC5
#define SET_CCFG_CCFG_TAP_DAP_0_TEST_TAP_ENABLE         0xC5
#define SET_CCFG_CCFG_TAP_DAP_1_PBIST2_TAP_ENABLE       0xC5
#define SET_CCFG_CCFG_TAP_DAP_1_PBIST1_TAP_ENABLE       0xC5
#define SET_CCFG_CCFG_TAP_DAP_1_WUC_TAP_ENABLE          0xC5
#else
#define SET_CCFG_CCFG_TI_OPTIONS_TI_FA_ENABLE           0x00
#define SET_CCFG_CCFG_TAP_DAP_0_CPU_DAP_ENABLE          0x00
#define SET_CCFG_CCFG_TAP_DAP_0_PRCM_TAP_ENABLE         0x00
#define SET_CCFG_CCFG_TAP_DAP_0_TEST_TAP_ENABLE         0x00
#define SET_CCFG_CCFG_TAP_DAP_1_PBIST2_TAP_ENABLE       0x00
#define SET_CCFG_CCFG_TAP_DAP_1_PBIST1_TAP_ENABLE       0x00
#define SET_CCFG_CCFG_TAP_DAP_1_WUC_TAP_ENABLE          0x00
#endif
/** @} */
#endif /* CCXXWARE_CONF_H_ */
/*---------------------------------------------------------------------------*/
/**
 * @}
 * @}
 */
