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
/**
 * \addtogroup gpio-hal
 * @{
 *
 * \file
 *     Implementation of the platform-independent aspects of the GPIO HAL
 */
/*---------------------------------------------------------------------------*/
#include "contiki.h"
#include "dev/gpio-hal.h"
#include "lib/list.h"

#include <stdint.h>
#include <string.h>
/*---------------------------------------------------------------------------*/
LIST(handlers);
/*---------------------------------------------------------------------------*/
void
gpio_hal_register_handler(gpio_hal_event_handler_t *handler)
{
  list_add(handlers, handler);
}
/*---------------------------------------------------------------------------*/
void
gpio_hal_event_handler(gpio_hal_pin_mask_t pins)
{
  gpio_hal_event_handler_t *this;

  for(this = list_head(handlers); this != NULL; this = this->next) {
    if(pins & this->pin_mask) {
      if(this->handler != NULL) {
        this->handler(pins & this->pin_mask);
      }
    }
  }
}
/*---------------------------------------------------------------------------*/
void
gpio_hal_init()
{
  list_init(handlers);
}
/*---------------------------------------------------------------------------*/
#if GPIO_HAL_ARCH_SW_TOGGLE
/*---------------------------------------------------------------------------*/
void
gpio_hal_arch_toggle_pin(gpio_hal_pin_t pin)
{
  if(pin >= GPIO_HAL_PIN_COUNT) {
    return;
  }

  gpio_hal_arch_write_pin(pin, gpio_hal_arch_read_pin(pin) ^ 1);
}
/*---------------------------------------------------------------------------*/
void
gpio_hal_arch_toggle_pins(gpio_hal_pin_mask_t pins)
{
  gpio_hal_arch_write_pins(pins, ~gpio_hal_arch_read_pins(pins));
}
/*---------------------------------------------------------------------------*/
#endif /* GPIO_HAL_ARCH_SW_TOGGLE */
/*---------------------------------------------------------------------------*/
/**
 * @}
 */
