/*
 * Copyright (c) 2016, SICS Swedish ICT AB
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
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
 * \file
 *         Implementation of OMA LWM2M / IPSO Generic Control
 * \author
 *         Joakim Eriksson <joakime@sics.se>
 *         Niclas Finne <nfi@sics.se>
 */

#include "lwm2m-object.h"
#include "lwm2m-engine.h"
#include "coap-engine.h"
#include "ipso-control-template.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>

static lwm2m_status_t set_value(ipso_control_t *control, uint8_t value);
static lwm2m_status_t set_light_value(ipso_control_t *control, uint8_t value);

/* Name, Object ID, Instance ID, set_value callback */
IPSO_CONTROL(test_control, 3306, 0, set_value);

IPSO_CONTROL(light_control, 3311, 0, set_light_value);

/*---------------------------------------------------------------------------*/
static lwm2m_status_t
set_value(ipso_control_t *control, uint8_t value)
{
  /* do something with the value! */
  printf("Value set to: %u before: %u\n", value,
         ipso_control_get_value(control));
  return LWM2M_STATUS_OK;
}
/*---------------------------------------------------------------------------*/
static lwm2m_status_t
set_light_value(ipso_control_t *control, uint8_t value)
{
  /* do something with the value! */
  printf("Light value set to: %u before: %u\n", value,
         ipso_control_get_value(control));
  return LWM2M_STATUS_OK;
}
/*---------------------------------------------------------------------------*/
void
ipso_control_test_init(void)
{
  ipso_control_add(&test_control);
  ipso_control_add(&light_control);
}
/*---------------------------------------------------------------------------*/
