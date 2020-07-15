/******************************************************************************
*  Filename:       rf_patch_cpe_ble.h
*  Revised:        $Date: 2016-06-08 15:35:28 +0200 (on, 08 jun 2016) $
*  Revision:       $Revision: 17220 $
*
*  Description:    RF Core patch file for CC1350 Bluetooth Low Energy
*
*  Copyright (c) 2015, Texas Instruments Incorporated
*  All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions are met:
*
*  1) Redistributions of source code must retain the above copyright notice,
*     this list of conditions and the following disclaimer.
*
*  2) Redistributions in binary form must reproduce the above copyright notice,
*     this list of conditions and the following disclaimer in the documentation
*     and/or other materials provided with the distribution.
*
*  3) Neither the name of the ORGANIZATION nor the names of its contributors may
*     be used to endorse or promote products derived from this software without
*     specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
*  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
*  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
*  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
*  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
*  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
*  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
*  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
*  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
*  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
*  POSSIBILITY OF SUCH DAMAGE.
*
******************************************************************************/

#ifndef _RF_PATCH_CPE_BLE_H
#define _RF_PATCH_CPE_BLE_H

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <string.h>

#ifndef CPE_PATCH_TYPE
#define CPE_PATCH_TYPE static const uint32_t
#endif

#ifndef SYS_PATCH_TYPE
#define SYS_PATCH_TYPE static const uint32_t
#endif

#ifndef PATCH_FUN_SPEC
#define PATCH_FUN_SPEC static inline
#endif

#ifndef _APPLY_PATCH_TAB
#define _APPLY_PATCH_TAB
#endif


CPE_PATCH_TYPE patchImageBle[] = {
   0x21000435,
   0x21000447,
   0x21000447,
   0x2100048d,
   0x2100050d,
   0x2100046d,
   0x490b4c0c,
   0x28ff7820,
   0x7ac8d101,
   0x20077020,
   0x4c08e006,
   0x28ff7820,
   0x21ffd007,
   0x49047021,
   0x0224240f,
   0x31604320,
   0x467082c8,
   0x47001c80,
   0x40086200,
   0x21000534,
   0x4905b570,
   0xb6724a05,
   0x28017908,
   0x2001dc02,
   0x1d127088,
   0x4710b662,
   0x21000298,
   0x00004a81,
   0x4e1ab5f8,
   0x6b714605,
   0x09cc4819,
   0x2d0001e4,
   0x4918d011,
   0x29027809,
   0x7b00d00f,
   0xb6724304,
   0x4f152001,
   0x47b80240,
   0x38204811,
   0x09c18800,
   0xd00407c9,
   0x7ac0e016,
   0x7b40e7f0,
   0x490fe7ee,
   0x61cc6374,
   0x07c00a40,
   0x2001d00c,
   0x6b310380,
   0xd0012d00,
   0xe0004301,
   0x46084381,
   0x49076331,
   0x63483940,
   0x47b82000,
   0xbdf8b662,
   0x21000280,
   0x21000088,
   0x2100029b,
   0x00003f7b,
   0x40044040,
   0x28004907,
   0x2004d000,
   0xb6724a06,
   0x07c97809,
   0x5810d001,
   0x2080e000,
   0xb240b662,
   0x00004770,
   0x2100026b,
   0x40046058,
   0x000000ff,
};
#define _NWORD_PATCHIMAGE_BLE 71

#define _NWORD_PATCHSYS_BLE 0



#ifndef _BLE_SYSRAM_START
#define _BLE_SYSRAM_START 0x20000000
#endif

#ifndef _BLE_CPERAM_START
#define _BLE_CPERAM_START 0x21000000
#endif

#define _BLE_SYS_PATCH_FIXED_ADDR 0x20000000

#define _BLE_PARSER_PATCH_TAB_OFFSET 0x0338
#define _BLE_PATCH_TAB_OFFSET 0x0340
#define _BLE_IRQPATCH_OFFSET 0x03BC
#define _BLE_PATCH_VEC_OFFSET 0x041C

PATCH_FUN_SPEC void enterBleCpePatch(void)
{
   uint32_t *pPatchVec = (uint32_t *) (_BLE_CPERAM_START + _BLE_PATCH_VEC_OFFSET);

#if (_NWORD_PATCHIMAGE_BLE > 0)
   memcpy(pPatchVec, patchImageBle, sizeof(patchImageBle));
#endif
}

PATCH_FUN_SPEC void enterBleSysPatch(void)
{
}

PATCH_FUN_SPEC void configureBlePatch(void)
{
   uint8_t *pPatchTab = (uint8_t *) (_BLE_CPERAM_START + _BLE_PATCH_TAB_OFFSET);


   pPatchTab[112] = 0;
   pPatchTab[104] = 1;
   pPatchTab[105] = 2;
   pPatchTab[110] = 3;
   pPatchTab[65] = 4;
   pPatchTab[53] = 5;
}

PATCH_FUN_SPEC void applyBlePatch(void)
{
   enterBleSysPatch();
   enterBleCpePatch();
   configureBlePatch();
}

PATCH_FUN_SPEC void refreshBlePatch(void)
{
   enterBleCpePatch();
   configureBlePatch();
}

PATCH_FUN_SPEC void rf_patch_cpe_ble(void)
{
   applyBlePatch();
}


//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif //  _RF_PATCH_CPE_BLE_H

