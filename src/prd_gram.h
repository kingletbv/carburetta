#ifndef PRD_GRAM_H
#define PRD_GRAM_H

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

  /* Action sequence that executes when the production matches */
  struct snippet action_sequence_;
};

struct prd_grammar {
  int have_errors_ : 1; /* if non-zero, errors were issued and compilation failed */
  int accept_whitespace_ : 1; /* if non-zero, whitespace is passed on to the grammar as PRD_TOKEN */

  /* Productions making up the entire grammar */
  size_t num_productions_;
  size_t num_productions_allocated_;
  struct prd_production *productions_;
};

int prd_parse_tkr(struct prd_stack *stack, struct prd_grammar *g, struct tkr_tokenizer *tkr, int end_of_input, struct symbol_table *st);

void prd_production_sym_init(struct prd_production_sym *pps);
void prd_production_sym_cleanup(struct prd_production_sym *pps);
void prd_prod_init(struct prd_production *ppd);
void prd_prod_reset(struct prd_production *ppd);
void prd_prod_cleanup(struct prd_production *ppd);
void prd_grammar_init(struct prd_grammar *g);
void prd_grammar_cleanup(struct prd_grammar *g);
void prd_prod_swap(struct prd_production *a, struct prd_production *b);
int prd_grammar_check_production_reserve(struct prd_grammar *g);
int prd_prod_check_sym_reserve(struct prd_production *pd, struct xlts *loc);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* PRD_GRAM_H */
