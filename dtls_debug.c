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

#include "tinydtls.h"

#if defined(HAVE_ASSERT_H) && !defined(assert)
#include <assert.h>
#endif

#include <stdarg.h>
#include <stdio.h>

#ifdef HAVE_TIME_H
#include <time.h>
#endif

#include "global.h"
#include "dtls_debug.h"
#include "dtls_mutex.h"

#ifndef min2
#define min2(a,b) ((a) < (b) ? (a) : (b))
#endif

static int maxlog = DTLS_LOG_WARN;      /* default maximum log level */

log_t
dtls_get_log_level(void) {
  return maxlog;
}

void
dtls_set_log_level(log_t level) {
  maxlog = level;
}

/* this array has the same order as the type log_t */
static const char *loglevels[] = {
  "EMRG", "ALRT", "CRIT", "WARN", "NOTE", "INFO", "DEBG"
};

#ifdef HAVE_TIME_H

static inline size_t
print_timestamp(char *s, size_t len, time_t t) {
  struct tm *tmp;
  tmp = localtime(&t);
  return strftime(s, len, "%b %d %H:%M:%S", tmp);
}

#else /* alternative implementation: just print the timestamp */

static inline size_t
print_timestamp(char *s, size_t len, clock_time_t t) {
#ifdef HAVE_SNPRINTF
  return snprintf(s, len, "%u.%03u",
                  (unsigned int)(t / CLOCK_SECOND),
                  (unsigned int)(t % CLOCK_SECOND));
#else /* HAVE_SNPRINTF */
  /* @todo do manual conversion of timestamp */
  return 0;
#endif /* HAVE_SNPRINTF */
}

#endif /* HAVE_TIME_H */

#ifndef NDEBUG

/**
 * A length-safe strlen() fake.
 *
 * @param s      The string to count characters != 0.
 * @param maxlen The maximum length of @p s.
 *
 * @return The length of @p s.
 */
static inline size_t
dtls_strnlen(const char *s, size_t maxlen) {
  size_t n = 0;
  while(*s++ && n < maxlen)
    ++n;
  return n;
}

#endif /* NDEBUG */

#if !defined(WITH_CONTIKI) && !defined(_MSC_VER)

static void
dtls_logging_handler(log_t level, const char *message) {
  static char timebuf[32];
  FILE* log_fd = level <= DTLS_LOG_CRIT ? stderr : stdout;

  if (print_timestamp(timebuf,sizeof(timebuf), time(NULL)))
    fprintf(log_fd, "%s ", timebuf);

  if (level <= DTLS_LOG_DEBUG)
    fprintf(log_fd, "%s ", loglevels[level]);

  fwrite(message, strlen(message), 1, log_fd);
  fflush(log_fd);
}

static dtls_log_handler_t log_handler = dtls_logging_handler;

void
dtls_set_log_handler(dtls_log_handler_t app_handler) {
  if (app_handler == NULL)
    log_handler = dtls_logging_handler;
  else
    log_handler = app_handler;
}

#ifndef DTLS_DEBUG_BUF_SIZE
#define DTLS_DEBUG_BUF_SIZE 128
#endif

#ifdef DTLS_CONSTRAINED_STACK
static dtls_mutex_t static_mutex = DTLS_MUTEX_INITIALIZER;
static char message[DTLS_DEBUG_BUF_SIZE];
#endif /* DTLS_CONSTRAINED_STACK */

/*
 * Caution. If DTLS_CONSTRAINED_STACK is set, then the same mutex will get
 * locked in dtls_log() and/or dtls_dsrv_hexdump_log() so these functions
 * cannot call each other.  Furthermore, if log_handler() calls dsrv_log() /
 * dtls_dsrv_hexdump_log() there will be a recursive lookup.
 */
void
dsrv_log(log_t level, const char *format, ...) {
  va_list ap;
  size_t len;
#ifndef DTLS_CONSTRAINED_STACK
  char message[DTLS_DEBUG_BUF_SIZE];
#endif /* ! DTLS_CONSTRAINED_STACK */

  if (maxlog < (int) level)
    return;

#ifdef DTLS_CONSTRAINED_STACK
  dtls_mutex_lock(&static_mutex);
#endif /* DTLS_CONSTRAINED_STACK */
  va_start(ap, format);
  len = vsnprintf( message, sizeof(message), format, ap);
  va_end(ap);
  if (len + 1 > sizeof(message)) {
    /* 6 is needed as trailing 0 byte space needed */
    snprintf(&message[sizeof(message)-6], 6, " ...\n");
  }
  log_handler(level, message);
#ifdef DTLS_CONSTRAINED_STACK
  dtls_mutex_unlock(&static_mutex);
#endif /* DTLS_CONSTRAINED_STACK */
}

#elif defined (HAVE_VPRINTF) /* WITH_CONTIKI */
void
dsrv_log(log_t level, char *format, ...) {
  static char timebuf[32];
  va_list ap;

  if (maxlog < level)
    return;

  if (print_timestamp(timebuf,sizeof(timebuf), clock_time()))
    PRINTF("%s ", timebuf);

  if (level <= DTLS_LOG_DEBUG)
    PRINTF("%s ", loglevels[level]);

  va_start(ap, format);
  vprintf(format, ap);
  va_end(ap);
}
#endif /* WITH_CONTIKI */

