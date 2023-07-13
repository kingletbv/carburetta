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

#ifndef STDLIB_H_INCLUDED
#define STDLIB_H_INCLUDED
#include <stdlib.h>
#endif

#ifndef STDIO_H_INCLUDED
#define STDIO_H_INCLUDED
#include <stdio.h>
#endif

#ifndef CTYPE_H_INCLUDED
#define CTYPE_H_INCLUDED
#include <ctype.h>
#endif

#ifdef __linux__
/* expose ffsll - find first set - from string.h */
#define _GNU_SOURCE
#endif
#ifndef STRING_H_INCLUDED
#define STRING_H_INCLUDED
#include <string.h>
#endif

#ifdef _MSC_VER
#ifndef INTRIN_H_INCLUDED
#define INTRIN_H_INCLUDED
#include <intrin.h>
#endif
#endif

#ifndef ASSERT_H_INCLUDED
#define ASSERT_H_INCLUDED
#include <assert.h>
#endif

#ifndef REX_PARSE_H_INCLUDED
#define REX_PARSE_H_INCLUDED
struct rex_nfa;
#include "rex_parse.h"
#endif

#ifndef REX_H_INCLUDED
#define REX_H_INCLUDED
#include "rex.h"
#endif


void rex_nfa_init(struct rex_nfa *nfa) {
  nfa->failed_ = 0;

  nfa->num_nfa_nodes_ = nfa->num_nfa_nodes_allocated_ = 0;
  nfa->nfa_nodes_ = NULL;

  nfa->start_ = nfa->end_ = 0;
}

void rex_nfa_cleanup(struct rex_nfa *nfa) {
  size_t n;
  for (n = 0; n < nfa->num_nfa_nodes_; ++n) {
    struct rex_nfa_node *node = nfa->nfa_nodes_ + n;

    /* If we clean up all outbound transitions, we'll implicitly have freed up all
    * inbound transitions. */
    struct rex_nfa_trans *trans = node->outbound_;
    if (trans) {
      do {
        struct rex_nfa_trans *next = trans->from_peer_;

        free(trans);

        trans = next;
      } while (trans != node->outbound_);        
    }
  }
}

static int rex_nfa_trans_cmp_from(struct rex_nfa_trans *left, struct rex_nfa_trans *right) {
  if (left->is_empty_ && !right->is_empty_) {
    return -1;
  }
  if (!left->is_empty_ && right->is_empty_) {
    return 1;
  }
  if (left->is_empty_) {
    /* Both are empty transitions; compare on NFA node index we are going to */
    if (left->to_ < right->to_) {
      return -1;
    }
    else if (left->to_ > right->to_) {
      return 1;
    }
    return 0;
  }
  if (left->is_anchor_ && !right->is_anchor_) {
    return -1;
  }
  if (!left->is_anchor_ && right->is_anchor_) {
    return 1;
  }
  if (left->symbol_start_ < right->symbol_start_) {
    return -1;
  }
  if (left->symbol_start_ > right->symbol_start_) {
    return 1;
  }
  if (left->symbol_end_ < right->symbol_end_) {
    return -1;
  }
  if (left->symbol_end_ > right->symbol_end_) {
    return 1;
  }
  /* Equal transition symbols, compare on NFA node index they are going to */
  if (left->to_ < right->to_) {
    return -1;
  }
  if (left->to_ > right->to_) {
    return 1;
  }
  return 0;
}

static void rex_nfa_insert_trans(struct rex_nfa *nfa, struct rex_nfa_trans *t) {
  if (nfa->nfa_nodes_[t->to_].inbound_) {
    t->to_peer_ = nfa->nfa_nodes_[t->to_].inbound_->to_peer_;
  }
  else {
    t->to_peer_ = t;
  }
  nfa->nfa_nodes_[t->to_].inbound_ = t;

  if (nfa->nfa_nodes_[t->from_].outbound_) {
    struct rex_nfa_trans *peer = nfa->nfa_nodes_[t->from_].outbound_;
    int inserted = 0;
    do {
      /* While the peer is not the last one, check if we need to insert the new transition before the next peer */
      if (rex_nfa_trans_cmp_from(t, peer->from_peer_) <= 0) {
        t->from_peer_ = peer->from_peer_;
        peer->from_peer_ = t;
        inserted = 1;
      }
    } while (peer != nfa->nfa_nodes_[t->from_].outbound_);
    if (!inserted) {
      /* Insert on the tail */
      t->from_peer_ = peer->from_peer_;
      peer->from_peer_ = t;
      nfa->nfa_nodes_[t->from_].outbound_ = t;
    }
  }
  else {
    t->from_peer_ = t;
    nfa->nfa_nodes_[t->from_].outbound_ = t;
  }
}

void rex_nfa_make_empty_trans(struct rex_nfa *nfa, size_t from, size_t to) {
  if ((from == SIZE_MAX) || (to == SIZE_MAX)) {
    if (!nfa->failed_) nfa->failed_ = _REX_INTERNAL_ERROR;
    return;
  }

  struct rex_nfa_trans *t = (struct rex_nfa_trans *)malloc(sizeof(struct rex_nfa_trans));
  if (!t) {
    nfa->failed_ = _REX_NO_MEMORY;
    return;
  }
  t->from_ = from;
  t->to_ = to;
  t->is_anchor_ = 0;
  t->is_empty_ = 1;
  t->symbol_start_ = 0;
  t->symbol_end_ = 0;

  rex_nfa_insert_trans(nfa, t);
}

void rex_nfa_make_trans(struct rex_nfa *nfa, size_t from, size_t to, uint32_t sym) {
  if ((from == SIZE_MAX) || (to == SIZE_MAX)) {
    if (!nfa->failed_) nfa->failed_ = _REX_INTERNAL_ERROR;
    return;
  }

  struct rex_nfa_trans *t = (struct rex_nfa_trans *)malloc(sizeof(struct rex_nfa_trans));
  if (!t) {
    nfa->failed_ = _REX_NO_MEMORY;
    return;
  }
  t->from_ = from;
  t->to_ = to;
  t->is_anchor_ = 0;
  t->is_empty_ = 0;
  t->symbol_start_ = sym;
  t->symbol_end_ = sym + 1;

  rex_nfa_insert_trans(nfa, t);
}

void rex_nfa_make_ranged_trans(struct rex_nfa *nfa, size_t from, size_t to, uint32_t first_sym, uint32_t last_sym_inclusive) {
  if ((from == SIZE_MAX) || (to == SIZE_MAX)) {
    if (!nfa->failed_) nfa->failed_ = _REX_INTERNAL_ERROR;
    return;
  }

  struct rex_nfa_trans *t = (struct rex_nfa_trans *)malloc(sizeof(struct rex_nfa_trans));
  if (!t) {
    nfa->failed_ = _REX_NO_MEMORY;
    return;
  }
  t->from_ = from;
  t->to_ = to;
  t->is_anchor_ = 0;
  t->is_empty_ = 0;
  t->symbol_start_ = first_sym;
  t->symbol_end_ = last_sym_inclusive + 1;

  rex_nfa_insert_trans(nfa, t);
}

