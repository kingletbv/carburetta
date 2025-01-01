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

#ifndef STDIO_H_INCLUDED
#define STDIO_H_INCLUDED
#include <stdio.h>
#endif

#ifndef STDLIB_H_INCLUDED
#define STDLIB_H_INCLUDED
#include <stdlib.h>
#endif

#ifndef STDINT_H_INCLUDED
#define STDINT_H_INCLUDED
#include <stdint.h>
#endif

#ifndef SNIPPET_H_INCLUDED
#define SNIPPET_H_INCLUDED
#include "snippet.h"
#endif

#ifndef REPORT_ERROR_H_INCLUDED
#define REPORT_ERROR_H_INCLUDED
#include "report_error.h"
#endif

#ifndef TYPESTR_H_INCLUDED
#define TYPESTR_H_INCLUDED
#include "typestr.h"
#endif

static void typestr_init(struct typestr *ts);
static void typestr_cleanup(struct typestr *ts);

void typestr_table_init(struct typestr_table *tt) {
  tt->num_typestrs_ = tt->num_typestrs_allocated_ = 0;
  tt->typestrs_ = NULL;
}

void typestr_table_cleanup(struct typestr_table *tt) {
  size_t n;
  for (n = 0; n < tt->num_typestrs_; ++n) {
    struct typestr *ts = tt->typestrs_[n];

    typestr_cleanup(ts);
    free(ts);
  }

  if (tt->typestrs_) free(tt->typestrs_);
}

static void typestr_init(struct typestr *ts) {
  ts->is_symbol_type_ = 0;
  ts->is_raii_constructor_ = 0;
  snippet_init(&ts->typestr_snippet_);
  ts->ordinal_ = 0;
  snippet_init(&ts->constructor_snippet_);
  snippet_init(&ts->move_snippet_);
  snippet_init(&ts->destructor_snippet_);
  snippet_init(&ts->token_action_snippet_);
}

static void typestr_cleanup(struct typestr *ts) {
  snippet_cleanup(&ts->typestr_snippet_);
  snippet_cleanup(&ts->constructor_snippet_);
  snippet_cleanup(&ts->destructor_snippet_);
  snippet_cleanup(&ts->token_action_snippet_);
}

struct typestr *typestr_add(struct typestr_table *tt, const struct snippet *typestr_snippet) {
  /* Add new */
  if (tt->num_typestrs_ == tt->num_typestrs_allocated_) {
    size_t new_num = tt->num_typestrs_allocated_ * 2 + 1;
    if (new_num <= tt->num_typestrs_allocated_) {
      re_error_nowhere("Error, overflow");
      return NULL;
    }
    if (new_num > (SIZE_MAX / sizeof(struct typestr *))) {
      re_error_nowhere("Error, overflow");
      return NULL;
    }
    size_t alloc_size = new_num * sizeof(struct typestr *);
    void *p = realloc(tt->typestrs_, alloc_size);
    if (!p) {
      re_error_nowhere("Error, no memory");
      return NULL;
    }
    tt->typestrs_ = (struct typestr **)p;
    tt->num_typestrs_allocated_ = new_num;
  }
  struct typestr *ts;
  ts = (struct typestr *)malloc(sizeof(struct typestr));
  if (!ts) {
    re_error_nowhere("Error, no memory");
    return NULL;
  }
  typestr_init(ts);
  tt->typestrs_[tt->num_typestrs_] = ts;
  ts->ordinal_ = (int)tt->num_typestrs_++;

  int r;
  r = snippet_append_snippet(&ts->typestr_snippet_, typestr_snippet);
  if (r) {
    typestr_cleanup(ts);
    free(ts);
    return NULL;
  }

  return ts;
}
