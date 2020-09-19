#ifndef GRAMMAR_TABLE_H
#define GRAMMAR_TABLE_H

#ifndef STDINT_H_INCLUDED
#define STDINT_H_INCLUDED
#include <stdint.h>
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
int gt_generate_lalr(struct grammar_table *gt, lr_generator_t *lalr, int end_of_production_sym, int end_of_grammar_sym, int end_of_file_sym, int synthetic_s_sym);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* GRAMMAR_TABLE_H */