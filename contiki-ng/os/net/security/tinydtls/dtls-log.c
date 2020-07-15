/*******************************************************************************
 *
 * Copyright (c) 2011, 2012, 2013, 2014, 2015 Olaf Bergmann (TZI) and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v. 1.0 which accompanies this distribution.
 *
 * The Eclipse Public License is available at http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 * http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Olaf Bergmann  - initial API and implementation
 *    Hauke Mehrtens - memory optimization, ECC integration
 *
 *******************************************************************************/

#define LOG_MODULE "dtls-log"
#define LOG_LEVEL  LOG_LEVEL_NONE
#include "dtls-log.h"

const char *
dtls_package_name(void)
{
  return PACKAGE_NAME;
}

const char *
dtls_package_version(void)
{
  return PACKAGE_VERSION;
}

void
dtls_log_hexdump(const unsigned char *buf, int len)
{
  int n = 0;

  while(len-- > 0) {
    if(n % 16 == 0) {
      LOG_OUTPUT("%08X ", n);
    }

    LOG_OUTPUT("%02X ", *buf++);

    n++;
    if(n % 8 == 0) {
      if(n % 16 == 0) {
	LOG_OUTPUT("\n");
      } else {
	LOG_OUTPUT(" ");
      }
    }
  }
}

/** dump as narrow string of hex digits */
void
dtls_log_dump(const unsigned char *buf, int len)
{
  while(len-- > 0) {
    LOG_OUTPUT("%02x", *buf++);
  }
}
