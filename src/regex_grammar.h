#ifndef CARB_RXG_SRCREGEX_GRAMMAR_H_INCLUDED
#define CARB_RXG_SRCREGEX_GRAMMAR_H_INCLUDED

#include <stddef.h> /* size_t */

#ifdef __cplusplus
extern "C" {
#endif

#define _RXG_MATCH 1
#define _RXG_OVERFLOW 2
#define _RXG_NO_MEMORY 3
#define _RXG_FEED_ME 4
#define _RXG_END_OF_INPUT 5
#define _RXG_SYNTAX_ERROR 6
#define _RXG_INTERNAL_ERROR 7

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
#define RXG_TOKEN 21
#define RXG_WHITESPACE 22
#define RXG_ERROR 23
#define RXG_INPUT_END 24

struct rxg_stack {
  int error_recovery_:1;
  int report_error_:1;
  int pending_reset_:1;
  int need_sym_:1;
  int current_sym_;
  int mute_error_turns_;
  size_t pos_, num_stack_allocated_;
  struct rxg_sym_data *stack_;
  size_t scan_state_;
  size_t match_index_;
  size_t best_match_action_;
  size_t best_match_size_;
  size_t input_index_;
  size_t input_offset_;
  size_t match_buffer_size_;
  size_t match_buffer_size_allocated_;
  /* offset, line and column at the start of match_buffer_ */
  size_t match_offset_;
  int match_line_;
  int match_col_;
  /* offset, line and column at the zero-termination (best_match_size_) of a match 
   * in match_buffer_ (the actual buffer may well be longer.) */
  size_t best_match_offset_;
  int best_match_line_;
  int best_match_col_;
  size_t token_size_;
  char *match_buffer_;
  char terminator_repair_;
  int input_line_;
  int input_col_;
};

void rxg_stack_init(struct rxg_stack *stack);
void rxg_stack_cleanup(struct rxg_stack *stack);
int rxg_stack_reset(struct rxg_stack *stack);
int rxg_parse(struct rxg_stack *stack, int sym, struct prd_grammar *g, struct tkr_tokenizer *tkr, struct symbol_table *st, char char_value);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* CARB_RXG_SRCREGEX_GRAMMAR_H_INCLUDED */
