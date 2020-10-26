#ifndef CINDER_RXG_SRCREGEX_GRAMMAR_H_INCLUDED
#define CINDER_RXG_SRCREGEX_GRAMMAR_H_INCLUDED

#include <stddef.h> /* size_t */

#ifdef __cplusplus
extern "C" {
#endif

#define RXG_CHAR 3
#define RXG_IDENT 4
#define RXG_PAR_OPEN 5
#define RXG_PAR_CLOSE 6
#define RXG_CUR_OPEN 7
#define RXG_CUR_CLOSE 8
#define RXG_COLON 9
#define RXG_SEMICOLON 10
#define RXG_SQ_OPEN 11
#define RXG_SQ_CLOSE 12
#define RXG_DOT 13
#define RXG_DOLLAR 14
#define RXG_CARET 15
#define RXG_DASH 16
#define RXG_BAR 17
#define RXG_ASTERISK 18
#define RXG_PLUS 19
#define RXG_QUESTION_MARK 20
#define RXG_WHITESPACE 21
#define RXG_ERROR 22
#define RXG_INPUT_END 23

struct rxg_stack {
  int error_recovery_:1;
  int report_error_:1;
  int mute_error_turns_;
  size_t pos_, num_stack_allocated_;
  struct rxg_sym_data *stack_;
};

void rxg_stack_init(struct rxg_stack *stack);
void rxg_stack_cleanup(struct rxg_stack *stack);
int rxg_stack_reset(struct rxg_stack *stack);
int rxg_parse(struct rxg_stack *stack, int sym, struct prd_grammar *g, struct tkr_tokenizer *tkr, struct symbol_table *st, char char_value);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* CINDER_RXG_SRCREGEX_GRAMMAR_H_INCLUDED */
