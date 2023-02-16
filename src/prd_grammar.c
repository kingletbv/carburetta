/* Copyright 2020-2023 Kinglet B.V.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef STDIO_H_INCLUDED
#define STDIO_H_INCLUDED
#include <stdio.h>
#endif

#ifndef STDLIB_H_INCLUDED
#define STDLIB_H_INCLUDED
#include <stdlib.h>
#endif

#ifndef STDINT_H_INCLUDED
#define STDINT_H_INCLUDED
#include <stdint.h>
#endif

#ifndef STRING_H_INCLUDED
#define STRING_H_INCLUDED
#include <string.h>
#endif

#ifndef ERRNO_H_INCLUDED
#define ERNNO_H_INCLUDED
#include <errno.h>
#endif

#ifndef ASSERT_H_INCLUDED
#define ASSERT_H_INCLUDED
#include <assert.h>
#endif

#ifndef TOKENIZER_H_INCLUDED
#define TOKENIZER_H_INCLUDED
#include "tokenizer.h"
#endif

#ifndef TOKENS_H_INCLUDED
#define TOKENS_H_INCLUDED
#include "tokens.h"
#endif

#ifndef REPORT_ERROR_H_INCLUDED
#define REPORT_ERROR_H_INCLUDED
#include "report_error.h"
#endif

#ifndef PRD_GRAM_H_INCLUDED
#define PRD_GRAM_H_INCLUDED
#include "prd_gram.h"
#endif

#ifndef PRD_GRAMMAR_H_INCLUDED
#define PRD_GRAMMAR_H_INCLUDED
#include "prd_grammar.h"
#endif









/* --------- START OF GENERATED CODE ------------ */
#include <stdlib.h> /* realloc(), free(), NULL, size_t */
#include <string.h> /* memcpy() */
#include <stddef.h> /* size_t */
#include <stdint.h> /* SIZE_MAX */
struct prd_sym_data {
  int state_;
  union {
    struct { token_type_t match_ ; token_type_t variant_ ; struct xlts text_ ; } uv0_;
    struct prd_production uv1_;
  } v_;
};
static const int prd_minimum_sym = 3;
static const size_t prd_num_columns = 22;
static const size_t prd_num_rows = 54;
static const size_t prd_num_productions = 31;
static const int prd_parse_table[] = {
   -2,  0,  0,  0,  0,  0,  0,  0,  0, -2, 0,-2,53, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   -3,  0,  0,  0,  0,  0,  0,  0,  0, -3, 0,-3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   -5,  0,  0,  0,  0,  0,  0,  0,  0, -5, 0,-5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   -6,  0,  0,  0,  0,  0,  0,  0,  0, -6, 0,-6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0,  0,  0,  3,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   19, 20, 21,-30, 22, 26,  0, 29,  0, 23, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0,
   -7,  0,  0,  0,  0,  0,  0,  0,  0, -7, 0,-7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0,  0,  0,  0,  0,  0,  0,  0,  6,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   35, 36, 37, 38, 39, 43,  0, 46,-30, 40, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0,
  -20,-20,-20,-20,-20,-20,  0,-20,-20,-20, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0,
    0,  0,  0,  0,  0,  0,  0,  9,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0,  0, 32,  0,  0,  0,  0,-31,  0,  0, 0, 0, 0, 0, 0, 0, 5, 0, 0,10, 0, 0,
  -11,  0,-11,-11,  0,  0,  0,-11,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   12,  0,-29,  2,  0,  0,  0,-29,  0,  0, 0, 0, 0, 0, 0, 0, 0,11, 0, 0, 0, 0,
  -10,  0,-10,-10,  0,  0,  0,-10,  0,  0, 0, 0, 0,13, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 14,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   -4,  0,  0,  0,  0,  0,  0,  0,  0, -4, 0,-4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   -8,  0,  0,  0,  0,  0,  0,  0,  0, -8, 0,-8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0,  0,  0, 17,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  -13,-13,-13,-13,-13,-13,  0,-13,  0,-13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  -14,-14,-14,-14,-14,-14,  0,-14,  0,-14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  -15,-15,-15,-15,-15,-15,  0,-15,  0,-15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  -16,-16,-16,-16,-16,-16,  0,-16,  0,-16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  -17,-17,-17,-17,-17,-17,  0,-17,  0,-17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  -18,-18,-18,-18,-18,-18,  0,-18,  0,-18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   35, 36, 37, 38, 39, 43, 24, 46,  0, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  -20,-20,-20,-20,-20,-20,-20,-20,  0,-20, 0, 0, 0, 0, 0,25, 0, 0, 0, 0, 0, 0,
  -19,-19,-19,-19,-19,-19,  0,-19,  0,-19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   35, 36, 37, 38, 39, 43,  0, 46, 27, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  -20,-20,-20,-20,-20,-20,  0,-20,-20,-20, 0, 0, 0, 0, 0,28, 0, 0, 0, 0, 0, 0,
   19, 20, 21,-30, 22, 26,  0, 29,  0, 23, 0, 0, 0, 0, 0, 0, 0, 0,18, 0, 0, 0,
  -12,-12,-12,-12,-12,-12,  0,-12,  0,-12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  -31,-31,-31,-31,-31,-31,  0,-31,  0,-31, 0, 0, 0, 0, 0, 0, 0, 0, 0,31, 0, 0,
   -9,  0,  0,  0,  0,  0,  0,  0,  0, -9, 0,-9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0,  0,  0,  0,  0,  0,  0,  0, 33,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  -21,-21,-21,-21,-21,-21,-21,-21,-21,-21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  -22,-22,-22,-22,-22,-22,-22,-22,-22,-22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  -23,-23,-23,-23,-23,-23,-23,-23,-23,-23, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  -24,-24,-24,-24,-24,-24,-24,-24,-24,-24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  -25,-25,-25,-25,-25,-25,-25,-25,-25,-25, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  -26,-26,-26,-26,-26,-26,-26,-26,-26,-26, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  -27,-27,-27,-27,-27,-27,-27,-27,-27,-27, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   35, 36, 37, 38, 39, 43, 41, 46,  0, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  -20,-20,-20,-20,-20,-20,-20,-20,  0,-20, 0, 0, 0, 0, 0,42, 0, 0, 0, 0, 0, 0,
  -28,-28,-28,-28,-28,-28,-28,-28,-28,-28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   35, 36, 37, 38, 39, 43,  0, 46, 44, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  -20,-20,-20,-20,-20,-20,  0,-20,-20,-20, 0, 0, 0, 0, 0,45, 0, 0, 0, 0, 0, 0,
   35, 36, 37, 38, 39, 43,  0, 46,-30, 40, 0, 0, 0, 0, 0, 0, 0, 0,34, 0, 0, 0,
  -20,-20,-20,-20,-20,-20,  0,-20,-20,-20, 0, 0, 0, 0, 0,47, 0, 0, 0, 0, 0, 0,
    0,  0,  0,  0,  0,  0,  0, 48,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0,  0, 32,  0,  0,  0,  0,-31,  0,  0, 0, 0, 0, 0, 0, 0,30, 0, 0,49, 0, 0,
    0,  0,-29,  0,  0,  0,  0,-29,  0,  0, 0, 0, 0, 0, 0, 0, 0,50, 0, 0, 0, 0,
    0, 51,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   15,  0,  0,  0,  0,  0,  0,  0,  0, 52, 0,-1, 0, 0, 1, 0, 0, 0, 0, 0,16, 0,
};
static const size_t prd_production_lengths[] = {
  1,
  0,
  2,
  2,
  4,
  7,
  9,
  6,
  8,
  0,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  4,
  4,
  0,
  2,
  2,
  2,
  2,
  2,
  2,
  4,
  4,
  0,
  0,
  0
};
static const int prd_production_syms[] = {
  24,
  15,
  15,
  15,
  17,
  17,
  17,
  23,
  23,
  16,
  16,
  19,
  19,
  19,
  19,
  19,
  19,
  19,
  19,
  18,
  18,
  18,
  18,
  18,
  18,
  18,
  18,
  18,
  20,
  21,
  22
};

