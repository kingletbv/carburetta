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

#ifndef KLT_LOGGER2_H
#define KLT_LOGGER2_H

#ifndef STDARG_H_INCLUDED
#define STDARG_H_INCLUDED
#include <stdarg.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define KLT_LOG_NODE_FLAG_DEFINITIONS_MASK 3
#define KLT_LOG_NODE_FLAG_FUNCTION_DEFINITION 1
#define KLT_LOG_NODE_FLAG_ENABLED_DEFINITION 2

#define KLT_LOG_NODE_FLAG_ENABLED 128

#define KLT_LOG_ERROR_LEVEL 1
#define KLT_LOG_LOG_LEVEL 2
#define KLT_LOG_TRACE_LEVEL 3

struct klt_log_node;

typedef void (*klt_log_fn)(struct klt_log_node *ln, void *baton, int level, const char *file, int line, const char *fmt, va_list args);

struct klt_log_anchor {
  /* Compile-time, statically allocated name of the module as a full path. */
  const char *full_path_initializer_;

  /* Node at which this anchor exists (logging for the anchor occurs on this node) */
  struct klt_log_node *node_;
  struct klt_log_anchor *next_in_node_, *prev_in_node_;
};

struct klt_log_node {
  /* Node name of the module */
  char *name_;

  /* Navigation through the logger hierarchy.. */
  struct klt_log_node *parent_;
  struct klt_log_node *first_child_;
  struct klt_log_node *next_sibling_, *prev_sibling_;

  /* Flags field, the flags_carried_ field is used at runtime to
   * propagate attributes down the hierarchy. */
  int flags_, flags_carried_;

  /* Pointer to the log function used by this log node if it is enabled.
   * If this pointer is NULL, the log function is inherited from its parent. */
  klt_log_fn log_fn_ptr_;
  void *log_fn_baton_;

  /* Chain of anchors at this node, each of which may be either a source for logging or a destination
   * for logging. */
  struct klt_log_anchor *first_anchor_;
};

extern void *g_log_global_lock_;
extern struct klt_log_node *g_log_root_;
extern struct klt_log_anchor g_log_root_anchor_;

#ifndef KLT_LOG_MODULE
#define KLT_LOGGER_TO_USE g_log_root_anchor_
#else
#define KLT_LOGGER_TO_USE g_log_module_anchor
static struct klt_log_anchor g_log_module_anchor = {
  KLT_LOG_MODULE,
  NULL, NULL, NULL
};
#endif

#define LOGERROR(...) \
  klt_log(&KLT_LOGGER_TO_USE, KLT_LOG_ERROR_LEVEL, __FILE__, __LINE__, __VA_ARGS__);

#define LOG(...) \
  klt_log(&KLT_LOGGER_TO_USE, KLT_LOG_LOG_LEVEL, __FILE__, __LINE__, __VA_ARGS__);

#define TRACE(...) \
  klt_log(&KLT_LOGGER_TO_USE, KLT_LOG_TRACE_LEVEL, __FILE__, __LINE__, __VA_ARGS__);


void klt_logger_init(void);
void klt_logger_cleanup(void);

void klt_log_node_init(struct klt_log_node *node);
void klt_log_node_cleanup(struct klt_log_node *node);

void klt_log_set_function(const char *module, klt_log_fn fn, void *fn_baton);
void klt_log_clear_function(const char *module);

void klt_log_set_module_enable(const char *module, int enable);
void klt_log_clear_module_enable(const char *module);

klt_log_fn klt_log_set_current_thread_intercept(klt_log_fn fn, void *baton, void **original_baton);
klt_log_fn klt_log_clear_current_thread_intercept(void **original_baton);

void klt_log(struct klt_log_anchor *origin_anchor, int level, const char *file, int line, const char *fmt, ...);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* KLT_LOGGER2_H */
