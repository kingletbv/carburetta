/* Copyright 2020 Kinglet B.V.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http ://www.apache.org/licenses/LICENSE-2.0
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
static const int prd_state_syms[] = {
 -1,
 17,
 6,
 6,
 21,
 19,
 11,
 21,
 18,
 10,
 22,
 20,
 3,
 16,
 4,
 3,
 23,
 6,
 21,
 3,
 4,
 5,
 7,
 12,
 9,
 18,
 8,
 11,
 18,
 10,
 19,
 22,
 5,
 11,
 21,
 3,
 4,
 5,
 6,
 7,
 12,
 9,
 18,
 8,
 11,
 18,
 10,
 18,
 10,
 22,
 20,
 4,
 12,
 15
};

#ifndef CARB_PRD_SRCPRD_GRAMMAR_H_INCLUDED
struct prd_stack {
  int error_recovery_:1;
  int report_error_:1;
  int mute_error_turns_;
  size_t pos_, num_stack_allocated_;
  struct prd_sym_data *stack_;
};
#define _PRD_MATCH 1
#define _PRD_OVERFLOW 2
#define _PRD_NO_MEMORY 3
#define _PRD_FEED_ME 4
#define _PRD_END_OF_INPUT 5
#define _PRD_SYNTAX_ERROR 6
#define _PRD_INTERNAL_ERROR 7

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
#endif /* CARB_PRD_SRCPRD_GRAMMAR_H_INCLUDED */

void prd_stack_init(struct prd_stack *stack) {
  stack->error_recovery_ = 0;
  stack->report_error_ = 0;
  stack->mute_error_turns_ = 0;
  stack->pos_ = 0;
  stack->num_stack_allocated_ = 0;
  stack->stack_ = NULL;
}

void prd_stack_cleanup(struct prd_stack *stack) {
  size_t n;
  for (n = 1; n < stack->pos_; ++n) {
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
       xlts_cleanup(&((stack->stack_ + n)->v_.uv0_).text_);

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
       prd_prod_cleanup(&((stack->stack_ + n)->v_.uv1_));

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
  for (n = 1; n < stack->pos_; ++n) {
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
     xlts_cleanup(&((stack->stack_ + n)->v_.uv0_).text_);

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
     prd_prod_cleanup(&((stack->stack_ + n)->v_.uv1_));

    }
    break;
    } /* switch */
  }
  stack->pos_ = 0;
  stack->error_recovery_ = 0;
  stack->report_error_ = 0;
  stack->mute_error_turns_ = 0;
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

