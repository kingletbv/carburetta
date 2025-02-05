/* Copyright 2023-2025 Kinglet B.V.
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

#ifndef STDLIB_H_INCLUDED
#define STDLIB_H_INCLUDED
#include <stdlib.h>
#endif

#ifndef STDIO_H_INCLUDED
#define STDIO_H_INCLUDED
#include <stdio.h>
#endif

#ifndef STRING_H_INCLUDED
#define STRING_H_INCLUDED
#include <string.h>
#endif

#ifndef STDINT_H_INCLUDED
#define STDINT_H_INCLUDED
#include <stdint.h>
#endif

#ifndef C_COMPILER_H_INCLUDED
#define C_COMPILER_H_INCLUDED
#include "c_compiler.h"
#endif

#ifndef DECL_H_INCLUDED
#define DECL_H_INCLUDED
#include "decl.h"
#endif

#ifndef DYNAMIC_RUNTIME_LINKING_H_INCLUDED
#define DYNAMIC_RUNTIME_LINKING_H_INCLUDED
#include "dynamic_runtime_linking.h"
#endif

static struct c_compiler *g_cc_handle_for_printf_ = NULL;

static int drl_printf(const char *fmt, ...) {
  int r;
  va_list args;
  if (!g_cc_handle_for_printf_) return -1;
  va_start(args, fmt);
  r = g_cc_handle_for_printf_->vprintf_handler(g_cc_handle_for_printf_->vprintf_baton_, NULL, 0, fmt, args);
  va_end(args);
  return r;
}

int drl_resolve_sym(struct c_compiler *cc, struct decl *d, void **ptr) {
  *ptr = NULL;

  struct cc_link_name *name = NULL;
  name = (struct cc_link_name *)st_find(&cc->link_table_, d->sym_.ident_);
  if (name) {
    *ptr = name->ptr_;
    return 0;
  }

  if (!strcmp("printf", d->sym_.ident_)) {
    /* XXX: Not thread-safe */
    g_cc_handle_for_printf_ = cc;
    *ptr = (void *)drl_printf;
    return 0;
  }
  
  return -1;
}

int drl_register_sym(struct c_compiler *cc, const char *link_name, void *ptr) {
  struct cc_link_name *name = NULL;
  int is_new = 0;
  name = (struct cc_link_name *)st_find_or_insert(&cc->link_table_, link_name, &is_new, sizeof(*name));
  if (!name) return -1;
  /* It's fine to overwrite an existing pointer value with a different one (no need to check is_new) */
  name->ptr_ = ptr;
  return 0;
}