#ifndef CARB_PRD_CCARBURETTASRCPRD_GRAMMAR_H_INCLUDED
struct prd_stack {
  int error_recovery_:1;
  int pending_reset_:1;
  int discard_remaining_actions_:1;
  int slot_1_has_sym_data_:1;
  int slot_1_has_common_data_:1;
  int top_of_stack_has_sym_data_:1;
  int top_of_stack_has_common_data_:1;
  int slot_1_sym_;
  int continue_at_;
  int mute_error_turns_;
  size_t pos_, num_stack_allocated_;
  struct prd_sym_data *stack_;
  struct prd_sym_data *sym_data_;
  size_t current_production_length_;
  int current_production_nonterminal_;
};
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


#endif /* CARB_PRD_CCARBURETTASRCPRD_GRAMMAR_H_INCLUDED */

void prd_stack_init(struct prd_stack *stack) {
  stack->error_recovery_ = 0;
  stack->pending_reset_ = 1;
  stack->discard_remaining_actions_ = 0;
  stack->slot_1_has_sym_data_ = stack->slot_1_has_common_data_ = 0;
  stack->slot_1_sym_ = 0;
  stack->top_of_stack_has_sym_data_ = stack->top_of_stack_has_common_data_ = 0;
  stack->continue_at_ = 0;
  stack->mute_error_turns_ = 0;
  stack->pos_ = 0;
  stack->num_stack_allocated_ = 0;
  stack->stack_ = NULL;
  stack->sym_data_ = NULL;
  stack->current_production_length_ = 0;
  stack->current_production_nonterminal_ = 0;
}

void prd_stack_cleanup(struct prd_stack *stack) {
  size_t n;
  for (n = 2; n < stack->pos_; ++n) {
    if ((n != (stack->pos_ - 1)) || stack->top_of_stack_has_sym_data_) {
      switch (stack->stack_[n].state_) {
        case 2: /* semicolon */
        case 3: /* semicolon */
        case 6: /* cubrace-close */
        case 9: /* cubrace-open */
        case 12: /* ident */
        case 14: /* colon */
        case 15: /* ident */
        case 17: /* semicolon */
        case 19: /* ident */
        case 20: /* colon */
        case 21: /* equals */
        case 22: /* token */
        case 23: /* dollar */
        case 24: /* par-close */
        case 26: /* par-open */
        case 27: /* cubrace-close */
        case 29: /* cubrace-open */
        case 32: /* equals */
        case 33: /* cubrace-close */
        case 35: /* ident */
        case 36: /* colon */
        case 37: /* equals */
        case 38: /* semicolon */
        case 39: /* token */
        case 40: /* dollar */
        case 41: /* par-close */
        case 43: /* par-open */
        case 44: /* cubrace-close */
        case 46: /* cubrace-open */
        case 48: /* cubrace-open */
        case 51: /* colon */
        case 52: /* dollar */
        {
          {
             xlts_cleanup(&((stack->stack_ + n)->v_.uv0_).text_);
          }
          
        }
        break;
        case 1: /* production */
        case 5: /* stmt-action */
        case 8: /* action-sequence */
        case 13: /* rule */
        case 25: /* action-sequence */
        case 28: /* action-sequence */
        case 30: /* stmt-action */
        case 42: /* action-sequence */
        case 45: /* action-sequence */
        case 47: /* action-sequence */
        {
          {
             prd_prod_cleanup(&((stack->stack_ + n)->v_.uv1_));
          }
          
        }
        break;
      } /* switch */
    }
  }
  if (stack->slot_1_has_sym_data_) {
    switch (stack->slot_1_sym_) {
      case 3:
      case 4:
      case 5:
      case 6:
      case 7:
      case 8:
      case 9:
      case 10:
      case 11:
      case 12:
      {
        {
           xlts_cleanup(&(stack->stack_[1].v_.uv0_).text_);
        }
        
      }
      break;
      case 16:
      case 17:
      case 18:
      case 19:
      {
        {
           prd_prod_cleanup(&(stack->stack_[1].v_.uv1_));
        }
        
      }
      break;
    } /* switch */
  }
  
  if (stack->stack_) free(stack->stack_);
}

static int prd_push_state(struct prd_stack *stack, int state) {
  if (stack->num_stack_allocated_ == stack->pos_) {
    size_t new_num_allocated = 0;
    if (stack->num_stack_allocated_) {
      new_num_allocated = stack->num_stack_allocated_ * 2;
      if (new_num_allocated <= stack->num_stack_allocated_) {
        /* Overflow in allocation */
        return _PRD_OVERFLOW;
      }
    }
    else {
      new_num_allocated = 16;
    }
    
    if (new_num_allocated > (SIZE_MAX / sizeof(struct prd_sym_data))) {
      /* Overflow in allocation */
      return _PRD_OVERFLOW;
    }
    
    void *p = realloc(stack->stack_, new_num_allocated * sizeof(struct prd_sym_data));
    if (!p) {
      /* Out of memory */
      return _PRD_NO_MEMORY;
    }
    stack->stack_ = (struct prd_sym_data *)p;
    stack->num_stack_allocated_ = new_num_allocated;
  }
  stack->stack_[stack->pos_++].state_ = state;
  return 0;
}

