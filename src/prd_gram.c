#if 0
/* Use prd_gram_gen.c implementation */
#include "prd_gram_gen.c"
#elif 1
#include "prd_grammar_alt.c"
#else /* Use prd_gram.c implementation */

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

#ifndef REPORT_ERROR_H_INCLUDED
#define REPORT_ERROR_H_INCLUDED
#include "report_error.h"
#endif

#ifndef PRD_GRAM_H_INCLUDED
#define PRD_GRAM_H_INCLUDED
#include "prd_gram.h"
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
  xx(PRD_START_C_TOKENIZER) \
  xx(PRD_END_C_TOKENIZER) \
  xx(PRD_ACCEPT_WHITESPACE) \
\
  xx(SYNTH_S)

typedef enum prd_symbol_enum {
#define xx(ident) ident,
  PRD_SYMBOL_ENUM
#undef xx
} prd_symbol_t;

static void prd_prod_init(struct prd_production *ppd);
static void prd_prod_cleanup(struct prd_production *ppd);

void prd_stack_init(struct prd_stack *stack) {
  stack->have_errors_ = 0;
  stack->accept_whitespace_ = 0;
  stack->pos_ = 0;
  stack->num_stack_allocated_ = 0;
  stack->states_ = NULL;
  stack->syms_ = NULL;
  stack->num_productions_ = stack->num_productions_allocated_ = 0;
  stack->productions_ = NULL;
}

void prd_stack_cleanup(struct prd_stack *stack) {
  if (stack->states_) free(stack->states_);
  
  size_t n;
  for (n = 0; n < stack->pos_; ++n) {
    xlts_cleanup(&stack->syms_[n].text_);
  }

  if (stack->syms_) free(stack->syms_);

  for (n = 0; n < stack->num_productions_; ++n) {
    prd_prod_cleanup(stack->productions_ + n);
  }
  if (stack->productions_) free(stack->productions_);
}

static int g_lalr_grammar_[] = {
  PRD_GRAMMAR,          RULE_END,
  PRD_GRAMMAR,          PRD_GRAMMAR, PRD_PRODUCTION, RULE_END,

  PRD_PRODUCTION,       PRD_IDENT, PRD_COLON, PRD_RULE, PRD_SEMICOLON, RULE_END,
  PRD_PRODUCTION,       PRD_IDENT, PRD_COLON, PRD_RULE, PRD_START_C_TOKENIZER, PRD_STMT_ACTION, PRD_END_C_TOKENIZER, PRD_SEMICOLON, RULE_END,
  PRD_PRODUCTION,       PRD_IDENT, PRD_COLON, PRD_RULE, PRD_START_C_TOKENIZER, PRD_ACCEPT_WHITESPACE, PRD_CUBRACE_OPEN, PRD_ACTION_SEQUENCE, PRD_END_C_TOKENIZER, PRD_CUBRACE_CLOSE, RULE_END,

  PRD_RULE,             RULE_END,
  PRD_RULE,             PRD_RULE, PRD_IDENT, RULE_END,

  PRD_STMT_ACTION,      PRD_EQUALS, PRD_ACCEPT_WHITESPACE, RULE_END, /* this intentionally skips the whitespace immediately after PRD_EQUALS */
  PRD_STMT_ACTION,      PRD_STMT_ACTION, PRD_IDENT, RULE_END,
  PRD_STMT_ACTION,      PRD_STMT_ACTION, PRD_COLON, RULE_END,
  PRD_STMT_ACTION,      PRD_STMT_ACTION, PRD_EQUALS, RULE_END,
  PRD_STMT_ACTION,      PRD_STMT_ACTION, PRD_TOKEN, RULE_END,
  PRD_STMT_ACTION,      PRD_STMT_ACTION, PRD_PAR_OPEN, PRD_ACTION_SEQUENCE, PRD_PAR_CLOSE, RULE_END,
  PRD_STMT_ACTION,      PRD_STMT_ACTION, PRD_CUBRACE_OPEN, PRD_ACTION_SEQUENCE, PRD_CUBRACE_CLOSE, RULE_END,

  PRD_ACTION_SEQUENCE,  RULE_END,
  PRD_ACTION_SEQUENCE,  PRD_ACTION_SEQUENCE, PRD_IDENT, RULE_END,
  PRD_ACTION_SEQUENCE,  PRD_ACTION_SEQUENCE, PRD_COLON, RULE_END,
  PRD_ACTION_SEQUENCE,  PRD_ACTION_SEQUENCE, PRD_EQUALS, RULE_END,
  PRD_ACTION_SEQUENCE,  PRD_ACTION_SEQUENCE, PRD_SEMICOLON, RULE_END,
  PRD_ACTION_SEQUENCE,  PRD_ACTION_SEQUENCE, PRD_TOKEN, RULE_END,
  PRD_ACTION_SEQUENCE,  PRD_ACTION_SEQUENCE, PRD_PAR_OPEN, PRD_ACTION_SEQUENCE, PRD_PAR_CLOSE, RULE_END,
  PRD_ACTION_SEQUENCE,  PRD_ACTION_SEQUENCE, PRD_CUBRACE_OPEN, PRD_ACTION_SEQUENCE, PRD_CUBRACE_CLOSE, RULE_END,

  PRD_START_C_TOKENIZER,RULE_END, /* start strict C-style ident interpretation */
  PRD_END_C_TOKENIZER,  RULE_END,   /* end strict C-style ident interpretation and resets prd_stack::accept_whitespace_ to 0 */

  PRD_ACCEPT_WHITESPACE, RULE_END, /* sets prd_stack::accept_whitespace_, causing whitespace to appear as tokens in the grammar. */

  GRAMMAR_END
};


