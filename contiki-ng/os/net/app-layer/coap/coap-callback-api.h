/*
 * Copyright (c) 2016, SICS Swedish ICT
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
 *
 */

/**
 * \file
 *      Callback API for doing CoAP requests
 *      Adapted from the blocking API
 * \author
 *      Joakim Eriksson, joakime@sics.se
 */

/**
 * \addtogroup coap
 * @{
 */

#ifndef COAP_CALLBACK_API_H_
#define COAP_CALLBACK_API_H_

#include "coap-engine.h"
#include "coap-transactions.h"
#include "sys/cc.h"

/*---------------------------------------------------------------------------*/
/*- Client Part -------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
typedef struct coap_request_state coap_request_state_t;

struct coap_request_state {
  coap_transaction_t *transaction;
  coap_message_t *response;
  coap_message_t *request;
  coap_endpoint_t *remote_endpoint;
  uint32_t block_num;
  void *user_data;
  coap_timer_t coap_timer;
  void (*callback)(coap_request_state_t *state);
};

void coap_send_request(coap_request_state_t *state, coap_endpoint_t *endpoint,
                       coap_message_t *request,
                       void (*callback)(coap_request_state_t *state));

#endif /* COAP_CALLBACK_API_H_ */
/** @} */
