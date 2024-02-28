#ifndef CARB_PRD_PRD_GRAMMAR_H_INCLUDED
#define CARB_PRD_PRD_GRAMMAR_H_INCLUDED

#include <stddef.h> /* size_t */

#ifdef __cplusplus
extern "C" {
  #endif
  
  #define _PRD_FINISH 0
  #define _PRD_MATCH 1
  #define _PRD_OVERFLOW 2
  #define _PRD_NO_MEMORY 3
  #define _PRD_FEED_ME 4
  #define _PRD_END_OF_INPUT 5
  #define _PRD_SYNTAX_ERROR 6
  #define _PRD_LEXICAL_ERROR 7
  #define _PRD_INTERNAL_ERROR 8
  
  #define PRD_IDENT 3
  #define PRD_COLON 4
  #define PRD_EQUALS 5
  #define PRD_SEMICOLON 6
  #define PRD_TOKEN 7
  #define PRD_PAR_OPEN 8
  #define PRD_PAR_CLOSE 9
  #define PRD_CUBRACE_OPEN 10
  #define PRD_CUBRACE_CLOSE 11
  #define PRD_DOLLAR 12
  #define PRD_ERROR 13
  #define PRD_INPUT_END 14
  
  #define PRD_GRAMMAR 15
  #define PRD_RULE 16
  #define PRD_PRODUCTION 17
  #define PRD_ACTION_SEQUENCE 18
  #define PRD_STMT_ACTION 19
  #define PRD_START_C_TOKENIZER 20
  #define PRD_END_C_TOKENIZER 21
  #define PRD_ACCEPT_WHITESPACE 22
  #define PRD_COMMON_ACTION 23
  
  
  struct prd_stack {
    int error_recovery_:1;
    int pending_reset_:1;
    int discard_remaining_actions_:1;
    int slot_1_has_sym_data_:1;
    int slot_1_has_common_data_:1;
    int top_of_stack_has_sym_data_:1;
    int top_of_stack_has_common_data_:1;
    int newbuf_pos_has_common_data_:1;
    int newbuf_pos_has_sym_data_:1;
    int stack_newbuf_pos_has_common_data_:1;
    int stack_newbuf_pos_has_sym_data_:1;
    int action_preservation_;
    int current_err_action_;
    int slot_1_sym_;
    int continue_at_;
    int mute_error_turns_;
    size_t pos_, num_stack_allocated_;
    struct prd_sym_data *stack_;
    struct prd_sym_data *sym_data_;
    struct prd_sym_data *new_buf_;
    size_t new_buf_num_allocated_;
    size_t new_buf_sym_partial_pos_;
    size_t current_production_length_;
    int current_production_nonterminal_;
    size_t sym_idx_;
  };
  
  void prd_stack_init(struct prd_stack *stack);
  void prd_stack_cleanup(struct prd_stack *stack);
  int prd_stack_reset(struct prd_stack *stack);
  int prd_stack_can_recover(struct prd_stack *stack);
  int prd_stack_accepts(struct prd_stack *stack, int sym);
  int prd_parse(struct prd_stack *stack, int sym, struct prd_grammar *g, struct tkr_tokenizer *tkr, struct symbol_table *st);
  
  #ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* CARB_PRD_PRD_GRAMMAR_H_INCLUDED */