int prd_parse(struct prd_stack *stack, int sym, struct prd_grammar *g, struct tkr_tokenizer *tkr, struct symbol_table *st) {
  if (stack->mute_error_turns_) stack->mute_error_turns_--;
  for (;;) {
    if (!stack->error_recovery_) {
      int action = prd_parse_table[prd_num_columns * stack->stack_[stack->pos_ - 1].state_ + (sym - prd_minimum_sym)];
      if (action > 0) {
        switch (prd_push_state(stack, action /* action for a shift is the ordinal */)) {
          case _PRD_OVERFLOW: {
            re_error_tkr(tkr, "Error: internal error\n"); return PRD_INTERNAL_ERROR;
          }
          break;
          case _PRD_NO_MEMORY: {
            re_error_tkr(tkr, "Error: no memory"); return PRD_INTERNAL_ERROR;
          }
          break;
        } /* switch */

        struct prd_sym_data *sym_data = stack->stack_ + stack->pos_ - 1;
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
            {
               (sym_data->v_.uv0_).match_ = (sym_data->v_.uv0_).variant_ = 0; \
             xlts_init(&(sym_data->v_.uv0_).text_);
            }
            {
               (sym_data->v_.uv0_).match_ = tkr->best_match_action_; \
              (sym_data->v_.uv0_).variant_ = tkr->best_match_variant_; \
			  xlts_append(&(sym_data->v_.uv0_).text_, &tkr->xmatch_);
            }
            break;
            {
               prd_prod_init(&(sym_data->v_.uv1_));
            }
            break;
        } /* switch */
        if (stack->report_error_) {
          /* We're shifting this sym following an error recovery on the same sym, report syntax error */
          stack->report_error_ = 0;
          /* Syntax error */ \
  if (sym != PRD_INPUT_END) {\
    re_error_tkr(tkr, "Syntax error \"%s\" not expected", tkr->xmatch_.translated_); \
  } \
  else { \
    re_error_tkr(tkr, "Syntax error: end of input not expected");   \
  } \
  return PRD_SYNTAX_ERROR;
        }
        else {
          return PRD_NEXT;
        }
      } /* action > 0 */
      else if (action < 0) {
        int production = -action - 1;
        int discard_action = 0;
        size_t production_length = prd_production_lengths[production];
        int nonterminal = prd_production_syms[production];
        if (0 == production) {
          return PRD_SUCCESS;
        }

        struct prd_sym_data nonterminal_sym_data_reduced_to = { 0 };
        { /* scope guard */
          struct prd_sym_data *sym_data = stack->stack_ + stack->pos_ - production_length;
          int r; \
        struct prd_production *pd;
          switch (production) {
            /* grammar: */
            case 1: {
    { }
            }
            break;
            /* grammar: grammar production */
            case 2: {
    {
	/* Store the PRD_PRODUCTION in the prd_grammar->productions_ array */
	r = prd_grammar_check_production_reserve(g);
	if (r) return r;
	pd = g->productions_ + g->num_productions_++;
	prd_prod_init(pd);
	prd_prod_swap(pd, &(sym_data[1].v_.uv1_));
  r = snippet_append_snippet(&pd->common_action_sequence_, &g->current_common_action_sequence_);
  if (r) return r;
}
            }
            break;
            /* grammar: grammar common-action */
            case 3: {
    {
  /* Common actions are recorded in their own productions */
}
            }
            break;
            /* production: IDENT COLON rule SEMICOLON */
            case 4: {
     prd_prod_init(&(nonterminal_sym_data_reduced_to.v_.uv1_));

            }
            {
              {
	prd_prod_swap(&(nonterminal_sym_data_reduced_to.v_.uv1_), &(sym_data[2].v_.uv1_));
	pd = &(nonterminal_sym_data_reduced_to.v_.uv1_);
	xlts_append(&pd->nt_.id_, &(sym_data[0].v_.uv0_).text_);
}
            }
            break;
            /* production: IDENT COLON rule start-c-tokenizer stmt-action end-c-tokenizer SEMICOLON */
            case 5: {
     prd_prod_init(&(nonterminal_sym_data_reduced_to.v_.uv1_));

            }
            {
              {
	prd_prod_swap(&(nonterminal_sym_data_reduced_to.v_.uv1_), &(sym_data[2].v_.uv1_));
	pd = &(nonterminal_sym_data_reduced_to.v_.uv1_);
	xlts_append(&pd->nt_.id_, &(sym_data[0].v_.uv0_).text_);

	/* Move snippets in; could do this more efficiently by manually swapping pointers; but this works.. */
	r = snippet_append_snippet(&pd->action_sequence_, & (sym_data[4].v_.uv1_).action_sequence_);
	if (r) return r;

	/* Append the semicolon to the snippets */
	r = snippet_append(&pd->action_sequence_, (sym_data[6].v_.uv0_).match_, (sym_data[6].v_.uv0_).variant_, &(sym_data[6].v_.uv0_).text_);
	if (r) return r;
}
            }
            break;
            /* production: IDENT COLON rule start-c-tokenizer accept-whitespace CUBRACE_OPEN action-sequence end-c-tokenizer CUBRACE_CLOSE */
            case 6: {
     prd_prod_init(&(nonterminal_sym_data_reduced_to.v_.uv1_));

            }
            {
              {
	prd_prod_swap(&(nonterminal_sym_data_reduced_to.v_.uv1_), &(sym_data[2].v_.uv1_));
	pd = &(nonterminal_sym_data_reduced_to.v_.uv1_);
	xlts_append(&pd->nt_.id_, &(sym_data[0].v_.uv0_).text_);

	/* Move snippets in; could do this more efficiently by manually swapping pointers; but this works.. */
	/* Append the cubrace-open to the snippets */
	r = snippet_append(&pd->action_sequence_, (sym_data[5].v_.uv0_).match_, (sym_data[5].v_.uv0_).variant_, &(sym_data[5].v_.uv0_).text_);
	if (r) return r;

	/* Move snippets in; could do this more efficiently by manually swapping pointers; but this works.. */
	r = snippet_append_snippet(&pd->action_sequence_, &(sym_data[6].v_.uv1_).action_sequence_);
	if (r) return r;

	/* Append the cubrace-close to the snippets */
	r = snippet_append(&pd->action_sequence_, (sym_data[8].v_.uv0_).match_, (sym_data[8].v_.uv0_).variant_, &(sym_data[8].v_.uv0_).text_);
	if (r) return r;
}
            }
            break;
            /* common-action: DOLLAR COLON start-c-tokenizer stmt-action end-c-tokenizer SEMICOLON */
            case 7: {
    {
  /* Analogous to regular production actions, except we're moving it into the grammar's
   * current common action sequence. */
  snippet_clear(&g->current_common_action_sequence_);

  /* append stmt-action */
  r = snippet_append_snippet(&g->current_common_action_sequence_, &(sym_data[3].v_.uv1_).action_sequence_);
  if (r) return r;
  
  /* append semicolon */
  r = snippet_append(&g->current_common_action_sequence_, (sym_data[5].v_.uv0_).match_, (sym_data[5].v_.uv0_).variant_, &(sym_data[5].v_.uv0_).text_);
  if (r) return r;
}
            }
            break;
            /* common-action: DOLLAR COLON start-c-tokenizer accept-whitespace CUBRACE_OPEN action-sequence end-c-tokenizer CUBRACE_CLOSE */
            case 8: {
    {
  /* Analogous to regular production actions, except we're moving it into the grammar's
   * current common action sequence. */
  snippet_clear(&g->current_common_action_sequence_);

  /* If action-sequence is empty, then we'd like to preserve this empty-ness by making the common-action 
   * empty as well. This allows generating code to discover that there is no common-action */
  if ((sym_data[5].v_.uv1_).action_sequence_.num_tokens_) {
    r = snippet_append(&g->current_common_action_sequence_, (sym_data[4].v_.uv0_).match_, (sym_data[4].v_.uv0_).variant_, &(sym_data[4].v_.uv0_).text_);
    if (r) return r;

    r = snippet_append_snippet(&g->current_common_action_sequence_, &(sym_data[5].v_.uv1_).action_sequence_);
    if (r) return r;

    r = snippet_append(&g->current_common_action_sequence_, (sym_data[7].v_.uv0_).match_, (sym_data[7].v_.uv0_).variant_, &(sym_data[7].v_.uv0_).text_);
    if (r) return r;
  }
}
            }
            break;
            /* rule: */
            case 9: {
     prd_prod_init(&(nonterminal_sym_data_reduced_to.v_.uv1_));

            }
            {
              
            }
            break;
            /* rule: rule IDENT */
            case 10: {
     prd_prod_init(&(nonterminal_sym_data_reduced_to.v_.uv1_));

            }
            {
              {
	/* Grab ident from position 1, reduce */
	prd_prod_swap(&(nonterminal_sym_data_reduced_to.v_.uv1_), &(sym_data[0].v_.uv1_));
	pd = &(nonterminal_sym_data_reduced_to.v_.uv1_);
	if (pd->num_syms_ == pd->num_syms_allocated_) {
		r = prd_prod_check_sym_reserve(pd, &(sym_data[1].v_.uv0_).text_);
		if (r) return r;
	}
	struct prd_production_sym *pps = pd->syms_ + pd->num_syms_++;
	prd_production_sym_init(pps);
	r = xlts_append(&pps->id_, &(sym_data[1].v_.uv0_).text_);
	if (r) return PRD_INTERNAL_ERROR;
}
            }
            break;
            /* stmt-action: EQUALS accept-whitespace */
            case 11: {
     prd_prod_init(&(nonterminal_sym_data_reduced_to.v_.uv1_));

            }
            {
              
            }
            break;
            /* stmt-action: stmt-action IDENT */
            case 12: {
     prd_prod_init(&(nonterminal_sym_data_reduced_to.v_.uv1_));

            }
            {
              {
	prd_prod_swap(&(nonterminal_sym_data_reduced_to.v_.uv1_), &(sym_data[0].v_.uv1_));
	pd = &(nonterminal_sym_data_reduced_to.v_.uv1_);
	r = snippet_append(&pd->action_sequence_, (sym_data[1].v_.uv0_).match_, (sym_data[1].v_.uv0_).variant_, &(sym_data[1].v_.uv0_).text_);
	if (r) return PRD_INTERNAL_ERROR;
}
            }
            break;
            /* stmt-action: stmt-action COLON */
            case 13: {
     prd_prod_init(&(nonterminal_sym_data_reduced_to.v_.uv1_));

            }
            {
              {
	prd_prod_swap(&(nonterminal_sym_data_reduced_to.v_.uv1_), &(sym_data[0].v_.uv1_));
	pd = &(nonterminal_sym_data_reduced_to.v_.uv1_);
	r = snippet_append(&pd->action_sequence_, (sym_data[1].v_.uv0_).match_, (sym_data[1].v_.uv0_).variant_, &(sym_data[1].v_.uv0_).text_);
	if (r) return PRD_INTERNAL_ERROR;
}
            }
            break;
            /* stmt-action: stmt-action EQUALS */
            case 14: {
     prd_prod_init(&(nonterminal_sym_data_reduced_to.v_.uv1_));

            }
            {
              {
	prd_prod_swap(&(nonterminal_sym_data_reduced_to.v_.uv1_), &(sym_data[0].v_.uv1_));
	pd = &(nonterminal_sym_data_reduced_to.v_.uv1_);
	r = snippet_append(&pd->action_sequence_, (sym_data[1].v_.uv0_).match_, (sym_data[1].v_.uv0_).variant_, &(sym_data[1].v_.uv0_).text_);
	if (r) return PRD_INTERNAL_ERROR;
}
            }
            break;
            /* stmt-action: stmt-action TOKEN */
            case 15: {
     prd_prod_init(&(nonterminal_sym_data_reduced_to.v_.uv1_));

            }
            {
              {
	prd_prod_swap(&(nonterminal_sym_data_reduced_to.v_.uv1_), &(sym_data[0].v_.uv1_));
	pd = &(nonterminal_sym_data_reduced_to.v_.uv1_);
	r = snippet_append(&pd->action_sequence_, (sym_data[1].v_.uv0_).match_, (sym_data[1].v_.uv0_).variant_, &(sym_data[1].v_.uv0_).text_);
	if (r) return PRD_INTERNAL_ERROR;
}
            }
            break;
            /* stmt-action: stmt-action DOLLAR */
            case 16: {
     prd_prod_init(&(nonterminal_sym_data_reduced_to.v_.uv1_));

            }
            {
              {
  prd_prod_swap(&(nonterminal_sym_data_reduced_to.v_.uv1_), &(sym_data[0].v_.uv1_));
  pd = &(nonterminal_sym_data_reduced_to.v_.uv1_);
  r = snippet_append(&pd->action_sequence_, (sym_data[1].v_.uv0_).match_, (sym_data[1].v_.uv0_).variant_, &(sym_data[1].v_.uv0_).text_);
  if (r) return PRD_INTERNAL_ERROR;
}
            }
            break;
            /* stmt-action: stmt-action PAR_OPEN action-sequence PAR_CLOSE */
            case 17: {
     prd_prod_init(&(nonterminal_sym_data_reduced_to.v_.uv1_));

            }
            {
              {
	prd_prod_swap(&(nonterminal_sym_data_reduced_to.v_.uv1_), &(sym_data[0].v_.uv1_));
	pd = &(nonterminal_sym_data_reduced_to.v_.uv1_);

	/* Append the par-open to the snippets */
	r = snippet_append(&pd->action_sequence_, (sym_data[1].v_.uv0_).match_, (sym_data[1].v_.uv0_).variant_, &(sym_data[1].v_.uv0_).text_);
	if (r) return r;

	/* Move snippets in; could do this more efficiently by manually swapping pointers; but this works.. */
	r = snippet_append_snippet(&pd->action_sequence_, &(sym_data[2].v_.uv1_).action_sequence_);
	if (r) return r;

	/* Append the par-close to the snippets */
	r = snippet_append(&pd->action_sequence_, (sym_data[3].v_.uv0_).match_, (sym_data[3].v_.uv0_).variant_, &(sym_data[3].v_.uv0_).text_);
	if (r) return r;
}
            }
            break;
            /* stmt-action: stmt-action CUBRACE_OPEN action-sequence CUBRACE_CLOSE */
            case 18: {
     prd_prod_init(&(nonterminal_sym_data_reduced_to.v_.uv1_));

            }
            {
              {
	prd_prod_swap(&(nonterminal_sym_data_reduced_to.v_.uv1_), &(sym_data[0].v_.uv1_));
	pd = &(nonterminal_sym_data_reduced_to.v_.uv1_);
	/* Append the cubrace-open to the snippets */
	r = snippet_append(&pd->action_sequence_, (sym_data[1].v_.uv0_).match_, (sym_data[1].v_.uv0_).variant_, &(sym_data[1].v_.uv0_).text_);
	if (r) return r;

	/* Move snippets in; could do this more efficiently by manually swapping pointers; but this works.. */
	r = snippet_append_snippet(&pd->action_sequence_, &(sym_data[2].v_.uv1_).action_sequence_);
	if (r) return r;

	/* Append the cubrace-close to the snippets */
	r = snippet_append(&pd->action_sequence_, (sym_data[3].v_.uv0_).match_, (sym_data[3].v_.uv0_).variant_, &(sym_data[3].v_.uv0_).text_);
	if (r) return r;
}
            }
            break;
            /* action-sequence: */
            case 19: {
     prd_prod_init(&(nonterminal_sym_data_reduced_to.v_.uv1_));

            }
            {
              
            }
            break;
            /* action-sequence: action-sequence IDENT */
            case 20: {
     prd_prod_init(&(nonterminal_sym_data_reduced_to.v_.uv1_));

            }
            {
              {
	prd_prod_swap(&(nonterminal_sym_data_reduced_to.v_.uv1_), &(sym_data[0].v_.uv1_));
	pd = &(nonterminal_sym_data_reduced_to.v_.uv1_);
	r = snippet_append(&pd->action_sequence_, (sym_data[1].v_.uv0_).match_, (sym_data[1].v_.uv0_).variant_, &(sym_data[1].v_.uv0_).text_);
	if (r) return PRD_INTERNAL_ERROR;
}
            }
            break;
            /* action-sequence: action-sequence COLON */
            case 21: {
     prd_prod_init(&(nonterminal_sym_data_reduced_to.v_.uv1_));

            }
            {
              {
	prd_prod_swap(&(nonterminal_sym_data_reduced_to.v_.uv1_), &(sym_data[0].v_.uv1_));
	pd = &(nonterminal_sym_data_reduced_to.v_.uv1_);
	r = snippet_append(&pd->action_sequence_, (sym_data[1].v_.uv0_).match_, (sym_data[1].v_.uv0_).variant_, &(sym_data[1].v_.uv0_).text_);
	if (r) return PRD_INTERNAL_ERROR;
}
            }
            break;
            /* action-sequence: action-sequence EQUALS */
            case 22: {
     prd_prod_init(&(nonterminal_sym_data_reduced_to.v_.uv1_));

            }
            {
              {
	prd_prod_swap(&(nonterminal_sym_data_reduced_to.v_.uv1_), &(sym_data[0].v_.uv1_));
	pd = &(nonterminal_sym_data_reduced_to.v_.uv1_);
	r = snippet_append(&pd->action_sequence_, (sym_data[1].v_.uv0_).match_, (sym_data[1].v_.uv0_).variant_, &(sym_data[1].v_.uv0_).text_);
	if (r) return PRD_INTERNAL_ERROR;
}
            }
            break;
            /* action-sequence: action-sequence SEMICOLON */
            case 23: {
     prd_prod_init(&(nonterminal_sym_data_reduced_to.v_.uv1_));

            }
            {
              {
	prd_prod_swap(&(nonterminal_sym_data_reduced_to.v_.uv1_), &(sym_data[0].v_.uv1_));
	pd = &(nonterminal_sym_data_reduced_to.v_.uv1_);
	r = snippet_append(&pd->action_sequence_, (sym_data[1].v_.uv0_).match_, (sym_data[1].v_.uv0_).variant_, &(sym_data[1].v_.uv0_).text_);
	if (r) return PRD_INTERNAL_ERROR;
}
            }
            break;
            /* action-sequence: action-sequence TOKEN */
            case 24: {
     prd_prod_init(&(nonterminal_sym_data_reduced_to.v_.uv1_));

            }
            {
              {
	prd_prod_swap(&(nonterminal_sym_data_reduced_to.v_.uv1_), &(sym_data[0].v_.uv1_));
	pd = &(nonterminal_sym_data_reduced_to.v_.uv1_);
	r = snippet_append(&pd->action_sequence_, (sym_data[1].v_.uv0_).match_, (sym_data[1].v_.uv0_).variant_, &(sym_data[1].v_.uv0_).text_);
	if (r) return PRD_INTERNAL_ERROR;
}
            }
            break;
            /* action-sequence: action-sequence DOLLAR */
            case 25: {
     prd_prod_init(&(nonterminal_sym_data_reduced_to.v_.uv1_));

            }
            {
              {
	prd_prod_swap(&(nonterminal_sym_data_reduced_to.v_.uv1_), &(sym_data[0].v_.uv1_));
	pd = &(nonterminal_sym_data_reduced_to.v_.uv1_);
	r = snippet_append(&pd->action_sequence_, (sym_data[1].v_.uv0_).match_, (sym_data[1].v_.uv0_).variant_, &(sym_data[1].v_.uv0_).text_);
	if (r) return PRD_INTERNAL_ERROR;
}
            }
            break;
            /* action-sequence: action-sequence PAR_OPEN action-sequence PAR_CLOSE */
            case 26: {
     prd_prod_init(&(nonterminal_sym_data_reduced_to.v_.uv1_));

            }
            {
              {
	prd_prod_swap(&(nonterminal_sym_data_reduced_to.v_.uv1_), &(sym_data[0].v_.uv1_));
	pd = &(nonterminal_sym_data_reduced_to.v_.uv1_);
	/* Append the par-open to the snippets */
	r = snippet_append(&pd->action_sequence_, (sym_data[1].v_.uv0_).match_, (sym_data[1].v_.uv0_).variant_, &(sym_data[1].v_.uv0_).text_);
	if (r) return r;

	/* Move snippets in; could do this more efficiently by manually swapping pointers; but this works.. */
	r = snippet_append_snippet(&pd->action_sequence_, &(sym_data[2].v_.uv1_).action_sequence_);
	if (r) return r;

	/* Append the par-close to the snippets */
	r = snippet_append(&pd->action_sequence_, (sym_data[3].v_.uv0_).match_, (sym_data[3].v_.uv0_).variant_, &(sym_data[3].v_.uv0_).text_);
	if (r) return r;
}
            }
            break;
            /* action-sequence: action-sequence CUBRACE_OPEN action-sequence CUBRACE_CLOSE */
            case 27: {
     prd_prod_init(&(nonterminal_sym_data_reduced_to.v_.uv1_));

            }
            {
              {
	prd_prod_swap(&(nonterminal_sym_data_reduced_to.v_.uv1_), &(sym_data[0].v_.uv1_));
	pd = &(nonterminal_sym_data_reduced_to.v_.uv1_);
	/* Append the cubrace-open to the snippets */
	r = snippet_append(&pd->action_sequence_, (sym_data[1].v_.uv0_).match_, (sym_data[1].v_.uv0_).variant_, &(sym_data[1].v_.uv0_).text_);
	if (r) return r;

	/* Move snippets in; could do this more efficiently by manually swapping pointers; but this works.. */
	r = snippet_append_snippet(&pd->action_sequence_, &(sym_data[2].v_.uv1_).action_sequence_);
	if (r) return r;

	/* Append the cubrace-close to the snippets */
	r = snippet_append(&pd->action_sequence_, (sym_data[3].v_.uv0_).match_, (sym_data[3].v_.uv0_).variant_, &(sym_data[3].v_.uv0_).text_);
	if (r) return r;
}
            }
            break;
            /* start-c-tokenizer: */
            case 28: {
    tok_switch_to_c_idents(tkr);
            }
            break;
            /* end-c-tokenizer: */
            case 29: {
    {
	tok_switch_to_nonterminal_idents(tkr);
	g->accept_whitespace_ = 0; /* Reset to normal tokens */
}
            }
            break;
            /* accept-whitespace: */
            case 30: {
    {
	/* Welcome whitespace from this point. Note that this point is *after* the lookahead at the point
	 * that the PRD_ACCEPT_WHITESPACE non-terminal appears. Therefore, it is *after* the EQUALS sign and
	 * *after* the curly opening brace. */
	g->accept_whitespace_ = 1;
}
            }
            break;
          } /* switch */
        } /* scope guard */

        /* Free symdata for every symbol in the production, including the first slot where we will soon
         * push nonterminal_data_reduced_to */
        size_t prd_sym_idx;
        for (prd_sym_idx = stack->pos_ - production_length; prd_sym_idx < stack->pos_; ++prd_sym_idx) {
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
            {
               xlts_cleanup(&((stack->stack_ + prd_sym_idx)->v_.uv0_).text_);

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
               prd_prod_cleanup(&((stack->stack_ + prd_sym_idx)->v_.uv1_));

            }
            break;
          } /* switch */
        } /* for */
        stack->pos_ -= production_length;
        action = prd_parse_table[prd_num_columns * stack->stack_[stack->pos_ - 1].state_ + (nonterminal - prd_minimum_sym)];
        if (action <= 0) {
          re_error_tkr(tkr, "Error: internal error\n"); return PRD_INTERNAL_ERROR;
        }
        switch (prd_push_state(stack, action /* action for a "goto" shift is the ordinal */)) {
          case _PRD_OVERFLOW: {
            re_error_tkr(tkr, "Error: internal error\n"); return PRD_INTERNAL_ERROR;
          }
          break;
          case _PRD_NO_MEMORY: /* out of memory */ {
            re_error_tkr(tkr, "Error: no memory"); return PRD_INTERNAL_ERROR;
          }
          break;
        } /* switch */
        struct prd_sym_data *sd = stack->stack_ + stack->pos_ - 1;
        *sd = nonterminal_sym_data_reduced_to;
        sd->state_ = action;
      } /* action < 0 */
      else /* action == 0 */ {
        stack->error_recovery_ = 1;
        stack->report_error_ = !stack->mute_error_turns_;
        stack->mute_error_turns_ = 3;
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
            int err_sym_action = prd_parse_table[prd_num_columns * stack->stack_[n].state_ + (sym - prd_minimum_sym)];
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
                  {
                      xlts_cleanup(&((stack->stack_ + prd_sym_idx)->v_.uv0_).text_);

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
                      prd_prod_cleanup(&((stack->stack_ + prd_sym_idx)->v_.uv1_));

                  }
                  break;
                } /* switch */
              } /* for */
              stack->pos_ = n + 1;
              /* Push the state of the error transition */
              switch (prd_push_state(stack, err_action /* action for a shift is the state */)) {
                case _PRD_OVERFLOW: {
                  re_error_tkr(tkr, "Error: internal error\n"); return PRD_INTERNAL_ERROR;
                }
                break;
                case _PRD_NO_MEMORY: {
                  re_error_tkr(tkr, "Error: no memory"); return PRD_INTERNAL_ERROR;
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
        return PRD_NEXT;
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
