/* Copyright (C) 2009 Martijn Boekhorst
 * All rights reserved.
 *
 * $Id$
 */

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>

#include "chain.h"

#include "xlalr.h"

void xlr_init(xlr_gen_t *gen) {
  memset(gen, 0, sizeof(xlr_gen_t));
}

void xlr_cleanup(xlr_gen_t *gen) {
  size_t n;
  while (gen->states) {
    xlr_state_t *popped = CHAIN_POP(&gen->states, xlr_state_t, chain);
    if (popped->items) free(popped->items);

    for (n = 0; n < (size_t)(1 + gen->max_s - gen->min_s); ++n) {
      xlr_trans_t *trans = popped->outbound[n];
      if (trans) {
        if (trans->read_set) {
          free(trans->read_set);
        }
        if (trans->items) {
          free(trans->items);
        }
        if (trans->rels) {
          free(trans->rels);
        }
        free(trans);
      }
    }
    if (popped->outbound) {
      free(popped->outbound);
    }
    free(popped);
  }
  if (gen->productions) {
    free(gen->productions);
  }
  if (gen->is_nt_nullable) {
    free(gen->is_nt_nullable);
  }
  if (gen->non_kernel_considered) {
    free(gen->non_kernel_considered);
  }
  size_t row, col;
  size_t num_columns = 1 + gen->max_s - gen->min_s;
  for (row = 0; row < gen->nr_states; ++row) {
    for (col = 0; col < num_columns; ++col) {
      xlr_action_t *action = gen->action_table[num_columns * row + col];
      if (action) {
        free(action);
      }
    }
  }
  if (gen->action_table) {
    free(gen->action_table);
  }
  if (gen->state_array) {
    free(gen->state_array);
  }
}

static int __cdecl xlr_compare_items(const void *iteml, const void *itemr) {
  const xlr_item_t *left_item = (xlr_item_t *)iteml;
  const xlr_item_t *right_item = (xlr_item_t *)itemr;
  if (left_item->production < right_item->production) {
    return -1;
  }
  else if (left_item->production > right_item->production) {
    return 1;
  }
  else /* (left_item->production == right_item->production) */ {
    if (left_item->position < right_item->position) {
      return -1;
    }
    else if (left_item->position > right_item->position) {
      return 1;
    }
    else /* left_item->position == right_item->position) */ {
      return 0;
    }
  }
}

static size_t xlr_read_set_byte_size(xlr_gen_t *gen) {
  return sizeof(uint32_t) * ((32 + gen->max_t - gen->min_t) / 32);
}

static void xlr_set_read_set_term(xlr_gen_t *gen, uint32_t *read_set, int term) {
  int termidx = term - gen->min_t;
  assert(termidx >= 0);
  assert(termidx < (1 + gen->max_t - gen->min_t));
  read_set = read_set + termidx / 32;
  *read_set |= 1 << (termidx & 31);
}

static int xlr_get_read_set_term(xlr_gen_t *gen, uint32_t *read_set, int term) {
  int termidx = term - gen->min_t;
  assert(termidx >= 0);
  assert(termidx < (1 + gen->max_t - gen->min_t));
  read_set = read_set + termidx / 32;
  return !!((*read_set) & (1 << (termidx & 31)));
}

static void xlr_sort_items(size_t nr_items, xlr_item_t *items) {
  qsort(items, nr_items, sizeof(xlr_item_t), xlr_compare_items);
}

static int xlr_is_nt(xlr_gen_t *gen, int sym) {
  return (sym >= gen->min_nt) && (sym <= gen->max_nt);
}

static int xlr_is_t(xlr_gen_t *gen, int sym) {
  return (sym >= gen->min_t) && (sym <= gen->max_t);
}

static int xlr_is_nullable(xlr_gen_t *gen, int sym) {
  return xlr_is_nt(gen, sym) && gen->is_nt_nullable[sym - gen->min_nt];
}

