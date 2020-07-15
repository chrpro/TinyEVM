/*
 * Copyright (c) 2012, Texas Instruments Incorporated - http://www.ti.com/
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
 *
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
 * \addtogroup cc2538dk-button-sensor
 * @{
 *
 * \file
 *  Driver for the SmartRF06EB buttons
 */
#include "contiki.h"
#include "dev/nvic.h"
#include "dev/ioc.h"
#include "dev/gpio.h"
#include "dev/button-sensor.h"
#include "sys/timer.h"

#include <stdint.h>
#include <string.h>

#define BUTTON_SELECT_PORT_BASE  GPIO_PORT_TO_BASE(BUTTON_SELECT_PORT)
#define BUTTON_SELECT_PIN_MASK   GPIO_PIN_MASK(BUTTON_SELECT_PIN)

#define BUTTON_LEFT_PORT_BASE    GPIO_PORT_TO_BASE(BUTTON_LEFT_PORT)
#define BUTTON_LEFT_PIN_MASK     GPIO_PIN_MASK(BUTTON_LEFT_PIN)

#define BUTTON_RIGHT_PORT_BASE   GPIO_PORT_TO_BASE(BUTTON_RIGHT_PORT)
#define BUTTON_RIGHT_PIN_MASK    GPIO_PIN_MASK(BUTTON_RIGHT_PIN)

#define BUTTON_UP_PORT_BASE      GPIO_PORT_TO_BASE(BUTTON_UP_PORT)
#define BUTTON_UP_PIN_MASK       GPIO_PIN_MASK(BUTTON_UP_PIN)

#define BUTTON_DOWN_PORT_BASE    GPIO_PORT_TO_BASE(BUTTON_DOWN_PORT)
#define BUTTON_DOWN_PIN_MASK     GPIO_PIN_MASK(BUTTON_DOWN_PIN)
/*---------------------------------------------------------------------------*/
static struct timer debouncetimer;
/*---------------------------------------------------------------------------*/
/**
 * \brief Common initialiser for all buttons
 * \param port_base GPIO port's register offset
 * \param pin_mask Pin mask corresponding to the button's pin
 */
static void
config(uint32_t port_base, uint32_t pin_mask)
{
  /* Software controlled */
  GPIO_SOFTWARE_CONTROL(port_base, pin_mask);

  /* Set pin to input */
  GPIO_SET_INPUT(port_base, pin_mask);

  /* Enable edge detection */
  GPIO_DETECT_EDGE(port_base, pin_mask);

  /* Single edge */
  GPIO_TRIGGER_SINGLE_EDGE(port_base, pin_mask);

  /* Trigger interrupt on Falling edge */
  GPIO_DETECT_RISING(port_base, pin_mask);

  GPIO_ENABLE_INTERRUPT(port_base, pin_mask);
}
/*---------------------------------------------------------------------------*/
static void
button_press_handler(gpio_hal_pin_mask_t pin_mask)
{
  if(!timer_expired(&debouncetimer)) {
    return;
  }

  timer_set(&debouncetimer, CLOCK_SECOND / 8);

  if(pin_mask &
     (gpio_hal_pin_to_mask(BUTTON_SELECT_PIN) << (BUTTON_SELECT_PORT << 3))) {
    sensors_changed(&button_select_sensor);
  } else if(pin_mask &
      (gpio_hal_pin_to_mask(BUTTON_LEFT_PIN) << (BUTTON_LEFT_PORT << 3))) {
    sensors_changed(&button_left_sensor);
  } else if(pin_mask &
      (gpio_hal_pin_to_mask(BUTTON_RIGHT_PIN) << (BUTTON_RIGHT_PORT << 3))) {
    sensors_changed(&button_right_sensor);
  } else if(pin_mask &
      (gpio_hal_pin_to_mask(BUTTON_UP_PIN) << (BUTTON_UP_PORT << 3))) {
    sensors_changed(&button_up_sensor);
  } else if(pin_mask &
      (gpio_hal_pin_to_mask(BUTTON_DOWN_PIN) << (BUTTON_DOWN_PORT << 3))) {
    sensors_changed(&button_down_sensor);
  }
}
/*---------------------------------------------------------------------------*/
static gpio_hal_event_handler_t press_handler = {
  .next = NULL,
  .handler = button_press_handler,
  .pin_mask = 0,
};
/*---------------------------------------------------------------------------*/
static void
register_btn_callback(uint8_t port_num, uint8_t pin)
{
  press_handler.pin_mask |=
    gpio_hal_pin_to_mask(pin) << (port_num << 3);

  gpio_hal_register_handler(&press_handler);
}
/*---------------------------------------------------------------------------*/
/**
 * \brief Init function for the select button.
 *
 * Parameters are ignored. They have been included because the prototype is
 * dictated by the core sensor api. The return value is also not required by
 * the API but otherwise ignored.
 *
 * \param type ignored
 * \param value ignored
 * \return ignored
 */
