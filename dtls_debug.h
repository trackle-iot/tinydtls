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
typedef enum
{
       DTLS_LOG_EMERG = 0,
       DTLS_LOG_ALERT,
       DTLS_LOG_CRIT,
       DTLS_LOG_WARN,
       DTLS_LOG_NOTICE,
       DTLS_LOG_INFO,
       DTLS_LOG_DEBUG
} log_t;

// Pointer to logger function
extern void (*TinyDtls_logCallback)(unsigned int, const char *, ...);

// Dump logger function
void TinyDtls_logBuffer(unsigned int level, const char *name, const unsigned char *buff, int len);

#define dtls_emerg(...) TinyDtls_logCallback(DTLS_LOG_EMERG, __VA_ARGS__)
#define dtls_alert(...) TinyDtls_logCallback(DTLS_LOG_ALERT, __VA_ARGS__)
#define dtls_crit(...) TinyDtls_logCallback(DTLS_LOG_CRIT, __VA_ARGS__)
#define dtls_warn(...) TinyDtls_logCallback(DTLS_LOG_WARN, __VA_ARGS__)
#define dtls_notice(...) TinyDtls_logCallback(DTLS_LOG_NOTICE, __VA_ARGS__)
#define dtls_info(...) TinyDtls_logCallback(DTLS_LOG_INFO, __VA_ARGS__)
#define dtls_debug(...) TinyDtls_logCallback(DTLS_LOG_DEBUG, __VA_ARGS__)
#define dtls_debug_hexdump(name, buf, length) TinyDtls_logBuffer(DTLS_LOG_DEBUG, name, buf, length)
#define dtls_debug_dump(name, buf, length) TinyDtls_logBuffer(DTLS_LOG_DEBUG, name, buf, length)

#define dtls_dsrv_log_addr(level, name, addr) TinyDtls_logCallback(level, "%s session", name)

#endif /* _DTLS_DEBUG_H_ */
