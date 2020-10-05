#ifndef DFA_H
#define DFA_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "nfa.h"

typedef struct dfa dfa_t;
typedef struct dfa_node dfa_node_t;

struct dfa_node {
  /* Unique number of the DFA state, this becomes the row index for a
   * scanning table; assigned from dfa_context_t::next_dfa_num when the
   * DFA is "recorded" (dfa_record_node() in the C file) */
  size_t num;

  /* Next in chain of all distinct DFA nodes */
  dfa_node_t *next_dfa;

  /* Array of all moves, indexed by unsigned input character, NULL when 
   * the character is invalid. */
  dfa_node_t *moves[256];

  /* Bitmap of all NFA indices, dynamically allocated to be of sufficient
   * size to index all NFA's (eg. (num_nfa_nodes + 63) / 64. */
  uint64_t nfa_map[1];
};

struct dfa {
  /* All DFA nodes */
  dfa_node_t *first_dfa;

  /* Next DFA number to assign in dfa_node_t::num */
  size_t next_dfa_num;

  /* Total number of NFA nodes, this determines the
   * size of the allocation for DFA nodes due to 
   * the nfa_map member in dfa_node_t*/
  size_t num_nfa_nodes;
};

void dfa_init(dfa_t *dfa);
void dfa_cleanup(dfa_t *dfa);

int dfa_node_has_nfa(dfa_node_t *dfa_node, size_t nfa_node_index);

/* Returns the DFA node corresponding to the NFA start node */
dfa_node_t *dfa_make(dfa_t *dfa, nfa_t *nfa, size_t nfa_start);

void dfa_dump_node(dfa_t *odfa, dfa_node_t *d);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* DFA_H */