static int
config_select(int type, int value)
{
  config(BUTTON_SELECT_PORT_BASE, BUTTON_SELECT_PIN_MASK);

  ioc_set_over(BUTTON_SELECT_PORT, BUTTON_SELECT_PIN, IOC_OVERRIDE_PUE);

  NVIC_EnableIRQ(BUTTON_SELECT_VECTOR);

  register_btn_callback(BUTTON_SELECT_PORT, BUTTON_SELECT_PIN);
  return 1;
}
/*---------------------------------------------------------------------------*/
/**
 * \brief Init function for the left button.
 *
 * Parameters are ignored. They have been included because the prototype is
 * dictated by the core sensor api. The return value is also not required by
 * the API but otherwise ignored.
 *
 * \param type ignored
 * \param value ignored
 * \return ignored
 */
static int
config_left(int type, int value)
{
  config(BUTTON_LEFT_PORT_BASE, BUTTON_LEFT_PIN_MASK);

  ioc_set_over(BUTTON_LEFT_PORT, BUTTON_LEFT_PIN, IOC_OVERRIDE_PUE);

  NVIC_EnableIRQ(BUTTON_LEFT_VECTOR);

  register_btn_callback(BUTTON_LEFT_PORT, BUTTON_LEFT_PIN);
  return 1;
}
/*---------------------------------------------------------------------------*/
/**
 * \brief Init function for the right button.
 *
 * Parameters are ignored. They have been included because the prototype is
 * dictated by the core sensor api. The return value is also not required by
 * the API but otherwise ignored.
 *
 * \param type ignored
 * \param value ignored
 * \return ignored
 */
static int
config_right(int type, int value)
{
  config(BUTTON_RIGHT_PORT_BASE, BUTTON_RIGHT_PIN_MASK);

  ioc_set_over(BUTTON_RIGHT_PORT, BUTTON_RIGHT_PIN, IOC_OVERRIDE_PUE);

  NVIC_EnableIRQ(BUTTON_RIGHT_VECTOR);

  register_btn_callback(BUTTON_RIGHT_PORT, BUTTON_RIGHT_PIN);
  return 1;
}
/*---------------------------------------------------------------------------*/
/**
 * \brief Init function for the up button.
 *
 * Parameters are ignored. They have been included because the prototype is
 * dictated by the core sensor api. The return value is also not required by
 * the API but otherwise ignored.
 *
 * \param type ignored
 * \param value ignored
 * \return ignored
 */
static int
config_up(int type, int value)
{
  config(BUTTON_UP_PORT_BASE, BUTTON_UP_PIN_MASK);

  ioc_set_over(BUTTON_UP_PORT, BUTTON_UP_PIN, IOC_OVERRIDE_PUE);

  NVIC_EnableIRQ(BUTTON_UP_VECTOR);

  register_btn_callback(BUTTON_UP_PORT, BUTTON_UP_PIN);
  return 1;
}
/*---------------------------------------------------------------------------*/
/**
 * \brief Init function for the down button.
 *
 * Parameters are ignored. They have been included because the prototype is
 * dictated by the core sensor api. The return value is also not required by
 * the API but otherwise ignored.
 *
 * \param type ignored
 * \param value ignored
 * \return ignored
 */
static int
config_down(int type, int value)
{
  config(BUTTON_DOWN_PORT_BASE, BUTTON_DOWN_PIN_MASK);

  ioc_set_over(BUTTON_DOWN_PORT, BUTTON_DOWN_PIN, IOC_OVERRIDE_PUE);

  NVIC_EnableIRQ(BUTTON_DOWN_VECTOR);

  register_btn_callback(BUTTON_DOWN_PORT, BUTTON_DOWN_PIN);
  return 1;
}
/*---------------------------------------------------------------------------*/
void
button_sensor_init()
{
  timer_set(&debouncetimer, 0);
}
/*---------------------------------------------------------------------------*/
SENSORS_SENSOR(button_select_sensor, BUTTON_SENSOR, NULL, config_select, NULL);
SENSORS_SENSOR(button_left_sensor, BUTTON_SENSOR, NULL, config_left, NULL);
SENSORS_SENSOR(button_right_sensor, BUTTON_SENSOR, NULL, config_right, NULL);
SENSORS_SENSOR(button_up_sensor, BUTTON_SENSOR, NULL, config_up, NULL);
SENSORS_SENSOR(button_down_sensor, BUTTON_SENSOR, NULL, config_down, NULL);

/** @} */
