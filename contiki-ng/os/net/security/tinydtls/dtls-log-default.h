/*******************************************************************************
 *
 * Copyright (c) 2018, RISE SICS AB.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v. 1.0 which accompanies this distribution.
 *
 * The Eclipse Public License is available at http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 * http://www.eclipse.org/org/documents/edl-v10.php.
 *
 *******************************************************************************/

#ifndef DTLS_LOG_DEFAULT_H_
#define DTLS_LOG_DEFAULT_H_

#include "tinydtls.h"

#include <stdio.h>

/* The different log levels available */
#define LOG_LEVEL_NONE         0 /* No log */
#define LOG_LEVEL_ERR          1 /* Errors */
#define LOG_LEVEL_WARN         2 /* Warnings */
#define LOG_LEVEL_INFO         3 /* Basic info */
#define LOG_LEVEL_DBG          4 /* Detailed debug */

#ifndef LOG_LEVEL_DTLS
#define LOG_LEVEL_DTLS         LOG_LEVEL_INFO
#endif /* LOG_LEVEL_DTLS */

#ifndef LOG_WITH_LOC
#define LOG_WITH_LOC           0
#endif /* LOG_WITH_LOC */

#ifndef LOG_WITH_MODULE_PREFIX
#define LOG_WITH_MODULE_PREFIX 1
#endif /* LOG_WITH_MODULE_PREFIX */

/* Custom output function -- default is printf */
#ifdef LOG_CONF_OUTPUT
#define LOG_OUTPUT(...) LOG_CONF_OUTPUT(__VA_ARGS__)
#else /* LOG_CONF_OUTPUT */
#define LOG_OUTPUT(...) printf(__VA_ARGS__)
#endif /* LOG_CONF_OUTPUT */

/* Custom line prefix output function -- default is LOG_OUTPUT */
#ifdef LOG_CONF_OUTPUT_PREFIX
#define LOG_OUTPUT_PREFIX(level, levelstr, module) LOG_CONF_OUTPUT_PREFIX(level, levelstr, module)
#else /* LOG_CONF_OUTPUT_PREFIX */
#define LOG_OUTPUT_PREFIX(level, levelstr, module) LOG_OUTPUT("[%-4s: %-10s] ", levelstr, module)
#endif /* LOG_CONF_OUTPUT_PREFIX */

/* Main log function */

#define LOG(newline, level, levelstr, ...) do {                         \
    if(level <= (LOG_LEVEL)) {                                          \
      if(newline) {                                                     \
        if(LOG_WITH_MODULE_PREFIX) {                                    \
          LOG_OUTPUT_PREFIX(level, levelstr, LOG_MODULE);               \
        }                                                               \
        if(LOG_WITH_LOC) {                                              \
          LOG_OUTPUT("[%s: %d] ", __FILE__, __LINE__);                  \
        }                                                               \
      }                                                                 \
      LOG_OUTPUT(__VA_ARGS__);                                          \
    }                                                                   \
  } while (0)

/* More compact versions of LOG macros */
#define LOG_ERR(...)           LOG(1, LOG_LEVEL_ERR, "ERR", __VA_ARGS__)
#define LOG_WARN(...)          LOG(1, LOG_LEVEL_WARN, "WARN", __VA_ARGS__)
#define LOG_INFO(...)          LOG(1, LOG_LEVEL_INFO, "INFO", __VA_ARGS__)
#define LOG_DBG(...)           LOG(1, LOG_LEVEL_DBG, "DBG", __VA_ARGS__)

#define LOG_ERR_(...)          LOG(0, LOG_LEVEL_ERR, "ERR", __VA_ARGS__)
#define LOG_WARN_(...)         LOG(0, LOG_LEVEL_WARN, "WARN", __VA_ARGS__)
#define LOG_INFO_(...)         LOG(0, LOG_LEVEL_INFO, "INFO", __VA_ARGS__)
#define LOG_DBG_(...)          LOG(0, LOG_LEVEL_DBG, "DBG", __VA_ARGS__)

/* For testing log level */
#define LOG_ERR_ENABLED        ((LOG_LEVEL) >= (LOG_LEVEL_ERR))
#define LOG_WARN_ENABLED       ((LOG_LEVEL) >= (LOG_LEVEL_WARN))
#define LOG_INFO_ENABLED       ((LOG_LEVEL) >= (LOG_LEVEL_INFO))
#define LOG_DBG_ENABLED        ((LOG_LEVEL) >= (LOG_LEVEL_DBG))

#endif /* DTLS_LOG_DEFAULT_H_ */
