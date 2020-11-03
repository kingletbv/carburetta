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

#ifndef CARBURETTA_CONTEXT_H
#define CARBURETTA_CONTEXT_H

#ifndef SYMBOL_H_INCLUDED
#define SYMBOL_H_INCLUDED
#include "symbol.h"
#endif

#ifndef TYPESTR_H_INCLUDED
#define TYPESTR_H_INCLUDED
#include "typestr.h"
#endif

#ifndef PRD_GRAM_H_INCLUDED
#define PRD_GRAM_H_INCLUDED
#include "prd_gram.h"
#endif

#ifndef SNIPPET_H_INCLUDED
#define SNIPPET_H_INCLUDED
#include "snippet.h"
#endif

#ifndef XLTS_H_INCLUDED
#define XLTS_H_INCLUDED
#include "xlts.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct conflict_resolution {
  struct conflict_resolution *next_;

  struct prd_production prefer_prod_;
  int prefer_prod_place_;
  struct prd_production over_prod_;
  int over_prod_place_;
};

struct carburetta_context {
  struct snippet token_type_;
  struct snippet common_data_type_;
  struct symbol_table symtab_;
  struct typestr_table tstab_;
  struct typestr *most_recent_typestr_;
  struct typestr *token_assigned_type_;
  struct typestr *common_data_assigned_type_;
  struct xlts prefix_;
  char *prefix_uppercase_;
  struct xlts token_prefix_;
  char *token_prefix_uppercase_;
  struct snippet params_snippet_;
  struct snippet locals_snippet_;
  struct snippet on_success_snippet_;
  struct snippet on_syntax_error_snippet_;
  struct snippet on_alloc_error_snippet_;
  struct snippet on_internal_error_snippet_;
  struct snippet on_next_token_snippet_;
  struct symbol *input_end_sym_;
  struct symbol *error_sym_;
  struct prd_production prefer_prod_;
  int prefer_prod_place_;
  struct prd_production over_prod_;
  int over_prod_place_;
  struct conflict_resolution *conflict_resolutions_;
  int have_typed_symbols_ :1;
  char *h_output_filename_;
  char *c_output_filename_;
  char *include_guard_;
};

void carburetta_context_init(struct carburetta_context *cc);
void carburetta_context_cleanup(struct carburetta_context *cc);

void conflict_resolution_init(struct conflict_resolution *cr);
void conflict_resolution_cleanup(struct conflict_resolution *cr);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* CARBURETTA_CONTEXT_H */
