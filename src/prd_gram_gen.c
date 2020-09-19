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

static void prd_production_sym_init(struct prd_production_sym *pps) {
  xlts_init(&pps->id_);
  pps->sym_ = NULL;
}

static void prd_production_sym_cleanup(struct prd_production_sym *pps) {
  xlts_cleanup(&pps->id_);
}

static void prd_prod_init(struct prd_production *ppd) {
  prd_production_sym_init(&ppd->nt_);
  ppd->num_syms_ = ppd->num_syms_allocated_ = 0;
  ppd->syms_ = NULL;
  snippet_init(&ppd->action_sequence_);
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
  snippet_cleanup(&ppd->action_sequence_);
}

void prd_prod_swap(struct prd_production *a, struct prd_production *b) {
  struct prd_production swap;
  swap = *a;
  *a = *b;
  *b = swap;
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

int prd_init(void) {
  /* No-op; all tables are static */
  return 0;
}

void prd_cleanup(void) {
  /* No-op; all tables are static */
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




/* --------- HERE GOES THE GENERATED FLUFF ------------ */
union prd_sym_data_union {
 struct xlts uv0;
 struct snippet uv1 [ 5 ];
};
static int minimum_sym = 3;
static size_t num_columns = 19;
static size_t num_rows = 40;
static size_t num_productions = 26;
static int parse_table[] = {
 -2,  0,  0,  0,  0,  0,  0,  0,  0,-2,39, 0, 0, 0, 0, 0, 0, 0, 0,
 -3,  0,  0,  0,  0,  0,  0,  0,  0,-3, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 -4,  0,  0,  0,  0,  0,  0,  0,  0,-4, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 -5,  0,  0,  0,  0,  0,  0,  0,  0,-5, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0,  0,  0,  3,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
-10,-10,-10,-10,-10,-10,  0,-10,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
-11,-11,-11,-11,-11,-11,  0,-11,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
-12,-12,-12,-12,-12,-12,  0,-12,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
-13,-13,-13,-13,-13,-13,  0,-13,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
-14,-14,-14,-14,-14,-14,  0,-14,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 20, 21, 22, 23, 24, 27,  9, 30,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
-16,-16,-16,-16,-16,-16,-16,-16,  0, 0, 0, 0, 0,10, 0, 0, 0, 0, 0,
-15,-15,-15,-15,-15,-15,  0,-15,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 20, 21, 22, 23, 24, 27,  0, 30, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
-16,-16,-16,-16,-16,-16,  0,-16,-16, 0, 0, 0, 0,13, 0, 0, 0, 0, 0,
  5,  6,  7,-25,  8, 11,  0, 14,  0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0,
 -9, -9, -9, -9, -9, -9,  0, -9,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
-26,-26,-26,-26,-26,-26,  0,-26,  0, 0, 0, 0, 0, 0, 0, 0, 0,16, 0,
 -6,  0,  0,  0,  0,  0,  0,  0,  0,-6, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0,  0,  0,  0,  0,  0,  0,  0, 18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
-17,-17,-17,-17,-17,-17,-17,-17,-17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
-18,-18,-18,-18,-18,-18,-18,-18,-18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
-19,-19,-19,-19,-19,-19,-19,-19,-19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
-20,-20,-20,-20,-20,-20,-20,-20,-20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
-21,-21,-21,-21,-21,-21,-21,-21,-21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
-22,-22,-22,-22,-22,-22,-22,-22,-22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 20, 21, 22, 23, 24, 27, 25, 30,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
-16,-16,-16,-16,-16,-16,-16,-16,  0, 0, 0, 0, 0,26, 0, 0, 0, 0, 0,
-23,-23,-23,-23,-23,-23,-23,-23,-23, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 20, 21, 22, 23, 24, 27,  0, 30, 28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
-16,-16,-16,-16,-16,-16,  0,-16,-16, 0, 0, 0, 0,29, 0, 0, 0, 0, 0,
 20, 21, 22, 23, 24, 27,  0, 30,-25, 0, 0, 0, 0, 0, 0, 0,19, 0, 0,
-16,-16,-16,-16,-16,-16,  0,-16,-16, 0, 0, 0, 0,31, 0, 0, 0, 0, 0,
  0,  0,  0,  0,  0,  0,  0, 32,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0,  0, 17,  0,  0,  0,  0,-26,  0, 0, 0, 0, 0, 0,15, 0, 0,33, 0,
 -8,  0, -8, -8,  0,  0,  0, -8,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 35,  0,-24,  2,  0,  0,  0,-24,  0, 0, 0, 0, 0, 0, 0,34, 0, 0, 0,
 -7,  0, -7, -7,  0,  0,  0, -7,  0, 0, 0,36, 0, 0, 0, 0, 0, 0, 0,
  0, 37,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 38,  0,  0,  0,  0,  0,  0,  0,  0,-1, 0, 0, 1, 0, 0, 0, 0, 0, 0,
};
static size_t production_lengths[] = {
 1,
 0,
 2,
 4,
 7,
 9,
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
static int production_syms[] = {
 21,
 13,
 13,
 15,
 15,
 15,
 14,
 14,
 17,
 17,
 17,
 17,
 17,
 17,
 17,
 16,
 16,
 16,
 16,
 16,
 16,
 16,
 16,
 18,
 19,
 20
};
static int state_syms[] = {
 -1,
 15,
 6,
 6,
 19,
 3,
 4,
 5,
 7,
 9,
 16,
 8,
 11,
 16,
 10,
 17,
 20,
 5,
 11,
 19,
 3,
 4,
 5,
 6,
 7,
 9,
 16,
 8,
 11,
 16,
 10,
 16,
 10,
 20,
 18,
 3,
 14,
 4,
 3,
 13
};
#define NT_END 0
#define RULE_END 1
#define GRAMMAR_END 2
#define PRD_IDENT 3
#define PRD_COLON 4
#define PRD_EQUALS 5
#define PRD_SEMICOLON 6
#define PRD_TOKEN 7
#define PRD_PAR_OPEN 8
#define PRD_PAR_CLOSE 9
#define PRD_CUBRACE_OPEN 10
#define PRD_CUBRACE_CLOSE 11

#define INPUT_END 12
#define PRD_GRAMMAR 13
#define PRD_RULE 14
#define PRD_PRODUCTION 15
#define PRD_ACTION_SEQUENCE 16
#define PRD_STMT_ACTION 17
#define PRD_START_C_TOKENIZER 18
#define PRD_END_C_TOKENIZER 19
#define PRD_ACCEPT_WHITESPACE 20
#define SYNTHETIC_S 21
static int reduce(struct prd_stack *stack, struct tkr_tokenizer *tkr, int production, struct prd_sym_data *dst_sym, struct prd_sym_data *syms, struct symbol_table *st) {
  int r;
  struct prd_production *pd;
  struct symbol *sym;
  switch (production) {
  case 1: {

  }
  break;
  case 2: {
	/* Store the PRD_PRODUCTION in the prd_stack->productions_ array */
	r = prd_stack_check_production_reserve(stack);
	if (r) return r;
	pd = stack->productions_ + stack->num_productions_++;
	prd_prod_init(pd);
	prd_prod_swap(pd, &syms[1].prod_);
}
  break;
  case 3: {
	sym = symbol_find(st, syms[0].text_.translated_);
	if (!sym) {
		re_error(&syms[1].text_, "Error, symbol \"\" not declared as %nt", syms[0].text_.translated_);
		stack->have_errors_ = 1;
		return PRD_SUCCESS;
	}

	prd_prod_swap(&dst_sym->prod_, &syms[2].prod_);
	pd = &dst_sym->prod_;
	xlts_append(&pd->nt_.id_, &syms[0].text_);
	pd->nt_.sym_ = sym;
}
  break;
  case 4: {
	sym = symbol_find(st, syms[0].text_.translated_);
	if (!sym) {
		re_error(&syms[1].text_, "Error, symbol \"\" not declared as %nt", syms[0].text_.translated_);
		stack->have_errors_ = 1;
		return PRD_SUCCESS;
	}

	prd_prod_swap(&dst_sym->prod_, &syms[2].prod_);
	pd = &dst_sym->prod_;
	xlts_append(&pd->nt_.id_, &syms[0].text_);
	pd->nt_.sym_ = sym;

	/* Move snippets in; could do this more efficiently by manually swapping pointers; but this works.. */
	r = snippet_append_snippet(&pd->action_sequence_, &syms[4].prod_.action_sequence_);
	if (r) return r;

	/* Append the semicolon to the snippets */
	r = snippet_append(&pd->action_sequence_, syms[6].match_, syms[6].variant_, &syms[6].text_);
	if (r) return r;
}
  break;
  case 5: {
	sym = symbol_find(st, syms[0].text_.translated_);
	if (!sym) {
		re_error(&syms[1].text_, "Error, symbol \"\" not declared as %nt", syms[0].text_.translated_);
		stack->have_errors_ = 1;
		return PRD_SUCCESS;
	}

	prd_prod_swap(&dst_sym->prod_, &syms[2].prod_);
	pd = &dst_sym->prod_;
	xlts_append(&pd->nt_.id_, &syms[0].text_);
	pd->nt_.sym_ = sym;

	/* Move snippets in; could do this more efficiently by manually swapping pointers; but this works.. */
	/* Append the cubrace-open to the snippets */
	r = snippet_append(&pd->action_sequence_, syms[5].match_, syms[5].variant_, &syms[5].text_);
	if (r) return r;

	/* Move snippets in; could do this more efficiently by manually swapping pointers; but this works.. */
	r = snippet_append_snippet(&pd->action_sequence_, &syms[6].prod_.action_sequence_);
	if (r) return r;

	/* Append the cubrace-close to the snippets */
	r = snippet_append(&pd->action_sequence_, syms[8].match_, syms[8].variant_, &syms[8].text_);
	if (r) return r;
}
  break;
  case 6: {

  }
  break;
  case 7: {
	/* Grab ident from position 1, reduce */
	sym = symbol_find(st, syms[1].text_.translated_);
	if (!sym) {
		re_error(&syms[1].text_, "Error, symbol \"\" not declared as %nt or %token", syms[1].text_.translated_);
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
}
  break;
  case 8: {

  }
  break;
  case 9: {
	prd_prod_swap(&dst_sym->prod_, &syms[0].prod_);
	pd = &dst_sym->prod_;
	r = snippet_append(&pd->action_sequence_, syms[1].match_, syms[1].variant_, &syms[1].text_);
	if (r) return PRD_INTERNAL_ERROR;
}
  break;
  case 10: {
	prd_prod_swap(&dst_sym->prod_, &syms[0].prod_);
	pd = &dst_sym->prod_;
	r = snippet_append(&pd->action_sequence_, syms[1].match_, syms[1].variant_, &syms[1].text_);
	if (r) return PRD_INTERNAL_ERROR;
}
  break;
  case 11: {
	prd_prod_swap(&dst_sym->prod_, &syms[0].prod_);
	pd = &dst_sym->prod_;
	r = snippet_append(&pd->action_sequence_, syms[1].match_, syms[1].variant_, &syms[1].text_);
	if (r) return PRD_INTERNAL_ERROR;
}
  break;
  case 12: {
	prd_prod_swap(&dst_sym->prod_, &syms[0].prod_);
	pd = &dst_sym->prod_;
	r = snippet_append(&pd->action_sequence_, syms[1].match_, syms[1].variant_, &syms[1].text_);
	if (r) return PRD_INTERNAL_ERROR;
}
  break;
  case 13: {
	prd_prod_swap(&dst_sym->prod_, &syms[0].prod_);
	pd = &dst_sym->prod_;

	/* Append the par-open to the snippets */
	r = snippet_append(&pd->action_sequence_, syms[1].match_, syms[1].variant_, &syms[1].text_);
	if (r) return r;

	/* Move snippets in; could do this more efficiently by manually swapping pointers; but this works.. */
	r = snippet_append_snippet(&pd->action_sequence_, &syms[2].prod_.action_sequence_);
	if (r) return r;

	/* Append the par-close to the snippets */
	r = snippet_append(&pd->action_sequence_, syms[3].match_, syms[3].variant_, &syms[3].text_);
	if (r) return r;
}
  break;
  case 14: {
	prd_prod_swap(&dst_sym->prod_, &syms[0].prod_);
	pd = &dst_sym->prod_;
	/* Append the cubrace-open to the snippets */
	r = snippet_append(&pd->action_sequence_, syms[1].match_, syms[1].variant_, &syms[1].text_);
	if (r) return r;

	/* Move snippets in; could do this more efficiently by manually swapping pointers; but this works.. */
	r = snippet_append_snippet(&pd->action_sequence_, &syms[2].prod_.action_sequence_);
	if (r) return r;

	/* Append the cubrace-close to the snippets */
	r = snippet_append(&pd->action_sequence_, syms[3].match_, syms[3].variant_, &syms[3].text_);
	if (r) return r;
}
  break;
  case 15: {

  }
  break;
  case 16: {
	prd_prod_swap(&dst_sym->prod_, &syms[0].prod_);
	pd = &dst_sym->prod_;
	r = snippet_append(&pd->action_sequence_, syms[1].match_, syms[1].variant_, &syms[1].text_);
	if (r) return PRD_INTERNAL_ERROR;
}
  break;
  case 17: {
	prd_prod_swap(&dst_sym->prod_, &syms[0].prod_);
	pd = &dst_sym->prod_;
	r = snippet_append(&pd->action_sequence_, syms[1].match_, syms[1].variant_, &syms[1].text_);
	if (r) return PRD_INTERNAL_ERROR;
}
  break;
  case 18: {
	prd_prod_swap(&dst_sym->prod_, &syms[0].prod_);
	pd = &dst_sym->prod_;
	r = snippet_append(&pd->action_sequence_, syms[1].match_, syms[1].variant_, &syms[1].text_);
	if (r) return PRD_INTERNAL_ERROR;
}
  break;
  case 19: {
	prd_prod_swap(&dst_sym->prod_, &syms[0].prod_);
	pd = &dst_sym->prod_;
	r = snippet_append(&pd->action_sequence_, syms[1].match_, syms[1].variant_, &syms[1].text_);
	if (r) return PRD_INTERNAL_ERROR;
}
  break;
  case 20: {
	prd_prod_swap(&dst_sym->prod_, &syms[0].prod_);
	pd = &dst_sym->prod_;
	r = snippet_append(&pd->action_sequence_, syms[1].match_, syms[1].variant_, &syms[1].text_);
	if (r) return PRD_INTERNAL_ERROR;
}
  break;
  case 21: {
	prd_prod_swap(&dst_sym->prod_, &syms[0].prod_);
	pd = &dst_sym->prod_;
	/* Append the par-open to the snippets */
	r = snippet_append(&pd->action_sequence_, syms[1].match_, syms[1].variant_, &syms[1].text_);
	if (r) return r;

	/* Move snippets in; could do this more efficiently by manually swapping pointers; but this works.. */
	r = snippet_append_snippet(&pd->action_sequence_, &syms[2].prod_.action_sequence_);
	if (r) return r;

	/* Append the par-close to the snippets */
	r = snippet_append(&pd->action_sequence_, syms[3].match_, syms[3].variant_, &syms[3].text_);
	if (r) return r;
}
  break;
  case 22: {
	prd_prod_swap(&dst_sym->prod_, &syms[0].prod_);
	pd = &dst_sym->prod_;
	/* Append the cubrace-open to the snippets */
	r = snippet_append(&pd->action_sequence_, syms[1].match_, syms[1].variant_, &syms[1].text_);
	if (r) return r;

	/* Move snippets in; could do this more efficiently by manually swapping pointers; but this works.. */
	r = snippet_append_snippet(&pd->action_sequence_, &syms[2].prod_.action_sequence_);
	if (r) return r;

	/* Append the cubrace-close to the snippets */
	r = snippet_append(&pd->action_sequence_, syms[3].match_, syms[3].variant_, &syms[3].text_);
	if (r) return r;
}
  break;
  case 23: {
tok_switch_to_c_idents(tkr);
  }
  break;
  case 24: {
	tok_switch_to_nonterminal_idents(tkr);
	stack->accept_whitespace_ = 0; /* Reset to normal tokens */
}
  break;
  case 25: {
	/* Welcome whitespace from this point. Note that this point is *after* the lookahead at the point
	 * that the PRD_ACCEPT_WHITESPACE non-terminal appears. Therefore, it is *after* the EQUALS sign and
	 * *after* the curly opening brace. */
	stack->accept_whitespace_ = 1;
}
  break;
  } /* switch */
  return PRD_SUCCESS;
}
/* --------- HERE ENDS THE GENERATED FLUFF ------------ */


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
