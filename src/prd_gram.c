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

#ifndef XLALR_H_INCLUDED
#define XLALR_H_INCLUDED
#include "xlalr.h"
#endif

#ifndef LALR_H_INCLUDED
#define LALR_H_INCLUDED
#include "lalr.h"
#endif

#ifndef TOKENIZER_H_INCLUDED
#define TOKENIZER_H_INCLUDED
#include "tokenizer.h"
#endif

#ifndef TOKENS_H_INCLUDED
#define TOKENS_H_INCLUDED
#include "tokens.h"
#endif

#ifndef KLT_LOGGER_H_INCLUDED
#define KLT_LOGGER_H_INCLUDED
#define KLT_LOG_MODULE "prd"
#include "klt_logger.h"
#endif

#ifndef PRD_GRAM_H_INCLUDED
#define PRD_GRAM_H_INCLUDED
#include "prd_gram.h"
#endif

#define USE_LALR 1
#define USE_XLALR 0
#if USE_LALR && USE_XLALR
#error Either set USE_LALR or set USE_XLALR, cannot have both
#endif

#define PRD_SYMBOL_ENUM \
  xx(NT_END) \
  xx(RULE_END) \
  xx(GRAMMAR_END) \
\
  xx(PRD_IDENT) \
  xx(PRD_COLON) \
  xx(PRD_EQUALS) \
  xx(PRD_SEMICOLON) \
  xx(PRD_TOKEN) \
  xx(PRD_PAR_OPEN) \
  xx(PRD_PAR_CLOSE) \
  xx(PRD_CUBRACE_OPEN) \
  xx(PRD_CUBRACE_CLOSE) \
\
  xx(INPUT_END) \
\
  xx(PRD_GRAMMAR) \
  xx(PRD_RULE) \
  xx(PRD_PRODUCTION) \
  xx(PRD_ACTION_SEQUENCE) \
  xx(PRD_STMT_ACTION) \
  xx(PRD_COMP_ACTION) \
  xx(PRD_NONTERMINAL) \
  xx(PRD_START_C_TOKENIZER) \
  xx(PRD_END_C_TOKENIZER) \
\
  xx(SYNTH_S)

typedef enum prd_symbol_enum {
#define xx(ident) ident,
  PRD_SYMBOL_ENUM
#undef xx
} prd_symbol_t;

void prd_stack_init(struct prd_stack *stack) {
  stack->pos_ = 0;
  stack->num_stack_allocated_ = 0;
  stack->states_ = NULL;
  stack->syms_ = NULL;
}

void prd_stack_cleanup(struct prd_stack *stack) {
  if (stack->states_) free(stack->states_);
  
  size_t n;
  for (n = 0; n < stack->pos_; ++n) {
    if (stack->syms_[n].text_) free(stack->syms_[n].text_);
  }

  if (stack->syms_) free(stack->syms_);
}

