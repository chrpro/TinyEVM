/*
 * Copyright (c) 2018, Inria.
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
 *         The shell application
 * \author
 *         Simon Duquennoy <simon.duquennoy@inria.fr>
 */

/**
 * \addtogroup shell Shell
 The shell enables to inspect and manage the network layer and provides
 other system functionalities
 * \ingroup lib
 * @{
 */

#include "contiki.h"
#include "shell.h"
#include "shell-commands.h"
#include "net/ipv6/uip.h"
#include "net/ipv6/ip64-addr.h"

/*---------------------------------------------------------------------------*/
void
shell_output_6addr(shell_output_func output, const uip_ipaddr_t *ipaddr)
{
  uint16_t a;
  unsigned int i;
  int f;

  if(ipaddr == NULL) {
    SHELL_OUTPUT(output, "(NULL IP addr)");
    return;
  }

  if(ip64_addr_is_ipv4_mapped_addr(ipaddr)) {
    /* Printing IPv4-mapped addresses is done according to RFC 4291 */
    SHELL_OUTPUT(output, "::FFFF:%u.%u.%u.%u", ipaddr->u8[12], ipaddr->u8[13], ipaddr->u8[14], ipaddr->u8[15]);
  } else {
    for(i = 0, f = 0; i < sizeof(uip_ipaddr_t); i += 2) {
      a = (ipaddr->u8[i] << 8) + ipaddr->u8[i + 1];
      if(a == 0 && f >= 0) {
        if(f++ == 0) {
          SHELL_OUTPUT(output, "::");
        }
      } else {
        if(f > 0) {
          f = -1;
        } else if(i > 0) {
          SHELL_OUTPUT(output, ":");
        }
        SHELL_OUTPUT(output, "%x", a);
      }
    }
  }
}
/*---------------------------------------------------------------------------*/
void
shell_output_lladdr(shell_output_func output, const linkaddr_t *lladdr)
{
  if(lladdr == NULL) {
    SHELL_OUTPUT(output, "(NULL LL addr)");
    return;
  } else {
    unsigned int i;
    for(i = 0; i < LINKADDR_SIZE; i++) {
      if(i > 0 && i % 2 == 0) {
        SHELL_OUTPUT(output, ".");
      }
      SHELL_OUTPUT(output, "%02x", lladdr->u8[i]);
    }
  }
}
/*---------------------------------------------------------------------------*/
static void
output_prompt(shell_output_func output)
{
  SHELL_OUTPUT(output, "#");
  shell_output_lladdr(output, &linkaddr_node_addr);
  SHELL_OUTPUT(output, "> ");
}
/*---------------------------------------------------------------------------*/
PT_THREAD(shell_input(struct pt *pt, shell_output_func output, const char *cmd))
{
  static char *args;
  static struct shell_command_t *cmd_ptr;

  PT_BEGIN(pt);

  /* Shave off any leading spaces. */
  while(*cmd == ' ') {
    cmd++;
  }

  /* Look for arguments */
  args = strchr(cmd, ' ');
  if(args != NULL) {
    *args = '\0';
    args++;
  }

  /* Lookup for command */
  cmd_ptr = shell_commands;
  while(cmd_ptr->name != NULL) {
    if(strcmp(cmd, cmd_ptr->name) == 0) {
      static struct pt cmd_pt;
      PT_SPAWN(pt, &cmd_pt, cmd_ptr->func(&cmd_pt, output, args));
      goto done;
    }
    cmd_ptr++;
  }

  SHELL_OUTPUT(output, "Command not found. Type 'help' for a list of commands\n");

done:
  output_prompt(output);
  PT_END(pt);
}
/*---------------------------------------------------------------------------*/
void
shell_init(void)
{
  shell_commands_init();
}
/** @} */
