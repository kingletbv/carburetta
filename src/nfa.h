#ifndef NFA_H
#define NFA_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct nfa nfa_t;
typedef struct nfa_node nfa_node_t;
typedef struct nfa_trans nfa_trans_t;

struct nfa_trans {
  size_t node;
  nfa_trans_t *next_trans;
};

struct nfa_node {
  nfa_trans_t *empty_move;
  nfa_trans_t *moves[256];
};

struct nfa {
  /* All NFA nodes */
  size_t num_nfa_nodes, num_nfa_nodes_allocated;
  nfa_node_t *nfa_nodes;

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

void nfa_init(nfa_t *nfa);
void nfa_cleanup(nfa_t *nfa);
int nfa_parse_regexp(nfa_t *nfa, const char *regexp);

/* Copies the NFA in src in dst and merges the two NFAs such that
 * the start of dst also includes the start of src. The end of dst
 * is retained, the end of the src NFA as a clone inside dst is
 * stored in new_src_end_node.
 * If dst is an empty NFA, this operation duplicates src into dst
 * and instead overwrites dst->start_nfa and dst->stop_nfa with the
 * now only NFA in dst. */
int nfa_merge_nfas(nfa_t *dst, nfa_t *src, size_t *new_src_end_node);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NFA_H */
