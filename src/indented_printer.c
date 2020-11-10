#ifndef _MSC_VER
#define _GNU_SOURCE /* expose vasprintf() declaration in stdio.h */
#endif

#ifndef STDIO_H_INCLUDED
#define STDIO_H_INCLUDED
#include <stdio.h>
#endif

#ifndef STDARG_H_INCLUDED
#define STDARG_H_INCLUDED
#include <stdarg.h>
#endif

#ifndef STDLIB_H_INCLUDED
#define STDLIB_H_INCLUDED
#include <stdlib.h>
#endif

#ifndef STRING_H_INCLUDED
#define STRING_H_INCLUDED
#include <string.h>
#endif

#ifndef REPORT_ERROR_H_INCLUDED
#define REPORT_ERROR_H_INCLUDED
#include "report_error.h"
#endif

#ifndef INDENTED_PRINTER_H_INCLUDED
#define INDENTED_PRINTER_H_INCLUDED
#include "indented_printer.h"
#endif

void ip_init(struct indented_printer *ip, FILE *outfp, const char *filename) {
  ip->outfp_ = outfp;
  ip->filename_ = filename;
  ip->indentation_ = 0;
  ip->indent_size_ = 2;
  ip->had_error_ = 0;
  ip->at_start_of_line_ = 1;
}

void ip_cleanup(struct indented_printer *ip) {
}

void ip_write_no_indent(struct indented_printer *ip, const char *s, size_t len) {
  size_t count;
  count = fwrite(s, 1, len, ip->outfp_);
  if (count != len) {
    int err = errno;
    if (!ip->had_error_) re_error_nowhere("Failed to write to \"%s\": %s", ip->filename_, strerror(errno));
    ip->had_error_ = 1;
  }
  if (len && (s[len - 1] == '\n')) ip->at_start_of_line_ = 1;
}

void ip_puts_no_indent(struct indented_printer *ip, const char *s) {
  ip_write_no_indent(ip, s, strlen(s));
}

void ip_puts(struct indented_printer *ip, const char *s) {
  while (*s) {
    const char *start_of_line = s;
    int opening_cubraces = 0, closing_cubraces = 0;
    const char *p = s;
    while ((*p != '\n') && (*p != '\0')) {
      if (*p == '{') {
        opening_cubraces++;
      }
      if (*p == '}') {
        closing_cubraces++;
      }
      p++;
    }
    const char *end_of_line = p;

    if (closing_cubraces > opening_cubraces) {
      /* Take reduction in indentation before printing the line */
      ip->indentation_ += (opening_cubraces - closing_cubraces) * ip->indent_size_;
    }

    if (ip->at_start_of_line_) {
      while (*s == ' ') s++;
      if (0 > fprintf(ip->outfp_, "%*s", ip->indentation_, "")) {
        int err = errno;
        if (!ip->had_error_) re_error_nowhere("Failed to write to \"%s\": %s", ip->filename_, strerror(errno));
        ip->had_error_ = 1;
      }
      ip->at_start_of_line_ = 0;
    }

    if (closing_cubraces < opening_cubraces) {
      /* Take increase in indentation after printing the line */
      ip->indentation_ += (opening_cubraces - closing_cubraces) * ip->indent_size_;
    }

    size_t count;
    size_t len;
    if (*end_of_line == '\n') {
      /* include newline with end of line */
      end_of_line++;
      ip->at_start_of_line_ = 1; /* next text starts a new line */
    }
    len = end_of_line - s;
    count = fwrite(s, 1, len, ip->outfp_);
    if (count != len) {
      int err = errno;
      if (!ip->had_error_) re_error_nowhere("Failed to write to \"%s\": %s", ip->filename_, strerror(errno));
      ip->had_error_ = 1;
    }

    s = end_of_line;
  }
}

void ip_force_indent_print(struct indented_printer *ip) {
  if (0 > fprintf(ip->outfp_, "%*s", ip->indentation_, "")) {
    int err = errno;
    if (!ip->had_error_) re_error_nowhere("Failed to write to \"%s\": %s", ip->filename_, strerror(errno));
    ip->had_error_ = 1;
  }
  ip->at_start_of_line_ = 0;
}

#ifdef _MSC_VER
void ip_vprintf(struct indented_printer *ip, const char *format, va_list args) {
  size_t len = _vscprintf(format, args);
  char *p = (char *)malloc(len + 1);
  if (!p) {
    re_error_nowhere("Error, no memory");
    ip->had_error_ = 1;
    return ;
  }
  vsprintf(p, format, args);
  ip_puts(ip, p);
  free(p);
}
void ip_vprintf_no_indent(struct indented_printer *ip, const char *format, va_list args) {
  size_t len = _vscprintf(format, args);
  char *p = (char *)malloc(len + 1);
  if (!p) {
    re_error_nowhere("Error, no memory");
    ip->had_error_ = 1;
    return ;
  }
  vsprintf(p, format, args);
  ip_puts_no_indent(ip, p);
  free(p);
}
#else /* GCC/clang */
void ip_vprintf(struct indented_printer *ip, const char *format, va_list args) {
  char *p;
  if (-1 == vasprintf(&p, format, args)) {
    re_error_nowhere("Error, no memory");
    ip->had_error_ = 1;
    return ;
  }
  ip_puts(ip, p);
  free(p);
}
void ip_vprintf_no_indent(struct indented_printer *ip, const char *format, va_list args) {
  char *p;
  if (-1 == vasprintf(&p, format, args)) {
    re_error_nowhere("Error, no memory");
    ip->had_error_ = 1;
    return ;
  }
  ip_puts_no_indent(ip, p);
  free(p);
}
#endif

void ip_printf(struct indented_printer *ip, const char *format, ...) {
  va_list args;
  va_start(args, format);
  ip_vprintf(ip, format, args);
  va_end(args);
}

void ip_printf_no_indent(struct indented_printer *ip, const char *format, ...) {
  va_list args;
  va_start(args, format);
  ip_vprintf_no_indent(ip, format, args);
  va_end(args);
}
