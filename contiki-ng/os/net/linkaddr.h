/*
 * Copyright (c) 2007, Swedish Institute of Computer Science.
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

/**
 * \file
 *         Header file for the link-layer address representation
 * \author
 *         Adam Dunkels <adam@sics.se>
 */

/**
 * \addtogroup link-layer
 * @{
 */

/**
 * \defgroup linkaddr Link-layer addresses
 * @{
 *
 * The linkaddr module handles link-layer addresses.
 *
 */

#ifndef LINKADDR_H_
#define LINKADDR_H_

#include "contiki.h"

#ifdef LINKADDR_CONF_SIZE
#define LINKADDR_SIZE LINKADDR_CONF_SIZE
#else /* LINKADDR_SIZE */
#define LINKADDR_SIZE 8
#endif /* LINKADDR_SIZE */

typedef union {
  unsigned char u8[LINKADDR_SIZE];
#if LINKADDR_SIZE == 2
  uint16_t u16;
#else
  uint16_t u16[LINKADDR_SIZE/2];
#endif /* LINKADDR_SIZE == 2 */
} linkaddr_t;

typedef union {
  uint8_t u8[8];
  uint16_t u16[4];
} linkaddr_extended_t;

/**
 * \brief      Copy a link-layer address
 * \param dest The destination
 * \param from The source
 *
 *             This function copies a link-layer address from one location
 *             to another.
 *
 */
void linkaddr_copy(linkaddr_t *dest, const linkaddr_t *from);

/**
 * \brief      Compare two link-layer addresses
 * \param addr1 The first address
 * \param addr2 The second address
 * \return     Non-zero if the addresses are the same, zero if they are different
 *
 *             This function compares two link-layer addresses and returns
 *             the result of the comparison. The function acts like
 *             the '==' operator and returns non-zero if the addresses
 *             are the same, and zero if the addresses are different.
 *
 */
int linkaddr_cmp(const linkaddr_t *addr1, const linkaddr_t *addr2);


/**
 * \brief      Set the address of the current node
 * \param addr The address
 *
 *             This function sets the link-layer address of the node.
 *
 */
void linkaddr_set_node_addr(linkaddr_t *addr);

/**
 * \brief      The link-layer address of the node
 *
 *             This variable contains the link-layer address of the
 *             node. This variable should not be changed directly;
 *             rather, the linkaddr_set_node_addr() function should be
 *             used.
 *
 */
extern linkaddr_t linkaddr_node_addr;

/**
 * \brief      The null link-layer address
 *
 *             This variable contains the null link-layer address. The null
 *             address is used in route tables to indicate that the
 *             table entry is unused. Nodes with no configured address
 *             has the null address. Nodes with their node address set
 *             to the null address will have problems communicating
 *             with other nodes.
 *
 */
extern const linkaddr_t linkaddr_null;

#endif /* LINKADDR_H_ */
/** @} */
/** @} */
