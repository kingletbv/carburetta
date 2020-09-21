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

struct prd_sym_data {
  struct prd_production prod_; /* Note: Statically allocated, cannot rely on pointer stability */
  token_type_t match_; /* match_ & variant_ for shifted terminal tokens */
  token_type_t variant_;
  struct xlts text_;
};

struct prd_stack {
  size_t pos_;
  size_t num_stack_allocated_; /* size in elements of states and syms */
  int *states_;
  struct prd_sym_data *syms_;
  union prd_sym_data_union *sym_data_;
};

struct prd_grammar {
  int have_errors_ : 1; /* if non-zero, errors were issued and compilation failed */
  int accept_whitespace_ : 1; /* if non-zero, whitespace is passed on to the grammar as PRD_TOKEN */

  /* Productions making up the entire grammar */
  size_t num_productions_;
  size_t num_productions_allocated_;
  struct prd_production *productions_;
};

int prd_init(void);
void prd_cleanup(void);
int prd_parse(struct prd_stack *stack, struct prd_grammar *g, struct tkr_tokenizer *tkr, int end_of_input, struct symbol_table *st);

void prd_stack_init(struct prd_stack *stack);
int prd_stack_reset(struct prd_stack *stack);
void prd_stack_cleanup(struct prd_stack *stack);

void prd_grammar_init(struct prd_grammar *g);
void prd_grammar_cleanup(struct prd_grammar *g);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* PRD_GRAM_H */