void rex_nfa_make_anchor_trans(struct rex_nfa *nfa, size_t from, size_t to, uint32_t anchor_sym) {
  if ((from == SIZE_MAX) || (to == SIZE_MAX)) {
    if (!nfa->failed_) nfa->failed_ = _REX_INTERNAL_ERROR;
    return;
  }

  struct rex_nfa_trans *t = (struct rex_nfa_trans *)malloc(sizeof(struct rex_nfa_trans));
  if (!t) {
    nfa->failed_ = _REX_NO_MEMORY;
    return;
  }
  t->from_ = from;
  t->to_ = to;
  t->is_anchor_ = 1;
  t->is_empty_ = 0;
  t->symbol_start_ = anchor_sym;
  t->symbol_end_ = 0;

  rex_nfa_insert_trans(nfa, t);
}

size_t rex_nfa_make_node(struct rex_nfa *nfa) {
  if (nfa->num_nfa_nodes_ == nfa->num_nfa_nodes_allocated_) {
    size_t new_num_allocated = nfa->num_nfa_nodes_allocated_ * 2 + 1;
    if (new_num_allocated <= nfa->num_nfa_nodes_allocated_) {
      nfa->failed_ = _REX_OVERFLOW;
      /* overflow */
      return SIZE_MAX;
    }
    if (new_num_allocated < 8) {
      new_num_allocated = 8;
    }
    if ((SIZE_MAX / sizeof(struct rex_nfa_node)) < new_num_allocated) {
      nfa->failed_ = _REX_OVERFLOW;
      /* overflow */
      return SIZE_MAX;
    }

    size_t size_to_alloc = sizeof(struct rex_nfa_node) * new_num_allocated;
    void *buf = realloc(nfa->nfa_nodes_, size_to_alloc);
    if (!buf) {
      nfa->failed_ = _REX_NO_MEMORY;
      /* no memory */
      return SIZE_MAX;
    }
    nfa->nfa_nodes_ = (struct rex_nfa_node *)buf;
    nfa->num_nfa_nodes_allocated_ = new_num_allocated;
  }

  size_t nfa_node_index = nfa->num_nfa_nodes_++;
  struct rex_nfa_node *n = nfa->nfa_nodes_ + nfa_node_index;
  n->inbound_ = NULL;
  n->outbound_ = NULL;
  n->pattern_matched_ = NULL;
  return nfa_node_index;
}

void rex_nfa_compute_closure(struct rex_nfa *nfa, uint64_t *nfa_map) {
  size_t n;
  int found_new_node;
  do {
    found_new_node = 0;
    for (n = 0; n < nfa->num_nfa_nodes_; ++n) {
      int is_set = !!(nfa_map[n >> 6] & (((uint64_t)1) << (n & 0x3F)));
      if (is_set) {
        struct rex_nfa_node *node = nfa->nfa_nodes_ + n;

        struct rex_nfa_trans *t;
        t = node->outbound_; /* tail ptr, t is last in cyclic chain */
        if (t) {
          do {
            t = t->from_peer_;

            if (t->is_empty_) {
              int target_is_set = !(nfa_map[t->to_ >> 6] & (((uint64_t)1) << (t->to_ & 0x3F)));

              found_new_node = found_new_node || target_is_set;

              nfa_map[t->to_ >> 6] |= ((uint64_t)1) << (t->to_ & 0x3F);
            }

          } while (t != node->outbound_);
        }
      }
    }
  } while (found_new_node);
}

void rex_init(struct rex_scanner *rex) {
  rex_stack_init(&rex->stack_);
  rex_nfa_init(&rex->nfa_);
  rex_dfa_init(&rex->dfa_);
  rex->patterns_ = NULL;
  rex->modes_ = NULL;
  rex->next_pattern_ordinal_ = 0;
}

void rex_cleanup(struct rex_scanner *rex) {
  struct rex_pattern *pat;
  pat = rex->patterns_;
  if (pat) {
    do {
      struct rex_pattern *next = pat->chain_;

      free(pat);

      pat = next;

    } while (pat != rex->patterns_);
  }
  struct rex_mode *mode;
  mode = rex->modes_;
  if (mode) {
    do {
      struct rex_mode *next = mode->chain_;

      struct rex_pattern_mode *pm = mode->patterns_;
      if (pm) {
        do {
          struct rex_pattern_mode *next = pm->next_in_mode_;

          free(pm);

          pm = next;
        } while (pm != mode->patterns_);
      }

      free(mode);

      mode = next;
    } while (mode != rex->modes_);
  }
  rex_stack_cleanup(&rex->stack_);
  rex_nfa_cleanup(&rex->nfa_);
  rex_dfa_cleanup(&rex->dfa_);
}

int rex_alloc_pattern(struct rex_scanner *rex, uintptr_t action, struct rex_pattern **ppat) {
  if (rex->nfa_.failed_) {
    return rex->nfa_.failed_;
  }

  struct rex_pattern *pat = (struct rex_pattern *)malloc(sizeof(struct rex_pattern));
  if (!pat) return _REX_NO_MEMORY;

  pat->nfa_begin_state_ = 0;
  pat->nfa_final_state_ = 0;
  pat->ordinal_ = rex->next_pattern_ordinal_++;
  pat->action_ = action;
  pat->modes_ = NULL;

  if (rex->patterns_) {
    pat->chain_ = rex->patterns_->chain_;
    rex->patterns_->chain_ = pat;
  }
  else {
    pat->chain_ = pat;
  }
  rex->patterns_ = pat;

  if (ppat) *ppat = pat;

  return 0;
}

int rex_add_pattern(struct rex_scanner *rex, const char *regex, uintptr_t action, struct rex_pattern **ppat) {
  int r;

  if (rex->nfa_.failed_) {
    return rex->nfa_.failed_;
  }

  r = rex_stack_reset(&rex->stack_);
  if (r) return r;

  rex_set_input(&rex->stack_, regex, strlen(regex), 1);

  size_t start, end;
  r = rex_scan(&rex->stack_, &rex->nfa_, &start, &end);
  if (r) return r;

  if (rex->nfa_.failed_) {
    return rex->nfa_.failed_;
  }

  struct rex_pattern *pat = NULL;
  r = rex_alloc_pattern(rex, action, &pat);

  if (!pat) return _REX_NO_MEMORY;

  pat->nfa_begin_state_ = start;
  pat->nfa_final_state_ = end;

  rex->nfa_.nfa_nodes_[end].pattern_matched_ = pat;

  if (ppat) *ppat = pat;

  return 0;
}

int rex_add_mode(struct rex_scanner *rex, struct rex_mode **pmode) {
  size_t new_nfa_node = rex_nfa_make_node(&rex->nfa_);
  if (rex->nfa_.failed_) {
    return rex->nfa_.failed_;
  }

  struct rex_mode *mode = (struct rex_mode *)malloc(sizeof(struct rex_mode));
  if (!mode) {
    return rex->nfa_.failed_ = _REX_NO_MEMORY;
  }
  if (rex->modes_) {
    mode->chain_ = rex->modes_->chain_;
    rex->modes_->chain_ = mode;
  }
  else {
    mode->chain_ = mode;
  }
  rex->modes_ = mode;
  mode->patterns_ = NULL;
  mode->nfa_begin_state_ = new_nfa_node;
  mode->dfa_node_ = NULL;

  if (pmode) *pmode = mode;

  return 0;
}

