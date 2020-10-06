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

#ifndef DFA_H
#define DFA_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef STDDEF_H_INCLUDED
#define STDDEF_H_INCLUDED
#include <stddef.h> /* size_t */
#endif

#ifndef NFA_H_INCLUDED
#define NFA_H_INCLUDED
#include "nfa.h"
#endif

struct dfa_node {
  /* Unique number of the DFA state, this becomes the row index for a
   * scanning table; assigned from dfa_context_t::next_dfa_num when the
   * DFA is "recorded" (dfa_record_node() in the C file) */
  size_t num;

  /* Next in chain of all distinct DFA nodes */
  struct dfa_node *next_dfa;

  /* Array of all moves, indexed by unsigned input character, NULL when 
   * the character is invalid. */
  struct dfa_node *moves[256];

  /* Bitmap of all NFA indices, dynamically allocated to be of sufficient
   * size to index all NFA's (eg. (num_nfa_nodes + 63) / 64. */
  uint64_t nfa_map[1];
};

struct dfa {
  /* All DFA nodes */
  struct dfa_node *first_dfa;

  /* Next DFA number to assign in dfa_node::num */
  size_t next_dfa_num;

  /* Total number of NFA nodes, this determines the
   * size of the allocation for DFA nodes due to 
   * the nfa_map member in dfa_node*/
  size_t num_nfa_nodes;
};

void dfa_init(struct dfa *dfa);
void dfa_cleanup(struct dfa *dfa);

int dfa_node_has_nfa(struct dfa_node *dfa_node, size_t nfa_node_index);

/* Returns the DFA node corresponding to the NFA start node */
struct dfa_node *dfa_make(struct dfa *dfa, struct nfa *nfa, size_t nfa_start);

void dfa_dump_node(struct dfa *odfa, struct dfa_node *d);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* DFA_H */
