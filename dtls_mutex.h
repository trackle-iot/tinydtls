/*******************************************************************************
 *
 * Copyright (c) 2019 Olaf Bergmann (TZI) and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v. 1.0 which accompanies this distribution.
 *
 * The Eclipse Public License is available at http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at 
 * http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Jon Shallow    - Initall add in of generic mutex support
 *
 *******************************************************************************/

/**
 * @file dtls_mutex.h
 * @brief DTLS mutex mechanism wrapper
 */

#ifndef _DTLS_MUTEX_H_
#define _DTLS_MUTEX_H_

#ifdef IS_MBEDOS

typedef int dtls_mutex_t;
#define DTLS_MUTEX_INITIALIZER 0
#define dtls_mutex_lock(a) *(a) = 1
#define dtls_mutex_trylock(a) *(a) = 1
#define dtls_mutex_unlock(a) *(a) = 0

#elif defined(WITH_ZEPHYR) || defined(IS_WINDOWS) || defined(WITH_LWIP)

/* zephyr supports mutex, but this port doesn't use it */

// TODO: Add Windows compatible mutex definitions

typedef int dtls_mutex_t;
#define DTLS_MUTEX_INITIALIZER 0
#define dtls_mutex_lock(a) *(a) = 1
#define dtls_mutex_trylock(a) *(a) = 1
#define dtls_mutex_unlock(a) *(a) = 0


#else /* ! WITH_ZEPHYR && ! IS_WINDOWS */

#include <pthread.h>

typedef pthread_mutex_t dtls_mutex_t;
#define DTLS_MUTEX_INITIALIZER PTHREAD_MUTEX_INITIALIZER
#define dtls_mutex_lock(a) pthread_mutex_lock(a)
#define dtls_mutex_trylock(a) pthread_mutex_trylock(a)
#define dtls_mutex_unlock(a) pthread_mutex_unlock(a)

#endif /* ! IS_WINDOWS */

#endif /* _DTLS_MUTEX_H_ */
