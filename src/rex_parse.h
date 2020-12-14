#ifndef CARB_REX_CREXBUILDWIN_X86DEBUGREXOBJREX_PARSE_H_INCLUDED
#define CARB_REX_CREXBUILDWIN_X86DEBUGREXOBJREX_PARSE_H_INCLUDED

#include <stddef.h> /* size_t */

#ifdef __cplusplus
extern "C" {
  #endif
  
  #define _REX_FINISH 0
  #define _REX_MATCH 1
  #define _REX_OVERFLOW 2
  #define _REX_NO_MEMORY 3
  #define _REX_FEED_ME 4
  #define _REX_END_OF_INPUT 5
  #define _REX_SYNTAX_ERROR 6
  #define _REX_LEXICAL_ERROR 7
  #define _REX_INTERNAL_ERROR 8
  
  #define REX_PAR_OPEN 3
  #define REX_PAR_CLOSE 4
  #define REX_SQBRACE_OPEN 5
  #define REX_SQBRACE_CLOSE 6
  #define REX_CUBRACE_OPEN 7
  #define REX_CUBRACE_CLOSE 8
  #define REX_DOT 9
  #define REX_DOLLAR 10
  #define REX_CARET 11
  #define REX_DASH 12
  #define REX_BAR 13
  #define REX_ASTERISK 14
  #define REX_PLUS 15
  #define REX_QUESTION_MARK 16
  #define REX_RESERVED_REGEX_CHAR 17
  #define REX_SEMICOLON 18
  #define REX_CHAR 19
  #define REX_WHITESPACE 20
  #define REX_START_OF_INPUT_ESC 21
  #define REX_END_OF_INPUT_ESC 22
  #define REX_ERROR 23
  #define REX_INPUT_END 24
  
  #define REX_REGEX 25
  #define REX_EXP 26
  #define REX_TERM 27
  #define REX_ELM 28
  #define REX_SYM 29
  #define REX_RANGE 30
  #define REX_RANGE_ELM 31
  
  struct rex_stack {
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
    struct rex_sym_data *stack_;
    struct rex_sym_data *sym_data_;
    size_t current_production_length_;
    int current_production_nonterminal_;
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
  
  void rex_stack_init(struct rex_stack *stack);
  void rex_stack_cleanup(struct rex_stack *stack);
  int rex_stack_reset(struct rex_stack *stack);
  int rex_stack_can_recover(struct rex_stack *stack);
  int rex_stack_accepts(struct rex_stack *stack, int sym);
  void rex_set_input(struct rex_stack *stack, const char *input, size_t input_size, int is_final_input);
  int rex_scan(struct rex_stack *stack, struct rex_nfa *nfa, size_t *pstart, size_t *pend);
  void rex_set_location(struct rex_stack *stack, int line, int col, size_t offset);
  const char *rex_text(struct rex_stack *stack);
  size_t rex_len(struct rex_stack *stack);
  int rex_line(struct rex_stack *stack);
  int rex_column(struct rex_stack *stack);
  size_t rex_offset(struct rex_stack *stack);
  int rex_endline(struct rex_stack *stack);
  int rex_endcolumn(struct rex_stack *stack);
  size_t rex_endoffset(struct rex_stack *stack);
  int rex_lex(struct rex_stack *stack);
  int rex_parse(struct rex_stack *stack, int sym, struct rex_nfa *nfa, size_t *pstart, size_t *pend);
  
  #ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* CARB_REX_CREXBUILDWIN_X86DEBUGREXOBJREX_PARSE_H_INCLUDED */
