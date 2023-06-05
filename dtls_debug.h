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

#ifndef _DTLS_DEBUG_H_
#define _DTLS_DEBUG_H_

#include <stdlib.h>

#include "tinydtls.h"
#include "global.h"
#include "session.h"

/** Pre-defined log levels akin to what is used in \b syslog. */
typedef enum { DTLS_LOG_EMERG=0, DTLS_LOG_ALERT, DTLS_LOG_CRIT, DTLS_LOG_WARN, 
       DTLS_LOG_NOTICE, DTLS_LOG_INFO, DTLS_LOG_DEBUG
} log_t;

/** Returns the current log level. */
log_t dtls_get_log_level(void);

/** Sets the log level to the specified value. */
void dtls_set_log_level(log_t level);

/**
 * Logging callback handler definition.
 *
 * Note: The maximum message length is controlled by the size of the definition
 * for DTLS_DEBUG_BUF_SIZE (default 128) less 1 for the zero termination.
 *
 * @param level One of the DTLS_LOG_* values.
 * @param message Zero-terminated string message to log.
 */
typedef void (*dtls_log_handler_t) (log_t level, const char *message);

/**
 * Add a custom log callback handler.
 *
 * @param app_handler The logging handler to use or @p NULL to use default handler.
 */
void dtls_set_log_handler(dtls_log_handler_t app_handler);

#define dtls_emerg(...) dsrv_log(DTLS_LOG_EMERG, __VA_ARGS__)
#define dtls_alert(...) dsrv_log(DTLS_LOG_ALERT, __VA_ARGS__)
#define dtls_crit(...) dsrv_log(DTLS_LOG_CRIT, __VA_ARGS__)
#define dtls_warn(...) dsrv_log(DTLS_LOG_WARN, __VA_ARGS__)
#define dtls_notice(...) dsrv_log(DTLS_LOG_NOTICE, __VA_ARGS__)
#define dtls_info(...) dsrv_log(DTLS_LOG_INFO, __VA_ARGS__)
#define dtls_debug(...) dsrv_log(DTLS_LOG_DEBUG, __VA_ARGS__)
#define dtls_debug_hexdump(name, buf, length) dtls_dsrv_hexdump_log(DTLS_LOG_DEBUG, name, buf, length, 1)
#define dtls_debug_dump(name, buf, length) dtls_dsrv_hexdump_log(DTLS_LOG_DEBUG, name, buf, length, 0)

#endif /* _DTLS_DEBUG_H_ */