static xlr_error_t xlr_init_gen_grammar(xlr_gen_t *gen, int *grammar,
                                        int eop_sym, int eog_sym, int eof_sym, int eopn_sym, int synths_sym) {
  size_t nr_productions;
  size_t n, prod;
  int found_nullable;
                                          
  gen->eop_sym = eop_sym; gen->eog_sym = eog_sym; gen->eof_sym = eof_sym; gen->eopn_sym = eopn_sym; gen->synths_sym = synths_sym;

  /* Count the number of productions, this is equivalent to counting the number of end-of-production-syms (eop_syms) until
   * we find an end-of-grammar-sym. Note that there is an implied synthetic S production which also counts to the total
   * number of productions. */
  nr_productions = 1; /* Synthetic S production is a given.. */
  for (n = 0; grammar[n] != gen->eog_sym; n++) {
    if (grammar[n] == eop_sym) nr_productions++;
  }
  gen->nr_productions = nr_productions;

  /* Allocate all production-count dependant structures. */
  if (gen->productions) free(gen->productions); /* free any previous allocations */
  gen->productions = (xlr_prod_t *)malloc(sizeof(xlr_prod_t) * gen->nr_productions);
  
  /* Initialize the start of each production; the first production is the synthetic S': S special case */
  gen->synth_s_prod[0] = gen->synths_sym;
  gen->synth_s_prod[1] = gen->eopn_sym;
  gen->synth_s_prod[2] = grammar[0]; /* Non-terminal the first user-passed production reduces to */
  gen->synth_s_prod[3] = gen->eop_sym;

  gen->productions[0].nr_reduction_syms = 1;
  gen->productions[0].production = gen->synth_s_prod + 2;
  gen->productions[0].production_length = 1;
  gen->productions[0].reduction_syms = gen->synth_s_prod;

  prod = 1;
  for (n = 0; grammar[n] != gen->eog_sym; n++) {
    gen->productions[prod].reduction_syms = grammar + n;
    /* Count out how many non-terminals this production reduces to, or if no end-of-production-nonterminals
     * exists for this production, the length of the production. */
    while ((grammar[n] != gen->eopn_sym) &&
           (grammar[n] != gen->eop_sym)) {
      n++;
    }
    if (grammar[n] == gen->eop_sym) {
      /* End of production encountered without an end-of-production-nonterminals sym in it,
       * there is only one non-terminal this reduces to, and the rest is the production. */
      gen->productions[prod].production = gen->productions[prod].reduction_syms + 1;
      gen->productions[prod].nr_reduction_syms = 1;
      gen->productions[prod].production_length = (grammar + n) - gen->productions[prod].production;
    }
    else {
      /* End of production-nonterminals found.. Record, and look for end of production next. */
      gen->productions[prod].nr_reduction_syms = (grammar + n) - gen->productions[prod].reduction_syms;
      n++;
      gen->productions[prod].production = grammar + n;
      while (grammar[n] != gen->eop_sym) {
        n++;
      }
      gen->productions[prod].production_length = (grammar + n) - gen->productions[prod].production;
    }
    /* End of production always reached here, increment production number */
    prod++;
  }

  /* Determine the non-terminal and terminal symbol ordinal ranges and make sure they don't overlap. */
  gen->max_nt = gen->min_nt = gen->synths_sym;
  for (prod = 1; prod < gen->nr_productions; prod++) {
    for (n = 0; n < gen->productions[prod].nr_reduction_syms; n++) {
      int nt = gen->productions[prod].reduction_syms[n];
      if (nt < gen->min_nt) gen->min_nt = nt;
      if (nt > gen->max_nt) gen->max_nt = nt;
    }
  }
  /* Min & max for terminals are a bit more complicated and O(n**2) because we don't want to depend
   * on the ranges for non-terminals if we're to identify any overlap */
  gen->min_t = gen->max_t = gen->eof_sym;
  for (prod = 1; prod < gen->nr_productions; prod++) {
    size_t pos;
    for (pos = 0; pos < gen->productions[prod].production_length; pos++) {
      int sym = gen->productions[prod].production[pos];
      /* For this sym, determine if it is a non-terminal, if it IS (i.e. it appears in a reduction)
       * then don't include it in range determination for terminals. */
      int is_nt = 0;
      for (n = 0; !is_nt && (n < gen->nr_productions); n++) {
        size_t rsi;
        for (rsi = 0; !is_nt && (rsi < gen->productions[n].nr_reduction_syms); rsi++) {
          int ntsym = gen->productions[n].reduction_syms[rsi];
          if (ntsym == sym) {
            /* Found sym as non-terminal */
            is_nt = 1;
          }
        }
      }
      if (!is_nt) {
        /* not a non-terminal, include for range of terminals */
        if (sym < gen->min_t) gen->min_t = sym;
        if (sym > gen->max_t) gen->max_t = sym;
      }
    }
  }
  /* Perform range-check of terminals and non-terminals - we do this to ensure we can
   * test them for terminal-ness by range check only later on. */
  if ((gen->min_t <= gen->max_nt) && (gen->max_t >= gen->min_nt)) {
    return XLR_SYM_OVERLAP;
  }
  gen->min_s = gen->min_t;
  if (gen->min_nt < gen->min_s) gen->min_s = gen->min_nt;
  gen->max_s = gen->max_nt;
  if (gen->max_t > gen->max_s) gen->max_s = gen->max_t;

  /* Determine which of the nonterminals are nullable. */
  gen->is_nt_nullable = (int *)malloc(sizeof(int) * (1 + gen->max_nt - gen->min_nt));
  gen->non_kernel_considered = (int *)malloc(sizeof(int) * (1 + gen->max_nt - gen->min_nt));
  memset(gen->is_nt_nullable, 0, sizeof(int) * (1 + gen->max_nt - gen->min_nt));
  memset(gen->non_kernel_considered, 0, sizeof(int) * (1 + gen->max_nt - gen->min_nt));
  do {
    found_nullable = 0;
    for (prod = 0; prod < gen->nr_productions; prod++) {
      size_t pos;
      for (pos = 0; pos < gen->productions[prod].production_length; pos++) {
        int sym = gen->productions[prod].production[pos];
        if (!xlr_is_nullable(gen, sym)) {
          /* Production contains non-nullable element, and is not nullable. */
          break;
        }
      }
      if (pos == gen->productions[prod].production_length) {
        /* Did not encounter any non-nullable items, therefore all reduction
         * syms for this production are nullable; check if this is new information
         * to us (and whether there might be a knock-on effect worthy of another
         * iteration.) */
        size_t redidx;
        for (redidx = 0; redidx < gen->productions[prod].nr_reduction_syms; redidx++) {
          int sym = gen->productions[prod].reduction_syms[redidx];
          size_t sym_idx = (size_t)(sym - gen->min_nt);
          if (!gen->is_nt_nullable[sym_idx]) {
            /* Newly discovered nullable non-terminal */
            gen->is_nt_nullable[sym_idx] = 1;
            found_nullable = 1;
          }
        }
      }
    }
  } while (found_nullable);
  return XLR_OK;
}

static void xlr_lr0_push_item_outbound(xlr_gen_t *gen, xlr_state_t *state, int production, int position) {
  if (gen->productions[production].production_length == (size_t)position) {
    /* Nothing to do for this item, cannot push to subsequent item as the end of the production
     * has been reached and a reduce is normally in order here. */
  }
  else {
    int sym = gen->productions[production].production[position];
    xlr_trans_t *trans;
    size_t idx;
    trans = state->outbound[sym - gen->min_s];
    if (!trans) {
      /* Create new outbound transition as the state does not yet have one for this sym */
      trans = (xlr_trans_t *)malloc(sizeof(xlr_trans_t));
      memset(trans, 0, sizeof(xlr_trans_t));
      trans->chain_inbound = NULL;
      trans->from = state;
      trans->nr_items_allocated = 4;
      trans->nr_items = 0;
      if (xlr_is_nt(gen, sym)) {
        /* Transition of a non-terminal symbol, it maintains a read set. */
        trans->read_set = (uint32_t *)malloc(xlr_read_set_byte_size(gen));
        /* Read set is initially empty */
        memset(trans->read_set, 0, xlr_read_set_byte_size(gen));
      }
      else {
        /* Only non-terminals maintain read sets */
        trans->read_set = NULL;
      }
      trans->items = (xlr_item_t *)malloc(sizeof(xlr_item_t) * trans->nr_items_allocated);
      state->outbound[sym - gen->min_s] = trans;
    }
    if (trans->nr_items_allocated == trans->nr_items) {
      trans->nr_items_allocated += 8;
      trans->items = (xlr_item_t *)realloc(trans->items, sizeof(xlr_item_t) * trans->nr_items_allocated);
    }
    /* We're looking to add the item *after* the current position; and it might already exist (in which
     * case we should not add it.) */
    for (idx = 0; idx < trans->nr_items; idx++) {
      if ((trans->items[idx].production == production) &&
          (trans->items[idx].position == (position + 1))) {
        /* Already have this item. */
        break;
      }
    }
    if (idx < trans->nr_items) {
      /* Item already existed as for loop did not finish to completion. No need to add it again. */
    }
    else {
      /* Item did not yet exist */
      trans->items[trans->nr_items].position = position + 1;
      trans->items[trans->nr_items].production = production;
      trans->nr_items++;

      /* We'll need to cascade this to all the production's first symbols that reduce to
       * the sym we just allowed for transfer; if it has productions (eg. if it is a 
       * non-terminal.) */
      if (xlr_is_nt(gen, sym)) {
        size_t prod;
        for (prod = 0; prod < gen->nr_productions; prod++) {
          size_t redix;
          for (redix = 0; redix < gen->productions[prod].nr_reduction_syms; redix++) {
            if (gen->productions[prod].reduction_syms[redix] == sym) {
              /* This production (potentially) reduces to the sym, therefore add
               * its first sym as a transition from state. */
              xlr_lr0_push_item_outbound(gen, state, (int)prod, 0);
            }
          }
        }
      }
    }
  }
}