static int g_grammar_[] = {
  PRD_GRAMMAR, NT_END, RULE_END,
  PRD_GRAMMAR, NT_END, PRD_GRAMMAR, PRD_PRODUCTION, RULE_END,

  PRD_PRODUCTION, NT_END, PRD_NONTERMINAL, PRD_COLON, PRD_RULE, PRD_SEMICOLON, RULE_END,
  PRD_PRODUCTION, NT_END, PRD_NONTERMINAL, PRD_COLON, PRD_RULE, PRD_START_C_TOKENIZER, PRD_STMT_ACTION, PRD_END_C_TOKENIZER, PRD_SEMICOLON, RULE_END,
  PRD_PRODUCTION, NT_END, PRD_NONTERMINAL, PRD_COLON, PRD_RULE, PRD_START_C_TOKENIZER, PRD_COMP_ACTION, PRD_END_C_TOKENIZER, PRD_SEMICOLON, RULE_END,

  PRD_NONTERMINAL, NT_END, PRD_IDENT, RULE_END,

  PRD_RULE, NT_END, RULE_END,
  PRD_RULE, NT_END, PRD_RULE, PRD_IDENT, RULE_END,

  PRD_STMT_ACTION, NT_END, PRD_EQUALS, RULE_END,
  PRD_STMT_ACTION, NT_END, PRD_STMT_ACTION, PRD_IDENT, RULE_END,
  PRD_STMT_ACTION, NT_END, PRD_STMT_ACTION, PRD_COLON, RULE_END,
  PRD_STMT_ACTION, NT_END, PRD_STMT_ACTION, PRD_TOKEN, RULE_END,
  PRD_STMT_ACTION, NT_END, PRD_STMT_ACTION, PRD_PAR_OPEN, PRD_ACTION_SEQUENCE, PRD_PAR_CLOSE, RULE_END,
  PRD_STMT_ACTION, NT_END, PRD_STMT_ACTION, PRD_CUBRACE_OPEN, PRD_ACTION_SEQUENCE, PRD_CUBRACE_CLOSE, RULE_END,

  PRD_COMP_ACTION, NT_END, PRD_CUBRACE_OPEN, PRD_ACTION_SEQUENCE, PRD_CUBRACE_CLOSE, RULE_END,

  PRD_ACTION_SEQUENCE, NT_END, RULE_END,
  PRD_ACTION_SEQUENCE, NT_END, PRD_ACTION_SEQUENCE, PRD_IDENT, RULE_END,
  PRD_ACTION_SEQUENCE, NT_END, PRD_ACTION_SEQUENCE, PRD_COLON, RULE_END,
  PRD_ACTION_SEQUENCE, NT_END, PRD_ACTION_SEQUENCE, PRD_EQUALS, RULE_END,
  PRD_ACTION_SEQUENCE, NT_END, PRD_ACTION_SEQUENCE, PRD_SEMICOLON, RULE_END,
  PRD_ACTION_SEQUENCE, NT_END, PRD_ACTION_SEQUENCE, PRD_TOKEN, RULE_END,
  PRD_ACTION_SEQUENCE, NT_END, PRD_ACTION_SEQUENCE, PRD_PAR_OPEN, PRD_ACTION_SEQUENCE, PRD_PAR_CLOSE, RULE_END,
  PRD_ACTION_SEQUENCE, NT_END, PRD_ACTION_SEQUENCE, PRD_CUBRACE_OPEN, PRD_ACTION_SEQUENCE, PRD_CUBRACE_CLOSE, RULE_END,

  PRD_START_C_TOKENIZER, NT_END, RULE_END, /* start strict C-style ident interpretation */
  PRD_END_C_TOKENIZER, NT_END, RULE_END,   /* end strict C-style ident interpretation */

  GRAMMAR_END
};

static int g_lalr_grammar_[] = {
  PRD_GRAMMAR,          RULE_END,
  PRD_GRAMMAR,          PRD_GRAMMAR, PRD_PRODUCTION, RULE_END,

  PRD_PRODUCTION,       PRD_NONTERMINAL, PRD_COLON, PRD_RULE, PRD_SEMICOLON, RULE_END,
  PRD_PRODUCTION,       PRD_NONTERMINAL, PRD_COLON, PRD_RULE, PRD_START_C_TOKENIZER, PRD_STMT_ACTION, PRD_END_C_TOKENIZER, PRD_SEMICOLON, RULE_END,
  PRD_PRODUCTION,       PRD_NONTERMINAL, PRD_COLON, PRD_RULE, PRD_START_C_TOKENIZER, PRD_COMP_ACTION, PRD_END_C_TOKENIZER, PRD_SEMICOLON, RULE_END,

  PRD_NONTERMINAL,      PRD_IDENT, RULE_END,

  PRD_RULE,             RULE_END,
  PRD_RULE,             PRD_RULE, PRD_IDENT, RULE_END,

  PRD_STMT_ACTION,      PRD_EQUALS, RULE_END,
  PRD_STMT_ACTION,      PRD_STMT_ACTION, PRD_IDENT, RULE_END,
  PRD_STMT_ACTION,      PRD_STMT_ACTION, PRD_COLON, RULE_END,
  PRD_STMT_ACTION,      PRD_STMT_ACTION, PRD_TOKEN, RULE_END,
  PRD_STMT_ACTION,      PRD_STMT_ACTION, PRD_PAR_OPEN, PRD_ACTION_SEQUENCE, PRD_PAR_CLOSE, RULE_END,
  PRD_STMT_ACTION,      PRD_STMT_ACTION, PRD_CUBRACE_OPEN, PRD_ACTION_SEQUENCE, PRD_CUBRACE_CLOSE, RULE_END,

  PRD_COMP_ACTION,      PRD_CUBRACE_OPEN, PRD_ACTION_SEQUENCE, PRD_CUBRACE_CLOSE, RULE_END,

  PRD_ACTION_SEQUENCE,  RULE_END,
  PRD_ACTION_SEQUENCE,  PRD_ACTION_SEQUENCE, PRD_IDENT, RULE_END,
  PRD_ACTION_SEQUENCE,  PRD_ACTION_SEQUENCE, PRD_COLON, RULE_END,
  PRD_ACTION_SEQUENCE,  PRD_ACTION_SEQUENCE, PRD_EQUALS, RULE_END,
  PRD_ACTION_SEQUENCE,  PRD_ACTION_SEQUENCE, PRD_SEMICOLON, RULE_END,
  PRD_ACTION_SEQUENCE,  PRD_ACTION_SEQUENCE, PRD_TOKEN, RULE_END,
  PRD_ACTION_SEQUENCE,  PRD_ACTION_SEQUENCE, PRD_PAR_OPEN, PRD_ACTION_SEQUENCE, PRD_PAR_CLOSE, RULE_END,
  PRD_ACTION_SEQUENCE,  PRD_ACTION_SEQUENCE, PRD_CUBRACE_OPEN, PRD_ACTION_SEQUENCE, PRD_CUBRACE_CLOSE, RULE_END,

  PRD_START_C_TOKENIZER,RULE_END, /* start strict C-style ident interpretation */
  PRD_END_C_TOKENIZER,  RULE_END,   /* end strict C-style ident interpretation */

  GRAMMAR_END
};


