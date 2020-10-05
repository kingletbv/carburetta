/* Copyright (C) 2009 Martijn Boekhorst
 * All rights reserved.
 *
 * $Id$
 */

#ifndef XLALR_H
#define XLALR_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Extended-LALR(1) parser & table generator
 * The extension is to allow runtime variants in the reductions of the kind:
 * VAR | CONST | FUNC : IDENT = sym_lookup(IDENT)
 * where sym_lookup will, at reduction-time, determine whether the IDENT is a VAR, a CONST, or a FUNC.
 * Such a production differs from the LALR alternative:
 *   var  : IDENT = sym_lookup(IDENT)
 *   const: IDENT = sym_lookup(IDENT)
 *   func : IDENT = sym_lookup(IDENT)
 * The LALR alternative creates various reduce/reduce conflicts if a var, const or func are ever 
 * accepted together (they cannot make a decision.) In Extended-LALR this reduce-reduce conflict
 * does not occur because the production for all three non-terminals is the same, and the pro-
 * duction's reduce function is expected to indicate which of the three just occurred.
 */

typedef enum xlr_error_enum xlr_error_t;
typedef enum xlr_action_type_enum xlr_action_type_t;
typedef struct xlr_gen_struct xlr_gen_t;
typedef struct xlr_item_struct xlr_item_t;
typedef struct xlr_prod_struct xlr_prod_t;
typedef struct xlr_state_struct xlr_state_t;
typedef struct xlr_trans_struct xlr_trans_t;
typedef struct xlr_action_struct xlr_action_t;
typedef struct xlr_conflict_resolution_struct xlr_conflict_resolution_t;
typedef struct xlr_conflict_struct xlr_conflict_t;

enum xlr_error_enum {
  /* Everything went well, parser is available. */
  XLR_OK,
  /* The Grammar passed in is not LR(n) for any N */
  XLR_NOT_LR_GRAMMAR,
  /* The Grammar passed in is ambiguous */
  XLR_AMBIGUOUS_GRAMMAR,
  /* Shift-reduce or reduce-reduce conflicts were detected. */
  XLR_CONFLICTS,
  /* Terminals and non-terminals are in overlapping ranges */
  XLR_SYM_OVERLAP,
  /* Internal failure trying to backtrack items */
  XLR_ITEM_BACKTRACK_FAILED
};

enum xlr_action_type_enum {
  /* Action is a shift, xlr_action_t::state points to the
   * next state. */
  XLR_SHIFT,

  /* Action is a reduce, xlr_action_t::production is an index
   * for the xlr_gen_t::productions table, pointing to the
   * production being reduced. */
  XLR_REDUCE
};

struct xlr_gen_struct {
  /* Special symbol constants.. */
  int synths_sym; /* S' synthetic root production non-terminal symbol */
  int eop_sym;    /* marks the end of a production */
  int eog_sym;    /* marks the end of a grammar */
  int eof_sym;    /* end-of-file synthetic terminal symbol */
  int eopn_sym;   /* end-of-production-nonterminals sym */

  /* All productions, each production points to the first symbol
   * in its production string. */
  size_t nr_productions;
  xlr_prod_t *productions;

  /* Min & max ordinals for non-terminals (including synth-s) and terminals (including eof) */
  int min_nt, max_nt;
  int min_t, max_t;
  /* Min & max for all syms (terminals and non-terminals) */
  int min_s, max_s;

  /* Buffer for the S' : S synthetic S production */
  int synth_s_prod[4]; /* = {synths_sym, eopn_sym, first-real-production-nonterminal, eop_sym} */

  /* Chain of all states that have not had their outbound transitions determined during LR0 formation 
   * chained through xlr_state_t::workchain */
  xlr_state_t *worklist;

  /* Chain of all fully formed LR0 states; chained through xlr_state_t::chain. */
  xlr_state_t *states;