int rex_add_pattern_to_mode(struct rex_mode *mode, struct rex_pattern *pat) {
  struct rex_pattern_mode *pm = (struct rex_pattern_mode *)malloc(sizeof(struct rex_pattern_mode));
  if (!pm) {
    return _REX_NO_MEMORY;
  }
  pm->mode_ = mode;
  pm->pattern_ = pat;

  if (mode->patterns_) {
    pm->next_in_mode_ = mode->patterns_->next_in_mode_;
    mode->patterns_->next_in_mode_ = pm;
  }
  else {
    pm->next_in_mode_ = pm;
  }
  mode->patterns_ = pm;

  if (pat->modes_) {
    pm->next_in_pattern_ = pat->modes_->next_in_pattern_;
    pat->modes_->next_in_pattern_ = pm;
  }
  else {
    pm->next_in_pattern_ = pm;
  }
  pat->modes_ = pm;

  return 0;
}

void rex_dfa_init(struct rex_dfa *dfa) {
  dfa->failed_ = 0;
  dfa->nodes_ = NULL;
  dfa->next_dfa_node_ordinal_ = 1;
  dfa->trans_groups_ = NULL;
  dfa->symbol_groups_ = NULL;
  size_t n;
  for (n = 0; n < REX_DFA_HASH_TABLE_SIZE; ++n) {
    dfa->hash_table_[n] = NULL;
  }
}

void rex_selector_cleanup(struct rex_selector *selector) {
  if (selector->next_in_dfa_transition_group_ == selector) {
    selector->dfa_transition_group_->selectors_ = NULL;
  }
  else {
    if (selector->dfa_transition_group_->selectors_ == selector) {
      selector->dfa_transition_group_->selectors_ = selector->next_in_dfa_transition_group_;
    }
    selector->next_in_dfa_transition_group_->prev_in_dfa_transition_group_ = selector->prev_in_dfa_transition_group_;
    selector->prev_in_dfa_transition_group_->next_in_dfa_transition_group_ = selector->next_in_dfa_transition_group_;
  }

  if (selector->next_in_symbol_group_ == selector) {
    selector->symbol_group_->selectors_ = NULL;
  }
  else {
    if (selector->symbol_group_->selectors_ == selector) {
      selector->symbol_group_->selectors_ = selector->next_in_symbol_group_;
    }
    selector->next_in_symbol_group_->prev_in_symbol_group_ = selector->prev_in_symbol_group_;
    selector->prev_in_symbol_group_->next_in_symbol_group_ = selector->next_in_symbol_group_;
  }
}

void rex_dfa_cleanup(struct rex_dfa *dfa) {
  struct rex_dfa_node *dn = dfa->nodes_;
  if (dn) {
    do {
      struct rex_dfa_node *next = dn->chain_;

      struct rex_dfa_trans *trans = dn->outbound_;
      if (trans) {
        do {
          struct rex_dfa_trans *next = trans->from_peer_;

          free(trans);

          trans = next;
        } while (trans != dn->outbound_);
      }

      free(dn);

      dn = next;

    } while (dn != dfa->nodes_);
  }
  struct rex_symbol_group *sg = dfa->symbol_groups_;
  if (sg) {
    do {
      struct rex_symbol_group *next = sg->chain_;

      struct rex_symbol_range *sr = sg->ranges_;
      if (sr) {
        do {
          struct rex_symbol_range *next = sr->chain_;

          free(sr);

          sr = next;
        } while (sr != sg->ranges_);
      }

      while (sg->selectors_) {
        struct rex_selector *selector = sg->selectors_;
        rex_selector_cleanup(selector);
        free(selector);
      }

      free(sg);

      sg = next;
    } while (sg != dfa->symbol_groups_);
  }
  struct rex_dfa_trans_group *tg = dfa->trans_groups_;
  if (tg) {
    do {
      tg = tg->sibling_;

      while (tg->selectors_) {
        struct rex_selector *selector = tg->selectors_;
        rex_selector_cleanup(selector);
        free(selector);
      }

    } while (tg != dfa->trans_groups_);
  }
}

static struct rex_dfa_node *rex_alloc_dfa_node(struct rex_scanner *rex) {
  size_t map_size = sizeof(uint64_t) * ((rex->nfa_.num_nfa_nodes_ + 63) / 64);
  size_t dfa_node_size = sizeof(struct rex_dfa_node) + map_size;
  struct rex_dfa_node *dn = (struct rex_dfa_node *)malloc(dfa_node_size);
  if (!dn) return NULL;

  /* Clearing the variable length map_ to 0 in particular here... */
  memset(dn, 0, dfa_node_size);
  dn->chain_ = dn;
  dn->hash_chain_ = NULL;
  dn->inbound_ = NULL;
  dn->outbound_ = NULL;

  dn->pattern_matched_ = NULL;

  dn->ordinal_ = rex->dfa_.next_dfa_node_ordinal_++;

  return dn;
}

static struct rex_dfa_trans *rex_alloc_dfa_trans(struct rex_scanner *rex, struct rex_dfa_node *from, struct rex_dfa_node *to) {
  struct rex_dfa_trans *dt = (struct rex_dfa_trans *)malloc(sizeof(struct rex_dfa_trans));
  if (!dt) return NULL;
  
  dt->symbol_start_ = dt->symbol_end_ = 0;

  dt->is_anchor_ = 0;

  dt->from_ = from;
  dt->to_ = to;

  if (from) {
    if (from->outbound_) {
      dt->from_peer_ = from->outbound_->from_peer_;
      from->outbound_->from_peer_ = dt;
    }
    else {
      dt->from_peer_ = dt;
    }
    from->outbound_ = dt;
  }
  else {
    dt->from_peer_ = dt;
  }

  if (to) {
    if (to->inbound_) {
      dt->to_peer_ = to->inbound_->to_peer_;
      to->inbound_->to_peer_ = dt;
    }
    else {
      dt->to_peer_ = dt;
    }
    from->inbound_ = dt;
  }
  else {
    dt->to_peer_ = dt;
  }

  dt->trans_group_sibling_ = NULL;

  return dt;
}

static struct rex_dfa_trans_group *rex_alloc_trans_group(struct rex_dfa *dfa) {
  struct rex_dfa_trans_group *tg = (struct rex_dfa_trans_group *)malloc(sizeof(struct rex_dfa_trans_group));
  if (!tg) {
    return NULL;
  }
  if (dfa->trans_groups_) {
    tg->ordinal_ = dfa->trans_groups_->ordinal_ + 1; /* tail's ordinal + 1*/
    tg->sibling_ = dfa->trans_groups_->sibling_;
    dfa->trans_groups_->sibling_ = tg;
  }
  else {
    /* First ordinal is 0 */
    tg->ordinal_ = 0;
    tg->sibling_ = tg;
  }
  dfa->trans_groups_ = tg;

