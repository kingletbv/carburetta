#if 0
/* Use prd_gram_gen.c implementation */
#include "prd_gram_gen.c"
#elif 1
#include "prd_grammar_alt.c"
#endif

#ifndef REPORT_ERROR_H_INCLUDED
#define REPORT_ERROR_H_INCLUDED
#include "report_error.h"
#endif

#ifndef PRD_GRAM_H_INCLUDED
#define PRD_GRAM_H_INCLUDED
#include "prd_gram.h"
#endif



void prd_production_sym_init(struct prd_production_sym *pps) {
  xlts_init(&pps->id_);
  pps->sym_ = NULL;
}

void prd_production_sym_reset(struct prd_production_sym *pps) {
  xlts_reset(&pps->id_);
  pps->sym_ = NULL;
}

void prd_production_sym_cleanup(struct prd_production_sym *pps) {
  xlts_cleanup(&pps->id_);
}

void prd_prod_init(struct prd_production *ppd) {
  prd_production_sym_init(&ppd->nt_);
  ppd->num_syms_ = ppd->num_syms_allocated_ = 0;
  ppd->syms_ = NULL;
  snippet_init(&ppd->action_sequence_);
}

void prd_prod_reset(struct prd_production *ppd) {
  prd_production_sym_reset(&ppd->nt_);
  size_t n;
  for (n = 0; n < ppd->num_syms_; ++n) {
    prd_production_sym_cleanup(&ppd->syms_[n]);
  }
  ppd->num_syms_ = 0;
  snippet_clear(&ppd->action_sequence_);
}

void prd_prod_cleanup(struct prd_production *ppd) {
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

void prd_grammar_init(struct prd_grammar *g) {
  g->have_errors_ = 0;
  g->accept_whitespace_ = 0;
  g->num_productions_ = g->num_productions_allocated_ = 0;
  g->productions_ = NULL;
}

void prd_grammar_cleanup(struct prd_grammar *g) {
  size_t n;

  for (n = 0; n < g->num_productions_; ++n) {
    prd_prod_cleanup(g->productions_ + n);
  }
  if (g->productions_) free(g->productions_);
}

void prd_prod_swap(struct prd_production *a, struct prd_production *b) {
  struct prd_production swap;
  swap = *a;
  *a = *b;
  *b = swap;
}

int prd_grammar_check_production_reserve(struct prd_grammar *g) {
  if (g->num_productions_ == g->num_productions_allocated_) {
    size_t new_num = g->num_productions_allocated_ * 2 + 1;
    if (new_num < 7) new_num = 7;
    if (new_num <= g->num_productions_allocated_) {
      re_error_nowhere("Internal error, overflow on allocation");
      return PRD_INTERNAL_ERROR;
    }
    if (new_num > (SIZE_MAX / sizeof(struct prd_production))) {
      re_error_nowhere("Internal error, overflow on allocation");
      return PRD_INTERNAL_ERROR;
    }
    size_t size_to_alloc = new_num * sizeof(struct prd_production);
    void *p = realloc(g->productions_, size_to_alloc);
    if (!p) {
      re_error_nowhere("Internal error, no memory");
      return PRD_INTERNAL_ERROR;
    }
    g->productions_ = (struct prd_production *)p;
    g->num_productions_allocated_ = new_num;
  }

  return 0;
}

int prd_prod_check_sym_reserve(struct prd_production *pd, struct xlts *loc) {
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