static int prd_get_parsetable(int **parse_table, size_t **production_lengths, int **production_syms,
                              int *minimum_sym, size_t *num_columns, size_t *num_rows, size_t *num_productions) {
  size_t row, col;
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
}

static void prd_production_sym_init(struct prd_production_sym *pps) {
  xlts_init(&pps->id_);
  pps->sym_ = NULL;
}

static void prd_production_sym_cleanup(struct prd_production_sym *pps) {
  xlts_cleanup(&pps->id_);
}

static void prd_snippet_init(struct prd_snippet *snip) {
  snip->original_match_ = 0;
  snip->original_variant_ = 0;
  xlts_init(&snip->code_);
}

static void prd_snippet_cleanup(struct prd_snippet *snip) {
  xlts_cleanup(&snip->code_);
}

static void prd_prod_init(struct prd_production *ppd) {
  prd_production_sym_init(&ppd->nt_);
  ppd->num_syms_ = ppd->num_syms_allocated_ = 0;
  ppd->syms_ = NULL;
  ppd->num_snippets_ = ppd->num_snippets_allocated_ = 0;
  ppd->snippets_ = NULL;
}

static void prd_prod_cleanup(struct prd_production *ppd) {
  size_t n;
  prd_production_sym_cleanup(&ppd->nt_);
  if (ppd->syms_) {
    for (n = 0; n < ppd->num_syms_; ++n) {
      prd_production_sym_cleanup(ppd->syms_ + n);
    }
    free(ppd->syms_);
  }
  if (ppd->snippets_) {
    for (n = 0; n < ppd->num_snippets_; ++n) {
      prd_snippet_cleanup(ppd->snippets_ + n);
    }
    free(ppd->snippets_);
  }
}

void prd_prod_swap(struct prd_production *a, struct prd_production *b) {
  struct prd_production swap;
  swap = *a;
  *a = *b;
  *b = swap;
}

