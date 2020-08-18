#ifndef LOG_FUNCTION_H
#define LOG_FUNCTION_H

#ifndef STDARG_H_INCLUDED
#define STDARG_H_INCLUDED
#include <stdarg.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct klt_log_node;

void log_function(struct klt_log_node *ln, void *baton, int level, const char *file, int line, const char *fmt, va_list args);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LOG_FUNCTION_H */