static void xlr_find_lr0_outbound(xlr_gen_t *gen, xlr_state_t *state) {
  size_t itemidx;
  int symidx;
  /* Iterate through all items, and examine their next symbol. Take one step 
   * for the outbound transition for that symbol on that item, thus producing
   * a next item for the outbound transition's to-state. If the symbol we
   * stepped is a non-terminal, then recursively examine all productions for
   * the non-terminal and step on the first symbol in their productions. */
  for (itemidx = 0; itemidx < state->nr_items; itemidx++) {
    xlr_item_t *item = state->items + itemidx;
    xlr_lr0_push_item_outbound(gen, state, item->production, item->position);
  }

  for (symidx = 0; symidx < (1 + gen->max_s - gen->min_s); symidx++) {
    int sym = gen->min_s + symidx;
    xlr_trans_t *trans = state->outbound[symidx];
    xlr_state_t *candidate;
    if (!trans) {
      continue;
    }
    /* Sort the items so any set of items can be compared with a binary compare
     * operation. */
    xlr_sort_items(trans->nr_items, trans->items);
    candidate = NULL;
    for (candidate = CHAIN_NEXT(gen->states, xlr_state_t, chain, candidate);
         candidate;
         candidate = CHAIN_NEXT(gen->states, xlr_state_t, chain, candidate)) {
      if ((candidate->nr_items == trans->nr_items) &&
          (!memcmp(candidate->items, trans->items, sizeof(xlr_item_t) * trans->nr_items))) {
        /* Matching state found */

        /* Candidate's sym must always match the transition's sym at this point, otherwise there
         * is a mismatch in the viable prefix for the state. */
        assert(candidate->sym == sym);

        trans->nr_items = trans->nr_items_allocated = 0;
        free(trans->items);
        trans->items = NULL;

        trans->to = candidate;

        CHAIN_INIT(trans, xlr_trans_t, chain_inbound);
        trans->to->inbound = CHAIN_APPEND(trans->to->inbound, trans, xlr_trans_t, chain_inbound);
        break;
      }
    }
    if (!candidate) {
      /* No candidate-state was found, create a new state and put it on the worklist */
      xlr_state_t *new_state = (xlr_state_t *)malloc(sizeof(xlr_state_t));
      memset(new_state, 0, sizeof(xlr_state_t));
      new_state->outbound = (xlr_trans_t **)malloc(sizeof(xlr_trans_t *) * (1 + gen->max_s - gen->min_s));
      memset(new_state->outbound, 0, sizeof(xlr_trans_t *) * (1 + gen->max_s - gen->min_s));

      /* State directly inherits item pointers */
      new_state->items = trans->items;
      new_state->nr_items = trans->nr_items;
      trans->nr_items = trans->nr_items_allocated = 0;
      trans->items = NULL;
      trans->to = new_state;
      new_state->sym = sym;

      CHAIN_INIT(trans, xlr_trans_t, chain_inbound);
      trans->to->inbound = CHAIN_APPEND(trans->to->inbound, trans, xlr_trans_t, chain_inbound);

      /* Insert on both state list and worklist */
      CHAIN_INIT(new_state, xlr_state_t, chain);
      CHAIN_INIT(new_state, xlr_state_t, workchain);
      gen->states = CHAIN_APPEND(gen->states, new_state, xlr_state_t, chain);
      gen->worklist = CHAIN_APPEND(gen->worklist, new_state, xlr_state_t, workchain);
    }
  }
}

static void xlr_populate_read_set(xlr_gen_t *gen) {
  xlr_state_t *state = NULL;
  xlr_trans_t *trans = NULL;
  int initial_sym;
  while (0 != (state = CHAIN_NEXT(gen->states, xlr_state_t, chain, state))) {
    if (xlr_is_nt(gen, state->sym)) {
      /* Most recent sym in state's viable prefix is a non-terminal, therefore
       * all transitions to this state should be populated with the read set. */
      xlr_trans_t *ot;
      int termidx;
      trans = CHAIN_FIRST(state->inbound, xlr_trans_t, chain_inbound);
      if (!trans) {
        /* No inbound transitions, continue to next; this is a little strange but might
         * happen if we mistake the sym on the initial state for being a non-terminal 
         * (the initial state does not have a sym.) */
        continue;
      }
      for (termidx = 0; termidx < (1 + gen->max_t - gen->min_t); termidx++) {
        int t = termidx + gen->min_t;
        if (state->outbound[t - gen->min_s]) {
          /* There's an outbound transition for this terminal, therefore set it in
           * the read set */
          xlr_set_read_set_term(gen, trans->read_set, t);
        }
      }
      /* trans's read_set is now populated with the appropriate first read set,
       * copy out to other transitions. */
      ot = trans;
      while (0 != (ot = CHAIN_NEXT(state->inbound, xlr_trans_t, chain_inbound, ot))) {
        memcpy(ot->read_set, trans->read_set, xlr_read_set_byte_size(gen));
      }
    }
  }
  /* There is one notable exception; the first state (which is the initial state
   * corresponding to the item S': .S) has an outbound transition on S which 
   * reads the end of file/stream symbol. */
  initial_sym = gen->productions[0].production[0];
  state = CHAIN_FIRST(gen->states, xlr_state_t, chain);
  trans = state->outbound[initial_sym - gen->min_s];
  assert(trans);
  xlr_set_read_set_term(gen, trans->read_set, gen->eof_sym);
}

