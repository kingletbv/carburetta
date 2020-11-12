/* Copyright 2020 Kinglet B.V.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http ://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef INDENTED_PRINTER_H
#define INDENTED_PRINTER_H

#ifndef STDIO_H_INCLUDED
#define STDIO_H_INCLUDED
#include <stdio.h>
#endif

#ifndef STDARG_H_INCLUDED
#define STDARG_H_INCLUDED
#include <stdarg.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct ip_retained_output_bucket {
  /* Size of struct allocation includes data in buf_ with null terminator (eg. buf_size_ + 1) */
  struct ip_retained_output_bucket *next_;
  size_t buf_size_;
  char buf_[1];
};

struct indented_printer {
  FILE *outfp_;
  const char *filename_;
  int indentation_;
  int indent_size_;
  int had_error_:1;
  int at_start_of_line_:1;
  int retain_output_:1;
  struct ip_retained_output_bucket *retained_output_; /* tail ptr */
};

void ip_init(struct indented_printer *ip, FILE *outfp, const char *filename);
void ip_cleanup(struct indented_printer *ip);
void ip_write_no_indent(struct indented_printer *ip, const char *s, size_t len);
void ip_puts_no_indent(struct indented_printer *ip, const char *s);
void ip_puts(struct indented_printer *ip, const char *s);
void ip_force_indent_print(struct indented_printer *ip);
void ip_vprintf(struct indented_printer *ip, const char *format, va_list args);
void ip_vprintf_no_indent(struct indented_printer *ip, const char *format, va_list args);
void ip_printf(struct indented_printer *ip, const char *format, ...);
void ip_printf_no_indent(struct indented_printer *ip, const char *format, ...);

void ip_free_retained_output_bucket_chain(struct ip_retained_output_bucket *robc);
void ip_write_retained_output_bucket_chain(struct indented_printer *ip, struct ip_retained_output_bucket *robc);
void ip_set_retained_output(struct indented_printer *ip, int enable);
struct ip_retained_output_bucket *ip_extract_retained_output(struct indented_printer *ip);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* INDENTED_PRINTER_H */