  tg->transitions_ = NULL;
  tg->heap_is_at_backside_ = 0;
  tg->selectors_ = NULL;

  return tg;
}

static uint32_t rex_dfa_trans_group_get_clipped_range_value(struct rex_dfa_trans_group *dtg) {
  /* Check the first of the transitions (these rotate round-robin during processing, always pointing to the current
   * symbol range for which the trans_group was inserted) and determine if the start of that range is valid for the
   * symbol_clip, if so, return it, if not, return the end of the range. */
  if (dtg->heap_is_at_backside_) {
    return dtg->transitions_->trans_group_sibling_->symbol_end_;
  }
  return dtg->transitions_->trans_group_sibling_->symbol_start_;
}

static void rex_dfa_trans_group_heapify(struct rex_dfa_trans_group **heap, size_t length, size_t at) {
  for (;;) {
    size_t left, right;
    left = at + at + 1;
    right = left + 1;
    size_t smallest;
    if ((left < length) && 
        (rex_dfa_trans_group_get_clipped_range_value(heap[left]) < rex_dfa_trans_group_get_clipped_range_value(heap[at]))) {
      smallest = left;
    }
    else {
      smallest = at;
    }
    if ((right < length) &&
        (rex_dfa_trans_group_get_clipped_range_value(heap[right]) < rex_dfa_trans_group_get_clipped_range_value(heap[smallest]))) {
      smallest = right;
    }
    if (smallest == at) {
      /* no swap needed, the heap is sound. */
      return;
    }
    /* Tail cyclic recursion; continue to heapify at index smallest */
    struct rex_dfa_trans_group *swap = heap[smallest];
    heap[smallest] = heap[at];
    heap[at] = swap;
    at = smallest;
  }
}

static int rex_dfa_make_trans_group_priority_queue(struct rex_dfa *dfa, size_t *plength, struct rex_dfa_trans_group ***pheap) {
  size_t queue_length = dfa->trans_groups_ ? (size_t)dfa->trans_groups_->ordinal_ + 1 : 0;
  if (!queue_length) return 0;

  struct rex_dfa_trans_group **heap = (struct rex_dfa_trans_group **)calloc(queue_length, sizeof(struct rex_dfa_trans_group *));
  if (!heap) {
    return _REX_NO_MEMORY;
  }

  /* Copy all trans_group pointers into the array and prepare to make a heap */
  struct rex_dfa_trans_group *dtg = dfa->trans_groups_;
  if (dtg) {
    do {
      dtg = dtg->sibling_;

      heap[dtg->ordinal_] = dtg;
      dtg->heap_is_at_backside_ = 0;

    } while (dtg != dfa->trans_groups_);
  }

  size_t n = queue_length / 2;
  do {
    rex_dfa_trans_group_heapify(heap, queue_length, n);
  } while (n--);

  *plength = queue_length;
  *pheap = heap;

  return 0;
}

