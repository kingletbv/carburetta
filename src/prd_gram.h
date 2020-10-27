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

#ifndef PRD_GRAM_H
#define PRD_GRAM_H

#ifndef STDDEF_H_INCLUDED
#define STDDEF_H_INCLUDED
#include <stddef.h> /* size_t */
#endif

#ifndef TOKENS_H_INCLUDED
#define TOKENS_H_INCLUDED
#include "tokens.h"
#endif

#ifndef XLTS_H_INCLUDED
#define XLTS_H_INCLUDED
#include "xlts.h"
#endif

#ifndef SYMBOL_H_INCLUDED
#define SYMBOL_H_INCLUDED
#include "symbol.h"
#endif

#ifndef TOKENS_H_INCLUDED
#define TOKENS_H_INCLUDED
#include "tokens.h"
#endif

#ifndef SNIPPET_H_INCLUDED
#define SNIPPET_H_INCLUDED
#include "snippet.h"
#endif

#ifndef PRD_GRAMMAR_H_INCLUDED
#define PRD_GRAMMAR_H_INCLUDED
#include "prd_grammar_alt.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define PRD_SUCCESS 0
#define PRD_SYNTAX_ERROR 1
#define PRD_NEXT 2
#define PRD_INTERNAL_ERROR 3

struct prd_production_sym {
  /* Resolved symbol for this part of a production */
  struct symbol *sym_;

  /* Original keyword as it appeared in the sentence
   * (Location is relevant, may also have - or _ and upper/lowercase differences */
  struct xlts id_;
};

struct prd_production {
  /* Non-terminal the production reduces to */
  struct prd_production_sym nt_;

  /* Sentence that defines the production */
  size_t num_syms_;
  size_t num_syms_allocated_;
  struct prd_production_sym *syms_;

  /* Action sequence common with other productions */
  struct snippet common_action_sequence_;

  /* Action sequence that executes when the production matches */
  struct snippet action_sequence_;
};

struct prd_pattern {
  /* Terminal token the pattern matches */
  struct prd_production_sym term_;

  /* Pattern that produces the production */
  char *regex_;

  /* Action sequence that executes when the pattern matches */
  struct snippet action_sequence_;
};

struct prd_grammar {
  int have_errors_ : 1; /* if non-zero, errors were issued and compilation failed */
  int accept_whitespace_ : 1; /* if non-zero, whitespace is passed on to the grammar as PRD_TOKEN */

  /* Attached to any production being added during parsing */
  struct snippet current_common_action_sequence_;

  /* Productions making up the entire grammar */
  size_t num_productions_;
  size_t num_productions_allocated_;
  struct prd_production *productions_;

  /* Patterns making up the entire scanner */
  size_t num_patterns_;
  size_t num_patterns_allocated_;
  struct prd_pattern *patterns_;
};

int prd_parse_tkr(struct prd_stack *stack, struct prd_grammar *g, struct tkr_tokenizer *tkr, int end_of_input, struct symbol_table *st);
int rxg_parse_tkr(struct rxg_stack *stack, struct prd_grammar *g, struct tkr_tokenizer *tkr, int end_of_input, struct symbol_table *st);

void prd_production_sym_init(struct prd_production_sym *pps);
void prd_production_sym_cleanup(struct prd_production_sym *pps);
void prd_prod_init(struct prd_production *ppd);
void prd_prod_reset(struct prd_production *ppd);
void prd_prod_cleanup(struct prd_production *ppd);
void prd_pattern_init(struct prd_pattern *pat);
void prd_pattern_cleanup(struct prd_pattern *pat);
void prd_grammar_init(struct prd_grammar *g);
void prd_grammar_cleanup(struct prd_grammar *g);
void prd_prod_swap(struct prd_production *a, struct prd_production *b);
int prd_grammar_check_production_reserve(struct prd_grammar *g);
int prd_grammar_check_pattern_reserve(struct prd_grammar *g);
int prd_prod_check_sym_reserve(struct prd_production *pd, struct xlts *loc);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* PRD_GRAM_H */
