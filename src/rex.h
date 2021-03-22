/* Copyright 2020-2021 Kinglet B.V.
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

#ifndef REX_H
#define REX_H

#ifndef STDINT_H_INCLUDED
#define STDINT_H_INCLUDED
#include <stdint.h>
#endif

struct rex_nfa;

#include "rex_parse.h"

#define REX_ANCHOR_START_OF_INPUT 0
#define REX_ANCHOR_START_OF_LINE 1
#define REX_ANCHOR_END_OF_LINE 2
#define REX_ANCHOR_END_OF_INPUT 3

#define REX_DFA_HASH_TABLE_SIZE 127

struct rex_nfa_trans {
  /* Valid if !is_empty_, contains symbol for this transition */
  uint32_t symbol_start_;
  uint32_t symbol_end_;

  /* If non-zero, this is an empty transition (eg. implicit transition) */
  int is_empty_:1;

  /* If non-zero, this is an anchor transition (eg. empty transition if runtime condition "symbol_" is met, absent otherwise.) */
  int is_anchor_:1;

  size_t from_;
  size_t to_;

  struct rex_nfa_trans *from_peer_;
  struct rex_nfa_trans *to_peer_;
};

struct rex_nfa_node {
  /* Tail pointer to ordered set of outbound transitions */
  struct rex_nfa_trans *outbound_;

  /* Tail pointer to set of inbound transitions (in no particular order) */
  struct rex_nfa_trans *inbound_;

  /* Pointer to the pattern matched when we enter this state, or NULL if no
   * pattern is matched */
  struct rex_pattern *pattern_matched_;
};

struct rex_nfa {
  int failed_;

  size_t num_nfa_nodes_;
  size_t num_nfa_nodes_allocated_;
  struct rex_nfa_node *nfa_nodes_;

  size_t start_, end_;
};


struct rex_dfa {
  int failed_;

  struct rex_dfa_node *nodes_;

  int next_dfa_node_ordinal_;

  struct rex_dfa_node *hash_table_[REX_DFA_HASH_TABLE_SIZE];
};

struct rex_dfa_node {
  struct rex_dfa_node *chain_, *hash_chain_;

  /* Tail pointer to ordered set of outbound transitions */
  struct rex_dfa_trans *outbound_;

  /* Tail pointer to set of inbound transitions (in no particular order) */
  struct rex_dfa_trans *inbound_;

  /* Pointer to the pattern matched when we enter this state, or NULL if no
   * pattern is matched - this pattern is the pattern from any of the NFAs
   * in the map_[] that has the higher priority (== lowest ordinal_). */
  struct rex_pattern *pattern_matched_;

  /* 1-based ordinal of the DFA node (value 0 is reserved for lexical errors) */
  int ordinal_;

  /* Map of all NFA nodes in this DFA state - note this is a variable length map (the '1' should be ignored.) */
  uint64_t map_[1];
};

struct rex_dfa_trans {
  /* Valid if !is_empty_, contains symbol for this transition */
  uint32_t symbol_start_;
  uint32_t symbol_end_;

  /* If non-zero, this is an anchor transition (eg. empty transition if runtime condition "symbol_" is met, absent otherwise.) */
  int is_anchor_:1;

  struct rex_dfa_node *from_;
  struct rex_dfa_node *to_;

  struct rex_dfa_trans *from_peer_;
  struct rex_dfa_trans *to_peer_;
};

struct rex_pattern {
  struct rex_pattern *chain_;
  size_t nfa_begin_state_, nfa_final_state_;
  int ordinal_;
  uintptr_t action_;

  struct rex_pattern_mode *modes_;
};

struct rex_pattern_mode;

struct rex_mode {
  struct rex_mode *chain_;
  struct rex_pattern_mode *patterns_;
  size_t nfa_begin_state_;
  struct rex_dfa_node *dfa_node_;
};

struct rex_pattern_mode {
  struct rex_pattern *pattern_;
  struct rex_mode *mode_;

  struct rex_pattern_mode *next_in_pattern_;
  struct rex_pattern_mode *next_in_mode_;
};

struct rex_scanner {
  struct rex_stack stack_;
  struct rex_nfa nfa_;
  struct rex_dfa dfa_;

  struct rex_pattern *patterns_;
  struct rex_mode *modes_;

  /* ordinal numbers assigned to keep a priority order on the pattern; patterns from earlier
   * rex_add_pattern() invocations win over patterns from later invocations. */
  int next_pattern_ordinal_;
};

void rex_nfa_init(struct rex_nfa *nfa);
void rex_nfa_cleanup(struct rex_nfa *nfa);
void rex_nfa_make_empty_trans(struct rex_nfa *nfa, size_t from, size_t to);
void rex_nfa_make_trans(struct rex_nfa *nfa, size_t from, size_t to, uint32_t sym);
void rex_nfa_make_ranged_trans(struct rex_nfa *nfa, size_t from, size_t to, uint32_t first_sym, uint32_t last_sym_inclusive);
void rex_nfa_make_anchor_trans(struct rex_nfa *nfa, size_t from, size_t to, uint32_t anchor_sym);
size_t rex_nfa_make_node(struct rex_nfa *nfa);

void rex_dfa_init(struct rex_dfa *dfa);
void rex_dfa_cleanup(struct rex_dfa *dfa);

void rex_init(struct rex_scanner *rex);
void rex_cleanup(struct rex_scanner *rex);
int rex_add_pattern(struct rex_scanner *rex, const char *regex, uintptr_t action, struct rex_pattern **ppat);
int rex_add_mode(struct rex_scanner *rex, struct rex_mode **pmode);
int rex_add_pattern_to_mode(struct rex_mode *mode, struct rex_pattern *pat);

int rex_realize_modes(struct rex_scanner *rex);

#endif /* REX_H */