int rex_dfa_make_symbol_groups(struct rex_dfa *dfa) {
  int r;
  size_t n;
  struct rex_dfa_trans_group **heap = NULL;
  uint64_t *dfa_trans_group_members = NULL;
  struct rex_symbol_group **symbol_group_hashtable = NULL;
  struct rex_dfa_node *dn;
  
  /* Group all DFA transitions from and to the identical DFA nodes into groups,
   * these groups can then be classified for input ranges such that we may produce
   * a new set of symbols "categories" that do not overlap, but strongly compress the
   * input ranges while at the same time allowing very large codepoint numbers on the
   * input.. */
  dn = dfa->nodes_;
  if (dn) {
    do {
      dn = dn->chain_;

      struct rex_dfa_trans *dt = dn->outbound_;
      if (dt) {
        do {
          dt = dt->from_peer_;

          dt->to_->trans_group_ = NULL;
          
        } while (dt != dn->outbound_);

        do {
          dt = dt->from_peer_;
                    
          if (!dt->is_anchor_) {
            if (!dt->to_->trans_group_) {
              dt->to_->trans_group_ = rex_alloc_trans_group(dfa);
              if (!dt->to_->trans_group_) {
                r = _REX_NO_MEMORY;
                goto cleanup;
              }
            }
            struct rex_dfa_trans_group *tg = dt->to_->trans_group_;

            if (tg->transitions_) {
              dt->trans_group_sibling_ = tg->transitions_->trans_group_sibling_;
              tg->transitions_->trans_group_sibling_ = dt;
            }
            else {
              dt->trans_group_sibling_ = dt;
            }
            tg->transitions_ = dt;
          }

        } while (dt != dn->outbound_);
      }

    } while (dn != dfa->nodes_);
  }

  /* Discover all possible dfa-trans-group intersections, each of these intersections
   * develop a bitmask (where a set bit is a dfa-trans-group's membership with the 
   * intersection). These intersections form the ranges of the symbol groups down the
   * line.. */
  size_t heap_length = 0;
  r = rex_dfa_make_trans_group_priority_queue(dfa, &heap_length, &heap);
  if (r) goto cleanup;

  size_t num_trans_groups = 1 + (size_t)dfa->trans_groups_->ordinal_;

  size_t dfa_trans_group_members_size = sizeof(uint64_t) * ((63 + num_trans_groups) / 64);
  dfa_trans_group_members = (uint64_t *)malloc(dfa_trans_group_members_size);
  if (!dfa_trans_group_members) {
    r = _REX_NO_MEMORY;
    goto cleanup;
  }
  memset(dfa_trans_group_members, 0, dfa_trans_group_members_size);

  symbol_group_hashtable = (struct rex_symbol_group **)malloc(sizeof(struct rex_symbol_group *) * REX_SYMBOL_GROUP_HASH_TABLE_SIZE);
  if (!symbol_group_hashtable) {
    r = _REX_NO_MEMORY;
    goto cleanup;
  }
  for (n = 0; n < REX_SYMBOL_GROUP_HASH_TABLE_SIZE; ++n) {
    symbol_group_hashtable[n] = NULL;
  }

  uint32_t symbol_clip = 0;

  uint32_t current_symbol_edge = 0;

  while (heap_length) {

    current_symbol_edge = rex_dfa_trans_group_get_clipped_range_value(heap[0]);
    do {
      if (!heap[0]->heap_is_at_backside_) {
        /* heap[0] joins the current set; re-insert it on its next symbol edge */
        dfa_trans_group_members[heap[0]->ordinal_ / 64] |= ((uint64_t)1) << (heap[0]->ordinal_ & 63);
        heap[0]->heap_is_at_backside_ = 1;
        rex_dfa_trans_group_heapify(heap, heap_length, 0);
      }
      else {
        /* heap[0] leaves the current set - check if this ends its entire consideration or 
         * if we need to re-insert it. */
        dfa_trans_group_members[heap[0]->ordinal_ / 64] &= ~(((uint64_t)1) << (heap[0]->ordinal_ & 63));
        
        /* Step to next DFA transition (in a round-robin fashion) and see if we arrived
         * back at the start (if so, then the start of the range is behind us (behind symbol_clip)). */
        heap[0]->transitions_ = heap[0]->transitions_->trans_group_sibling_;
        heap[0]->heap_is_at_backside_ = 0; /* start of next dfa transition range */
        if (rex_dfa_trans_group_get_clipped_range_value(heap[0]) < current_symbol_edge) {
          /* We've already passed the next range. */
          /* All of the DFA transitions in this DFA transition group are behind us, delete
           * from the heap */
          heap[0] = heap[--heap_length];
          rex_dfa_trans_group_heapify(heap, heap_length, 0);
        }
        else {
          /* We did not arrive back at the start, re-insert */
          rex_dfa_trans_group_heapify(heap, heap_length, 0);
        }
      }
    } while (heap_length && (current_symbol_edge == rex_dfa_trans_group_get_clipped_range_value(heap[0])));

    if (heap_length) {
      symbol_clip = rex_dfa_trans_group_get_clipped_range_value(heap[0]);

      /* Current range is from current_symbol_edge to symbol_clip; the current membership of DFA trans groups for
       * this range is dfa_trans_group_members. */
      /* Look for the symbol group with these DFA transition groups as members.. */
      uint64_t hash = 0;
      size_t n;
      for (n = 0; n < dfa_trans_group_members_size / sizeof(uint64_t); ++n) {
        /* Rotate Left by 13 bits (13 is a prime, this is intentional) */
        hash = (hash << 13) | (hash >> ((-13) & 63));
        hash += dfa_trans_group_members[n];
      }
      int hash_index = (int)(hash % REX_SYMBOL_GROUP_HASH_TABLE_SIZE);
      struct rex_symbol_group *tail = symbol_group_hashtable[hash_index];
      struct rex_symbol_group *sg = tail;
      int match_found = 0;
      if (tail) {
        do {
          sg = sg->hash_chain_;

          if (!memcmp(sg->dfa_trans_group_membership_, dfa_trans_group_members, dfa_trans_group_members_size)) {
            match_found = 1;
            break;
          }
        } while (sg != tail);
      }
      if (!match_found) {
        sg = (struct rex_symbol_group *)malloc(sizeof(struct rex_symbol_group) + dfa_trans_group_members_size - sizeof(uint64_t));
        if (!sg) {
          r = _REX_NO_MEMORY;
          goto cleanup;
        }
        memcpy(sg->dfa_trans_group_membership_, dfa_trans_group_members, dfa_trans_group_members_size);
        if (tail) {
          sg->hash_chain_ = tail->hash_chain_;
          tail->hash_chain_ = sg;
        }
        else {
          sg->hash_chain_ = sg;
        }
        symbol_group_hashtable[hash_index] = sg;
        sg->ranges_ = NULL;
        sg->selectors_ = NULL;

        if (dfa->symbol_groups_) {
          sg->chain_ = dfa->symbol_groups_->chain_;
          dfa->symbol_groups_->chain_ = sg;
          sg->ordinal_ = dfa->symbol_groups_->ordinal_ + 1;
        }
        else {
          sg->chain_ = sg;
          sg->ordinal_ = 1;
        }
        dfa->symbol_groups_ = sg;

        /* Chain the new symbol group to all DFA transition groups that it selects for (which transition on any
         * member range of the symbol group.) These are the active DFA trans groups on the heap. */
        for (n = 0; n < heap_length; ++n) {
          struct rex_dfa_trans_group *tg = heap[n];
          if (tg->heap_is_at_backside_) {
            /* If range is active (heap is at backside of range) then it must be an active member */
            assert(dfa_trans_group_members[tg->ordinal_ / 64] & (((uint64_t)1) << (tg->ordinal_ & 63)));
            struct rex_selector *selector = (struct rex_selector *)malloc(sizeof(struct rex_selector));
            if (!selector) {
              r = _REX_NO_MEMORY;
              goto cleanup;
            }
            selector->dfa_transition_group_ = tg;
            if (tg->selectors_) {
              selector->next_in_dfa_transition_group_ = tg->selectors_;
              selector->prev_in_dfa_transition_group_ = tg->selectors_->prev_in_dfa_transition_group_;
              selector->next_in_dfa_transition_group_->prev_in_dfa_transition_group_ = selector->prev_in_dfa_transition_group_->next_in_dfa_transition_group_ = selector;
            }
            else {
              selector->next_in_dfa_transition_group_ = selector->prev_in_dfa_transition_group_ = selector;
              tg->selectors_ = selector;
            }

            selector->symbol_group_ = sg;
            if (sg->selectors_) {
              selector->next_in_symbol_group_ = sg->selectors_;
              selector->prev_in_symbol_group_ = sg->selectors_->prev_in_symbol_group_;
              selector->prev_in_symbol_group_->next_in_symbol_group_ = selector->next_in_symbol_group_->prev_in_symbol_group_ = selector;
            }
            else {
              selector->next_in_symbol_group_ = selector->prev_in_symbol_group_ = selector;
              sg->selectors_ = selector;
            }
          }
          else {
            /* If range is not active (heap at frontside of some range,) then it must not be an active member */
            assert(!(dfa_trans_group_members[tg->ordinal_ / 64] & (((uint64_t)1) << (tg->ordinal_ & 63))));
          }
        }
      }

      /* Append current range to ranges of match - remember from above, the current range is from current_symbol_edge to symbol_clip */
      if (!sg->ranges_ || (sg->ranges_->symbol_end_ != current_symbol_edge)) {
        /* Allocate new range and append. */
        struct rex_symbol_range *sr = (struct rex_symbol_range *)malloc(sizeof(struct rex_symbol_range));
        if (!sr) {
          r = _REX_NO_MEMORY;
          goto cleanup;
        }
        if (sg->ranges_) {
          sr->chain_ = sg->ranges_->chain_;
          sg->ranges_->chain_ = sr;
        }
        else {
          sr->chain_ = sr;
        }
        sg->ranges_ = sr;

        sr->symbol_start_ = current_symbol_edge;
        sr->symbol_end_ = symbol_clip;
      }
      else /* (sg->ranges_ && (sg->ranges_->symbol_end_ == current_symbol_edge)) */ {
        /* Extend last range to include current one */
        sg->ranges_->symbol_end_ = symbol_clip;
      }
    }
  }

cleanup:
  if (heap) free(heap);
  if (dfa_trans_group_members) free(dfa_trans_group_members);
  if (symbol_group_hashtable) free(symbol_group_hashtable);
  return r;
}

