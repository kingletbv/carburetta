#ifdef _WIN32
#ifndef WINDOWS_H_INCLUDED
#define WINDOWS_H_INCLUDED
#include <Windows.h>
#endif
#else
#ifndef UNISTD_H_INCLUDED
#define UNISTD_H_INCLUDED
#include <unistd.h>
#endif
#endif

#ifndef STDIO_H_INCLUDED
#define STDIO_H_INCLUDED
#include <stdio.h>
#endif

#ifndef STDLIB_H_INCLUDED
#define STDLIB_H_INCLUDED
#include <stdlib.h>
#endif

#ifndef STDARG_H_INCLUDED
#define STDARG_H_INCLUDED
#include <stdarg.h>
#endif

#ifndef KLT_LOGGER_H_INCLUDED
#define KLT_LOGGER_H_INCLUDED
#include "klt_logger.h"
#endif

#ifdef _WIN32
void log_function(struct klt_log_node *ln, void *baton, int level, const char *file, int line, const char *fmt, va_list args) {
  HANDLE hc = GetStdHandle(STD_ERROR_HANDLE);
  if (hc == INVALID_HANDLE_VALUE) {
    goto fallback;
  }
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  memset(&csbi, 0, sizeof(csbi));
  if (!GetConsoleScreenBufferInfo(hc, &csbi)) {
    goto fallback;
  }
  switch (level) {
  case KLT_LOG_ERROR_LEVEL:
    SetConsoleTextAttribute(hc, FOREGROUND_RED | FOREGROUND_INTENSITY);
    break;
  case KLT_LOG_LOG_LEVEL:
    SetConsoleTextAttribute(hc, FOREGROUND_GREEN);
    break;
  case KLT_LOG_TRACE_LEVEL:
    /* Default is fine */
    SetConsoleTextAttribute(hc, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    break;
  default:
    goto fallback;
  }
  vfprintf(stderr, fmt, args);
  fflush(stderr);
  SetConsoleTextAttribute(hc, csbi.wAttributes);
  return;

fallback:
  vfprintf(stderr, fmt, args);
  return;
}

#else
void log_function(struct klt_log_node *ln, void *baton, int level, const char *file, int line, const char *fmt, va_list args) {
  switch (level) {
  case KLT_LOG_ERROR_LEVEL:
    if (isatty(fileno(stderr))) {
      fprintf(stderr, "\033[1;31m");
      vfprintf(stderr, fmt, args);
      fprintf(stderr, "\033[0m");
    }
    else {
      vfprintf(stderr, fmt, args);
    }
    break;
  case KLT_LOG_LOG_LEVEL:
    if (isatty(fileno(stderr))) {
      fprintf(stderr, "\033[32m");
      vfprintf(stderr, fmt, args);
      fprintf(stderr, "\033[0m");
    }
    else {
      vfprintf(stderr, fmt, args);
    }
    break;
  case KLT_LOG_TRACE_LEVEL:
    vfprintf(stderr, fmt, args);
    break;
  default:
    if (isatty(fileno(stderr))) {
      fprintf(stderr, "\033[1;35m(UNKNOWN LEVEL)\033[1;31m");
      vfprintf(stderr, fmt, args);
      fprintf(stderr, "\033[0m");
    }
    else {
      vfprintf(stderr, fmt, args);
    }
  }
}
#endif
