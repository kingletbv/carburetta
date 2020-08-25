#ifndef PRD_GRAM_H
#define PRD_GRAM_H

#ifndef TOKENS_H_INCLUDED
#define TOKENS_H_INCLUDED
#include "tokens.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define PRD_SUCCESS 0
#define PRD_SYNTAX_ERROR 1
#define PRD_NEXT 2
#define PRD_INTERNAL_ERROR 3


struct prd_sym_data {
  void *p_;
  char *text_;
};

struct prd_stack {
  size_t pos_;
  size_t num_stack_allocated_; /* size in elements of states and syms */
  int *states_;
  struct prd_sym_data *syms_;
};

int prd_init(void);
void prd_cleanup(void);
int prd_parse(struct prd_stack *stack, struct tkr_tokenizer *tkr, int end_of_input);

void prd_stack_init(struct prd_stack *stack);
int prd_reset(struct prd_stack *stack);
void prd_stack_cleanup(struct prd_stack *stack);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* PRD_GRAM_H */