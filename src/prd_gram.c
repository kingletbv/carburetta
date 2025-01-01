/* Copyright 2020-2025 Kinglet B.V.
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

#ifndef STDLIB_H_INCLUDED
#define STDLIB_H_INCLUDED
#include <stdlib.h>
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
  snippet_init(&ppd->common_action_sequence_);
  snippet_init(&ppd->action_sequence_);
}

void prd_prod_reset(struct prd_production *ppd) {
  prd_production_sym_reset(&ppd->nt_);
  size_t n;
  for (n = 0; n < ppd->num_syms_; ++n) {
    prd_production_sym_cleanup(&ppd->syms_[n]);
  }
  ppd->num_syms_ = 0;
  snippet_clear(&ppd->common_action_sequence_);
  snippet_clear(&ppd->action_sequence_);
}

void prd_prod_move(struct prd_production *dst, struct prd_production *src) {
  prd_prod_cleanup(dst);
  *dst = *src;
  src->syms_ = NULL;
  src->num_syms_ = 0;
  src->num_syms_allocated_ = 0;
  snippet_init(&src->common_action_sequence_);
  snippet_init(&src->action_sequence_);
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
  snippet_cleanup(&ppd->common_action_sequence_);
  snippet_cleanup(&ppd->action_sequence_);
}

void prd_pattern_init(struct prd_pattern *pat) {
  prd_production_sym_init(&pat->term_);
  pat->regex_ = NULL;
  snippet_init(&pat->common_action_sequence_);
  snippet_init(&pat->action_sequence_);
  pat->pat_ = NULL;
  pat->touched_by_mode_ = 0;
}

void prd_pattern_cleanup(struct prd_pattern *pat) {
  prd_production_sym_cleanup(&pat->term_);
  if (pat->regex_) free(pat->regex_);
  snippet_cleanup(&pat->common_action_sequence_);
  snippet_cleanup(&pat->action_sequence_);
}

void prd_grammar_init(struct prd_grammar *g) {
  g->have_errors_ = 0;
  g->accept_whitespace_ = 0;
  snippet_init(&g->current_common_action_sequence_);
  snippet_init(&g->current_common_pattern_action_sequence_);
  g->num_productions_ = g->num_productions_allocated_ = 0;
  g->productions_ = NULL;
  g->num_patterns_ = g->num_patterns_allocated_ = 0;
  g->patterns_ = NULL;
  g->num_mode_groups_ = g->num_mode_groups_allocated_ = 0;
  g->mode_groups_ = NULL;
}

void prd_grammar_cleanup(struct prd_grammar *g) {
  size_t n;

  snippet_cleanup(&g->current_common_action_sequence_);
  snippet_cleanup(&g->current_common_pattern_action_sequence_);
  for (n = 0; n < g->num_productions_; ++n) {
    prd_prod_cleanup(g->productions_ + n);
  }
  if (g->productions_) free(g->productions_);
  for (n = 0; n < g->num_patterns_; ++n) {
    prd_pattern_cleanup(g->patterns_ + n);
  }
  if (g->patterns_) free(g->patterns_);
  for (n = 0; n < g->num_mode_groups_; ++n) {
    prd_mode_group_cleanup(g->mode_groups_ + n);
  }
  if (g->mode_groups_) free(g->mode_groups_);
}

void prd_mode_init(struct prd_mode *m) {
  xlts_init(&m->id_);
}

void prd_mode_cleanup(struct prd_mode *m) {
  xlts_cleanup(&m->id_);
}

void prd_mode_group_init(struct prd_mode_group *mg) {
  mg->num_modes_ = mg->num_modes_allocated_ = 0;
  mg->modes_ = NULL;
  mg->pattern_start_index_ = mg->pattern_end_index_ = 0;
}

void prd_mode_group_cleanup(struct prd_mode_group *mg) {
  size_t n;
  for (n = 0; n < mg->num_modes_; ++n) {
    prd_mode_cleanup(mg->modes_ + n);
  }
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

int prd_grammar_check_pattern_reserve(struct prd_grammar *g) {
  if (g->num_patterns_ == g->num_patterns_allocated_) {
    size_t new_num = g->num_patterns_allocated_ * 2 + 1;
    if (new_num < 7) new_num = 7;
    if (new_num <= g->num_patterns_allocated_) {
      re_error_nowhere("Internal error, overflow on allocation");
      return PRD_INTERNAL_ERROR;
    }
    if (new_num > (SIZE_MAX / sizeof(struct prd_pattern))) {
      re_error_nowhere("Internal error, overflow on allocation");
      return PRD_INTERNAL_ERROR;
    }
    size_t size_to_alloc = new_num * sizeof(struct prd_pattern);
    void *p = realloc(g->patterns_, size_to_alloc);
    if (!p) {
      re_error_nowhere("Internal error, no memory");
      return PRD_INTERNAL_ERROR;
    }
    g->patterns_ = (struct prd_pattern *)p;
    g->num_patterns_allocated_ = new_num;
  }
  return 0;
}

int prd_grammar_check_mode_group_reserve(struct prd_grammar *g) {
  if (g->num_mode_groups_ == g->num_mode_groups_allocated_) {
    size_t new_num = g->num_mode_groups_allocated_ * 2 + 1;
    if (new_num < 7) new_num = 7;
    if (new_num <= g->num_mode_groups_allocated_) {
      re_error_nowhere("Internal error, overflow on allocation");
      return PRD_INTERNAL_ERROR;
    }
    if (new_num > (SIZE_MAX / sizeof(struct prd_mode_group))) {
      re_error_nowhere("Internal error, overflow on allocation");
      return PRD_INTERNAL_ERROR;
    }
    size_t size_to_alloc = new_num * sizeof(struct prd_mode_group);
    void *p = realloc(g->mode_groups_, size_to_alloc);
    if (!p) {
      re_error_nowhere("Internal error, no memory");
      return PRD_INTERNAL_ERROR;
    }
    g->mode_groups_ = (struct prd_mode_group *)p;
    g->num_mode_groups_allocated_ = new_num;
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

int prd_mode_group_check_mode_reserve(struct prd_mode_group *mg, struct xlts *loc) {
  if (mg->num_modes_ == mg->num_modes_allocated_) {
    size_t new_num = mg->num_modes_allocated_ * 2 + 1;
    if (new_num <= mg->num_modes_allocated_) {
      re_error(loc, "Internal error, overflow on allocation");
      return PRD_INTERNAL_ERROR;
    }
    if (new_num > (SIZE_MAX / sizeof(struct prd_mode))) {
      re_error(loc, "Internal error, overflow on allocation");
      return PRD_INTERNAL_ERROR;
    }
    size_t size_to_alloc = new_num * sizeof(struct prd_mode);
    void *p = realloc(mg->modes_, size_to_alloc);
    if (!p) {
      re_error(loc, "Internal error, no memory");
      return PRD_INTERNAL_ERROR;
    }
    mg->modes_ = (struct prd_mode *)p;
    mg->num_modes_allocated_ = new_num;
  }
  return 0;
}
