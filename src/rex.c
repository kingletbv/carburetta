
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

  struct rex_pattern *pat = (struct rex_pattern *)malloc(sizeof(struct rex_pattern));
  if (!pat) return _REX_NO_MEMORY;

  pat->nfa_begin_state_ = start;
  pat->nfa_final_state_ = end;
  pat->ordinal_ = rex->next_pattern_ordinal_++;
  pat->action_ = action;
  pat->modes_ = NULL;

  rex->nfa_.nfa_nodes_[end].pattern_matched_ = pat;

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
  size_t n;
  for (n = 0; n < REX_DFA_HASH_TABLE_SIZE; ++n) {
    dfa->hash_table_[n] = NULL;
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

  return dt;
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
