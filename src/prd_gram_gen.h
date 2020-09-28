#ifndef CINDER_PRD_PRD_GRAM_GEN_H_INCLUDED
#define CINDER_PRD_PRD_GRAM_GEN_H_INCLUDED

#include <stddef.h> /* size_t */
#ifdef __cplusplus
extern "C" {
#endif

#define PRD_IDENT 3
#define PRD_COLON 4
#define PRD_EQUALS 5
#define PRD_SEMICOLON 6
#define PRD_TOKEN 7
#define PRD_PAR_OPEN 8
#define PRD_PAR_CLOSE 9
#define PRD_CUBRACE_OPEN 10
#define PRD_CUBRACE_CLOSE 11
#define PRD_ERROR 12
#define PRD_INPUT_END 13

struct prd_stack {
  size_t pos_, num_stack_allocated_;
  struct prd_sym_data *stack_;
};

void prd_stack_init(struct prd_stack *stack);
void prd_stack_cleanup(struct prd_stack *stack);
int prd_stack_reset(struct prd_stack *stack);
int prd_parse(struct prd_stack *stack, int sym, struct prd_grammar *g, struct tkr_tokenizer *tkr, struct symbol_table *st);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* CINDER_PRD_PRD_GRAM_GEN_H_INCLUDED */