static void xlr_add_rel(xlr_trans_t *from, xlr_trans_t *to) {
  if (from->nr_rels == from->nr_rels_allocated) {
    from->nr_rels_allocated += 8;
    from->rels = (xlr_trans_t **)realloc(from->rels, sizeof(xlr_trans_t *) * from->nr_rels_allocated);
  }
  from->rels[from->nr_rels++] = to;
}

static void xlr_populate_reads_rel(xlr_gen_t *gen) {
  xlr_state_t *state = NULL;
  while (0 != (state = CHAIN_NEXT(gen->states, xlr_state_t, chain, state))) {
    if (xlr_is_nt(gen, state->sym)) {
      xlr_trans_t *inbound_trans = NULL;
      while (0 != (inbound_trans = CHAIN_NEXT(state->inbound, xlr_trans_t, chain_inbound, inbound_trans))) {
        int nt_sym;
        /* Reset any prior relations */
        inbound_trans->nr_rels = 0;
        for (nt_sym = gen->min_nt; nt_sym <= gen->max_nt; nt_sym++) {
          int nt_index = nt_sym - gen->min_nt;
          if (state->outbound[nt_sym - gen->min_s] && gen->is_nt_nullable[nt_index]) {
            /* Outbound non-terminal transition is nullable, therefore build the relation. */
            xlr_add_rel(inbound_trans, state->outbound[nt_sym - gen->min_s]);
          }
        }
      }
    }
  }
}

static xlr_error_t xlr_pop_inc_backtrack_item(xlr_gen_t *gen, xlr_state_t *state, xlr_trans_t *oritrans, xlr_item_t *btitem, int backsteps) {
  xlr_prod_t *prod = gen->productions + btitem->production;
  int backsym;
  xlr_error_t our_result;
  xlr_trans_t *predtrans;
  if (backsteps == btitem->position) {
    /* Reached root of item - form includes relation from oritrans to the outbound of all reduction syms of btitem. */
    size_t redsymidx;
    our_result = XLR_ITEM_BACKTRACK_FAILED;
    for (redsymidx = 0; redsymidx < prod->nr_reduction_syms; redsymidx++) {
      int redsym = prod->reduction_syms[redsymidx];
      xlr_trans_t *redtrans = state->outbound[redsym - gen->min_s];
      if (redsym == gen->synths_sym) {
        /* Synthetic S sym, ultimate root of grammar has no backtrack */
        return XLR_OK;
      }
      if (redtrans) {
        /* Reduction of variant recognized by state at root of item; form relation */
        xlr_add_rel(oritrans, redtrans);
        /* Only one variant needs to match for us to succeed.. */
        our_result = XLR_OK;
      }
    }
    /* Asserts only if we failed to backtrack; which is an internal error that *should* never happen. */
    assert(our_result == XLR_OK);
    return our_result;
  }
  /* Not yet at root of item, continue backtrack */
  backsteps++;
  backsym = prod->production[btitem->position - backsteps];
  predtrans = NULL;
  our_result = XLR_OK;
  if (state->sym == backsym) {
    /* Inbound transitions to this state will match */
    while (0 != (predtrans = CHAIN_NEXT(state->inbound, xlr_trans_t, chain_inbound, predtrans))) {
      /* Found matching transition for the item being backtracked. */
      xlr_error_t result = xlr_pop_inc_backtrack_item(gen, predtrans->from, oritrans, btitem, backsteps);
      if (result != XLR_OK) our_result = result;
    }
  }
  return our_result;
}

static void xlr_reset_rels(xlr_gen_t *gen) {
  xlr_state_t *state = NULL;
  while (0 != (state = CHAIN_NEXT(gen->states, xlr_state_t, chain, state))) {
    xlr_trans_t *trans = NULL;
    while (0 != (trans = CHAIN_NEXT(state->inbound, xlr_trans_t, chain_inbound, trans))) {
      trans->nr_rels = 0;
      trans->index = 0;
      trans->low_index = 0;
    }
  }
}

static xlr_error_t xlr_populate_includes_rel(xlr_gen_t *gen) {
  xlr_error_t res;
  xlr_state_t *state = NULL;
  while (0 != (state = CHAIN_NEXT(gen->states, xlr_state_t, chain, state))) {
    if (xlr_is_nt(gen, state->sym)) {
      xlr_trans_t *trans = NULL;
      while (0 != (trans = CHAIN_NEXT(state->inbound, xlr_trans_t, chain_inbound, trans))) {
        /* Establish the includes relation with the non-terminal transition(s) of the 
         * reduction of any item that has reached its reduction position. */
        size_t itemidx;
        /* Reset any prior relations */
        trans->nr_rels = 0;
        for (itemidx = 0; itemidx < state->nr_items; itemidx++) {
          int pos;
          xlr_item_t *item = state->items + itemidx;
          xlr_prod_t *prod = gen->productions + item->production;
          pos = item->position;

          while (prod->production_length != (size_t)pos) {
            int sym = prod->production[pos];
            if (!xlr_is_nullable(gen, sym)) {
              /* Not a nullable sym, therefore item is exempt from the includes relation */
              break;
            }
            pos++;
          }
          if (prod->production_length == (size_t)pos) {
            /* Backtrack for this ready-to-reduce item */
            res = xlr_pop_inc_backtrack_item(gen, trans->from, trans, item, 1);
            if (res != XLR_OK) return res;
          }
          else {
            /* Did not make it to the end of the production, therefore there were
             * non-nullable transitions on the production; no further action needed. */
          }
        }
      }
    }
  }
  return XLR_OK;
}