#ifndef NDEBUG
/** dumps packets in usual hexdump format */
void hexdump(const unsigned char *packet, int length) {
  int n = 0;

  while (length--) {
    if (n % 16 == 0)
      printf("%08X ",n);

    printf("%02X ", *packet++);

    n++;
    if (n % 8 == 0) {
      if (n % 16 == 0)
        printf("\n");
      else
        printf(" ");
    }
  }
}

/** dump as narrow string of hex digits */
void dump(unsigned char *buf, size_t len) {
  while (len--)
    printf("%02x", *buf++);
}

void dtls_dsrv_log_addr(log_t level, const char *name, const session_t *addr)
{
  char addrbuf[73];
  dsrv_log(level, "%s: %s\n", name, addrbuf);
}

#ifndef WITH_CONTIKI
void
dtls_dsrv_hexdump_log(log_t level, const char *name, const unsigned char *buf, size_t length, int extend) {
  int n = 0;
#ifndef DTLS_CONSTRAINED_STACK
  char message[DTLS_DEBUG_BUF_SIZE];
#endif /* ! DTLS_CONSTRAINED_STACK */
  size_t len;

  if (maxlog < (int) level)
    return;

#ifdef DTLS_CONSTRAINED_STACK
  dtls_mutex_lock(&static_mutex);
#endif /* DTLS_CONSTRAINED_STACK */
  if (extend) {
    snprintf(message, sizeof(message), "%s: (%zu bytes):\n", name, length);
    log_handler(level, message);
    len = 0;
    while (length-- && len + 1 < sizeof(message)) {
      if (n % 16 == 0) {
        len += snprintf(&message[len], sizeof(message)-len, "%08X ", n);
        if (len + 1 >= sizeof(message))
          break;
      }
      len += snprintf(&message[len], sizeof(message)-len, "%02X ", *buf++);
      if (len + 1 >= sizeof(message))
        break;
      n++;
      if (n % 8 == 0) {
        if (n % 16 == 0) {
          if (len + 2 < sizeof(message))
            snprintf(&message[len], sizeof(message)-len, "\n");
          else {
            /* 6 is needed as trailing 0 byte space needed */
            snprintf(&message[sizeof(message)-6], 6, " ...\n");
          }
          log_handler(level, message);
          len = 0;
        } else {
          len += snprintf(&message[len], sizeof(message)-len, " ");
        }
      }
    }
  } else {
    len = snprintf(message, sizeof(message), "%s: (%zu bytes): ", name, length);
    while (length-- && len + 1 < sizeof(message)) {
      len += snprintf(&message[len], sizeof(message)-len, "%02X", *buf++);
    }
  }
  if (len) {
    /* Process any new output */
    if (len + 2 < sizeof(message)) {
      snprintf(&message[len], sizeof(message)-len, "\n");
    } else {
      /* 6 is needed as trailing 0 byte space needed */
      snprintf(&message[sizeof(message)-6], 6, " ...\n");
    }
    log_handler(level, message);
  }
#ifdef DTLS_CONSTRAINED_STACK
  dtls_mutex_unlock(&static_mutex);
#endif /* DTLS_CONSTRAINED_STACK */
}
#else /* WITH_CONTIKI */
void
dtls_dsrv_hexdump_log(log_t level, const char *name, const unsigned char *buf, size_t length, int extend) {
  static char timebuf[32];
  int n = 0;

  if (maxlog < level)
    return;

  if (print_timestamp(timebuf,sizeof(timebuf), clock_time()))
    PRINTF("%s ", timebuf);

  if (level >= 0 && level <= DTLS_LOG_DEBUG)
    PRINTF("%s ", loglevels[level]);

  if (extend) {
    PRINTF("%s: (%zu bytes):\n", name, length);

    while (length--) {
      if (n % 16 == 0)
        PRINTF("%08X ", n);

      PRINTF("%02X ", *buf++);

      n++;
      if (n % 8 == 0) {
        if (n % 16 == 0)
          PRINTF("\n");
        else
          PRINTF(" ");
      }
    }
  } else {
    PRINTF("%s: (%zu bytes): ", name, length);
    while (length--)
      PRINTF("%02X", *buf++);
  }
  PRINTF("\n");
}
#endif /* WITH_CONTIKI */

#else /* NDEBUG */

void
hexdump(const unsigned char *packet, int length) {
  (void)packet;
  (void)length;
}

void
dump(unsigned char *buf, size_t len) {
  (void)buf;
  (void)len;
}

void
dtls_dsrv_hexdump_log(log_t level, const char *name, const unsigned char *buf, size_t length, int extend) {
  (void)level;
  (void)name;
  (void)buf;
  (void)length;
  (void)extend;
}

void
dtls_dsrv_log_addr(log_t level, const char *name, const session_t *addr) {
  (void)level;
  (void)name;
  (void)addr;
}

#endif /* NDEBUG */
