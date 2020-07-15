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

#ifndef DTLS_LOG_H_
#define DTLS_LOG_H_

#include "tinydtls.h"

#ifdef DTLS_LOG_CONF_PATH
#include DTLS_LOG_CONF_PATH
#else /* DTLS_LOG_CONF_PATH */
#include "dtls-log-default.h"
#endif /* DTLS_LOG_CONF_PATH */

/** Returns a zero-terminated string with the name of this library. */
const char *dtls_package_name(void);

/** Returns a zero-terminated string with the library version. */
const char *dtls_package_version(void);

/**
 * Logs data as a hexdump.
 */
void dtls_log_hexdump(const unsigned char *buf, int len);

/**
 * Logs data as a narrow strig of hex digits.
 */
void dtls_log_dump(const unsigned char *buf, int len);

/* DTLS address */
#define LOG_DTLS_ADDR(level, endpoint) do {                 \
    if(level <= (LOG_LEVEL)) {                              \
      dtls_session_log(endpoint);                           \
    }                                                       \
  } while (0)

#define LOG_ERR_DTLS_ADDR(endpoint)  LOG_DTLS_ADDR(LOG_LEVEL_ERR, endpoint)
#define LOG_WARN_DTLS_ADDR(endpoint) LOG_DTLS_ADDR(LOG_LEVEL_WARN, endpoint)
#define LOG_INFO_DTLS_ADDR(endpoint) LOG_DTLS_ADDR(LOG_LEVEL_INFO, endpoint)
#define LOG_DBG_DTLS_ADDR(endpoint)  LOG_DTLS_ADDR(LOG_LEVEL_DBG, endpoint)

/* DTLS log data as a hexdump */
#define LOG_DTLS_DUMP(level, data, len) do {                \
    if(level <= (LOG_LEVEL)) {                              \
      dtls_log_dump(data, len);                             \
    }                                                       \
  } while (0)

#define LOG_ERR_DTLS_DUMP(data, len)  LOG_DTLS_DUMP(LOG_LEVEL_ERR, data, len)
#define LOG_WARN_DTLS_DUMP(data, len) LOG_DTLS_DUMP(LOG_LEVEL_WARN, data, len)
#define LOG_INFO_DTLS_DUMP(data, len) LOG_DTLS_DUMP(LOG_LEVEL_INFO, data, len)
#define LOG_DBG_DTLS_DUMP(data, len)  LOG_DTLS_DUMP(LOG_LEVEL_DBG, data, len)

/* DTLS log as narrow string of hex digits */
#define LOG_DTLS_DUMP(level, data, len) do {                \
    if(level <= (LOG_LEVEL)) {                              \
      dtls_log_dump(data, len);                             \
    }                                                       \
  } while (0)

#define LOG_ERR_DTLS_DUMP(data, len)  LOG_DTLS_DUMP(LOG_LEVEL_ERR, data, len)
#define LOG_WARN_DTLS_DUMP(data, len) LOG_DTLS_DUMP(LOG_LEVEL_WARN, data, len)
#define LOG_INFO_DTLS_DUMP(data, len) LOG_DTLS_DUMP(LOG_LEVEL_INFO, data, len)
#define LOG_DBG_DTLS_DUMP(data, len)  LOG_DTLS_DUMP(LOG_LEVEL_DBG, data, len)

/* Limited support for old debug API */
#define dtls_debug(...) LOG_DBG(__VA_ARGS__)
#define dtls_info(...)  LOG_INFO(__VA_ARGS__)
#define dtls_warn(...)  LOG_WARN(__VA_ARGS__)
#define dtls_crit(...)  LOG_ERR(__VA_ARGS__)
#define dtls_alert(...) LOG_ERR(__VA_ARGS__)
#define dtls_emerg(...) LOG_ERR(__VA_ARGS__)

static inline void
dtls_debug_dump(const char *name, const unsigned char *buf, int len)
{
  LOG_DBG("%s: ", name);
  LOG_DBG_DTLS_DUMP(buf, len);
  LOG_DBG("\n");
}

static inline void
dtls_debug_hexdump(const char *name, const unsigned char *buf, int len)
{
  LOG_DBG("%s: ", name);
  LOG_DBG_DTLS_DUMP(buf, len);
  LOG_DBG("\n");
}

static inline void
dtls_debug_session(const char *name, const session_t *session)
{
  LOG_DBG("%s: ", name);
  LOG_DBG_DTLS_ADDR(session);
  LOG_DBG_("\n");
}

#endif /* DTLS_LOG_H_ */