  /* Array of all fully formed LR0 states; for random access. */
  xlr_state_t **state_array;

  /* Total number of distinct states */
  size_t nr_states;

  /* Array with a boolean for each non-terminal, if non-zero, the non-terminal is nullable (can
   * be reduced without first shifting non-terminals.) 
   * Size of the array is (1 + max_nt - min_nt). */
  int *is_nt_nullable;

  /* Array with a boolean for each non-terminal; used by reduction population algorithm to determine
   * if we already considered the non-kernel items for any particular non-terminal. */
  int *non_kernel_considered;

  /* Next index for assigning to xlr_trans_t::index */
  int next_index;

  /* Stack for Tarjan SCC; as a tailchain. */
  xlr_trans_t *tarjan_stack;

  /* Set to non-zero after calling xlr_tarjan_rel if SCCs of more than
   * one trans were encountered. This is valid for the includes relation,
   * but invalid for the reads relation. */
  int had_nontrivial_sccs;

  /* Action table, this is the parse table but with all conflicts still
   * retained in it. A single state-symbol combination may therefore have
   * multiple actions associated with it. A single state-symbol combination
   * is however guaranteed to have only one shift action associated with it.
   * The action table's width is (1 + xlr_gen_t::max_s - xlr_gen_t::min_s),
   * its height equals the number of states in xlr_gen_t::states, the
   * first row corresponds to state 0 which is also the initial state.
   * Each cell in the action table is a pointer to an xlr_action_t chain;
   * if the pointer is NULL, then any attempt to parse a symbol corresponding
   * to the cell while in the corresponding row (=state) of the cell is an
   * error. The action table is therefore laid out as a single-dimensional
   * array of pointers, thus an action_table[row][column] indexing is wrong.
   * If the action(s) in a cell include a shift, then it is guaranteed to
   * be the first action in the cell. */
  xlr_action_t **action_table;

  /* All conflict resolutions specified by caller */
  size_t nr_conflict_resolutions;
  size_t nr_conflict_resolutions_allocated;
  xlr_conflict_resolution_t *conflict_resolutions;

  /* All temporary conflicts */
  size_t nr_temp_conflicts;
  size_t nr_temp_conflicts_allocated;
  xlr_conflict_t *temp_conflicts;
};

struct xlr_prod_struct {
  /* Length of production, in number of symbols */
  size_t production_length;

  /* Points to start of production grammar */
  int *production;

  /* Number of non-terminal symbols the production can reduce to,
   * for a conventional context-free grammar this is 1, however,
   * xlalr allows non-context-free grammars, therefore reductions
   * can resolve to one of multiple symbols at runtime. */
  size_t nr_reduction_syms;

  /* Points to first non-terminal the production reduces to */
  int *reduction_syms;
};

struct xlr_item_struct {
  /* 0-based production number, note that production 0 is the synthetic
   * production "S' : S" which is added by xlr_generate. */
  int production;

  /* 0-based position of the item within the production */
  int position;
};

struct xlr_trans_struct {
  /* States between which the transition exists, this xlr_trans_t appears
   * in from->outbound of the from state, and in to->inbound on the to
   * state. The symbol of the transition is most easily accessed in 
   * to->sym */
  xlr_state_t *from, *to;

  /* Chain of transitions on to->inbound. */
  xlr_trans_t *chain_inbound;

  /* Set of items in the destination state while we're forming the 
   * outbound transitions and to is still NULL. Once the set of items
   * is formed, it'll be used to find an existing matching state with
   * the same set of items, or create a new state if such a matching
   * state does not yet exist. */
  size_t nr_items_allocated;
  size_t nr_items;
  xlr_item_t *items;