static int prd_get_parsetable(int **parse_table, size_t **production_lengths, int **production_syms,
                              int *minimum_sym, size_t *num_columns, size_t *num_rows, size_t *num_productions) {
  size_t row, col;
#if USE_LALR
  lr_generator_t lalr_gen;
  lr_init(&lalr_gen);

  lr_error_t lalr_err;
  lalr_err = lr_gen_parser(&lalr_gen, g_lalr_grammar_, RULE_END, GRAMMAR_END, INPUT_END, SYNTH_S);
  if (lalr_err != LR_OK) {
    return lalr_err;
  }

  *minimum_sym = lalr_gen.min_sym;
  *num_columns = 1 + lalr_gen.max_sym - lalr_gen.min_sym;
  *num_rows = lalr_gen.nr_states;
  *num_productions = lalr_gen.nr_productions;
  *parse_table = (int *)malloc(*num_columns * *num_rows * sizeof(int));
  *production_lengths = (size_t *)malloc(*num_productions * sizeof(size_t));
  *production_syms = (int *)malloc(*num_productions * sizeof(int));

  for (row = 0; row < *num_rows; ++row) {
    for (col = 0; col < *num_columns; ++col) {
      int action = lalr_gen.parse_table[*num_columns * row + col];
      if (!action) {
        /* 0 = no action, syntax error */
        (*parse_table)[*num_columns * row + col] = 0;
      }
      else if (action > 0) {
        /* positive = shift */
        (*parse_table)[*num_columns * row + col] = action;
      }
      else /* (action < 0) */ {
        /* negative = reduce */
        (*parse_table)[*num_columns * row + col] = action;
      }
    }
  }
  for (row = 0; row < *num_productions; ++row) {
    (*production_lengths)[row] = lalr_gen.production_lengths[row];
    (*production_syms)[row] = lalr_gen.productions[row][0];
  }

  lr_cleanup(&lalr_gen);

  return 0;
#endif
#if USE_XLALR
  xlr_gen_t xlalr_gen;
  xlr_init(&xlalr_gen);
  xlr_error_t err;
  err = xlr_generate(&xlalr_gen, g_grammar_, RULE_END, GRAMMAR_END, INPUT_END, NT_END, SYNTH_S);
  if (err != XLR_OK) {
    return err;
  }

  *minimum_sym = xlalr_gen.min_s;
  *num_columns = 1 + xlalr_gen.max_s - xlalr_gen.min_s;
  *num_rows = xlalr_gen.nr_states;
  *num_productions = xlalr_gen.nr_productions;
  *parse_table = (int *)malloc(*num_columns * *num_rows * sizeof(int));
  *production_lengths = (size_t *)malloc(*num_productions * sizeof(size_t));
  *production_syms = (int *)malloc(*num_productions * sizeof(int));
  for (row = 0; row < *num_rows; ++row) {
    for (col = 0; col < *num_columns; ++col) {
      xlr_action_t *action = xlalr_gen.action_table[*num_columns * row + col];
      if (action && action->chain != action) {
        LOGERROR("Conflicts found\n");
        return XLR_CONFLICTS;
      }
      if (!action) {
        /* 0 = no action, syntax error */
        (*parse_table)[*num_columns * row + col] = 0;
      }
      else if (action->action == XLR_SHIFT) {
        /* positive = shift + 1 */
        (*parse_table)[*num_columns * row + col] = action->state->ordinal + 1;
      }
      else if (action->action == XLR_REDUCE) {
        (*parse_table)[*num_columns * row + col] = -action->production - 1;
      }
    }
  }
  for (row = 0; row < *num_productions; ++row) {
    (*production_lengths)[row] = xlalr_gen.productions[row].production_length;
    (*production_syms)[row] = xlalr_gen.productions[row].reduction_syms[0];
  }
  xlr_cleanup(&xlalr_gen);

  return err;
#endif
}