void xlr_find_production_reduced_states(xlr_gen_t *gen, size_t *nr_states, size_t *nr_states_allocated, xlr_state_t ***states, xlr_state_t *current_state, xlr_item_t tracer) {
  xlr_prod_t *prod = gen->productions + tracer.production;
  size_t idx;
  /* Step tracer back until we reach a root position */
  if (tracer.position > 0) {
    xlr_trans_t *trans = NULL;
    tracer.position--;
    for (trans = CHAIN_FIRST(current_state->inbound, xlr_trans_t, chain_inbound);
         trans;
         trans = CHAIN_NEXT(current_state->inbound, xlr_trans_t, chain_inbound, trans)) {
      xlr_find_production_reduced_states(gen, nr_states, nr_states_allocated, states, trans->from, tracer);
    }
    return;
  }
  /* Determine all reduction symbols from the current production; and trace them out into outbound states */
  for (idx = 0; idx < prod->nr_reduction_syms; idx++) {
    int redsym = prod->reduction_syms[idx];
    xlr_trans_t *redoutbound = current_state->outbound[redsym - gen->min_s];
    if (redoutbound) {
      int have_it_already = 0;
      size_t sidx;
      /* Outbound found; add its state if we don't have it yet */
      for (sidx = 0; sidx < *nr_states; sidx++) {
        if (redoutbound->to == (*states)[sidx]) {
          have_it_already = 1;
          break;
        }
      }
      if (!have_it_already) {
        /* Add it */
        if (*nr_states == *nr_states_allocated) {
          *nr_states_allocated += 8;
          *states = (xlr_state_t **)realloc(*states, sizeof(xlr_state_t *) * (*nr_states_allocated));
        }
        (*states)[(*nr_states)++] = redoutbound->to;
      }
    }
  }
}

static void xlr_tarjan_visit(xlr_gen_t *gen, xlr_trans_t *trans) {
  size_t rel_idx, read_set_size;
  trans->index = gen->next_index++;
  trans->low_index = trans->index;
  
  /* Read set size is always a multiple of sizeof(uint32_t) */
  read_set_size = xlr_read_set_byte_size(gen);

  /* Push trans to stack */
  CHAIN_INIT(trans, xlr_trans_t, stack_chain);
  /* Push as front, we'll also pop from front. */
  gen->tarjan_stack = CHAIN_APPEND(trans, gen->tarjan_stack, xlr_trans_t, stack_chain);

  for (rel_idx = 0; rel_idx < trans->nr_rels; rel_idx++) {
    size_t n, nr_u32s = read_set_size / sizeof(uint32_t);
    xlr_trans_t *rel = trans->rels[rel_idx];
    if (!rel->index) {
      /* Rel is undefined */
      xlr_tarjan_visit(gen, rel);
      if (rel->low_index < trans->low_index) trans->low_index = rel->low_index;
    }
    else if (rel->stack_chain) {
      /* Rel is on stack. */
      if (rel->index < trans->low_index) trans->low_index = rel->index;
    }
    else {
      /* Rel was already handled, this is a cross-link; not an SCC. */
    }
    /* Accumulate relation dependency; doing this post-recursive ensures that,
     * if we discover an SCC later (eg. trans->index == trans->low_index), the
     * head of the SCC will have the full SCC's result. */
    if (rel->read_set) {
      for (n = 0; n < nr_u32s; n++) {
        trans->read_set[n] |= rel->read_set[n];
      }
    }
  }

  if (trans->index == trans->low_index) {
    /* Trans forms head of SCC, pop stack until trans is popped. */
    xlr_trans_t *scc_member = NULL;

    /* Pop members of the SCC and copy the aggregate read set for the whole SCC. */
    while (trans != (scc_member = CHAIN_POP(&gen->tarjan_stack, xlr_trans_t, stack_chain))) {
      memcpy(scc_member->read_set, trans->read_set, read_set_size);
    }
  }
}

static void xlr_tarjan_rel(xlr_gen_t *gen) {
  xlr_state_t *state = NULL;
  /* Index should start at 1, an index of 0 is considered "no index assigned yet" */
  gen->next_index = 1;
  while (0 != (state = CHAIN_NEXT(gen->states, xlr_state_t, chain, state))) {
    if (xlr_is_nt(gen, state->sym)) {
      xlr_trans_t *trans = NULL;
      while (0 != (trans = CHAIN_NEXT(state->inbound, xlr_trans_t, chain_inbound, trans))) {
        if (trans->index) {
          /* Already visited; skip. */
          continue;
        }
        xlr_tarjan_visit(gen, trans);
      }
    }
  }
}

/* Finds all lookahead symbols for the production to be reduced. The production may
 * reduce to one of multiple symbols chosen at runtime (see xlr_prod_t); the viable
 * lookaheads for any one such symbol is however not differentiated. Eg. if a production
 * may reduce to either A or B, and nonterminal A & B have different lookaheads for their
 * reduction, that information is not used within context. Consequently, a reduction is
 * not a guarantee that there will be a viable goto for the non-terminal reduced to. In
 * this respect, the xlalr differs from lalr in that a grammar is not context-free. */
static void xlr_find_reduce_lookaheads(xlr_gen_t *gen, uint32_t *read_set_result, xlr_state_t *state, xlr_state_t *decision_state, int production, int position) {
  xlr_trans_t *pred = NULL;
  size_t read_set_u32_size = xlr_read_set_byte_size(gen) / sizeof(uint32_t);
  if (!position) {
    /* Reached the root of the item, now find all outbound transitions and combine their read sets */
    size_t redix;
    int found_a_trans = 0;
    xlr_prod_t *prod = gen->productions + production;
    for (redix = 0; redix < prod->nr_reduction_syms; redix++) {
      int redsym = prod->reduction_syms[redix];
      xlr_trans_t *trans = state->outbound[redsym - gen->min_s];
      if (redsym == gen->synths_sym) {
        /* Root of entire grammar has EOF as a lookahead. */
        xlr_set_read_set_term(gen, read_set_result, gen->eof_sym);
        found_a_trans = 1;
      }
      if (trans) {
        /* Combine trans->read_set into the reduction's lookahead */
        size_t readsetidx;
        found_a_trans = 1;
        for (readsetidx = 0; readsetidx < read_set_u32_size; readsetidx++) {
          read_set_result[readsetidx] |= trans->read_set[readsetidx];
        }
      }
    }
    assert(found_a_trans);
    return;
  }
  position--;
  if (gen->productions[production].production[position] != state->sym) {
    /* Item cannot possibly pass through this state, discard. */
    return;
  }
  while (0 != (pred = CHAIN_NEXT(state->inbound, xlr_trans_t, chain_inbound, pred))) {
    /* All these transitions already match (see check above) as their symbol
     * is encoded on the destination state (== state); consequently, just
     * recurse. */
    xlr_find_reduce_lookaheads(gen, read_set_result, pred->from, decision_state, production, position);
  }
}