int prd_stack_reset(struct prd_stack *stack) {
  size_t n;
  stack->pending_reset_ = 0;
  stack->discard_remaining_actions_ = 0;
  for (n = 2; n < stack->pos_; ++n) {
    if ((n != (stack->pos_ - 1)) || stack->top_of_stack_has_sym_data_) {
      switch (stack->stack_[n].state_) {
        case 2: /* semicolon */
        case 3: /* semicolon */
        case 6: /* cubrace-close */
        case 9: /* cubrace-open */
        case 12: /* ident */
        case 14: /* colon */
        case 15: /* ident */
        case 17: /* semicolon */
        case 19: /* ident */
        case 20: /* colon */
        case 21: /* equals */
        case 22: /* token */
        case 23: /* dollar */
        case 24: /* par-close */
        case 26: /* par-open */
        case 27: /* cubrace-close */
        case 29: /* cubrace-open */
        case 32: /* equals */
        case 33: /* cubrace-close */
        case 35: /* ident */
        case 36: /* colon */
        case 37: /* equals */
        case 38: /* semicolon */
        case 39: /* token */
        case 40: /* dollar */
        case 41: /* par-close */
        case 43: /* par-open */
        case 44: /* cubrace-close */
        case 46: /* cubrace-open */
        case 48: /* cubrace-open */
        case 51: /* colon */
        case 52: /* dollar */
        {
          {
             xlts_cleanup(&((stack->stack_ + n)->v_.uv0_).text_);
          }
          
        }
        break;
        case 1: /* production */
        case 5: /* stmt-action */
        case 8: /* action-sequence */
        case 13: /* rule */
        case 25: /* action-sequence */
        case 28: /* action-sequence */
        case 30: /* stmt-action */
        case 42: /* action-sequence */
        case 45: /* action-sequence */
        case 47: /* action-sequence */
        {
          {
             prd_prod_cleanup(&((stack->stack_ + n)->v_.uv1_));
          }
          
        }
        break;
      } /* switch */
    }
  }
  if (stack->slot_1_has_sym_data_) {
    switch (stack->slot_1_sym_) {
      case 3:
      case 4:
      case 5:
      case 6:
      case 7:
      case 8:
      case 9:
      case 10:
      case 11:
      case 12:
      {
        {
           xlts_cleanup(&(stack->stack_[1].v_.uv0_).text_);
        }
        
      }
      break;
      case 16:
      case 17:
      case 18:
      case 19:
      {
        {
           prd_prod_cleanup(&(stack->stack_[1].v_.uv1_));
        }
        
      }
      break;
    } /* switch */
  }
  stack->slot_1_has_sym_data_ = stack->slot_1_has_common_data_ = 0;
  stack->slot_1_sym_ = 0;
  stack->top_of_stack_has_sym_data_ = 0;
  stack->top_of_stack_has_common_data_ = 0;
  stack->sym_data_ = NULL;
  stack->current_production_length_ = 0;
  stack->current_production_nonterminal_ = 0;
  stack->pos_ = 0;
  stack->error_recovery_ = 0;
  stack->continue_at_ = 0;
  stack->mute_error_turns_ = 0;
  /* reserve slot [0] for the token sym data */
  switch (prd_push_state(stack, 0)) {
    case _PRD_OVERFLOW:{
      return _PRD_OVERFLOW;
    }
    break;
    case _PRD_NO_MEMORY: {
      return _PRD_NO_MEMORY;
    }
    break;
  }
  /* reserve slot [1] for the initial state 0, corresponding to the synthetic S production
  ** sym_data in slot [1] is used for temporary nonterminal data storage */
  switch (prd_push_state(stack, 0)) {
    case _PRD_OVERFLOW:{
      return _PRD_OVERFLOW;
    }
    break;
    case _PRD_NO_MEMORY: {
      return _PRD_NO_MEMORY;
    }
    break;
  }
  return 0;
}

int prd_stack_can_recover(struct prd_stack *stack) {
  return !!stack->error_recovery_;
}

int prd_stack_accepts(struct prd_stack *stack, int sym) {
  if (!stack->pos_) return 0;
  return 0 != prd_parse_table[prd_num_columns * stack->stack_[stack->pos_ - 1].state_ + (sym - prd_minimum_sym)];}

