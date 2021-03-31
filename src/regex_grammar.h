#ifndef CARB_RXG_CARBURETTASRCREGEX_GRAMMAR_H_INCLUDED
#define CARB_RXG_CARBURETTASRCREGEX_GRAMMAR_H_INCLUDED

#include <stddef.h> /* size_t */

#ifdef __cplusplus
extern "C" {
  #endif
  
  #define _RXG_FINISH 0
  #define _RXG_MATCH 1
  #define _RXG_OVERFLOW 2
  #define _RXG_NO_MEMORY 3
  #define _RXG_FEED_ME 4
  #define _RXG_END_OF_INPUT 5
  #define _RXG_SYNTAX_ERROR 6
  #define _RXG_LEXICAL_ERROR 7
  #define _RXG_INTERNAL_ERROR 8
  
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
  #define RXG_START_OF_INPUT_ESC 23
  #define RXG_END_OF_INPUT_ESC 24
  #define RXG_LESS_THAN 25
  #define RXG_GREATER_THAN 26
  #define RXG_COMMA 27
  #define RXG_ERROR 28
  #define RXG_INPUT_END 29
  
  #define RXG_EXP 30
  #define RXG_TERM 31
  #define RXG_ELM 32
  #define RXG_SYM 33
  #define RXG_RANGE 34
  #define RXG_RANGE_ELM 35
  #define RXG_GRAMMAR 36
  #define RXG_PATTERN 37
  #define RXG_ACTION_SEQUENCE 38
  #define RXG_STMT_ACTION 39
  #define RXG_START_REGEX 40
  #define RXG_END_REGEX 41
  #define RXG_START_C_TOKENIZER 42
  #define RXG_END_C_TOKENIZER 43
  #define RXG_ACCEPT_WHITESPACE 44
  #define RXG_MODE 45
  #define RXG_MODE_LIST 46
  #define RXG_MODE_GROUP 47
  #define RXG_PATTERN_LIST 48
  
  #define M_RXG_DEFAULT 1
  
  struct rxg_stack {
    int error_recovery_:1;
    int pending_reset_:1;
    int discard_remaining_actions_:1;
    int slot_1_has_sym_data_:1;
    int slot_1_has_common_data_:1;
    int top_of_stack_has_sym_data_:1;
    int top_of_stack_has_common_data_:1;
    int need_sym_:1;
    int is_final_input_:1;
    int slot_0_has_current_sym_data_:1;
    int slot_0_has_common_data_:1;
    int current_sym_;
    size_t input_size_;
    const char *input_;
    int slot_1_sym_;
    int continue_at_;
    int mute_error_turns_;
    size_t pos_, num_stack_allocated_;
    struct rxg_sym_data *stack_;
    struct rxg_sym_data *sym_data_;
    size_t current_production_length_;
    int current_production_nonterminal_;
    size_t scan_state_;
    size_t current_mode_start_state_;
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
  int rxg_stack_can_recover(struct rxg_stack *stack);
  int rxg_stack_accepts(struct rxg_stack *stack, int sym);
  void rxg_set_mode(struct rxg_stack *stack, int mode);void rxg_set_input(struct rxg_stack *stack, const char *input, size_t input_size, int is_final_input);
  int rxg_scan(struct rxg_stack *stack, struct prd_grammar *g, struct tkr_tokenizer *tkr, struct symbol_table *st, char char_value);
  void rxg_set_location(struct rxg_stack *stack, int line, int col, size_t offset);
  const char *rxg_text(struct rxg_stack *stack);
  size_t rxg_len(struct rxg_stack *stack);
  int rxg_line(struct rxg_stack *stack);
  int rxg_column(struct rxg_stack *stack);
  size_t rxg_offset(struct rxg_stack *stack);
  int rxg_endline(struct rxg_stack *stack);
  int rxg_endcolumn(struct rxg_stack *stack);
  size_t rxg_endoffset(struct rxg_stack *stack);
  int rxg_lex(struct rxg_stack *stack);
  int rxg_parse(struct rxg_stack *stack, int sym, struct prd_grammar *g, struct tkr_tokenizer *tkr, struct symbol_table *st, char char_value);
  
  #ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* CARB_RXG_CARBURETTASRCREGEX_GRAMMAR_H_INCLUDED */