static void xlr_populate_state_reductions(xlr_gen_t *gen, uint32_t *temp_read_set, xlr_state_t *state, int production, int position) {
  size_t prodix;
  int nt;
  xlr_prod_t *prod = gen->productions + production;
  size_t read_set_byte_size = xlr_read_set_byte_size(gen);

  if (prod->production_length == (size_t)position) {
    int sym;
    /* Reduction-item; locate all lookahead terminals */
    memset(temp_read_set, 0, read_set_byte_size);
    xlr_find_reduce_lookaheads(gen, temp_read_set, state, state, production, position);
    for (sym = gen->min_t; sym <= gen->max_t; sym++) {
      if (xlr_get_read_set_term(gen, temp_read_set, sym)) {
        /* Part of the lookahead, reduction on prod., create the action. */
        xlr_action_t **action_cell;
        xlr_action_t *new_action;
        xlr_action_t *action = NULL;
        /* Look for the action, it is possible for it to already exist as we'll be coming here
         * for each reduction symbol. */
        action_cell = gen->action_table + state->ordinal * (1 + gen->max_s - gen->min_s) + (sym - gen->min_s);
        while (0 != (action = CHAIN_NEXT(*action_cell, xlr_action_t, chain, action))) {
          if ((action->action == XLR_REDUCE) && (action->production == production)) {
            /* Already have the action to reduce */
            break;
          }
        }
        if (!action) {
          /* Don't yet have the action to reduce; add it */
          new_action = (xlr_action_t *)malloc(sizeof(xlr_action_t));
          CHAIN_INIT(new_action, xlr_action_t, chain);
          new_action->action = XLR_REDUCE;
          new_action->production = production;
          new_action->state = NULL;
          *action_cell = CHAIN_APPEND(*action_cell, new_action, xlr_action_t, chain);
        }
      }
    }
    /* Done for this production */
    return;
  }
  nt = prod->production[position];
  if (xlr_is_nt(gen, nt) && !gen->non_kernel_considered[nt - gen->min_nt]) {
    /* Non-terminal, recurse */
    gen->non_kernel_considered[nt - gen->min_nt] = 1; /* avoid cyclic recursion */
    for (prodix = 0; prodix < gen->nr_productions; prodix++) {
      size_t redix;
      for (redix = 0; redix < gen->productions[prodix].nr_reduction_syms; redix++) {
        int redsym = gen->productions[prodix].reduction_syms[redix];
        if (redsym == nt) {
          /* Production indexed by prodix potentially reduces to redsym */
          xlr_populate_state_reductions(gen, temp_read_set, state, (int)prodix, 0);
        }
      }
    }
  }
}

static void xlr_build_action_table(xlr_gen_t *gen) {
  xlr_state_t *state = NULL;
  size_t nr_states = 0;
  size_t table_width;
  size_t read_set_byte_size = xlr_read_set_byte_size(gen);
  uint32_t *temp_reduction_lookahead;
  while (0 != (state = CHAIN_NEXT(gen->states, xlr_state_t, chain, state))) {
    state->ordinal = (int)nr_states++;
  }
  gen->nr_states = nr_states;

  /* Allocate the action table. */
  table_width = 1 + gen->max_s - gen->min_s;
  gen->action_table = (xlr_action_t **)malloc(sizeof(xlr_action_t *) * table_width * nr_states);
  memset(gen->action_table, 0, sizeof(xlr_action_t *) * table_width * nr_states);

  /* Allocate buffer used for accumulating the read sets from reductions */
  temp_reduction_lookahead = (uint32_t *)malloc(read_set_byte_size);

  while (0 != (state = CHAIN_NEXT(gen->states, xlr_state_t, chain, state))) {
    size_t itemidx;
    int sym;
    /* Insert all shifts for the state */
    for (sym = gen->min_s; sym <= gen->max_s; sym++) {
      xlr_trans_t *trans = state->outbound[sym - gen->min_s];
      if (trans) {
        xlr_action_t **action_cell;
        xlr_action_t *new_action = (xlr_action_t *)malloc(sizeof(xlr_action_t));
        CHAIN_INIT(new_action, xlr_action_t, chain);
        new_action->action = XLR_SHIFT;
        new_action->production = 0;
        new_action->state = trans->to;
        action_cell = gen->action_table + state->ordinal * (1 + gen->max_s - gen->min_s) + (sym - gen->min_s);
        /* Assert that action cell is always empty; otherwise we have an addressing failure (and would like to
         * know about it. */
        assert(!(*action_cell));
        *action_cell = CHAIN_APPEND(*action_cell, new_action, xlr_action_t, chain);
      }
    }

    /* Populate all reductions for the state */
    memset(gen->non_kernel_considered, 0, sizeof(int) * (1 + gen->max_nt - gen->min_nt));
    for (itemidx = 0; itemidx < state->nr_items; itemidx++) {
      xlr_item_t *item = state->items + itemidx;
      xlr_populate_state_reductions(gen, temp_reduction_lookahead, state, item->production, item->position);
    }
  }

  free(temp_reduction_lookahead);
}