static int minimum_sym;
static size_t num_columns;
static size_t num_rows;
static size_t num_productions;
static int *parse_table = NULL;
static size_t *production_lengths = NULL;
static int *production_syms = NULL;

int prd_init(void) {
  int res;
  res = prd_get_parsetable(&parse_table, &production_lengths, &production_syms, &minimum_sym, &num_columns, &num_rows, &num_productions);
  if (res != XLR_OK) {
    LOGERROR("Failed to generate parse table.\n");
    return res;
  }

  return 0;
}

void prd_cleanup(void) {
  if (parse_table) free(parse_table);
  if (production_lengths) free(production_lengths);
  if (production_syms) free(production_syms);
}

static int push_state(struct prd_stack *stack, int state) {
  if (stack->num_stack_allocated_ == stack->pos_) {
    size_t new_num_allocated = 0;
    if (stack->num_stack_allocated_) {
      new_num_allocated = stack->num_stack_allocated_ * 2;
      if (new_num_allocated <= stack->num_stack_allocated_) {
        LOGERROR("Overflow in allocation\n");
        return EOVERFLOW;
      }
    }
    else {
      new_num_allocated = 16;
    }

    if (new_num_allocated > (SIZE_MAX / sizeof(struct prd_sym_data))) {
      LOGERROR("Overflow in allocation\n");
      return EOVERFLOW;
    }
    if (new_num_allocated > (SIZE_MAX / sizeof(int))) {
      LOGERROR("Overflow in allocation\n");
      return EOVERFLOW;
    }

    void *p = realloc(stack->syms_, new_num_allocated * sizeof(struct prd_sym_data));
    if (!p) {
      LOGERROR("Out of memory\n");
      return ENOMEM;
    }
    stack->syms_ = (struct prd_sym_data *)p;
    void *p2 = realloc(stack->states_, new_num_allocated * sizeof(int));
    if (!p2) {
      LOGERROR("Out of memory\n");
      return ENOMEM;
    }
    stack->states_ = p2;
    stack->num_stack_allocated_ = new_num_allocated;
  }
  stack->states_[stack->pos_++] = state;
  return 0;
}

static int top_state(struct prd_stack *stack) {
  assert(stack->pos_ && "Invalid stack position, should never be empty");
  return stack->states_[stack->pos_ - 1];
}

static void popn_state(struct prd_stack *stack, size_t num_pops) {
  assert((stack->pos_ > num_pops) && "Cannot pop down to zero or below");
  stack->pos_ -= num_pops;
}

int prd_reset(struct prd_stack *stack) {
  int r;
  stack->pos_ = 0;
  r = push_state(stack, 0);
  if (r) {
    return r;
  }
  stack->syms_[0].p_ = NULL;
  stack->syms_[0].text_ = NULL;
  return 0;
}


static const char *prd_sym_to_cstr(int sym) {
  switch (sym) {
#define xx(id) case id: return #id;
    PRD_SYMBOL_ENUM
#undef xx
  }
  return "?";
}

static int reduce(struct prd_stack *stack, int production, struct prd_sym_data *syms) {
  TRACE("Reducing production %d to symbol %s\n", production, prd_sym_to_cstr(production_syms[production]));
  return PRD_SUCCESS;
}