static void prd_sym_data_init(struct prd_sym_data *psd) {
  psd->p_ = NULL;
  psd->sym_ = NULL;
  prd_prod_init(&psd->prod_);
  xlts_init(&psd->text_);
  psd->match_ = 0;
  psd->variant_ = 0;
}

static void prd_sym_data_cleanup(struct prd_sym_data *psd) {
  prd_prod_cleanup(&psd->prod_);
  xlts_cleanup(&psd->text_);
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
  xlts_init(&stack->syms_[0].text_);
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

static int prd_prod_check_sym_reserve(struct prd_production *pd, struct xlts *loc) {
  if (pd->num_syms_ == pd->num_syms_allocated_) {
    size_t new_num = pd->num_syms_allocated_ * 2 + 1;
    if (new_num < 7) new_num = 7;
    if (new_num <= pd->num_syms_allocated_) {
      re_error(loc, "Internal error, overflow on allocation");
      return PRD_INTERNAL_ERROR;
    }
    if (new_num > (SIZE_MAX / sizeof(struct prd_production_sym))) {
      re_error(loc, "Internal error, overflow on allocation");
      return PRD_INTERNAL_ERROR;
    }
    size_t size_to_alloc = new_num * sizeof(struct prd_production_sym);
    void *p = realloc(pd->syms_, size_to_alloc);
    if (!p) {
      re_error(loc, "Internal error, no memory");
      return PRD_INTERNAL_ERROR;
    }
    pd->syms_ = (struct prd_production_sym *)p;
    pd->num_syms_allocated_ = new_num;
  }
  return 0;
}

static int prd_prod_check_snippet_reserve(struct prd_production *pd, struct xlts *loc) {
  if (pd->num_snippets_ == pd->num_snippets_allocated_) {
    size_t new_num = pd->num_snippets_allocated_ * 2 + 1;
    if (new_num < 7) new_num = 7;
    if (new_num <= pd->num_snippets_allocated_) {
      re_error(loc, "Internal error, overflow on allocation");
      return PRD_INTERNAL_ERROR;
    }
    if (new_num > (SIZE_MAX / sizeof(struct prd_snippet))) {
      re_error(loc, "Internal error, overflow on allocation");
      return PRD_INTERNAL_ERROR;
    }
    size_t size_to_alloc = new_num * sizeof(struct prd_snippet);
    void *p = realloc(pd->snippets_, size_to_alloc);
    if (!p) {
      re_error(loc, "Internal error, no memory");
      return PRD_INTERNAL_ERROR;
    }
    pd->snippets_ = (struct prd_snippet *)p;
    pd->num_snippets_allocated_ = new_num;
  }
  return 0;
}

static int prd_stack_check_production_reserve(struct prd_stack *ps) {
  if (ps->num_productions_ == ps->num_productions_allocated_) {
    size_t new_num = ps->num_productions_allocated_ * 2 + 1;
    if (new_num < 7) new_num = 7;
    if (new_num <= ps->num_productions_allocated_) {
      LOGERROR("Internal error, overflow on allocation");
      return PRD_INTERNAL_ERROR;
    }
    if (new_num > (SIZE_MAX / sizeof(struct prd_production))) {
      LOGERROR("Internal error, overflow on allocation");
      return PRD_INTERNAL_ERROR;
    }
    size_t size_to_alloc = new_num * sizeof(struct prd_production);
    void *p = realloc(ps->productions_, size_to_alloc);
    if (!p) {
      LOGERROR("Internal error, no memory");
      return PRD_INTERNAL_ERROR;
    }
    ps->productions_ = (struct prd_production *)p;
    ps->num_productions_allocated_ = new_num;
  }

  return 0;
}

static int prd_append_token_to_snippets(struct prd_production *pd, struct xlts *text, uintptr_t match, uintptr_t variant) {
  int r;
  if (pd->num_snippets_ == pd->num_snippets_allocated_) {
    r = prd_prod_check_snippet_reserve(pd, text);
    if (r) return r;
  }
  struct prd_snippet *snip = pd->snippets_ + pd->num_snippets_++;
  prd_snippet_init(snip);
  r = xlts_append(&snip->code_, text);
  if (r) return PRD_INTERNAL_ERROR;
  snip->original_match_ = match;
  snip->original_variant_ = variant;
  return 0;
}

/****************************** ---- Start of Initially generated piece ---- ***********************/

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

static int reduce(struct prd_stack *stack, struct tkr_tokenizer *tkr, int production, struct prd_sym_data *dst_sym, struct prd_sym_data *syms, struct symbol_table *st) {
  int r;
  size_t n;
  struct prd_production *pd;
  struct symbol *sym;

  if (production_syms[production] == PRD_START_C_TOKENIZER) {
    tok_switch_to_c_idents(tkr);
  }
  else if (production_syms[production] == PRD_END_C_TOKENIZER) {
    tok_switch_to_nonterminal_idents(tkr);
    stack->accept_whitespace_ = 0; /* Reset to normal tokens */
  }
  else if (production_syms[production] == PRD_ACCEPT_WHITESPACE) {
    /* Welcome whitespace from this point. Note that this point is *after* the lookahead at the point
     * that the PRD_ACCEPT_WHITESPACE non-terminal appears. Therefore, it is *after* the EQUALS sign and
     * *after* the curly opening brace. */
    stack->accept_whitespace_ = 1;
  }
  else {
    switch (production) {

    case 2: /* PRD_GRAMMAR, PRD_GRAMMAR, PRD_PRODUCTION, RULE_END, */
      /* Store the PRD_PRODUCTION in the prd_stack->productions_ array */
      r = prd_stack_check_production_reserve(stack);
      if (r) return r;
      pd = stack->productions_ + stack->num_productions_++;
      prd_prod_init(pd);
      prd_prod_swap(pd, &syms[1].prod_);

      break;
    case 3: /* PRD_PRODUCTION, PRD_IDENT, PRD_COLON, PRD_RULE, PRD_SEMICOLON, RULE_END, */
    case 4: /* PRD_PRODUCTION, PRD_IDENT, PRD_COLON, PRD_RULE, PRD_START_C_TOKENIZER, PRD_STMT_ACTION, PRD_END_C_TOKENIZER, PRD_SEMICOLON, RULE_END, */
      assert(production_syms[production] == PRD_PRODUCTION);
      if (production == 3) {
        assert(production_lengths[production] == 4);
      }
      else {
        assert(production_lengths[production] == 7);
      }
      sym = symbol_find(st, syms[0].text_.translated_);
      if (!sym) {
        re_error(&syms[1].text_, "Error, symbol \"%s\" not declared as %%nt", syms[0].text_.translated_);
        stack->have_errors_ = 1;
        return PRD_SUCCESS;
      }

      prd_prod_swap(&dst_sym->prod_, &syms[2].prod_);
      pd = &dst_sym->prod_;
      xlts_append(&pd->nt_.id_, &syms[0].text_);
      pd->nt_.sym_ = sym;

      if (production == 4) {
        assert(production_lengths[production] == 7);
        /* Move snippets in; could do this more efficiently by manually swapping pointers; but this works.. */
        for (n = 0; n < syms[4].prod_.num_snippets_; ++n) {
          r = prd_append_token_to_snippets(pd, &syms[4].prod_.snippets_[n].code_, syms[4].prod_.snippets_[n].original_match_, syms[4].prod_.snippets_[n].original_variant_);
          if (r) return r;
        }
        /* Append the semicolon to the snippets */
        r = prd_append_token_to_snippets(pd, &syms[6].text_, syms[6].match_, syms[6].variant_);
        if (r) return r;
      }

      break;

    case 5: /* PRD_PRODUCTION, PRD_IDENT, PRD_COLON, PRD_RULE, PRD_START_C_TOKENIZER, PRD_ACCEPT_WHITESPACE, PRD_CUBRACE_OPEN, PRD_ACTION_SEQUENCE, PRD_END_C_TOKENIZER, PRD_CUBRACE_CLOSE, RULE_END, */
      assert(production_syms[production] == PRD_PRODUCTION);
      assert(production_lengths[production] == 9);
      sym = symbol_find(st, syms[0].text_.translated_);
      if (!sym) {
        re_error(&syms[1].text_, "Error, symbol \"%s\" not declared as %%nt", syms[0].text_.translated_);
        stack->have_errors_ = 1;
        return PRD_SUCCESS;
      }

      prd_prod_swap(&dst_sym->prod_, &syms[2].prod_);
      pd = &dst_sym->prod_;
      xlts_append(&pd->nt_.id_, &syms[0].text_);
      pd->nt_.sym_ = sym;

      /* Move snippets in; could do this more efficiently by manually swapping pointers; but this works.. */
      r = prd_append_token_to_snippets(pd, &syms[5].text_, syms[5].match_, syms[5].variant_); /* 5 == PRD_CUBRACE_OPEN */
      if (r) return r;
      for (n = 0; n < syms[6].prod_.num_snippets_; ++n) { /* 6 == PRD_ACTION_SEQUENCE */
        r = prd_append_token_to_snippets(pd, &syms[6].prod_.snippets_[n].code_, syms[6].prod_.snippets_[n].original_match_, syms[6].prod_.snippets_[n].original_variant_);
        if (r) return r;
      }
      r = prd_append_token_to_snippets(pd, &syms[8].text_, syms[8].match_, syms[8].variant_); /* 8 == PRD_CUBRACE_CLOSE */
      if (r) return r;

      break;

    case 6: /* PRD_RULE, RULE_END */
      assert(production_syms[production] == PRD_RULE);
      /* Default PRD_RULE reduction sym has empty initialized prod_ which works for us. */
      break;
    case 7: /* PRD_RULE, PRD_RULE, PRD_IDENT, RULE_END, */
      assert(production_syms[production] == PRD_RULE);
      /* Grab ident from position 1, reduce */
      sym = symbol_find(st, syms[1].text_.translated_);
      if (!sym) {
        re_error(&syms[1].text_, "Error, symbol \"%s\" not declared as %%nt or %%token", syms[1].text_.translated_);
        stack->have_errors_ = 1;
        return PRD_SUCCESS;
      }
      prd_prod_swap(&dst_sym->prod_, &syms[0].prod_);
      pd = &dst_sym->prod_;
      if (pd->num_syms_ == pd->num_syms_allocated_) {
        r = prd_prod_check_sym_reserve(pd, &syms[1].text_);
        if (r) return r;
      }
      struct prd_production_sym *pps = pd->syms_ + pd->num_syms_++;
      prd_production_sym_init(pps);
      r = xlts_append(&pps->id_, &syms[1].text_);
      if (r) return PRD_INTERNAL_ERROR;
      pps->sym_ = sym;

      break;

    case 8:  /* PRD_STMT_ACTION, PRD_EQUALS, PRD_ACCEPT_WHITESPACE, RULE_END, */ /* this intentionally skips the whitespace immediately after PRD_EQUALS */
      /* PRD_STMT_ACTION, PRD_COMP_ACTION and PRD_ACTION_SEQUENCE consist of prd_production with only the snippets_ field relevant. Return vacuous prd_production. */
      assert(production_syms[production] == PRD_STMT_ACTION);
      break;
    case 9:  /* PRD_STMT_ACTION, PRD_STMT_ACTION, PRD_IDENT, RULE_END, */
    case 10: /* PRD_STMT_ACTION, PRD_STMT_ACTION, PRD_COLON, RULE_END, */
    case 11: /* PRD_STMT_ACTION, PRD_STMT_ACTION, PRD_EQUALS, RULE_END, */
    case 12: /* PRD_STMT_ACTION, PRD_STMT_ACTION, PRD_TOKEN, RULE_END, */
      assert(production_syms[production] == PRD_STMT_ACTION);
      prd_prod_swap(&dst_sym->prod_, &syms[0].prod_);
      pd = &dst_sym->prod_;
      r = prd_append_token_to_snippets(pd, &syms[1].text_, syms[1].match_, syms[1].variant_);
      if (r) return PRD_INTERNAL_ERROR;
      break;

    case 13: /* PRD_STMT_ACTION, PRD_STMT_ACTION, PRD_PAR_OPEN, PRD_ACTION_SEQUENCE, PRD_PAR_CLOSE, RULE_END, */
    case 14: /* PRD_STMT_ACTION, PRD_STMT_ACTION, PRD_CUBRACE_OPEN, PRD_ACTION_SEQUENCE, PRD_CUBRACE_CLOSE, RULE_END, */
      assert(production_syms[production] == PRD_STMT_ACTION);
      prd_prod_swap(&dst_sym->prod_, &syms[0].prod_);
      pd = &dst_sym->prod_;
      r = prd_append_token_to_snippets(pd, &syms[1].text_, syms[1].match_, syms[1].variant_);
      if (r) return r;
      for (n = 0; n < syms[2].prod_.num_snippets_; ++n) {
        r = prd_append_token_to_snippets(pd, &syms[2].prod_.snippets_[n].code_, syms[2].prod_.snippets_[n].original_match_, syms[2].prod_.snippets_[n].original_variant_);
        if (r) return r;
      }
      r = prd_append_token_to_snippets(pd, &syms[3].text_, syms[3].match_, syms[3].variant_);
      if (r) return r;
      break;

    case 15: /* PRD_ACTION_SEQUENCE, RULE_END, */
      /* PRD_STMT_ACTION, PRD_COMP_ACTION and PRD_ACTION_SEQUENCE consist of prd_production with only the snippets_ field relevant. Return vacuous prd_production. */
      assert(production_syms[production] == PRD_ACTION_SEQUENCE);
      break;

    case 16: /* PRD_ACTION_SEQUENCE, PRD_ACTION_SEQUENCE, PRD_IDENT, RULE_END, */
    case 17: /* PRD_ACTION_SEQUENCE, PRD_ACTION_SEQUENCE, PRD_COLON, RULE_END, */
    case 18: /* PRD_ACTION_SEQUENCE, PRD_ACTION_SEQUENCE, PRD_EQUALS, RULE_END, */
    case 19: /* PRD_ACTION_SEQUENCE, PRD_ACTION_SEQUENCE, PRD_SEMICOLON, RULE_END, */
    case 20: /* PRD_ACTION_SEQUENCE, PRD_ACTION_SEQUENCE, PRD_TOKEN, RULE_END, */
      assert(production_syms[production] == PRD_ACTION_SEQUENCE);
      prd_prod_swap(&dst_sym->prod_, &syms[0].prod_);
      pd = &dst_sym->prod_;
      r = prd_append_token_to_snippets(pd, &syms[1].text_, syms[1].match_, syms[1].variant_);
      if (r) return r;
      break;
    case 21: /* PRD_ACTION_SEQUENCE, PRD_ACTION_SEQUENCE, PRD_PAR_OPEN, PRD_ACTION_SEQUENCE, PRD_PAR_CLOSE, RULE_END, */
    case 22: /* PRD_ACTION_SEQUENCE, PRD_ACTION_SEQUENCE, PRD_CUBRACE_OPEN, PRD_ACTION_SEQUENCE, PRD_CUBRACE_CLOSE, RULE_END, */
      assert(production_syms[production] == PRD_ACTION_SEQUENCE);
      prd_prod_swap(&dst_sym->prod_, &syms[0].prod_);
      pd = &dst_sym->prod_;
      r = prd_append_token_to_snippets(pd, &syms[1].text_, syms[1].match_, syms[1].variant_);
      if (r) return r;
      for (n = 0; n < syms[2].prod_.num_snippets_; ++n) {
        r = prd_append_token_to_snippets(pd, &syms[2].prod_.snippets_[n].code_, syms[2].prod_.snippets_[n].original_match_, syms[2].prod_.snippets_[n].original_variant_);
        if (r) return r;
      }
      r = prd_append_token_to_snippets(pd, &syms[3].text_, syms[3].match_, syms[3].variant_);
      if (r) return r;
      break;

    }
  }
  return PRD_SUCCESS;
}

/****************************** ---- End of Initially generated piece ---- ***********************/

int prd_parse(struct prd_stack *stack, struct tkr_tokenizer *tkr, int end_of_input, struct symbol_table *st) {
  int sym;
  int r;

  if (!end_of_input) {
    token_type_t tkt = (token_type_t)tkr->best_match_variant_;
    if (!stack->accept_whitespace_ && (tkt == TOK_WHITESPACE)) {
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
      re_error_tkr(tkr, "Syntax error \"%s\" not expected", tkr->xmatch_.translated_);
    }
    else {
      re_error_tkr(tkr, "Syntax error end of input not expected", tkr->xmatch_.translated_);
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

    struct prd_sym_data nonterminal_data_reduced_to;
    prd_sym_data_init(&nonterminal_data_reduced_to);
    r = reduce(stack, tkr, production, &nonterminal_data_reduced_to, stack->syms_ + stack->pos_ - production_length, st);
    if (r) {
      /* Semantic error */
      return r;
    }

    /* Free symdata for every symbol in the production, including the first slot where we will soon
     * push nonterminal_data_reduced_to */
    size_t n;
    for (n = stack->pos_ - production_length; n < stack->pos_; ++n) {
      prd_sym_data_cleanup(stack->syms_ + n);
    }

    popn_state(stack, production_length);

    current_state = top_state(stack);
    action = parse_table[num_columns * current_state + (nonterminal - minimum_sym)];

    if (!action) {
      re_error_tkr(tkr, "Internal error \"%s\" cannot shift an already reduced nonterminal", tkr->xmatch_.translated_);
      return PRD_INTERNAL_ERROR;
    }
    if (action < 0) {
      re_error_tkr(tkr, "Internal error \"%s\" reduced non-terminal not shifting", tkr->xmatch_.translated_);
      return PRD_INTERNAL_ERROR;
    }
    push_state(stack, action /* action for a shift is the ordinal */);
    struct prd_sym_data *sd = stack->syms_ + stack->pos_ - 1;
    *sd = nonterminal_data_reduced_to;

    /* XXX: Decide when to drop this */
    xlts_append_xlat(&sd->text_, strlen(prd_sym_to_cstr(production_syms[production])), prd_sym_to_cstr(production_syms[production]));

    current_state = top_state(stack);
    action = parse_table[num_columns * current_state + (sym - minimum_sym)];
    if (!action) {
      /* Syntax error */
      if (sym != INPUT_END) {
        re_error_tkr(tkr, "Syntax error \"%s\" not expected", tkr->xmatch_.translated_);
      }
      else {
        re_error_tkr(tkr, "Syntax error end of input not expected", tkr->xmatch_.translated_);
      }
      return PRD_SYNTAX_ERROR;
    }
  }

  /* Shift token onto stack */
  if (action > 0 /* shift? */) {
    push_state(stack, action /* action for a shift is the ordinal */);
    struct prd_sym_data *sym = stack->syms_ + stack->pos_ - 1;
    
    /* Fill in the sym from the tokenizer */
    prd_sym_data_init(sym);
    xlts_append(&sym->text_, &tkr->xmatch_);
    sym->match_ = tkr->best_match_action_;
    sym->variant_ = tkr->best_match_variant_;
  }
  else {
    re_error_tkr(tkr, "Syntax error \"%s\" not expected", tkr->xmatch_.translated_);
    return PRD_SYNTAX_ERROR;
  }

  return PRD_NEXT;
}

int prd_debug_compare_scanners(lr_generator_t *lr) {
  int mismatch = 0;
  if (lr->min_sym != minimum_sym) {
    LOGERROR("min_sym mismatch\n");
    mismatch = 1;
  }
  if ((1 + lr->max_sym - lr->min_sym) != num_columns) {
    LOGERROR("num_columns mismatch\n");
    mismatch = 1;
  }
  if (lr->nr_states != num_rows) {
    LOGERROR("num_rows mismatch\n");
    mismatch = 1;
  }
  if (lr->nr_productions != num_productions) {
    LOGERROR("num_productions mismatch\n");
    mismatch = 1;
  }
  size_t row, col;

  for (row = 0; row < num_rows; ++row) {
    for (col = 0; col < num_columns; ++col) {
      if (lr->parse_table[num_columns * row + col] != parse_table[num_columns * row + col]) {
        LOGERROR("Parse table mismatch\n");
        mismatch = 1;
        goto after_table;
      }
    }
  }

  FILE *outfp = stderr;
  lr_generator_t lalr;
  lalr = *lr;
  size_t num_columns = (size_t)(1 + lalr.max_sym - lalr.min_sym);
  fprintf(outfp, "static int minimum_sym = %d;\n", lalr.min_sym);
  fprintf(outfp, "static size_t num_columns = %zu;\n", num_columns);
  fprintf(outfp, "static size_t num_rows = %zu;\n", (size_t)lalr.nr_states);
  fprintf(outfp, "static size_t num_productions = %zu;\n", lalr.nr_productions);
  fprintf(outfp, "static int parse_table[] = {\n");

  char *column_widths = (char *)malloc(num_columns);
  if (!column_widths) {
    LOGERROR("Error, no memory\n");
    return -1;
  }

  for (col = 0; col < num_columns; ++col) {
    column_widths[col] = 1;
    for (row = 0; row < lalr.nr_states; ++row) {
      int action = lalr.parse_table[row * num_columns + col];
      int width_needed = 1;
      if (action <= -1000) {
        width_needed = 5;
      }
      else if (action <= -100) {
        width_needed = 4;
      }
      else if (action <= -10) {
        width_needed = 3;
      }
      else if (action < 100) {
        width_needed = 2;
      }
      else if (action < 1000) {
        width_needed = 3;
      }
      else if (action < 10000) {
        width_needed = 4;
      }
      else {
        width_needed = 5;
      }
      if (width_needed > column_widths[col]) {
        column_widths[col] = width_needed;
      }
    }
  }
  for (row = 0; row < lalr.nr_states; ++row) {
    for (col = 0; col < num_columns; ++col) {
      int action = lalr.parse_table[row * num_columns + col];

      fprintf(outfp, "%*d,%s", column_widths[col], action, col == (num_columns - 1) ? "\n" : "");
    }
  }
  free(column_widths);
  fprintf(outfp, "};\n");
  fprintf(outfp, "static size_t production_lengths[] = {\n");
  for (row = 0; row < lalr.nr_productions; ++row) {
    fprintf(outfp, " %d%s\n", lalr.production_lengths[row], (row == lalr.nr_productions - 1) ? "" : ",");
  }
  fprintf(outfp, "};\n");
  fprintf(outfp, "static int production_syms[] = {\n");
  for (row = 0; row < lalr.nr_productions; ++row) {
    fprintf(outfp, " %d%s\n", lalr.production_lengths[row], (row == lalr.nr_productions - 1) ? "" : ",");
  }
  fprintf(outfp, "};\n");


after_table:
  return 0;
}

#endif /* implementation switch (see top) */