xlr_error_t xlr_generate(xlr_gen_t *gen, int *grammar,
                         int eop_sym, int eog_sym, int eof_sym, int eopn_sym, int synths_sym) {
  xlr_error_t xlre;
  xlr_state_t *initial_state, *work_state, *s;
  size_t n;

  xlre = xlr_init_gen_grammar(gen, grammar, eop_sym, eog_sym, eof_sym, eopn_sym, synths_sym);

  if (xlre != XLR_OK) return xlre;

  /* Set up the initial state: S' : .S */
  initial_state = (xlr_state_t *)malloc(sizeof(xlr_state_t));
  memset(initial_state, 0, sizeof(xlr_state_t));
  CHAIN_INIT(initial_state, xlr_state_t, chain);
  CHAIN_INIT(initial_state, xlr_state_t, workchain);
  gen->states = CHAIN_APPEND(gen->states, initial_state, xlr_state_t, chain);
  gen->worklist = CHAIN_APPEND(gen->worklist, initial_state, xlr_state_t, workchain);
  initial_state->nr_items = 1;
  initial_state->items = (xlr_item_t *)malloc(sizeof(xlr_item_t) * initial_state->nr_items);
  initial_state->items[0].production = 0;
  initial_state->items[0].position = 0;
  initial_state->outbound = (xlr_trans_t **)malloc(sizeof(xlr_trans_t *) * (1 + gen->max_s - gen->min_s));
  memset(initial_state->outbound, 0, sizeof(xlr_trans_t *) * (1 + gen->max_s - gen->min_s));
  while (0 != (work_state = CHAIN_POP(&gen->worklist, xlr_state_t, workchain))) {
    xlr_find_lr0_outbound(gen, work_state);
  }
  /* We now have the set of LR0 items...
   * .. populate the initial directly-reads readsets */
  xlr_populate_read_set(gen);

  /* .. perform the reads-relation aggregation */
  xlr_populate_reads_rel(gen);
  gen->had_nontrivial_sccs = 0;
  xlr_tarjan_rel(gen);
  if (gen->had_nontrivial_sccs) {
    /* Ambiguous grammar */
    return XLR_NOT_LR_GRAMMAR;
  }

  /* .. perform the includes-relation aggregation */
  xlr_reset_rels(gen);
  xlre = xlr_populate_includes_rel(gen);
  if (xlre != XLR_OK) return xlre;
  /* Any non-trivial SCCs that do not have empty read sets will cause
   * conflicts down the line.... let conflict resolution pick up on that */
  xlr_tarjan_rel(gen);
  
  /* .. build action table */
  xlr_build_action_table(gen);

  /* .. build array of states */
  gen->state_array = (xlr_state_t **)malloc(sizeof(xlr_state_t *) * gen->nr_states);
  n = 0;
  s = NULL;
  while (0 != (s = CHAIN_NEXT(gen->states, xlr_state_t, chain, s))) {
    gen->state_array[n++] = s;
  }
  
  return XLR_OK;
}

void xlr_add_conflict_resolution(xlr_gen_t *gen,
                                 int dominant_production, int dominant_position,
                                 int yielding_production, int yielding_position) {
  xlr_conflict_resolution_t *cr;
  if (gen->nr_conflict_resolutions == gen->nr_conflict_resolutions_allocated) {
    gen->nr_conflict_resolutions_allocated += 8;
    gen->conflict_resolutions = (xlr_conflict_resolution_t *)realloc(gen->conflict_resolutions, sizeof(xlr_conflict_resolution_t) * gen->nr_conflict_resolutions_allocated);
  }
  cr = gen->conflict_resolutions + gen->nr_conflict_resolutions++;
  cr->prefer.production = dominant_production;
  cr->prefer.position = dominant_position;
  cr->over.production = yielding_production;
  cr->over.position = yielding_position;
}

static void xlr_count_conflict_pairs(size_t nr_shift_items, size_t nr_reductions,
                                     size_t *nr_shift_reduces, size_t *nr_reduce_reduces) {
  /* All possible pairs of reductions are conflicts, excluding that a reduction cannot
   * conflict with itself (hence -1), and a conflict is order independant ((A,B) is the same
   * conflict as (B,A)) (hence /2). */
  *nr_reduce_reduces = (nr_reductions * (nr_reductions - 1)) / 2;
  /* Any one of the shift items combined with a reduction is a conflict. Shift items
   * are internally not conflicts (as a state can hold multiple.) */
  *nr_shift_reduces = nr_shift_items * nr_reductions;
}

static xlr_action_t *xlr_find_nth_reduce_action(xlr_action_t *action_list, size_t ordinal) {
  xlr_action_t *action = NULL;
  while (0 != (action = CHAIN_NEXT(action_list, xlr_action_t, chain, action))) {
    if (action->action == XLR_REDUCE) {
      if (!ordinal--) break;
    }
  }
  return action;
}

static xlr_action_t *xlr_find_shift_action(xlr_action_t *action_list) {
  xlr_action_t *action = NULL;
  while (0 != (action = CHAIN_NEXT(action_list, xlr_action_t, chain, action))) {
    if (action->action == XLR_SHIFT) break;
  }
  return action;
}

static void xlr_find_conflict_pair(xlr_gen_t *gen, xlr_action_t *actionlist, size_t conflict_number,
                                   xlr_item_t *itema, xlr_item_t *itemb) {
  size_t nr_reduce_reduces, nr_shift_reduces;
  size_t nr_shifts = 0, nr_reduces = 0;
  xlr_action_t *action = NULL;
  xlr_action_t *shift_state = NULL;
  /* Count number of shifts and reduces */
  while (0 != (action = CHAIN_NEXT(actionlist, xlr_action_t, chain, action))) {
    if (action->action == XLR_SHIFT) {
      nr_shifts = action->state->nr_items;
      shift_state = action;
    }
    else /* (action->action == XLR_REDUCE) */ {
      nr_reduces++;
    }
  }
  xlr_count_conflict_pairs(nr_shifts, nr_reduces, &nr_shift_reduces, &nr_reduce_reduces);
  if (conflict_number < nr_shift_reduces) {
    size_t shift_selected = conflict_number / nr_reduces;
    size_t reduce_selected = conflict_number % nr_reduces;
    /* Make pair of the shift_selected and reduce_selected items */
    xlr_action_t *shift_action = xlr_find_shift_action(actionlist);
    xlr_action_t *reduce_action = xlr_find_nth_reduce_action(actionlist, reduce_selected);
    itema->production = shift_action->state->items[shift_selected].production;
    itema->position = shift_action->state->items[shift_selected].position - 1;
    itemb->production = reduce_action->production;
    itemb->position = (int)gen->productions[itemb->production].production_length;
  }
  else {
    size_t first_reduce = 0;
    size_t nr_second_pair_candidates = nr_reduces - 1;
    size_t second_reduce = 0;
    xlr_action_t *first_reduce_action, *second_reduce_action;
    /* Trying to find the ordinal for all possible matches, this can be visualized
     * according to the following triangle. We'll step rows (first_candidates) until 
     * we have a row in which the second index lies.
     *  ABCDEFGHIJK
     * A-0123456789
     * B -abcdef012
     * C  -3456789a
     * D   -bcdef01
     * E    -234567
     * F     -89abc
     * G      -def0
     * H       -123
     * I        -45
     * J         -6
     * K          -
     */
    while (second_reduce > nr_second_pair_candidates) {
      first_reduce++;
      second_reduce -= nr_second_pair_candidates;
      nr_second_pair_candidates--;
    }
    second_reduce += first_reduce + 1;
    first_reduce_action = xlr_find_nth_reduce_action(actionlist, first_reduce);
    second_reduce_action = xlr_find_nth_reduce_action(actionlist, second_reduce);
    itema->production = first_reduce_action->production;
    itema->position = (int)gen->productions[itema->production].production_length;
    itemb->production = second_reduce_action->production;
    itemb->position = (int)gen->productions[itemb->production].production_length;
  }
}