int prd_parse(struct prd_stack *stack, struct tkr_tokenizer *tkr, int end_of_input) {
  int sym;
  int r;

  if (!end_of_input) {
    token_type_t tkt = (token_type_t)tkr->best_match_variant_;
    if (tkt == TOK_WHITESPACE) {
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
    default: sym = PRD_TOKEN; break;
    }
  }
  else {
    sym = INPUT_END;
  }

  int current_state = top_state(stack);
  int action = parse_table[num_columns * current_state + (sym - minimum_sym)];
  if (!action) {
    /* Syntax error */
    if (sym != INPUT_END) {
      LOGERROR("%s(%d): Syntax error \"%s\" not expected at column %d\n", tkr->filename_, tkr->best_match_line_, tkr->match_, tkr->best_match_col_);
    }
    else {
      LOGERROR("%s(%d): Syntax error end of input not expected at column %d\n", tkr->filename_, tkr->best_match_line_, tkr->best_match_col_);
    }
    /* XXX: Pop until we transition */
    return PRD_SYNTAX_ERROR;
  }
  while (action < 0) {
    /* While we're reducing.. */
    int production = -action - 1;
    size_t production_length = production_lengths[production];
    int nonterminal = production_syms[production];

    if (0 == production) {
      /* Synth S we're done. */
      return PRD_SUCCESS;
    }

    r = reduce(stack, production, stack->syms_ + stack->pos_ - production_length);
    if (r) {
      /* Semantic error */
      return r;
    }

    /* Free symdata for every symbol in the production, except for the first symbol,
     * which is to be preserved as the data of the non-terminal we just reduced to. */
    size_t n;
    for (n = stack->pos_ - production_length + 1; n < stack->pos_; ++n) {
      if (stack->syms_[n].text_) {
        free(stack->syms_[n].text_);
        stack->syms_[n].text_ = NULL;
      }
    }

    popn_state(stack, production_length);

    current_state = top_state(stack);
    action = parse_table[num_columns * current_state + (nonterminal - minimum_sym)];

    if (!action) {
      LOGERROR("%s(%d): Internal error \"%s\" cannot shift an already reduced nonterminal at column %d\n", tkr->filename_, tkr->best_match_line_, tkr->match_, tkr->best_match_col_);
      return PRD_INTERNAL_ERROR;
    }
    if (action < 0) {
      LOGERROR("%s(%d): Internal error \"%s\" reduced non-terminal not shifting at column %d\n", tkr->filename_, tkr->best_match_line_, tkr->match_, tkr->best_match_col_);
      return PRD_INTERNAL_ERROR;
    }
#if USE_XLALR
    push_state(stack, action - 1 /* action for a shift is ordinal + 1 */);
#endif
#if USE_LALR
    push_state(stack, action /* action for a shift is the ordinal */);
#endif
    struct prd_sym_data *sd = stack->syms_ + stack->pos_ - 1;
    sd->text_ = strdup(prd_sym_to_cstr(production_syms[production]));

    current_state = top_state(stack);
    action = parse_table[num_columns * current_state + (sym - minimum_sym)];
    if (!action) {
      /* Syntax error */
      if (sym != INPUT_END) {
        LOGERROR("%s(%d): Syntax error \"%s\" not expected at column %d\n", tkr->filename_, tkr->best_match_line_, tkr->match_, tkr->best_match_col_);
      }
      else {
        LOGERROR("%s(%d): Syntax error end of input not expected at column %d\n", tkr->filename_, tkr->best_match_line_, tkr->best_match_col_);
      }
      return PRD_SYNTAX_ERROR;
    }
  }

  /* Shift token onto stack */
  if (action > 0 /* shift? */) {
#if USE_XLALR
    push_state(stack, action - 1 /* action for a shift is ordinal + 1 */);
#endif
#if USE_LALR
    push_state(stack, action /* action for a shift is the ordinal */);
#endif
    struct prd_sym_data *sym = stack->syms_ + stack->pos_ - 1;
    
    /* Fill in the sym from the tokenizer */
    sym->text_ = strdup(tkr->match_);
  }
  else {
    LOGERROR("%s(%d): Syntax error \"%s\" not expected at column %d\n", tkr->filename_, tkr->best_match_line_, tkr->match_, tkr->best_match_col_);
    return PRD_SYNTAX_ERROR;
  }

  return PRD_NEXT;
}