int rex_realize_modes(struct rex_scanner *rex) {
  int r;
  struct rex_mode *mode;
  mode = rex->modes_;
  if (mode) {
    do {
      mode = mode->chain_;

      struct rex_pattern_mode *pm;
      pm = mode->patterns_;
      if (pm) {
        do {
          pm = pm->next_in_mode_;

          rex_nfa_make_empty_trans(&rex->nfa_, pm->mode_->nfa_begin_state_, pm->pattern_->nfa_begin_state_);
          if (rex->nfa_.failed_) {
            return rex->nfa_.failed_;
          }

        } while (pm != mode->patterns_);
      }
    } while (mode != rex->modes_);
  }

  size_t map_size = sizeof(uint64_t) * ((rex->nfa_.num_nfa_nodes_ + 63) / 64);
  uint64_t *closure = (uint64_t *)malloc(map_size);
  
  struct rex_dfa_node *dfa_nodes_to_realize_tail_ = NULL;

  mode = rex->modes_;
  if (mode) {
    do {
      mode = mode->chain_;

      memset(closure, 0, map_size);
      closure[mode->nfa_begin_state_ >> 6] |= (((uint64_t)1) << (mode->nfa_begin_state_ & 0x3F));
      rex_nfa_compute_closure(&rex->nfa_, closure);

      /* Look for the DFA with this closure, if none exists, create it. */
      uint64_t hash = 0;
      size_t n;
      for (n = 0; n < (map_size/sizeof(uint64_t)); ++n) {
        /* Rotate Left by 13 bits (13 is a prime, this is intentional) */
        hash = (hash << 13) | (hash >> ((-13) & 63));
        hash += closure[n];
      }
      size_t hash_index = hash % REX_DFA_HASH_TABLE_SIZE;
      struct rex_dfa_node *tail = rex->dfa_.hash_table_[hash_index];
      struct rex_dfa_node *dn = tail;
      int match_found = 0;
      if (dn) {
        do {
          dn = dn->hash_chain_;

          if (!memcmp(dn->map_, closure, map_size)) {
            /* Found a match */
            match_found = 1;
            break;
          }

        } while (dn != tail);
      }

      if (!match_found) {
        /* Create DFA node, queue it for further realization */
        dn = rex_alloc_dfa_node(rex);
        if (!dn) {
          r = _REX_NO_MEMORY;
          goto cleanup;
        }
        if (tail) {
          dn->hash_chain_ = tail->hash_chain_;
          tail->hash_chain_ = dn;
        }
        else {
          dn->hash_chain_ = dn;
        }
        tail = dn;
        rex->dfa_.hash_table_[hash_index] = tail;

        if (dfa_nodes_to_realize_tail_) {
          dn->chain_ = dfa_nodes_to_realize_tail_->chain_;
          dfa_nodes_to_realize_tail_->chain_ = dn;
        }
        else {
          dn->chain_ = dn;
        }
        dfa_nodes_to_realize_tail_ = dn;
        dn->outbound_ = NULL;
        dn->inbound_ = NULL;
        memcpy(dn->map_, closure, map_size);
      }

      mode->dfa_node_ = dn;

    } while (mode != rex->modes_);
  }
  
  /* Now realize all DFA nodes in dfa_nodes_to_realize_tail_, which will lead us to discover yet more nodes to realize, until we're done. */
  while (dfa_nodes_to_realize_tail_) {
    struct rex_dfa_node *dn = dfa_nodes_to_realize_tail_->chain_;
    if (dn != dfa_nodes_to_realize_tail_) {
      dfa_nodes_to_realize_tail_->chain_ = dn->chain_;
    }
    else {
      dfa_nodes_to_realize_tail_ = NULL;
    }

    if (rex->dfa_.nodes_) {
      dn->chain_ = rex->dfa_.nodes_->chain_;
      rex->dfa_.nodes_->chain_ = dn;
    }
    else {
      dn->chain_ = dn;
    }
    rex->dfa_.nodes_ = dn;

    /* Find anchors to pursue, if any */
    int found_anchor = 0;
    uint32_t lowest_valid_anchor = 0;
    uint32_t next_anchor = 0;

    do {
      size_t n;
      size_t base_nfa_index = 0;
      found_anchor = 0;
      for (n = 0; n < map_size / sizeof(uint64_t); ++n) {
        uint64_t m = dn->map_[n];
        while (m) {
          size_t bit_position = 0;
#ifdef __linux__
          bit_position = (size_t)ffsll(m);
          bit_position--; /* bit position value 0 is "no bit set" */
#elif defined(_MSC_VER) && defined(_WIN64)
          unsigned long index = 0;
          /* Cannot fail as m is non-zero */
          _BitScanForward64(&index, m);
          bit_position = (size_t)index;
#elif defined(_MSC_VER)
          unsigned long index = 0;
          if (_BitScanForward(&index, (uint32_t)m)) {
            bit_position = (size_t)index;
          }
          else {
            _BitScanForward(&index, (uint32_t)(m >> 32));
            bit_position = (size_t)index + 32;
          }
#endif
          /* clear lowest bit */
          m = m & (m - 1);

          size_t nfa_index = base_nfa_index + bit_position;

          struct rex_nfa_node *nn = rex->nfa_.nfa_nodes_ + nfa_index;
          struct rex_nfa_trans *nt = nn->outbound_;
          if (nt) {
            do {
              nt = nt->from_peer_;

              if (nt->is_anchor_) {
                if (lowest_valid_anchor <= nt->symbol_start_) {
                  if (!found_anchor) {
                    found_anchor = 1;
                    next_anchor = nt->symbol_start_;
                  }
                  else if (next_anchor > nt->symbol_start_) {
                    next_anchor = nt->symbol_start_;
                  }
                }
              }

            } while (nt != nn->outbound_);
          }

        }

        base_nfa_index += 64;
      }

      if (found_anchor) {
        /* Look for DFA state consisting of all NFAs where the anchor is valid/taken ; this is an
         * extension of the current DFA's closure with the additional states of the anchor transitions.
         * There may be many transitions for any given anchor, so no choice but to re-iterate all
         * transitions again. */
        memcpy(closure, dn->map_, map_size);

        int added_nfas;
        
        do {
          /* Keep looping while we're adding NFAs, this is necessary because multiple consecutive
           * anchors are still a valid match, for instance ("^^a" should match if an "a" appears
           * at the start of the line, that is, "^^a" is identical to "^a".) */
          added_nfas = 0;

          base_nfa_index = 0;
          for (n = 0; n < (map_size/sizeof(uint64_t)); ++n) {
            uint64_t m = closure[n];
            while (m) {
              size_t bit_position = 0;
#ifdef __linux__
              bit_position = (size_t)ffsll(m);
              bit_position--; /* bit position value 0 is "no bit set" */
#elif defined(_MSC_VER) && defined(_WIN64)
              unsigned long index = 0;
              /* Cannot fail as m is non-zero */
              _BitScanForward64(&index, m);
              bit_position = (size_t)index;
#elif defined(_MSC_VER)
              unsigned long index = 0;
              if (_BitScanForward(&index, (uint32_t)m)) {
                bit_position = (size_t)index;
              }
              else {
                _BitScanForward(&index, (uint32_t)(m >> 32));
                bit_position = (size_t)index + 32;
              }
#endif
              /* clear lowest bit */
              m = m & (m - 1);

              size_t nfa_index = base_nfa_index + bit_position;

              struct rex_nfa_node *nn = rex->nfa_.nfa_nodes_ + nfa_index;
              struct rex_nfa_trans *nt = nn->outbound_;
              if (nt) {
                do {
                  nt = nt->from_peer_;

                  if (nt->is_anchor_ && (nt->symbol_start_ == next_anchor)) {
                    /* This is a transition we're taking; include the destinatino NFA in the existing set */
                    if (!(closure[nt->to_ >> 6] & (((uint64_t)1) << (nt->to_ & 0x3F)))) {
                      /* Not NFA is not yet in the set, include it. We check for this so we know when to set added_nfas */
                      added_nfas = 1;
                      closure[nt->to_ >> 6] |= (((uint64_t)1) << (nt->to_ & 0x3F));
                    }
                  }
                  else if (nt->is_empty_) {
                    /* Implicitly follow empty transitions while looking for anchor transitions. */
                    if (!(closure[nt->to_ >> 6] & (((uint64_t)1) << (nt->to_ & 0x3F)))) {
                      /* Not NFA is not yet in the set, include it. We check for this so we know when to set added_nfas */
                      added_nfas = 1;
                      closure[nt->to_ >> 6] |= (((uint64_t)1) << (nt->to_ & 0x3F));
                    }
                  }

                } while (nt != nn->outbound_);
              }
            }
            base_nfa_index += 64;
          }
        } while (added_nfas);

        /* Closure contains the nfa map for the DFA that results from taking the anchor condition "next_anchor"; link it
         * to a DFA or create a new one if no such DFA exists... */
        uint64_t hash = 0;
        size_t n;
        for (n = 0; n < (map_size/sizeof(uint64_t)); ++n) {
          /* Rotate Left by 13 bits (13 is a prime, this is intentional) */
          hash = (hash << 13) | (hash >> ((-13) & 63));
          hash += closure[n];
        }
        size_t hash_index = hash % REX_DFA_HASH_TABLE_SIZE;
        struct rex_dfa_node *tail = rex->dfa_.hash_table_[hash_index];
        struct rex_dfa_node *ddn = tail;
        int match_found = 0;
        if (ddn) {
          do {
            ddn = ddn->hash_chain_;

            if (!memcmp(ddn->map_, closure, map_size)) {
              /* Found a match */
              match_found = 1;
              break;
            }

          } while (ddn != tail);
        }

        if (!match_found) {
          /* Create DFA node, queue it for further realization */
          ddn = rex_alloc_dfa_node(rex);
          if (!ddn) {
            r = _REX_NO_MEMORY;
            goto cleanup;
          }
          if (tail) {
            ddn->hash_chain_ = tail->hash_chain_;
            tail->hash_chain_ = ddn;
          }
          else {
            ddn->hash_chain_ = ddn;
          }
          tail = ddn;
          rex->dfa_.hash_table_[hash_index] = tail;

          if (dfa_nodes_to_realize_tail_) {
            ddn->chain_ = dfa_nodes_to_realize_tail_->chain_;
            dfa_nodes_to_realize_tail_->chain_ = ddn;
          }
          else {
            ddn->chain_ = ddn;
          }
          dfa_nodes_to_realize_tail_ = ddn;
          ddn->outbound_ = NULL;
          ddn->inbound_ = NULL;
          memcpy(ddn->map_, closure, map_size);
        }

        /* Create DFA transition for next_anchor from dn to ddn */
        struct rex_dfa_trans *dt;
        dt = rex_alloc_dfa_trans(rex, dn, ddn);
        if (!dt) {
          r = _REX_NO_MEMORY;
          goto cleanup;
        }
        dt->is_anchor_ = 1;
        dt->symbol_start_ = next_anchor;
      }

      lowest_valid_anchor = next_anchor + 1;
    } while (found_anchor);
  
    /* Find symbol transitions to pursue, if any */
    uint32_t start_at = 0;

    for (;;) {
      uint32_t minimum_clipped_start = UINT32_MAX;
      size_t base_nfa_index = 0;
      size_t n;
      for (n = 0; n < (map_size/sizeof(uint64_t)); ++n) {
        uint64_t m = dn->map_[n];
        while (m) {
          size_t bit_position = 0;
#ifdef __linux__
          bit_position = (size_t)ffsll(m);
          bit_position--; /* bit position value 0 is "no bit set" */
#elif defined(_MSC_VER) && defined(_WIN64)
          unsigned long index = 0;
          /* Cannot fail as m is non-zero */
          _BitScanForward64(&index, m);
          bit_position = (size_t)index;
#elif defined(_MSC_VER)
          unsigned long index = 0;
          if (_BitScanForward(&index, (uint32_t)m)) {
            bit_position = (size_t)index;
          }
          else {
            _BitScanForward(&index, (uint32_t)(m >> 32));
            bit_position = (size_t)index + 32;
          }
#endif
          /* clear lowest bit */
          m = m & (m - 1);

          size_t nfa_index = base_nfa_index + bit_position;

          struct rex_nfa_node *nn = rex->nfa_.nfa_nodes_ + nfa_index;
          struct rex_nfa_trans *nt = nn->outbound_;
          if (nt) {
            do {
              nt = nt->from_peer_;

              if (!nt->is_empty_ && !nt->is_anchor_) {
                if (nt->symbol_end_ > start_at) {
                  if (nt->symbol_start_ < minimum_clipped_start) {
                    minimum_clipped_start = (nt->symbol_start_ < start_at) ? start_at : nt->symbol_start_;
                  }
                }
              }
            } while (nt != nn->outbound_);
          }
        }

        base_nfa_index += 64;
      }

      start_at = minimum_clipped_start;
      if (start_at == UINT32_MAX) {
        /* Done finding symbol ranges.. */
        break;
      }

      uint32_t end_at = UINT32_MAX;
      base_nfa_index = 0;
      for (n = 0; n < (map_size/sizeof(uint64_t)); ++n) {
        uint64_t m = dn->map_[n];
        while (m) {
          size_t bit_position = 0;
#ifdef __linux__
          bit_position = (size_t)ffsll(m);
          bit_position--; /* bit position value 0 is "no bit set" */
#elif defined(_MSC_VER) && defined(_WIN64)
          unsigned long index = 0;
          /* Cannot fail as m is non-zero */
          _BitScanForward64(&index, m);
          bit_position = (size_t)index;
#elif defined(_MSC_VER)
          unsigned long index = 0;
          if (_BitScanForward(&index, (uint32_t)m)) {
            bit_position = (size_t)index;
          }
          else {
            _BitScanForward(&index, (uint32_t)(m >> 32));
            bit_position = (size_t)index + 32;
          }
#endif
          /* clear lowest bit */
          m = m & (m - 1);

          size_t nfa_index = base_nfa_index + bit_position;

          struct rex_nfa_node *nn = rex->nfa_.nfa_nodes_ + nfa_index;
          struct rex_nfa_trans *nt = nn->outbound_;
          if (nt) {
            do {
              nt = nt->from_peer_;

              if (!nt->is_empty_ && !nt->is_anchor_) {
                if ((nt->symbol_start_ <= start_at) && (nt->symbol_end_ > start_at)) {
                  if (nt->symbol_end_ < end_at) {
                    end_at = nt->symbol_end_;
                  }
                }
                else if ((nt->symbol_start_ > start_at) && (nt->symbol_start_ < end_at)) {
                  /* Each region must have a consistent set of transitions that overlap it (the symbol
                   * range must apply uniform for all transitions that intersect it.) */
                  end_at = nt->symbol_start_;
                }
              }
            } while (nt != nn->outbound_);
          }
        }

        base_nfa_index += 64;
      }

      /* From range start_at to range end_at (exclusive) is the next range of symbols whose
       * transitions we're considering.. */
      memset(closure, 0, map_size);

      base_nfa_index = 0;
      for (n = 0; n < (map_size/sizeof(uint64_t)); ++n) {
        uint64_t m = dn->map_[n];
        while (m) {
          size_t bit_position = 0;
#ifdef __linux__
          bit_position = (size_t)ffsll(m);
          bit_position--; /* bit position value 0 is "no bit set" */
#elif defined(_MSC_VER) && defined(_WIN64)
          unsigned long index = 0;
          /* Cannot fail as m is non-zero */
          _BitScanForward64(&index, m);
          bit_position = (size_t)index;
#elif defined(_MSC_VER)
          unsigned long index = 0;
          if (_BitScanForward(&index, (uint32_t)m)) {
            bit_position = (size_t)index;
          }
          else {
            _BitScanForward(&index, (uint32_t)(m >> 32));
            bit_position = (size_t)index + 32;
          }
#endif
          /* clear lowest bit */
          m = m & (m - 1);

          size_t nfa_index = base_nfa_index + bit_position;

          struct rex_nfa_node *nn = rex->nfa_.nfa_nodes_ + nfa_index;
          struct rex_nfa_trans *nt = nn->outbound_;
          if (nt) {
            do {
              nt = nt->from_peer_;

              if (!nt->is_empty_ && !nt->is_anchor_) {
                if ((nt->symbol_start_ < end_at) && (nt->symbol_end_ > start_at)) {
                  closure[nt->to_ >> 6] |= ((uint64_t)1) << (nt->to_ & 0x3F);
                }
              }
            } while (nt != nn->outbound_);
          }
        }

        base_nfa_index += 64;
      }
      rex_nfa_compute_closure(&rex->nfa_, closure);

      /* Find or create the DFA corresponding to the closure NFA map; then create the DFA transition with it.. */
      uint64_t hash = 0;
      for (n = 0; n < (map_size/sizeof(uint64_t)); ++n) {
        /* Rotate Left by 13 bits (13 is a prime, this is intentional) */
        hash = (hash << 13) | (hash >> ((-13) & 63));
        hash += closure[n];
      }
      size_t hash_index = hash % REX_DFA_HASH_TABLE_SIZE;
      struct rex_dfa_node *tail = rex->dfa_.hash_table_[hash_index];
      struct rex_dfa_node *ddn = tail;
      int match_found = 0;
      if (ddn) {
        do {
          ddn = ddn->hash_chain_;

          if (!memcmp(ddn->map_, closure, map_size)) {
            /* Found a match */
            match_found = 1;
            break;
          }

        } while (ddn != tail);
      }

      if (!match_found) {
        /* Create DFA node, queue it for further realization */
        ddn = rex_alloc_dfa_node(rex);
        if (!ddn) {
          r = _REX_NO_MEMORY;
          goto cleanup;
        }
        if (tail) {
          ddn->hash_chain_ = tail->hash_chain_;
          tail->hash_chain_ = ddn;
        }
        else {
          ddn->hash_chain_ = ddn;
        }
        tail = ddn;
        rex->dfa_.hash_table_[hash_index] = tail;

        if (dfa_nodes_to_realize_tail_) {
          ddn->chain_ = dfa_nodes_to_realize_tail_->chain_;
          dfa_nodes_to_realize_tail_->chain_ = ddn;
        }
        else {
          ddn->chain_ = ddn;
        }
        dfa_nodes_to_realize_tail_ = ddn;
        ddn->outbound_ = NULL;
        ddn->inbound_ = NULL;
        memcpy(ddn->map_, closure, map_size);
      }

      /* Create the DFA transition for the symbol range [start_at .. end_at> */
      struct rex_dfa_trans *dt;
      dt = rex_alloc_dfa_trans(rex, dn, ddn);
      if (!dt) {
        r = _REX_NO_MEMORY;
        goto cleanup;
      }
      dt->symbol_start_ = start_at;
      dt->symbol_end_ = end_at;

      start_at = end_at;

    } /* for (;;) { .. iterate all regular symbol transitions .. } */
  }

  /* All DFA nodes have been realized, now locate the dominant pattern matched for each
   * DFA node if any. */
  struct rex_dfa_node *dn;
  dn = rex->dfa_.nodes_;
  if (dn) {
    do {
      dn = dn->chain_;

      struct rex_pattern *pat = NULL;

      size_t base_nfa_index = 0;
      size_t n;
      for (n = 0; n < (map_size/sizeof(uint64_t)); ++n) {
        uint64_t m = dn->map_[n];
        while (m) {
          size_t bit_position = 0;
#ifdef __linux__
          bit_position = (size_t)ffsll(m);
          bit_position--; /* bit position value 0 is "no bit set" */
#elif defined(_MSC_VER) && defined(_WIN64)
          unsigned long index = 0;
          /* Cannot fail as m is non-zero */
          _BitScanForward64(&index, m);
          bit_position = (size_t)index;
#elif defined(_MSC_VER)
          unsigned long index = 0;
          if (_BitScanForward(&index, (uint32_t)m)) {
            bit_position = (size_t)index;
          }
          else {
            _BitScanForward(&index, (uint32_t)(m >> 32));
            bit_position = (size_t)index + 32;
          }
#endif
          /* clear lowest bit */
          m = m & (m - 1);

          size_t nfa_index = base_nfa_index + bit_position;

          struct rex_nfa_node *nn = rex->nfa_.nfa_nodes_ + nfa_index;

          if (nn->pattern_matched_) {
            if (pat) {
              pat = (pat->ordinal_ < nn->pattern_matched_->ordinal_) ? pat : nn->pattern_matched_;
            }
            else {
              pat = nn->pattern_matched_;
            }
          }
        }

        base_nfa_index += 64;
      }

      dn->pattern_matched_ = pat;
    } while (dn != rex->dfa_.nodes_);
  }

  r = 0;
cleanup:
  dn = dfa_nodes_to_realize_tail_;
  if (dn) {
    do {
      struct rex_dfa_node *next = dn->chain_;

      struct rex_dfa_trans *trans = dn->outbound_;
      if (trans) {
        do {
          struct rex_dfa_trans *next = trans->from_peer_;

          free(trans);

          trans = trans->from_peer_;
        } while (trans != dn->outbound_);
      }

      free(dn);

      dn = dn->chain_;

    } while (dn != dfa_nodes_to_realize_tail_);
  }

  free(closure);
  return r;
}