static void xlr_find_conflicts(xlr_gen_t *gen, xlr_action_t *action_list) {
  size_t nr_reduce_reduces, nr_shift_reduces;
  size_t nr_shifts = 0, nr_reduces = 0;
  size_t n;
  xlr_action_t *action = NULL;
  xlr_action_t *shift_state = NULL;
  /* Count number of shifts and reduces */
  while (0 != (action = CHAIN_NEXT(action_list, xlr_action_t, chain, action))) {
    if (action->action == XLR_SHIFT) {
      nr_shifts = action->state->nr_items;
      shift_state = action;
    }
    else /* (action->action == XLR_REDUCE) */ {
      nr_reduces++;
    }
  }
  xlr_count_conflict_pairs(nr_shifts, nr_reduces, &nr_shift_reduces, &nr_reduce_reduces);
  gen->nr_temp_conflicts = nr_shift_reduces + nr_reduce_reduces;
  if (gen->nr_temp_conflicts > gen->nr_temp_conflicts_allocated) {
    gen->nr_temp_conflicts_allocated = gen->nr_temp_conflicts;
    gen->temp_conflicts = (xlr_conflict_t *)realloc(gen->temp_conflicts, sizeof(xlr_conflict_t) * gen->nr_temp_conflicts_allocated);
  }
  for (n = 0; n < gen->nr_temp_conflicts; n++) {
    xlr_find_conflict_pair(gen, action_list, n, &gen->temp_conflicts[n].item_a, &gen->temp_conflicts[n].item_b);
  }
}

static int xlr_items_equal(xlr_item_t *a, xlr_item_t *b) {
  return ((a->production == b->production) && (a->position == b->position));
}

static int xlr_conflict_equals_resolution(xlr_conflict_t *conflict, xlr_conflict_resolution_t *resolution) {
  return (((xlr_items_equal(&conflict->item_a, &resolution->prefer)) && xlr_items_equal(&conflict->item_b, &resolution->over)) ||
          ((xlr_items_equal(&conflict->item_b, &resolution->prefer)) && xlr_items_equal(&conflict->item_a, &resolution->over)));
}

xlr_action_t *xlr_resolve_conflicts(xlr_gen_t *gen, xlr_action_t *action_list) {
  size_t n;
  int conflicting_resolutions = 0;
  xlr_action_t *preferred_action = NULL;

  /* Reset conflicting resolutions */
  for (n = 0; n < gen->nr_conflict_resolutions; n++) {
    gen->conflict_resolutions[n].conflicting = 0;
  }

  /* Locate all conflicts for this list of actions */
  xlr_find_conflicts(gen, action_list);

  /* Attempt to resolve all conflicts from xlr_find_conflicts -- note that 
   * xlr_find_conflicts stores its results in gen->temp_conflicts. */
  for (n = 0; n < gen->nr_temp_conflicts; /* n does not increment by for() */ ) {
    xlr_conflict_t *c = gen->temp_conflicts + n;
    size_t cp;
    for (cp = 0; cp < gen->nr_conflict_resolutions; cp++) {
      xlr_conflict_resolution_t *cr = gen->conflict_resolutions + cp;
      if (xlr_conflict_equals_resolution(c, cr)) {
        /* Mark conflict resolution as having been used for this series of
         * actions. */
        cr->conflicting = 1;

        /* Conflict resolved, cancel it on the conflicts list. */
        gen->nr_temp_conflicts--;
        *c = gen->temp_conflicts[gen->nr_temp_conflicts];

        /* Conflict has been resolved in favor of cr->prefer; find the action
         * that corresponds to cr->prefer. */
        if (gen->productions[cr->prefer.production].production_length == (size_t)cr->prefer.position) {
          /* A reduce of cr->prefer.production is preferred. */
          xlr_action_t *action = NULL;
          while (0 != (action = CHAIN_NEXT(action_list, xlr_action_t, chain, action))) {
            if ((action->action == XLR_REDUCE) && (action->production == cr->prefer.production)) {
              if (!preferred_action) {
                preferred_action = action;
                break;
              }
              if (preferred_action == action) {
                break;
              }
              /* Mismatch between actions preferred by various conflict resolutions. */
              conflicting_resolutions = 1;
            }
          }
        }
        else {
          /* The shift is preferred. */
          xlr_action_t *shift_action = xlr_find_shift_action(action_list);
          if (!preferred_action) {
            preferred_action = shift_action;
          }
          else if (preferred_action == shift_action) {
            /* Congruent choice of resolutions */
          }
          else {
            /* Mismatch in choice of conflict resolutions. */
            conflicting_resolutions = 1;
          }
        }

        /* Breaking out will indicate to the temp_conflicts loop that we
         * resolved the conflict (and won't need to step forward in the
         * conflicts.) */
        break;
      }
    }
    if (cp != gen->nr_conflict_resolutions) {
      /* Conflict resolved, the conflict at ordinal 'n' is now different from when
       * we entered this iteration; so no need to step 'n' or we'd be skipping an
       * unresolved conflict. */
    }
    else {
      /* Conflict not resolved, step to next conflict; we already know that we'll
       * fail eventually (as we cannot pick an action) but the caller might want to
       * know the full scope of the failure. */
      n++;
    }
  }

  if (gen->nr_temp_conflicts) {
    /* Failure to resolve all conflicts */
    return NULL;
  }

  if (conflicting_resolutions) {
    /* Return failure - multiple conflict resolutions are prefering different outcomes. */
    return NULL;
  }

  if (!preferred_action) {
    /* Conflict resolution did not yield a prefer action, so just pick any action */
    return action_list;
  }
  return preferred_action;
}

