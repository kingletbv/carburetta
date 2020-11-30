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


#ifndef STDLIB_H_INCLUDED
#define STDLIB_H_INCLUDED
#include <stdlib.h>
#endif

#ifndef STRING_H_INCLUDED
#define STRING_H_INCLUDED
#include <string.h>
#endif

#ifndef CARBURETTA_CONTEXT_H_INCLUDED
#define CARBURETTA_CONTEXT_H_INCLUDED
#include "carburetta_context.h"
#endif

static void parts_free(struct part *parts);

void carburetta_context_init(struct carburetta_context *cc) {
  snippet_init(&cc->token_type_);
  snippet_init(&cc->common_data_type_);
  symbol_table_init(&cc->symtab_);
  typestr_table_init(&cc->tstab_);
  cc->most_recent_typestr_ = NULL;
  cc->token_assigned_type_ = NULL;
  cc->common_data_assigned_type_ = NULL;
  xlts_init(&cc->prefix_);
  cc->prefix_uppercase_ = NULL;
  xlts_init(&cc->token_prefix_);
  cc->token_prefix_uppercase_ = NULL;
  snippet_init(&cc->params_snippet_);
  snippet_init(&cc->locals_snippet_);
  snippet_init(&cc->on_success_snippet_);
  snippet_init(&cc->on_syntax_error_snippet_);
  snippet_init(&cc->on_lexical_error_snippet_);
  snippet_init(&cc->on_alloc_error_snippet_);
  snippet_init(&cc->on_internal_error_snippet_);
  snippet_init(&cc->on_next_token_snippet_);
  snippet_init(&cc->on_feed_me_snippet_);
  cc->input_end_sym_ = NULL;
  cc->error_sym_ = NULL;
  prd_prod_init(&cc->prefer_prod_);
  cc->prefer_prod_place_ = -1;
  prd_prod_init(&cc->over_prod_);
  cc->over_prod_place_ = -1;
  cc->conflict_resolutions_ = NULL;
  cc->have_typed_symbols_ = 0;
  cc->h_output_filename_ = NULL;
  cc->c_output_filename_ = NULL;
  cc->include_guard_ = NULL;
  cc->prologue_ = NULL;
  cc->epilogue_ = NULL;
  cc->current_snippet_continuation_ = 0;
  cc->continuation_enabled_ = 1;
}

void carburetta_context_cleanup(struct carburetta_context *cc) {
  snippet_cleanup(&cc->token_type_);
  snippet_cleanup(&cc->common_data_type_);
  symbol_table_cleanup(&cc->symtab_);
  typestr_table_cleanup(&cc->tstab_);
  xlts_cleanup(&cc->prefix_);
  if (cc->prefix_uppercase_) free(cc->prefix_uppercase_);
  xlts_cleanup(&cc->token_prefix_);
  if (cc->token_prefix_uppercase_) {
    free(cc->token_prefix_uppercase_);
  }
  snippet_cleanup(&cc->params_snippet_);
  snippet_cleanup(&cc->locals_snippet_);
  snippet_cleanup(&cc->on_success_snippet_);
  snippet_cleanup(&cc->on_syntax_error_snippet_);
  snippet_cleanup(&cc->on_lexical_error_snippet_);
  snippet_cleanup(&cc->on_alloc_error_snippet_);
  snippet_cleanup(&cc->on_internal_error_snippet_);
  snippet_cleanup(&cc->on_next_token_snippet_);
  snippet_cleanup(&cc->on_feed_me_snippet_);
  prd_prod_cleanup(&cc->prefer_prod_);
  prd_prod_cleanup(&cc->over_prod_);
  struct conflict_resolution *cr, *next;
  cr = cc->conflict_resolutions_;
  if (cr) {
    next = cr->next_;
    do {
      cr = next;
      next = cr->next_;

      conflict_resolution_cleanup(cr);
      free(cr);

    } while (cr != cc->conflict_resolutions_);
  }
  if (cc->c_output_filename_) free(cc->c_output_filename_);
  if (cc->h_output_filename_) free(cc->h_output_filename_);
  if (cc->include_guard_) free(cc->include_guard_);
  parts_free(cc->prologue_);
  parts_free(cc->epilogue_);
}

void conflict_resolution_init(struct conflict_resolution *cr) {
  prd_prod_init(&cr->prefer_prod_);
  cr->prefer_prod_place_ = -1;
  prd_prod_init(&cr->over_prod_);
  cr->over_prod_place_ = -1;
}

void conflict_resolution_cleanup(struct conflict_resolution *cr) {
  prd_prod_cleanup(&cr->prefer_prod_);
  prd_prod_cleanup(&cr->over_prod_);
}

static void parts_free(struct part *parts) {
  struct part *p = parts;
  struct part *next;
  if (p) {
    next = p->next_;
    do {
      p = next;
      next = p->next_;

      if (p->chars_) free(p->chars_);
      free(p);

    } while (p != parts);
  }
}

struct part *parts_append(struct part **tailptr, size_t num_chars, char *chars) {
  struct part *p = (struct part *)malloc(sizeof(struct part));
  if (!p) return NULL;
  p->chars_ = (char *)malloc(num_chars + 1);
  if (!p->chars_) {
    free(p);
    return NULL;
  }
  memcpy(p->chars_, chars, num_chars);
  p->chars_[num_chars] = '\0';
  p->num_chars_ = num_chars;
  if (*tailptr) {
    p->next_ = (*tailptr)->next_;
    (*tailptr)->next_ = p;
    *tailptr = p;
  }
  else {
    *tailptr = p;
    p->next_ = p;
  }

  return p;
}