int prd_parse(struct prd_stack *stack, int sym, struct prd_grammar *g, struct tkr_tokenizer *tkr, struct symbol_table *st) {
  if (stack->pending_reset_) {
    int r;
    r = prd_stack_reset(stack);
    if (r) return r;
  }
    switch (stack->continue_at_) {
    case 0: break; /* fall through to regular code path */
    case 1: goto C1;
    case 2: goto C2;
    case 3: goto C3;
    case 4: goto C4;
    case 5: goto C5;
    case 6: goto C6;
    case 7: goto C7;
    case 8: goto C8;
    case 9: goto C9;
    case 10: goto C10;
    case 11: goto C11;
    case 12: goto C12;
    case 13: goto C13;
    case 14: goto C14;
    case 15: goto C15;
    case 16: goto C16;
    case 17: goto C17;
    case 18: goto C18;
    case 19: goto C19;
    case 20: goto C20;
    case 21: goto C21;
    case 22: goto C22;
    case 23: goto C23;
    case 24: goto C24;
    case 25: goto C25;
    case 26: goto C26;
    case 27: goto C27;
    case 28: goto C28;
    case 29: goto C29;
    case 30: goto C30;
    case 31: goto C31;
    case 32: goto C32;
    case 33: goto C33;
    case 34: goto C34;
    case 35: goto C35;
    case 36: goto C36;
    case 37: goto C37;
    case 38: goto C38;
    case 39: goto C39;
    case 40: goto C40;
    case 41: goto C41;
    case 42: goto C42;
    case 43: goto C43;
    case 44: goto C44;
    case 45: goto C45;
    case 46: goto C46;
    case 47: goto C47;
    case 48: goto C48;
    case 49: goto C49;
    case 50: goto C50;
    case 51: goto C51;
    case 52: goto C52;
    case 53: goto C53;
    case 54: goto C54;
    case 55: goto C55;
  } /* continuation switch */
if (stack->mute_error_turns_) stack->mute_error_turns_--;
  for (;;) {
    if (!stack->error_recovery_) {
      int action = prd_parse_table[prd_num_columns * stack->stack_[stack->pos_ - 1].state_ + (sym - prd_minimum_sym)];
      if (action > 0) {
        switch (prd_push_state(stack, action)) {
          case _PRD_OVERFLOW: {
            stack->continue_at_ = 0;
            {
              re_error_tkr(tkr, "Error: internal error\n"); return PRD_INTERNAL_ERROR;
              
            }
          }
          break;
          case _PRD_NO_MEMORY: {
            stack->continue_at_ = 0;
            {
              re_error_tkr(tkr, "Error: no memory"); return PRD_INTERNAL_ERROR;
              
            }
          }
          break;
        } /* switch */
        
        stack->sym_data_ = stack->stack_ + stack->pos_ - 1;
        stack->top_of_stack_has_sym_data_ = 0;
        switch (sym) {
          case PRD_IDENT:
          case PRD_COLON:
          case PRD_EQUALS:
          case PRD_SEMICOLON:
          case PRD_TOKEN:
          case PRD_PAR_OPEN:
          case PRD_PAR_CLOSE:
          case PRD_CUBRACE_OPEN:
          case PRD_CUBRACE_CLOSE:
          case PRD_DOLLAR:
          stack->top_of_stack_has_sym_data_ = 1;
          stack->continue_at_ = 1;
          {
             (stack->sym_data_->v_.uv0_).match_ = (stack->sym_data_->v_.uv0_).variant_ = (token_type_t)0; \
             xlts_init(&(stack->sym_data_->v_.uv0_).text_);
          }
          C1:;
          stack->continue_at_ = 2;
          {
             (stack->sym_data_->v_.uv0_).match_ = (token_type_t)tkr->best_match_action_; \
              (stack->sym_data_->v_.uv0_).variant_ = (token_type_t)tkr->best_match_variant_; \
			  xlts_append(&(stack->sym_data_->v_.uv0_).text_, &tkr->xmatch_);
          }
          C2:;
          break;
        } /* switch */
        stack->continue_at_ = 0;
        {
          return PRD_NEXT;
          
        }
      } /* action > 0 */
      else if (action < 0) {
        int production = -action - 1;
        stack->discard_remaining_actions_ = 0;
        stack->current_production_length_ = prd_production_lengths[production];
        stack->current_production_nonterminal_ = prd_production_syms[production];
        if (0 == production) {
          stack->pending_reset_ = 1;
          return PRD_SUCCESS;
        }
        
        memset(&stack->stack_[1], 0, sizeof(struct prd_sym_data));
        { /* scope guard */
          stack->sym_data_ = stack->stack_ + stack->pos_ - stack->current_production_length_;
          int r; \
          struct prd_production *pd;
          switch (production) {
            /* grammar: */
            case 1: {
              stack->continue_at_ = 3;
              {
                { }
              }
              C3:;
            }
            break;
            /* grammar: grammar production */
            case 2: {
              stack->continue_at_ = 4;
              {
                {
	/* Store the PRD_PRODUCTION in the prd_grammar->productions_ array */
	r = prd_grammar_check_production_reserve(g);
	if (r) return r;
	pd = g->productions_ + g->num_productions_++;
	prd_prod_init(pd);
	prd_prod_swap(pd, &(stack->sym_data_[1].v_.uv1_));
  r = snippet_append_snippet(&pd->common_action_sequence_, &g->current_common_action_sequence_);
  if (r) return r;
}
              }
              C4:;
            }
            break;
            /* grammar: grammar common-action */
            case 3: {
              stack->continue_at_ = 5;
              {
                {
  /* Common actions are recorded in their own productions */
}
              }
              C5:;
            }
            break;
            /* production: IDENT COLON rule SEMICOLON */
            case 4: {
              stack->slot_1_has_sym_data_ = 1;
              stack->slot_1_sym_ = PRD_PRODUCTION;
              stack->continue_at_ = 6;
              {
                 prd_prod_init(&(stack->stack_[1].v_.uv1_));
              }
              C6:;
              stack->continue_at_ = 7;
              {
                {
	prd_prod_swap(&(stack->stack_[1].v_.uv1_), &(stack->sym_data_[2].v_.uv1_));
	pd = &(stack->stack_[1].v_.uv1_);
	xlts_append(&pd->nt_.id_, &(stack->sym_data_[0].v_.uv0_).text_);
}
              }
              C7:;
            }
            break;
            /* production: IDENT COLON rule start-c-tokenizer stmt-action end-c-tokenizer SEMICOLON */
            case 5: {
              stack->slot_1_has_sym_data_ = 1;
              stack->slot_1_sym_ = PRD_PRODUCTION;
              stack->continue_at_ = 8;
              {
                 prd_prod_init(&(stack->stack_[1].v_.uv1_));
              }
              C8:;
              stack->continue_at_ = 9;
              {
                {
	prd_prod_swap(&(stack->stack_[1].v_.uv1_), &(stack->sym_data_[2].v_.uv1_));
	pd = &(stack->stack_[1].v_.uv1_);
	xlts_append(&pd->nt_.id_, &(stack->sym_data_[0].v_.uv0_).text_);

	/* Move snippets in; could do this more efficiently by manually swapping pointers; but this works.. */
	r = snippet_append_snippet(&pd->action_sequence_, & (stack->sym_data_[4].v_.uv1_).action_sequence_);
	if (r) return r;

	/* Append the semicolon to the snippets */
	r = snippet_append(&pd->action_sequence_, (stack->sym_data_[6].v_.uv0_).match_, (stack->sym_data_[6].v_.uv0_).variant_, &(stack->sym_data_[6].v_.uv0_).text_);
	if (r) return r;
}
              }
              C9:;
            }
            break;
            /* production: IDENT COLON rule start-c-tokenizer accept-whitespace CUBRACE_OPEN action-sequence end-c-tokenizer CUBRACE_CLOSE */
            case 6: {
              stack->slot_1_has_sym_data_ = 1;
              stack->slot_1_sym_ = PRD_PRODUCTION;
              stack->continue_at_ = 10;
              {
                 prd_prod_init(&(stack->stack_[1].v_.uv1_));
              }
              C10:;
              stack->continue_at_ = 11;
              {
                {
	prd_prod_swap(&(stack->stack_[1].v_.uv1_), &(stack->sym_data_[2].v_.uv1_));
	pd = &(stack->stack_[1].v_.uv1_);
	xlts_append(&pd->nt_.id_, &(stack->sym_data_[0].v_.uv0_).text_);

	/* Move snippets in; could do this more efficiently by manually swapping pointers; but this works.. */
	/* Append the cubrace-open to the snippets */
	r = snippet_append(&pd->action_sequence_, (stack->sym_data_[5].v_.uv0_).match_, (stack->sym_data_[5].v_.uv0_).variant_, &(stack->sym_data_[5].v_.uv0_).text_);
	if (r) return r;

	/* Move snippets in; could do this more efficiently by manually swapping pointers; but this works.. */
	r = snippet_append_snippet(&pd->action_sequence_, &(stack->sym_data_[6].v_.uv1_).action_sequence_);
	if (r) return r;

	/* Append the cubrace-close to the snippets */
	r = snippet_append(&pd->action_sequence_, (stack->sym_data_[8].v_.uv0_).match_, (stack->sym_data_[8].v_.uv0_).variant_, &(stack->sym_data_[8].v_.uv0_).text_);
	if (r) return r;
}
              }
              C11:;
            }
            break;
            /* common-action: DOLLAR COLON start-c-tokenizer stmt-action end-c-tokenizer SEMICOLON */
            case 7: {
              stack->continue_at_ = 12;
              {
                {
  /* Analogous to regular production actions, except we're moving it into the grammar's
   * current common action sequence. */
  snippet_clear(&g->current_common_action_sequence_);

  /* append stmt-action */
  r = snippet_append_snippet(&g->current_common_action_sequence_, &(stack->sym_data_[3].v_.uv1_).action_sequence_);
  if (r) return r;
  
  /* append semicolon */
  r = snippet_append(&g->current_common_action_sequence_, (stack->sym_data_[5].v_.uv0_).match_, (stack->sym_data_[5].v_.uv0_).variant_, &(stack->sym_data_[5].v_.uv0_).text_);
  if (r) return r;
}
              }
              C12:;
            }
            break;
            /* common-action: DOLLAR COLON start-c-tokenizer accept-whitespace CUBRACE_OPEN action-sequence end-c-tokenizer CUBRACE_CLOSE */
            case 8: {
              stack->continue_at_ = 13;
              {
                {
  /* Analogous to regular production actions, except we're moving it into the grammar's
   * current common action sequence. */
  snippet_clear(&g->current_common_action_sequence_);

  /* If action-sequence is empty, then we'd like to preserve this empty-ness by making the common-action 
   * empty as well. This allows generating code to discover that there is no common-action */
  if ((stack->sym_data_[5].v_.uv1_).action_sequence_.num_tokens_) {
    r = snippet_append(&g->current_common_action_sequence_, (stack->sym_data_[4].v_.uv0_).match_, (stack->sym_data_[4].v_.uv0_).variant_, &(stack->sym_data_[4].v_.uv0_).text_);
    if (r) return r;

    r = snippet_append_snippet(&g->current_common_action_sequence_, &(stack->sym_data_[5].v_.uv1_).action_sequence_);
    if (r) return r;

    r = snippet_append(&g->current_common_action_sequence_, (stack->sym_data_[7].v_.uv0_).match_, (stack->sym_data_[7].v_.uv0_).variant_, &(stack->sym_data_[7].v_.uv0_).text_);
    if (r) return r;
  }
}
              }
              C13:;
            }
            break;
            /* rule: */
            case 9: {
              stack->slot_1_has_sym_data_ = 1;
              stack->slot_1_sym_ = PRD_RULE;
              stack->continue_at_ = 14;
              {
                 prd_prod_init(&(stack->stack_[1].v_.uv1_));
              }
              C14:;
            }
            break;
            /* rule: rule IDENT */
            case 10: {
              stack->slot_1_has_sym_data_ = 1;
              stack->slot_1_sym_ = PRD_RULE;
              stack->continue_at_ = 15;
              {
                 prd_prod_init(&(stack->stack_[1].v_.uv1_));
              }
              C15:;
              stack->continue_at_ = 16;
              {
                {
	/* Grab ident from position 1, reduce */
	prd_prod_swap(&(stack->stack_[1].v_.uv1_), &(stack->sym_data_[0].v_.uv1_));
	pd = &(stack->stack_[1].v_.uv1_);
	if (pd->num_syms_ == pd->num_syms_allocated_) {
		r = prd_prod_check_sym_reserve(pd, &(stack->sym_data_[1].v_.uv0_).text_);
		if (r) return r;
	}
	struct prd_production_sym *pps = pd->syms_ + pd->num_syms_++;
	prd_production_sym_init(pps);
	r = xlts_append(&pps->id_, &(stack->sym_data_[1].v_.uv0_).text_);
	if (r) return PRD_INTERNAL_ERROR;
}
              }
              C16:;
            }
            break;
            /* stmt-action: EQUALS accept-whitespace */
            case 11: {
              stack->slot_1_has_sym_data_ = 1;
              stack->slot_1_sym_ = PRD_STMT_ACTION;
              stack->continue_at_ = 17;
              {
                 prd_prod_init(&(stack->stack_[1].v_.uv1_));
              }
              C17:;
            }
            break;
            /* stmt-action: stmt-action IDENT */
            case 12: {
              stack->slot_1_has_sym_data_ = 1;
              stack->slot_1_sym_ = PRD_STMT_ACTION;
              stack->continue_at_ = 18;
              {
                 prd_prod_init(&(stack->stack_[1].v_.uv1_));
              }
              C18:;
              stack->continue_at_ = 19;
              {
                {
	prd_prod_swap(&(stack->stack_[1].v_.uv1_), &(stack->sym_data_[0].v_.uv1_));
	pd = &(stack->stack_[1].v_.uv1_);
	r = snippet_append(&pd->action_sequence_, (stack->sym_data_[1].v_.uv0_).match_, (stack->sym_data_[1].v_.uv0_).variant_, &(stack->sym_data_[1].v_.uv0_).text_);
	if (r) return PRD_INTERNAL_ERROR;
}
              }
              C19:;
            }
            break;
            /* stmt-action: stmt-action COLON */
            case 13: {
              stack->slot_1_has_sym_data_ = 1;
              stack->slot_1_sym_ = PRD_STMT_ACTION;
              stack->continue_at_ = 20;
              {
                 prd_prod_init(&(stack->stack_[1].v_.uv1_));
              }
              C20:;
              stack->continue_at_ = 21;
              {
                {
	prd_prod_swap(&(stack->stack_[1].v_.uv1_), &(stack->sym_data_[0].v_.uv1_));
	pd = &(stack->stack_[1].v_.uv1_);
	r = snippet_append(&pd->action_sequence_, (stack->sym_data_[1].v_.uv0_).match_, (stack->sym_data_[1].v_.uv0_).variant_, &(stack->sym_data_[1].v_.uv0_).text_);
	if (r) return PRD_INTERNAL_ERROR;
}
              }
              C21:;
            }
            break;
            /* stmt-action: stmt-action EQUALS */
            case 14: {
              stack->slot_1_has_sym_data_ = 1;
              stack->slot_1_sym_ = PRD_STMT_ACTION;
              stack->continue_at_ = 22;
              {
                 prd_prod_init(&(stack->stack_[1].v_.uv1_));
              }
              C22:;
              stack->continue_at_ = 23;
              {
                {
	prd_prod_swap(&(stack->stack_[1].v_.uv1_), &(stack->sym_data_[0].v_.uv1_));
	pd = &(stack->stack_[1].v_.uv1_);
	r = snippet_append(&pd->action_sequence_, (stack->sym_data_[1].v_.uv0_).match_, (stack->sym_data_[1].v_.uv0_).variant_, &(stack->sym_data_[1].v_.uv0_).text_);
	if (r) return PRD_INTERNAL_ERROR;
}
              }
              C23:;
            }
            break;
            /* stmt-action: stmt-action TOKEN */
            case 15: {
              stack->slot_1_has_sym_data_ = 1;
              stack->slot_1_sym_ = PRD_STMT_ACTION;
              stack->continue_at_ = 24;
              {
                 prd_prod_init(&(stack->stack_[1].v_.uv1_));
              }
              C24:;
              stack->continue_at_ = 25;
              {
                {
	prd_prod_swap(&(stack->stack_[1].v_.uv1_), &(stack->sym_data_[0].v_.uv1_));
	pd = &(stack->stack_[1].v_.uv1_);
	r = snippet_append(&pd->action_sequence_, (stack->sym_data_[1].v_.uv0_).match_, (stack->sym_data_[1].v_.uv0_).variant_, &(stack->sym_data_[1].v_.uv0_).text_);
	if (r) return PRD_INTERNAL_ERROR;
}
              }
              C25:;
            }
            break;
            /* stmt-action: stmt-action DOLLAR */
            case 16: {
              stack->slot_1_has_sym_data_ = 1;
              stack->slot_1_sym_ = PRD_STMT_ACTION;
              stack->continue_at_ = 26;
              {
                 prd_prod_init(&(stack->stack_[1].v_.uv1_));
              }
              C26:;
              stack->continue_at_ = 27;
              {
                {
  prd_prod_swap(&(stack->stack_[1].v_.uv1_), &(stack->sym_data_[0].v_.uv1_));
  pd = &(stack->stack_[1].v_.uv1_);
  r = snippet_append(&pd->action_sequence_, (stack->sym_data_[1].v_.uv0_).match_, (stack->sym_data_[1].v_.uv0_).variant_, &(stack->sym_data_[1].v_.uv0_).text_);
  if (r) return PRD_INTERNAL_ERROR;
}
              }
              C27:;
            }
            break;
            /* stmt-action: stmt-action PAR_OPEN action-sequence PAR_CLOSE */
            case 17: {
              stack->slot_1_has_sym_data_ = 1;
              stack->slot_1_sym_ = PRD_STMT_ACTION;
              stack->continue_at_ = 28;
              {
                 prd_prod_init(&(stack->stack_[1].v_.uv1_));
              }
              C28:;
              stack->continue_at_ = 29;
              {
                {
	prd_prod_swap(&(stack->stack_[1].v_.uv1_), &(stack->sym_data_[0].v_.uv1_));
	pd = &(stack->stack_[1].v_.uv1_);

	/* Append the par-open to the snippets */
	r = snippet_append(&pd->action_sequence_, (stack->sym_data_[1].v_.uv0_).match_, (stack->sym_data_[1].v_.uv0_).variant_, &(stack->sym_data_[1].v_.uv0_).text_);
	if (r) return r;

	/* Move snippets in; could do this more efficiently by manually swapping pointers; but this works.. */
	r = snippet_append_snippet(&pd->action_sequence_, &(stack->sym_data_[2].v_.uv1_).action_sequence_);
	if (r) return r;

	/* Append the par-close to the snippets */
	r = snippet_append(&pd->action_sequence_, (stack->sym_data_[3].v_.uv0_).match_, (stack->sym_data_[3].v_.uv0_).variant_, &(stack->sym_data_[3].v_.uv0_).text_);
	if (r) return r;
}
              }
              C29:;
            }
            break;
            /* stmt-action: stmt-action CUBRACE_OPEN action-sequence CUBRACE_CLOSE */
            case 18: {
              stack->slot_1_has_sym_data_ = 1;
              stack->slot_1_sym_ = PRD_STMT_ACTION;
              stack->continue_at_ = 30;
              {
                 prd_prod_init(&(stack->stack_[1].v_.uv1_));
              }
              C30:;
              stack->continue_at_ = 31;
              {
                {
	prd_prod_swap(&(stack->stack_[1].v_.uv1_), &(stack->sym_data_[0].v_.uv1_));
	pd = &(stack->stack_[1].v_.uv1_);
	/* Append the cubrace-open to the snippets */
	r = snippet_append(&pd->action_sequence_, (stack->sym_data_[1].v_.uv0_).match_, (stack->sym_data_[1].v_.uv0_).variant_, &(stack->sym_data_[1].v_.uv0_).text_);
	if (r) return r;

	/* Move snippets in; could do this more efficiently by manually swapping pointers; but this works.. */
	r = snippet_append_snippet(&pd->action_sequence_, &(stack->sym_data_[2].v_.uv1_).action_sequence_);
	if (r) return r;

	/* Append the cubrace-close to the snippets */
	r = snippet_append(&pd->action_sequence_, (stack->sym_data_[3].v_.uv0_).match_, (stack->sym_data_[3].v_.uv0_).variant_, &(stack->sym_data_[3].v_.uv0_).text_);
	if (r) return r;
}
              }
              C31:;
            }
            break;
            /* action-sequence: */
            case 19: {
              stack->slot_1_has_sym_data_ = 1;
              stack->slot_1_sym_ = PRD_ACTION_SEQUENCE;
              stack->continue_at_ = 32;
              {
                 prd_prod_init(&(stack->stack_[1].v_.uv1_));
              }
              C32:;
            }
            break;
            /* action-sequence: action-sequence IDENT */
            case 20: {
              stack->slot_1_has_sym_data_ = 1;
              stack->slot_1_sym_ = PRD_ACTION_SEQUENCE;
              stack->continue_at_ = 33;
              {
                 prd_prod_init(&(stack->stack_[1].v_.uv1_));
              }
              C33:;
              stack->continue_at_ = 34;
              {
                {
	prd_prod_swap(&(stack->stack_[1].v_.uv1_), &(stack->sym_data_[0].v_.uv1_));
	pd = &(stack->stack_[1].v_.uv1_);
	r = snippet_append(&pd->action_sequence_, (stack->sym_data_[1].v_.uv0_).match_, (stack->sym_data_[1].v_.uv0_).variant_, &(stack->sym_data_[1].v_.uv0_).text_);
	if (r) return PRD_INTERNAL_ERROR;
}
              }
              C34:;
            }
            break;
            /* action-sequence: action-sequence COLON */
            case 21: {
              stack->slot_1_has_sym_data_ = 1;
              stack->slot_1_sym_ = PRD_ACTION_SEQUENCE;
              stack->continue_at_ = 35;
              {
                 prd_prod_init(&(stack->stack_[1].v_.uv1_));
              }
              C35:;
              stack->continue_at_ = 36;
              {
                {
	prd_prod_swap(&(stack->stack_[1].v_.uv1_), &(stack->sym_data_[0].v_.uv1_));
	pd = &(stack->stack_[1].v_.uv1_);
	r = snippet_append(&pd->action_sequence_, (stack->sym_data_[1].v_.uv0_).match_, (stack->sym_data_[1].v_.uv0_).variant_, &(stack->sym_data_[1].v_.uv0_).text_);
	if (r) return PRD_INTERNAL_ERROR;
}
              }
              C36:;
            }
            break;
            /* action-sequence: action-sequence EQUALS */
            case 22: {
              stack->slot_1_has_sym_data_ = 1;
              stack->slot_1_sym_ = PRD_ACTION_SEQUENCE;
              stack->continue_at_ = 37;
              {
                 prd_prod_init(&(stack->stack_[1].v_.uv1_));
              }
              C37:;
              stack->continue_at_ = 38;
              {
                {
	prd_prod_swap(&(stack->stack_[1].v_.uv1_), &(stack->sym_data_[0].v_.uv1_));
	pd = &(stack->stack_[1].v_.uv1_);
	r = snippet_append(&pd->action_sequence_, (stack->sym_data_[1].v_.uv0_).match_, (stack->sym_data_[1].v_.uv0_).variant_, &(stack->sym_data_[1].v_.uv0_).text_);
	if (r) return PRD_INTERNAL_ERROR;
}
              }
              C38:;
            }
            break;
            /* action-sequence: action-sequence SEMICOLON */
            case 23: {
              stack->slot_1_has_sym_data_ = 1;
              stack->slot_1_sym_ = PRD_ACTION_SEQUENCE;
              stack->continue_at_ = 39;
              {
                 prd_prod_init(&(stack->stack_[1].v_.uv1_));
              }
              C39:;
              stack->continue_at_ = 40;
              {
                {
	prd_prod_swap(&(stack->stack_[1].v_.uv1_), &(stack->sym_data_[0].v_.uv1_));
	pd = &(stack->stack_[1].v_.uv1_);
	r = snippet_append(&pd->action_sequence_, (stack->sym_data_[1].v_.uv0_).match_, (stack->sym_data_[1].v_.uv0_).variant_, &(stack->sym_data_[1].v_.uv0_).text_);
	if (r) return PRD_INTERNAL_ERROR;
}
              }
              C40:;
            }
            break;
            /* action-sequence: action-sequence TOKEN */
            case 24: {
              stack->slot_1_has_sym_data_ = 1;
              stack->slot_1_sym_ = PRD_ACTION_SEQUENCE;
              stack->continue_at_ = 41;
              {
                 prd_prod_init(&(stack->stack_[1].v_.uv1_));
              }
              C41:;
              stack->continue_at_ = 42;
              {
                {
	prd_prod_swap(&(stack->stack_[1].v_.uv1_), &(stack->sym_data_[0].v_.uv1_));
	pd = &(stack->stack_[1].v_.uv1_);
	r = snippet_append(&pd->action_sequence_, (stack->sym_data_[1].v_.uv0_).match_, (stack->sym_data_[1].v_.uv0_).variant_, &(stack->sym_data_[1].v_.uv0_).text_);
	if (r) return PRD_INTERNAL_ERROR;
}
              }
              C42:;
            }
            break;
            /* action-sequence: action-sequence DOLLAR */
            case 25: {
              stack->slot_1_has_sym_data_ = 1;
              stack->slot_1_sym_ = PRD_ACTION_SEQUENCE;
              stack->continue_at_ = 43;
              {
                 prd_prod_init(&(stack->stack_[1].v_.uv1_));
              }
              C43:;
              stack->continue_at_ = 44;
              {
                {
	prd_prod_swap(&(stack->stack_[1].v_.uv1_), &(stack->sym_data_[0].v_.uv1_));
	pd = &(stack->stack_[1].v_.uv1_);
	r = snippet_append(&pd->action_sequence_, (stack->sym_data_[1].v_.uv0_).match_, (stack->sym_data_[1].v_.uv0_).variant_, &(stack->sym_data_[1].v_.uv0_).text_);
	if (r) return PRD_INTERNAL_ERROR;
}
              }
              C44:;
            }
            break;
            /* action-sequence: action-sequence PAR_OPEN action-sequence PAR_CLOSE */
            case 26: {
              stack->slot_1_has_sym_data_ = 1;
              stack->slot_1_sym_ = PRD_ACTION_SEQUENCE;
              stack->continue_at_ = 45;
              {
                 prd_prod_init(&(stack->stack_[1].v_.uv1_));
              }
              C45:;
              stack->continue_at_ = 46;
              {
                {
	prd_prod_swap(&(stack->stack_[1].v_.uv1_), &(stack->sym_data_[0].v_.uv1_));
	pd = &(stack->stack_[1].v_.uv1_);
	/* Append the par-open to the snippets */
	r = snippet_append(&pd->action_sequence_, (stack->sym_data_[1].v_.uv0_).match_, (stack->sym_data_[1].v_.uv0_).variant_, &(stack->sym_data_[1].v_.uv0_).text_);
	if (r) return r;

	/* Move snippets in; could do this more efficiently by manually swapping pointers; but this works.. */
	r = snippet_append_snippet(&pd->action_sequence_, &(stack->sym_data_[2].v_.uv1_).action_sequence_);
	if (r) return r;

	/* Append the par-close to the snippets */
	r = snippet_append(&pd->action_sequence_, (stack->sym_data_[3].v_.uv0_).match_, (stack->sym_data_[3].v_.uv0_).variant_, &(stack->sym_data_[3].v_.uv0_).text_);
	if (r) return r;
}
              }
              C46:;
            }
            break;
            /* action-sequence: action-sequence CUBRACE_OPEN action-sequence CUBRACE_CLOSE */
            case 27: {
              stack->slot_1_has_sym_data_ = 1;
              stack->slot_1_sym_ = PRD_ACTION_SEQUENCE;
              stack->continue_at_ = 47;
              {
                 prd_prod_init(&(stack->stack_[1].v_.uv1_));
              }
              C47:;
              stack->continue_at_ = 48;
              {
                {
	prd_prod_swap(&(stack->stack_[1].v_.uv1_), &(stack->sym_data_[0].v_.uv1_));
	pd = &(stack->stack_[1].v_.uv1_);
	/* Append the cubrace-open to the snippets */
	r = snippet_append(&pd->action_sequence_, (stack->sym_data_[1].v_.uv0_).match_, (stack->sym_data_[1].v_.uv0_).variant_, &(stack->sym_data_[1].v_.uv0_).text_);
	if (r) return r;

	/* Move snippets in; could do this more efficiently by manually swapping pointers; but this works.. */
	r = snippet_append_snippet(&pd->action_sequence_, &(stack->sym_data_[2].v_.uv1_).action_sequence_);
	if (r) return r;

	/* Append the cubrace-close to the snippets */
	r = snippet_append(&pd->action_sequence_, (stack->sym_data_[3].v_.uv0_).match_, (stack->sym_data_[3].v_.uv0_).variant_, &(stack->sym_data_[3].v_.uv0_).text_);
	if (r) return r;
}
              }
              C48:;
            }
            break;
            /* start-c-tokenizer: */
            case 28: {
              stack->continue_at_ = 49;
              {
                tok_switch_to_c_idents(tkr);
              }
              C49:;
            }
            break;
            /* end-c-tokenizer: */
            case 29: {
              stack->continue_at_ = 50;
              {
                {
	tok_switch_to_nonterminal_idents(tkr);
	g->accept_whitespace_ = 0; /* Reset to normal tokens */
}
              }
              C50:;
            }
            break;
            /* accept-whitespace: */
            case 30: {
              stack->continue_at_ = 51;
              {
                {
	/* Welcome whitespace from this point. Note that this point is *after* the lookahead at the point
	 * that the PRD_ACCEPT_WHITESPACE non-terminal appears. Therefore, it is *after* the EQUALS sign and
	 * *after* the curly opening brace. */
	g->accept_whitespace_ = 1;
}
              }
              C51:;
            }
            break;
          } /* switch */
        } /* scope guard */
        
        /* Free symdata for every symbol in the production, including the first slot where we will soon
        * push nonterminal_data_reduced_to */
        size_t prd_sym_idx;
        for (prd_sym_idx = stack->pos_ - stack->current_production_length_; prd_sym_idx < stack->pos_; ++prd_sym_idx) {
          switch (stack->stack_[prd_sym_idx].state_) {
            case 2: /* semicolon */
            case 3: /* semicolon */
            case 6: /* cubrace-close */
            case 9: /* cubrace-open */
            case 12: /* ident */
            case 14: /* colon */
            case 15: /* ident */
            case 17: /* semicolon */
            case 19: /* ident */
            case 20: /* colon */
            case 21: /* equals */
            case 22: /* token */
            case 23: /* dollar */
            case 24: /* par-close */
            case 26: /* par-open */
            case 27: /* cubrace-close */
            case 29: /* cubrace-open */
            case 32: /* equals */
            case 33: /* cubrace-close */
            case 35: /* ident */
            case 36: /* colon */
            case 37: /* equals */
            case 38: /* semicolon */
            case 39: /* token */
            case 40: /* dollar */
            case 41: /* par-close */
            case 43: /* par-open */
            case 44: /* cubrace-close */
            case 46: /* cubrace-open */
            case 48: /* cubrace-open */
            case 51: /* colon */
            case 52: /* dollar */
            stack->continue_at_ = 52;
            {
               xlts_cleanup(&((stack->stack_ + prd_sym_idx)->v_.uv0_).text_);
            }
            C52:;
            break;
            case 1: /* production */
            case 5: /* stmt-action */
            case 8: /* action-sequence */
            case 13: /* rule */
            case 25: /* action-sequence */
            case 28: /* action-sequence */
            case 30: /* stmt-action */
            case 42: /* action-sequence */
            case 45: /* action-sequence */
            case 47: /* action-sequence */
            stack->continue_at_ = 53;
            {
               prd_prod_cleanup(&((stack->stack_ + prd_sym_idx)->v_.uv1_));
            }
            C53:;
            break;
          } /* switch */
        } /* for */
        stack->pos_ -= stack->current_production_length_;
        action = prd_parse_table[prd_num_columns * stack->stack_[stack->pos_ - 1].state_ + (stack->current_production_nonterminal_ - prd_minimum_sym)];
        if (action <= 0) {
          stack->continue_at_ = 0;
          {
            re_error_tkr(tkr, "Error: internal error\n"); return PRD_INTERNAL_ERROR;
            
          }
        }
        switch (prd_push_state(stack, action /* action for a "goto" shift is the ordinal */)) {
          case _PRD_OVERFLOW: {
            stack->continue_at_ = 0;
            {
              re_error_tkr(tkr, "Error: internal error\n"); return PRD_INTERNAL_ERROR;
              
            }
          }
          break;
          case _PRD_NO_MEMORY: {
            stack->continue_at_ = 0;
            {
              re_error_tkr(tkr, "Error: no memory"); return PRD_INTERNAL_ERROR;
              
            }
          }
          break;
        } /* switch */
        struct prd_sym_data *sd = stack->stack_ + stack->pos_ - 1;
        *sd = stack->stack_[1];
        sd->state_ = action;
        stack->slot_1_has_common_data_ = 0;
        stack->slot_1_has_sym_data_ = 0;
        stack->top_of_stack_has_common_data_ = 1;
        stack->top_of_stack_has_sym_data_ = 1;
      } /* action < 0 */
      else /* action == 0 */ {
        /* check if we can recover using an error token. */
        size_t n;
        for (n = 0; n < stack->pos_; ++n) {
          int err_action = prd_parse_table[prd_num_columns * stack->stack_[n].state_ + (13 /* error token */ - prd_minimum_sym)];
          if (err_action > 0) {
            /* we can transition on the error token somewhere on the stack */
            break;
          }
        }
        /* Enter error-token recovery mode given that such a recovery is possible */
        stack->error_recovery_ = (n != stack->pos_);
        /* Issue the error here */
        if (!stack->mute_error_turns_) {
          stack->mute_error_turns_ = 3;
          stack->continue_at_ = 0;
          {
            /* Syntax error */ \
            if (sym != PRD_INPUT_END) {\
              re_error_tkr(tkr, "Syntax error \"%s\" not expected", tkr->xmatch_.translated_); \
              } \
            else { \
              re_error_tkr(tkr, "Syntax error: end of input not expected");   \
              } \
            return PRD_SYNTAX_ERROR;
            
          }
        }
        else {
          stack->mute_error_turns_--;
          if (sym == PRD_INPUT_END) {
            /* EOF means we cannot shift to recover, and errors are muted, so return completion */
            stack->pending_reset_ = 1;
            return PRD_SUCCESS;
          }
          /* Retain EOF but discard any other sym so we make progress */
          stack->continue_at_ = 0;
          {
            return PRD_NEXT;
            
          }
        }
      }
    } /* !stack->error_recovery_ */
    if (stack->error_recovery_) {
      size_t n;
      n = stack->pos_;
      if (n) {
        do {
          --n;
          /* Can we shift an error token? */
          int err_action = prd_parse_table[prd_num_columns * stack->stack_[n].state_ + (13 /* error token */ - prd_minimum_sym)];
          if (err_action > 0) {
            /* Does the resulting state accept the current symbol? */
            int err_sym_action = prd_parse_table[prd_num_columns * err_action + (sym - prd_minimum_sym)];
            if (err_sym_action) {
              /* Current symbol is accepted, recover error condition by shifting the error token and then process the symbol as usual */
              /* Free symdata for every symbol up to the state where we will shift the error token */
              size_t prd_sym_idx;
              for (prd_sym_idx = n + 1; prd_sym_idx < stack->pos_; ++prd_sym_idx) {
                switch (stack->stack_[prd_sym_idx].state_) {
                  case 2: /* semicolon */
                  case 3: /* semicolon */
                  case 6: /* cubrace-close */
                  case 9: /* cubrace-open */
                  case 12: /* ident */
                  case 14: /* colon */
                  case 15: /* ident */
                  case 17: /* semicolon */
                  case 19: /* ident */
                  case 20: /* colon */
                  case 21: /* equals */
                  case 22: /* token */
                  case 23: /* dollar */
                  case 24: /* par-close */
                  case 26: /* par-open */
                  case 27: /* cubrace-close */
                  case 29: /* cubrace-open */
                  case 32: /* equals */
                  case 33: /* cubrace-close */
                  case 35: /* ident */
                  case 36: /* colon */
                  case 37: /* equals */
                  case 38: /* semicolon */
                  case 39: /* token */
                  case 40: /* dollar */
                  case 41: /* par-close */
                  case 43: /* par-open */
                  case 44: /* cubrace-close */
                  case 46: /* cubrace-open */
                  case 48: /* cubrace-open */
                  case 51: /* colon */
                  case 52: /* dollar */
                  stack->continue_at_ = 54;
                  {
                     xlts_cleanup(&((stack->stack_ + prd_sym_idx)->v_.uv0_).text_);
                  }
                  C54:;
                  break;
                  case 1: /* production */
                  case 5: /* stmt-action */
                  case 8: /* action-sequence */
                  case 13: /* rule */
                  case 25: /* action-sequence */
                  case 28: /* action-sequence */
                  case 30: /* stmt-action */
                  case 42: /* action-sequence */
                  case 45: /* action-sequence */
                  case 47: /* action-sequence */
                  stack->continue_at_ = 55;
                  {
                     prd_prod_cleanup(&((stack->stack_ + prd_sym_idx)->v_.uv1_));
                  }
                  C55:;
                  break;
                } /* switch */
              } /* for */
              stack->pos_ = n + 1;
              /* Push the state of the error transition */
              switch (prd_push_state(stack, err_action)) {
                case _PRD_OVERFLOW: {
                  stack->continue_at_ = 0;
                  {
                    re_error_tkr(tkr, "Error: internal error\n"); return PRD_INTERNAL_ERROR;
                    
                  }
                }
                break;
                case _PRD_NO_MEMORY: {
                  stack->continue_at_ = 0;
                  {
                    re_error_tkr(tkr, "Error: no memory"); return PRD_INTERNAL_ERROR;
                    
                  }
                }
                break;
              } /* switch */
              stack->error_recovery_ = 0;
              /* Break out of do { .. } while loop, we've recovered */
              break;
            } /* if (err_sym_action) (if the current sym can continue after an error transition) */
          } /* if (err_action) (if the state at position 'n' can accept an error transition) */
        } while (n);
      }
      if (stack->error_recovery_) {
        /* Did not yet recover, discard current sym and get next */
        if (sym == PRD_INPUT_END) {
          /* EOF means we cannot shift to recover, so return completion */
          stack->pending_reset_ = 1;
          return PRD_SUCCESS;
        }
        stack->continue_at_ = 0;
        {
          return PRD_NEXT;
          
        }
      }
    } /* stack->error_recovery_ */
  } /* for (;;) */
}
/* --------- END OF GENERATED CODE ------------ */

