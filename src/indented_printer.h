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

struct indented_printer {
  FILE *outfp_;
  const char *filename_;
  int indentation_;
  int indent_size_;
  int had_error_:1;
  int at_start_of_line_:1;
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


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* INDENTED_PRINTER_H */
