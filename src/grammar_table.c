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

#ifndef STDLIB_H_INCLUDED
#define STDLIB_H_INCLUDED
#include <stdlib.h>
#endif

#ifndef STDINT_H_INCLUDED
#define STDINT_H_INCLUDED
#include <stdint.h>
#endif

#ifndef ASSERT_H_INCLUDED
#define ASSERT_H_INCLUDED
#include <assert.h>
#endif


#ifndef REPORT_ERROR_H_INCLUDED
#define REPORT_ERROR_H_INCLUDED
#include "report_error.h"
#endif

#ifndef GRAMMAR_TABLE_H_INCLUDED
#define GRAMMAR_TABLE_H_INCLUDED
#include "grammar_table.h"
#endif

void gt_grammar_table_init(struct grammar_table *gt) {
  gt->num_ordinals_ = gt->num_ordinals_allocated_ = 0;
  gt->ordinals_ = NULL;
}

void gt_grammar_table_cleanup(struct grammar_table *gt) {
  if (gt->ordinals_) free(gt->ordinals_);
}

int gt_emit(struct grammar_table *gt, int ordinal) {
  if (gt->num_ordinals_ == gt->num_ordinals_allocated_) {
    size_t new_num_allocated = gt->num_ordinals_allocated_ * 2 + 1;
    if (new_num_allocated <= gt->num_ordinals_allocated_) {
      re_error_nowhere("Error, overflow during allocation\n");
      return GT_OVERFLOW;
    }
    if (new_num_allocated > (SIZE_MAX / sizeof(int))) {
      re_error_nowhere("Error, overflow during allocation\n");
      return GT_OVERFLOW;
    }
    size_t size_to_allocate = new_num_allocated * sizeof(int);
    void *p = realloc(gt->ordinals_, size_to_allocate);
    if (!p) {
      re_error_nowhere("Error, no memory\n");
      return GT_NOMEM;
    }
    gt->ordinals_ = (int *)p;
    gt->num_ordinals_allocated_ = new_num_allocated;
  }
  gt->ordinals_[gt->num_ordinals_++] = ordinal;
  return 0;
}

int gt_transcribe_grammar(struct grammar_table *gt, size_t num_productions, struct prd_production *productions, int end_of_production_sym, int end_of_grammar_sym) {
  int r;
  size_t prod_idx;
  gt->num_ordinals_ = 0;
  for (prod_idx = 0; prod_idx < num_productions; ++prod_idx) {
    struct prd_production *prod = productions + prod_idx;
    r = gt_emit(gt, prod->nt_.sym_->ordinal_);
    if (r) return r;
    
    size_t sym_idx;
    for (sym_idx = 0; sym_idx < prod->num_syms_; ++sym_idx) {
      struct prd_production_sym *sd = prod->syms_ + sym_idx;
      r = gt_emit(gt, sd->sym_->ordinal_);
      if (r) return r;
    }

    r = gt_emit(gt, end_of_production_sym);
    if (r) return r;
  }

  r = gt_emit(gt, end_of_grammar_sym);
  return r;
}

void gt_debug_grammar(struct grammar_table *gt, size_t num_productions, struct prd_production *productions, int end_of_production_sym, int end_of_grammar_sym) {
  size_t prod_idx;
  gt->num_ordinals_ = 0;
  for (prod_idx = 0; prod_idx < num_productions; ++prod_idx) {
    struct prd_production *prod = productions + prod_idx;
    fprintf(stderr, "%2d:", prod->nt_.sym_->ordinal_);

    size_t sym_idx;
    for (sym_idx = 0; sym_idx < prod->num_syms_; ++sym_idx) {
      struct prd_production_sym *sd = prod->syms_ + sym_idx;
      fprintf(stderr, " %2d", sd->sym_->ordinal_);
    }

    fprintf(stderr, " %2d\n", end_of_production_sym);
  }

  fprintf(stderr, "%2d\n", end_of_grammar_sym);
}

int gt_generate_lalr(struct grammar_table *gt, struct lr_generator *lalr, int end_of_production_sym, int end_of_grammar_sym, int end_of_file_sym, int synthetic_s_sym) {
  lr_error_t lr_err;
  lr_err = lr_gen_parser(lalr, gt->ordinals_, end_of_production_sym, end_of_grammar_sym, end_of_file_sym, synthetic_s_sym);
  switch (lr_err) {
  case LR_OK:
    return 0;
  case LR_NOT_LR_GRAMMAR:
    re_error_nowhere("Error, not an LR grammar\n");
    return GT_NOT_LR_GRAMMAR;
  case LR_AMBIGUOUS_GRAMMAR:
    re_error_nowhere("Error, grammar is ambiguous\n");
    return GT_AMBIGUOUS_GRAMMAR;
  case LR_CONFLICTS:
    /* Caller handles and reports this */
    return GT_CONFLICTS;
  case LR_INTERNAL_ERROR:
    re_error_nowhere("Error, no memory or integer overflow\n");
    return GT_INTERNAL_ERROR;
  }
  assert(0 && "Unhandled return value\n");
  return GT_INTERNAL_ERROR;
}