int prd_parse_tkr(struct prd_stack *stack, struct prd_grammar *g, struct tkr_tokenizer *tkr, int end_of_input, struct symbol_table *st) {
  int sym;

  if (!end_of_input) {
    token_type_t tkt = (token_type_t)tkr->best_match_variant_;
    if (!g->accept_whitespace_ && (tkt == TOK_WHITESPACE)) {
      /* Eat whitespace */
      return PRD_NEXT;
    }
    switch (tkt) {
    case TOK_IDENT: sym = PRD_IDENT; break;
    case TOK_COLON: sym = PRD_COLON; break;
    case TOK_EQUALS: sym = PRD_EQUALS; break;
    case TOK_SEMICOLON: sym = PRD_SEMICOLON; break;
    case TOK_PAR_OPEN: sym = PRD_PAR_OPEN; break;
    case TOK_PAR_CLOSE: sym = PRD_PAR_CLOSE; break;
    case TOK_CUBRACE_OPEN: sym = PRD_CUBRACE_OPEN; break;
    case TOK_CUBRACE_CLOSE: sym = PRD_CUBRACE_CLOSE; break;
    case TOK_SPECIAL_IDENT: 
      if (!strcmp("$", tkr_str(tkr))) {
        /* Single '$' is recognized as a dollar.. */
        sym = PRD_DOLLAR;
      }
      else {
        sym = PRD_TOKEN;
      }
      break;
    default: sym = PRD_TOKEN; break;
    }
  }
  else {
    sym = PRD_INPUT_END;
  }
  
#if 0
  switch (prd_parse(stack, sym, g, tkr, st)) {
  case -2: return PRD_INTERNAL_ERROR;
  case -1: return PRD_SYNTAX_ERROR;
  case 0: return PRD_SUCCESS;
  case 1: return PRD_NEXT;
  default: return PRD_INTERNAL_ERROR;
  }
#else
  return prd_parse(stack, sym, g, tkr, st);
#endif
}
