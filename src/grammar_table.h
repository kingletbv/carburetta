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

#ifndef GRAMMAR_TABLE_H
#define GRAMMAR_TABLE_H

#ifndef STDDEF_H_INCLUDED
#define STDDEF_H_INCLUDED
#include <stddef.h> /* size_t */
#endif

#ifndef LALR_H_INCLUDED
#define LALR_H_INCLUDED
#include "lalr.h"
#endif

#ifndef PRD_GRAM_H_INCLUDED
#define PRD_GRAM_H_INCLUDED
#include "prd_gram.h"
#endif

#ifndef SYMBOL_H_INCLUDED
#define SYMBOL_H_INCLUDED
#include "symbol.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define GT_OVERFLOW 1
#define GT_NOMEM 2
#define GT_NOT_LR_GRAMMAR 3
#define GT_AMBIGUOUS_GRAMMAR 4
#define GT_CONFLICTS 5
#define GT_INTERNAL_ERROR 6

struct grammar_table {
  size_t num_ordinals_;
  size_t num_ordinals_allocated_;
  int *ordinals_;
};

void gt_grammar_table_init(struct grammar_table *gt);
void gt_grammar_table_cleanup(struct grammar_table *gt);

int gt_emit(struct grammar_table *gt, int ordinal);

int gt_transcribe_grammar(struct grammar_table *gt, size_t num_productions, struct prd_production *productions, int end_of_production_sym, int end_of_grammar_sym);
void gt_debug_grammar(struct grammar_table *gt, size_t num_productions, struct prd_production *productions, int end_of_production_sym, int end_of_grammar_sym);
int gt_generate_lalr(struct grammar_table *gt, struct lr_generator *lalr, int end_of_production_sym, int end_of_grammar_sym, int end_of_file_sym, int synthetic_s_sym);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* GRAMMAR_TABLE_H */
