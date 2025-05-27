/* Copyright 2020-2025 Kinglet B.V.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

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

#ifndef ERRNO_H_INCLUDED
#define ERRNO_H_INCLUDED
#include <errno.h>
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
  ip->retain_output_ = 0;
  ip->current_line_num_ = 1;
  ip->retained_output_ = NULL;
}

void ip_cleanup(struct indented_printer *ip) {
  ip_free_retained_output_bucket_chain(ip->retained_output_);
}

void ip_free_retained_output_bucket_chain(struct ip_retained_output_bucket *robc) {
  struct ip_retained_output_bucket *b;
  b = robc;
  if (b) {
    struct ip_retained_output_bucket *next;
    next = b->next_;
    do {
      b = next;
      next = b->next_;
      free(b);
    } while (b != robc);
  }
}

void ip_write_retained_output_bucket_chain(struct indented_printer *ip, struct ip_retained_output_bucket *robc) {
  struct ip_retained_output_bucket *b;
  b = robc;
  if (b) {
    do {
      b = b->next_;

      if (b->kind_ == IPROBT_BUFFER) {
        ip_write_no_indent(ip, b->buf_, b->buf_size_);
      }
      else {
        if (0 > fprintf(ip->outfp_, "%d", ip->current_line_num_ + 1)) {
          int err = errno;
          if (!ip->had_error_) re_error_nowhere("Failed to write to \"%s\": %s", ip->filename_, strerror(err));
          ip->had_error_ = 1;
        }
      }

    } while (b != robc);
  }
}

void ip_set_retained_output(struct indented_printer *ip, int enable) {
  if (enable && !ip->retain_output_) {
    ip->retained_line_num_ = ip->current_line_num_;
  }
  else if (!enable && ip->retain_output_) {
    ip->current_line_num_ = ip->retained_line_num_;
  }
  ip->retain_output_ = !!enable;
}

struct ip_retained_output_bucket *ip_extract_retained_output(struct indented_printer *ip) {
  struct ip_retained_output_bucket *rob = ip->retained_output_;
  ip->retained_output_ = NULL;
  return rob;
}

static struct ip_retained_output_bucket *ip_dst_alloc_bucket(struct indented_printer *ip, size_t num_bytes) {
  struct ip_retained_output_bucket *b;
  /* Allocate enough size for the bucket struct and its variable length buf_ member.
   * Note that buf_ already has size 1, which corresponds to the size needed for the
   * null terminator, plus num_bytes for the content = size of allocation. */
  b = (struct ip_retained_output_bucket *)malloc(sizeof(struct ip_retained_output_bucket) + num_bytes);
  if (!b) {
    errno = ENOMEM;
    return NULL;
  }
  b->kind_ = IPROBT_BUFFER;
  if (ip->retained_output_) {
    b->next_ = ip->retained_output_->next_;
    ip->retained_output_->next_ = b;
    ip->retained_output_ = b;
  }
  else {
    b->next_ = b;
    ip->retained_output_ = b;
  }
  b->buf_size_ = num_bytes;
  return b;
}

static struct ip_retained_output_bucket *ip_dst_alloc_next_line_num_bucket(struct indented_printer *ip) {
  struct ip_retained_output_bucket *b;
  b = (struct ip_retained_output_bucket *)malloc(sizeof(struct ip_retained_output_bucket));
  if (!b) {
    errno = ENOMEM;
    return NULL;
  }
  b->kind_ = IPROBT_LINE_NUM_PLUS_1;
  if (ip->retained_output_) {
    b->next_ = ip->retained_output_->next_;
    ip->retained_output_->next_ = b;
    ip->retained_output_ = b;
  }
  else {
    b->next_ = b;
    ip->retained_output_ = b;
  }
  b->buf_size_ = 0;
  return b;
}



static int ip_dst_write(struct indented_printer *ip, const void *bytes, size_t num_bytes) {
  if (ip->retain_output_) {
    struct ip_retained_output_bucket *b;
    b = ip_dst_alloc_bucket(ip, num_bytes);
    if (!b) return errno;
    memcpy(b->buf_, bytes, num_bytes);
    b->buf_[num_bytes] = '\0';
    return 0;
  }
  size_t count;
  count = fwrite(bytes, 1, num_bytes, ip->outfp_);
  if (count != num_bytes) {
    return errno;
  }
  return 0;
}

static int ip_dst_write_spaces(struct indented_printer *ip, int num_spaces) {
  if (ip->retained_output_) {
    struct ip_retained_output_bucket *b;
    b = ip_dst_alloc_bucket(ip, num_spaces);
    if (!b) return errno;
    memset(b->buf_, ' ', num_spaces);
    b->buf_[num_spaces] = '\0';
    return 0;
  }
  if (0 > fprintf(ip->outfp_, "%*s", num_spaces, "")) {
    return errno;
  }
  return 0;
}

void ip_write_next_line_num(struct indented_printer *ip) {
  if (ip->retain_output_) {
    struct ip_retained_output_bucket *b;
    b = ip_dst_alloc_next_line_num_bucket(ip);
    if (!b) {
      if (!ip->had_error_) re_error_nowhere("Failed to write to \"%s\": %s", ip->filename_, strerror(ENOMEM));
      ip->had_error_ = 1;
    }
  }
  else if (0 > fprintf(ip->outfp_, "%d", ip->current_line_num_ + 1)) {
    int err = errno;
    if (err) {
      if (!ip->had_error_) re_error_nowhere("Failed to write to \"%s\": %s", ip->filename_, strerror(err));
      ip->had_error_ = 1;
    }
  }
}

void ip_write_no_indent(struct indented_printer *ip, const char *s, size_t len) {
  if (ip_dst_write(ip, s, len)) {
    int err = errno;
    if (!ip->had_error_) re_error_nowhere("Failed to write to \"%s\": %s", ip->filename_, strerror(err));
    ip->had_error_ = 1;
  }
  size_t n;
  for (n = 0; n < len; ++n) {
    if (s[n] == '\n') ip->current_line_num_++;
  }
  if (len && (s[len - 1] == '\n')) ip->at_start_of_line_ = 1;
}

void ip_puts_no_indent(struct indented_printer *ip, const char *s) {
  ip_write_no_indent(ip, s, strlen(s));
}

void ip_puts(struct indented_printer *ip, const char *s) {
  while (*s) {
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
      if (ip_dst_write_spaces(ip, ip->indentation_)) {
        int err = errno;
        if (!ip->had_error_) re_error_nowhere("Failed to write to \"%s\": %s", ip->filename_, strerror(err));
        ip->had_error_ = 1;
      }
      ip->at_start_of_line_ = 0;
    }

    if (closing_cubraces < opening_cubraces) {
      /* Take increase in indentation after printing the line */
      ip->indentation_ += (opening_cubraces - closing_cubraces) * ip->indent_size_;
    }

    size_t len;
    if (*end_of_line == '\n') {
      /* include newline with end of line */
      end_of_line++;
      ip->at_start_of_line_ = 1; /* next text starts a new line */
      ip->current_line_num_++;
    }
    len = end_of_line - s;
    if (ip_dst_write(ip, s, len)) {
      int err = errno;
      if (!ip->had_error_) re_error_nowhere("Failed to write to \"%s\": %s", ip->filename_, strerror(err));
      ip->had_error_ = 1;
    }

    s = end_of_line;
  }
}

void ip_force_indent_print(struct indented_printer *ip) {
  if (ip_dst_write_spaces(ip, ip->indentation_)) {
    int err = errno;
    if (!ip->had_error_) re_error_nowhere("Failed to write to \"%s\": %s", ip->filename_, strerror(err));
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
