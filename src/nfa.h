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

#ifndef NFA_H
#define NFA_H

#ifndef STDDEF_H_INCLUDED
#define STDDEF_H_INCLUDED
#include <stddef.h> /* size_t */
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct nfa_trans {
  size_t node;
  struct nfa_trans *next_trans;
};

struct nfa_node {
  struct nfa_trans *empty_move;
  struct nfa_trans *moves[256];
};

struct nfa {
  /* All NFA nodes */
  size_t num_nfa_nodes, num_nfa_nodes_allocated;
  struct nfa_node *nfa_nodes;

  /* If free_nfa is not ~0, it points to an NFA node
   * that is free and will be returned first in
   * allocation prior to expanding the array. This helps
   * reduce the size of the array as we frequently need
   * to temporarily let go of an NFA node when stitching
   * NFA strings together. */
  size_t free_nfa;

  /* Indices of the NFA nodes where the NFA should start and stop */
  size_t start_nfa, stop_nfa;
};

void nfa_init(struct nfa *nfa);
void nfa_cleanup(struct nfa *nfa);
int nfa_parse_regexp(struct nfa *nfa, const char *regexp);

/* Copies the NFA in src in dst and merges the two NFAs such that
 * the start of dst also includes the start of src. The end of dst
 * is retained, the end of the src NFA as a clone inside dst is
 * stored in new_src_end_node.
 * If dst is an empty NFA, this operation duplicates src into dst
 * and instead overwrites dst->start_nfa and dst->stop_nfa with the
 * now only NFA in dst. */
int nfa_merge_nfas(struct nfa *dst, struct nfa *src, size_t *new_src_end_node);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NFA_H */