  /* Read set for this transition, one bit for each terminal. Note that
   * only non-terminal transitions have read-sets allocated; terminals
   * have a NULL here.
   * To select bit:
   * ti = t - xlr_gen_t::min_t
   * x = 1 << (ti & 31)
   * bit = read_set[ti >> 5] & x
   * Size of array = (32 + xlr_gen_t::max_t - xlr_gen_t::min_t) / 32.
   * (Note that the "32 +" is not a typo, but could be read as
   *  "31 + 1 +" where the 1 incidates that min and max form an
   *  inclusive range (max is an actual symbol), and 31 is used
   *  to round up the division by 32 that follows.)
   */
  uint32_t *read_set;

  /* Related transitions; used to accumulate read sets for reads-
   * and includes- relations. */
  size_t nr_rels;
  size_t nr_rels_allocated;
  xlr_trans_t **rels;

  /* Index values for Tarjan's Strongly Connected Components algorithm,
   * allows us to do depth-first graph traversal for accumulating 
   * read_set's even in the face of cycles.
   * Note that a zero-index is a magic value of "no index assigned"
   * needed for the algorithm. Assigned indices start at 1. */
  int index, low_index;

  /* Stack chain for Tarjan's algorithm; will be non-NULL if the trans
   * is on the stack, and NULL if it is not on the stack. Note that
   * CHAIN_INIT will make it non-NULL and should therefore not be used
   * as the default initialization for this field. */
  xlr_trans_t *stack_chain;
};

struct xlr_state_struct {
  /* Chain of states on xlr_gen_t::worklist */
  xlr_state_t *workchain;

  /* Chain of all known, fully formed, states */
  xlr_state_t *chain;

  /* Items that together make up the state. */
  size_t nr_items;
  xlr_item_t *items;

  /* Outbound transitions; array entry is NULL if no outbound transition
   * exists on the indexed symbol. The size of this array is defined 
   * as (1 + xlr_gen_t::max_s - xlr_gen_t::min_s) -- i.e. it can
   * hold all syms; both goto table and terminals. */
  xlr_trans_t **outbound;

  /* Symbol on entry of the state; all states have this except for
   * the initial state. */
  int sym;

  /* Ordinal of the state, corresponding to its row in the parse table */
  int ordinal;

  /* Inbound transitions, cyclic link through xlr_trans_t::chain_inbound */
  xlr_trans_t *inbound;
};

struct xlr_action_struct {
  /* Chain of other actions occupying the same cell; when found, this implies
   * a conflict for parsing that must be resolved. */
  xlr_action_t *chain;

  /* Type of action for this xlr_action_t. */
  xlr_action_type_t action;

  /* Successor state, if action type is XLR_SHIFT */
  xlr_state_t *state;

  /* Production being reduced, if action type is XLR_REDUCE */
  int production;
};

struct xlr_conflict_resolution_struct {
  /* If non-zero, the resolution suggested has been found to conflict
   * with one or more other resolutions; thus still not resolving the
   * original conflict. */
  int conflicting;

  /* Item to be preferred .. */
  xlr_item_t prefer;

  /* Item over which prefer is to be preferred */
  xlr_item_t over;
};

struct xlr_conflict_struct {
  /* First and second conflict items */
  xlr_item_t item_a, item_b;
};

void xlr_init(xlr_gen_t *gen);
void xlr_cleanup(xlr_gen_t *gen);

void xlr_add_conflict_resolution(xlr_gen_t *gen,
                                 int dominant_production, int dominant_position,
                                 int yielding_production, int yielding_position);

xlr_error_t xlr_generate(xlr_gen_t *gen, int *grammar,
                         int eop_sym, int eog_sym, int eof_sym, int eopn_sym, int synths_sym);

xlr_action_t *xlr_resolve_conflicts(xlr_gen_t *gen, xlr_action_t *action_list);

/* helper for tracing items to their production's goto transition */
void xlr_find_production_reduced_states(xlr_gen_t *gen, size_t *nr_states, size_t *nr_states_allocated, xlr_state_t ***states, xlr_state_t *current_state, xlr_item_t tracer);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* XLALR_H */
