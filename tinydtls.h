/* tinydtls.h.  Generated from tinydtls.h.in by configure.  */
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

/**
 * @file tinydtls.h
 * @brief public tinydtls API
 */

#ifndef _DTLS_TINYDTLS_H_
#define _DTLS_TINYDTLS_H_

/**
 * @brief Add log function that accepts log level as first parameter.
 *
 * @param logCallback Log function to use.
 */
void TinyDtls_set_log_callback(void (*logCallback)(unsigned int, const char *, ...));

#ifdef RIOT_VERSION
#include "platform-specific/riot_boards.h"
#endif /* RIOT_VERSION */

#if defined(_WIN32) || defined(_WIN64)
#define IS_WINDOWS 1
#endif

#define WITH_SHA256 1

#if defined WITH_LWIP || defined(IS_MBEDOS)
#include "platform-specific/lwip_platform.h"
#endif

#ifndef WITH_LWIP
#ifndef IS_MBEDOS
#ifndef CONTIKI
#ifndef RIOT_VERSION
#ifndef IS_WINDOWS
#ifndef WITH_ESPIDF
#ifndef WITH_POSIX
/* TODO: To remove in a future */
#define WITH_POSIX 1
#endif /* WITH_POSIX */
#endif /* WITH_ESPIDF */
#endif /* IS_WINDOWS */
#include "dtls_config.h"
#endif /* RIOT_VERSION */
#endif /* CONTIKI */
#endif /* IS_MBEDOS */
#endif /* WITH_LWIP */

#ifndef DTLS_ECC
#ifndef DTLS_PSK
#error "TinyDTLS requires at least one Cipher suite!"
#endif /* DTLS_PSK */
#endif /* DTLS_ECC */

#endif /* _DTLS_TINYDTLS_H_ */
