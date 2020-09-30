/* Copyright (C) 2009 Martijn Boekhorst
 * All rights reserved.
 *
 * $Id$
 */

#include <stdlib.h> /* malloc(), free() */
#include <string.h> /* memset(), memcpy() */
#include "lalr.h"

/* If you are reading this, you are probably suspecting a bug in this code, wish to modify it, or just in
 * general, would like to understand how it works. A couple of pointers to get you started:
 * References:
 * [1] "Compilers, principles, techniques and tools." - Aho, Sethi, Ullman, (inc. Lam in the 2nd ed.)
 *     You'll need to understand how LR and LALR parsers work and the theory behind them; this is the best
 *     description; unfortunately it does not describe the best LALR generator algorithm.
 * [2] "Efficient Computation of LALR(1) Look-Ahead Sets" - Pennelo, DeRemer
 *     This is "the" way to generate LALR parsers most efficiently, the article can be had for about $10 from
 *     ACM. It is a fairly old paper and consequently the type-setting on it is horrid with manual scribbles
 *     to compensate the use of a type-writer and an obscure underscore encoding to compensate for the lack
 *     of greek symbols; furthermore, the writing style is very dense and difficult to follow. However it is
 *     the best way to generate LALR's which is why you'll have to muddle through it; hopefully the 
 *     "programmer's" description below will help you out a bit.
 * [3] "Depth-first search and linear graph algorithms" - SIAM Journal on Computing Vol.1 - Robert Tarjan
 *     A beautifully simple algorithm for detecting cycles in graphs ("strongly connected components"); you'll
 *     need to read up on this seperately because the Pennelo, DeRemer description is not useful and worse, 
 *     I'm not even sure if it's accurate. Getting the article nowadays seems impossible though googling for
 *     the algorithm should suffice as it is very simple and clean.
 *
 * With that under your belt, the algorithm works as follows:
 *
 * For any given grammar, we first discover the LR(0) set of items using the closure type techniques described
 * in [1]; we now have a graph consisting of states, with each state holding a set of kernel items and we have
 * a set of transitions between the states, corresponding to a symbol in each state's first set.
 *
 * The algorithm in [2] works exclusively on the set of states just generated with the goal of determining for
 * each non-terminal transition (a transition for whose symbol we have one or more productions), what terminal
 * symbols may follow it. The reasoning behind this is that each non-terminal transition corresponds to a
 * reduction for that non-terminal; so if we know what lies ahead of the non-terminal transition, we know the
 * lookaheads based on which we should perform the reduction.
 * 
 * To help determine this below, we have a bitmap "read_set" in each transition that describes the symbols we
 * know can follow this transition; and we populate this read_set in successive stages:
 * 1. The read_set is initialized as consisting of all terminal transitions *from* the state that a transition
 *    goes *to* - eg. if [P -> .Tq, T] is a transition on T from a state holding item [P->.Tq] , then [P -> T.q]
 *    is an item in the state that the transition goes to. Furthermore, 'q' is an outbound terminal transition
 *    in that state; so we would mark 'q' in the read_set for [P -> .Tq, T].
 * 2. Let's say the string "ABc" is valid, A and B are non-terminals, and c is a terminal. In this case, if it
 *    is possible for B to be "nullable" anything that follows B may also follow A -- consequently we say
 *    "A reads B" -- in this case the implication is that, whatever is in the read_set for B, should also be in
 *    the read_set for A.
 *    This is a relation with a knock-on effect; eg. it might be "ABCDq" - anything visible from D is also
 *    visible from A if BCD are nullable; consequently we model it in our code using lr_rel_t -- a relationship
 *    between two transitions.
 * 3. Now that we've established the "reads" relation, "Tarjan's" algorithm is used to traverse the graph and
 *    propagate all the read_sets - eg. ensuring A sees q in the earlier example.
 * 4. We next model a different relation, the "includes" relation -- say we have two productions A -> Bq, 
 *    and B -> C. Provided we have the right viable prefix (that is to say, we came to B-> C from A -> Bq), 
 *    transition [B -> .C, C] is said to "include" [A-> .Bq, B] -- so the transition on C could be followed
 *    by the terminal symbol 'q' -- consequently, in this state, the terminal symbol 'q' should be in the 
 *    lookahead set for the reduction on C.
 * 5. Again run the Tarjan algorithm, now for the "includes" relation.
 * 6. As a final step, we need to examine all states and locate reductions; each reduction in such a state
 *    has one or more corresponding transitions of the non-terminal (the transition we will travel as the goto
 *    after popping all the production's symbols off the stack. The read_sets for those transitions combine into 
 *    the lookahead for the reduction.
 * 7. Now we can generate the parser table and discover any conflicts. Note that in the code, 6 & 7 are combined.
 *
 * There are a lot of details that I'm leaving out, but this should be enought to get you started.
 */

static lr_state_t *lr_create_state(lr_generator_t *gen) {
  lr_state_t *s = (lr_state_t *)malloc(sizeof(lr_state_t));
  if (!s) return NULL;
  s->kernel_items = NULL;
  s->transitions_from_state = NULL;
  s->transitions_to_state = NULL;
  s->gen_chain = gen->new_states;
  gen->new_states = s;
  return s;
}

static lr_item_t *lr_find_or_create_item(lr_generator_t *gen, lr_state_t *state, int production, int position) {
  lr_item_t **pi, *next_i;
  gen; /* unused var */
  for (pi = &state->kernel_items; *pi; pi = &((*pi)->state_chain)) {
    if (((*pi)->position == position) && ((*pi)->production == production)) {
      /* have it, return. */
      return *pi;
    }
    else if ((((*pi)->production == production) && ((*pi)->position > position)) ||
             ((*pi)->production > production)) {
      /* ascending order indicates we don't have it, insert here. */
      break;
    }
  }
  next_i = *pi;
  *pi = (lr_item_t *)malloc(sizeof(lr_item_t));
  if (!pi) {
    return NULL;
  }
  (*pi)->production = production;
  (*pi)->position = position;
  (*pi)->state_chain = next_i;
  return *pi;
}

/* NOTE: Always creates a new state on the other end of it. */
static lr_transition_t *lr_find_or_create_outbound_transition(lr_generator_t *gen, lr_state_t *from, int sym) {
  lr_transition_t *t;
  /* first look, if not found, then create new outbound transition *AND* state */
  for (t = from->transitions_from_state; t; t = t->from_chain) {
    if (t->sym == sym) {
      return t;
    }
  }
  t = (lr_transition_t *)malloc(sizeof(lr_transition_t) + (gen->highest_term - gen->lowest_term + 1 + 7) / 8);
  if (!t) {
    return NULL;
  }
  t->sym = sym;
  t->from = from;
  t->to = lr_create_state(gen);
  if (!t->to) {
    return NULL;
  }
  t->from_chain = from->transitions_from_state;
  from->transitions_from_state = t;
  t->to_chain = NULL;
  t->to->transitions_to_state = t;
  t->inbound_rels = NULL;
  t->outbound_rels = NULL;
  t->stack_chain = NULL;
  t->index = t->lowlink = 0;
  return t;
}

static lr_transition_t *lr_find_or_create_transition(lr_generator_t *gen, lr_state_t *from, int sym, lr_state_t *to) {
  lr_transition_t *t;
  /* First look for the transition, if not found, create a new one. */
  for (t = from->transitions_from_state; t; t = t->from_chain) {
    if ((t->sym == sym) && (t->to == to)) {
      return t;
    }
  }
  t = (lr_transition_t *)malloc(sizeof(lr_transition_t) + (gen->highest_term - gen->lowest_term + 1 + 7) / 8);
  if (!t) {
    return NULL;
  }
  t->sym = sym;
  t->from = from;
  t->from_chain = from->transitions_from_state;
  from->transitions_from_state = t;
  t->to = to;
  t->to_chain = to->transitions_to_state;
  to->transitions_to_state = t;
  t->inbound_rels = NULL;
  t->outbound_rels = NULL;
  t->stack_chain = NULL;
  t->index = t->lowlink = 0;
  return t;
}

static lr_state_t *lr_find_identical_state(lr_generator_t *gen, lr_state_t *state) {
  lr_state_t *s;
  for (s = gen->states; s; s = s->gen_chain) {
    lr_item_t *ia, *ib;
    ia = s->kernel_items;
    ib = state->kernel_items;
    while (ia && ib && (ia->production == ib->production) && (ia->position == ib->position)) {
      ia = ia->state_chain;
      ib = ib->state_chain;
    }
    if ((!ia) && (!ib)) {
      /* They match */
      return s;
    }
  }
  /* No match found */
  return NULL;
}

static int lr_closure(lr_generator_t *gen, lr_state_t *state) {
  lr_item_t *i;
  for (i = state->kernel_items; i; i = i->state_chain) {
    int sym = gen->productions[i->production][i->position + 1 /* [0] is production reduction sym */ ];
    if (sym == gen->eop_sym) {
      /* Reduction */
    }
    else if ((sym >= gen->lowest_nonterm) && (sym <= gen->highest_nonterm)) {
      size_t prodix;
      int nt_sym;
      int have_unprocessed_nonterminals;

      /* Non-terminal */
      lr_transition_t *t = lr_find_or_create_outbound_transition(gen, state, sym);
      if (!t) return -1;
      lr_item_t *new_item = lr_find_or_create_item(gen, t->to, i->production, i->position + 1);
      if (!new_item) return -1;

      /* Find non-kernel items for non-terminal transition and add their initial transitions. */
      memset(gen->nonterm_scratchpad, 0, sizeof(int) * (gen->highest_nonterm - gen->lowest_nonterm + 1));
      /* Flag non-terminal for processing.. */
      gen->nonterm_scratchpad[sym - gen->lowest_nonterm] = 1;
      have_unprocessed_nonterminals = 1;
      while (have_unprocessed_nonterminals) {
        have_unprocessed_nonterminals = 0;
        for (nt_sym = gen->lowest_nonterm; nt_sym <= gen->highest_nonterm; nt_sym++) {
          /* Check if non-terminal is to be processed.. */
          if (gen->nonterm_scratchpad[nt_sym - gen->lowest_nonterm] == 1) {
            /* Mark non-terminal as processed. */
            gen->nonterm_scratchpad[nt_sym - gen->lowest_nonterm] = 2;

            /* Process all first transitions for the non-terminal */
            for (prodix = 0; prodix < gen->nr_productions; prodix++) {
              if (gen->productions[prodix][0] == nt_sym) {
                int first_sym = gen->productions[prodix][1];
                if (first_sym == gen->eop_sym) {
                  /* null production */
                }
                else {
                  /* Add transition */
                  lr_transition_t *first_t = lr_find_or_create_outbound_transition(gen, state, first_sym);
                  lr_item_t *first_new_item = lr_find_or_create_item(gen, first_t->to, (int)prodix, 1);
                  first_new_item; /* touch first_new_item so it does not go unreferenced. */

                  if ((first_sym >= gen->lowest_nonterm) && (first_sym <= gen->highest_nonterm)) {
                    /* First symbol is a non-terminal, meaning we'll need to process the first symbols
                     * for its productions as well, unless we've already done so. */
                    if (gen->nonterm_scratchpad[first_sym - gen->lowest_nonterm] == 0) {
                      gen->nonterm_scratchpad[first_sym - gen->lowest_nonterm] = 1;
                      have_unprocessed_nonterminals = 1;
                    }
                    else {
                      /* first_sym is either already marked for processing, has already been processed,
                       * or is currently being processed -- no further consideration. */
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
    else {
      /* Terminal */
      lr_transition_t *t = lr_find_or_create_outbound_transition(gen, state, sym);
      if (!t) return -1;
      lr_item_t *new_item = lr_find_or_create_item(gen, t->to, i->production, i->position + 1);
      if (!new_item) return -1;
    }
  }
  return 0;
}

static void lr_destroy_state(lr_generator_t *gen, lr_state_t *moriturus) {
  /* Drop all inbound and outbound transitions.. */
  lr_transition_t *t, *nt;
  lr_item_t *i, *ni;
  /* touch gen so it does not go unreferenced. */
  gen;
  for (t = moriturus->transitions_to_state; t; t = nt) {
    lr_state_t *ds = t->from;
    lr_transition_t **pt;
    for (pt = &(ds->transitions_from_state); *pt; pt = &((*pt)->from_chain)) {
      if (*pt == t) {
        *pt = t->from_chain;
        break;
      }
    }
    nt = t->to_chain;
    free(t);
  }
  for (t = moriturus->transitions_from_state; t; t = nt) {
    lr_state_t *ds = t->to;
    lr_transition_t **pt;
    for (pt = &(ds->transitions_to_state); *pt; pt = &((*pt)->to_chain)) {
      if (*pt == t) {
        *pt = t->to_chain;
        break;
      }
    }
    nt = t->from_chain;
    free(t);
  }

  /* Drop all items */
  for (i = moriturus->kernel_items; i; i = ni) {
    ni = i->state_chain;
    free(i);
  }
  free(moriturus);
}

static int lr_merge_states(lr_generator_t *gen, lr_state_t *from, lr_state_t *to) {
  /* Merge all inbound transitions to 'from' into 'to'. */
  lr_transition_t *t, *t2;
  for (t = from->transitions_to_state; t; t = t->to_chain) {
    /* .. lr_find_or_create_transition checks for dupes. */
    t2 = lr_find_or_create_transition(gen, t->from, t->sym, to);
    if (!t2) return -1;
  }
  return 0;
}

static int lr_compute_lr0_set(lr_generator_t *gen, lr_state_t *initial_state) {
  int next_index = 1;
  lr_state_t *s;

  while (gen->new_states) {
    lr_state_t *dup;
    s = gen->new_states;
    gen->new_states = s->gen_chain;
    dup = lr_find_identical_state(gen, s);

    if (dup) {
      /* Merge s into dup. */
      /* s won't yet (luckily) have any outbound transitions, 
       * but one or more inbound ones, as well as backlinks 
       * and lookbacks on its items. */
      if (lr_merge_states(gen, s, dup)) {
        return -1;
      }
      lr_destroy_state(gen, s);
    }
    else {
      /* No duplicate, push onto mature states list and 
       * compute closure. */
      s->gen_chain = gen->states;
      gen->states = s;
      gen->nr_states++;
      if (lr_closure(gen, s)) {
        return -1;
      }
    }
  }

  /* Assign rows; makes debugging easier to do it now than to wait until parse table generation.*/
  for (s = gen->states; s; s = s->gen_chain) {
    if (s == initial_state) {
      s->row = 0;
    }
    else {
      s->row = next_index++;
    }
  }
  return 0;
}

static void lr_populate_directly_reads(lr_generator_t *gen, lr_state_t *initial_state) {
  lr_state_t *s;
  lr_transition_t *ts;
  for (s = gen->states; s; s = s->gen_chain) {
    lr_transition_t *t1st, *totr;
    /* First populate a single transition, then copy its directly-read
     * terminals into any other transitions. */
    t1st = s->transitions_to_state;
    if (t1st) {
      if ((t1st->sym >= gen->lowest_nonterm) && (t1st->sym <= gen->highest_nonterm)) {
        lr_transition_t *tout;
        memset(t1st->read_set, 0, (size_t)(gen->highest_term - gen->lowest_term + 1 + 7) / 8);
        
        for (tout = s->transitions_from_state; tout; tout = tout->from_chain) {
          if ((tout->sym < gen->lowest_nonterm) || (tout->sym > gen->highest_nonterm)) {
            int term_offset = tout->sym - gen->lowest_term;
            t1st->read_set[term_offset / 8] |= (1 << (term_offset & 7));
          }
        }

        /* now locate any subsequent non-terminal transitions to state s and copy 
         * the findings for the first non-terminal transition into them. */
        for (totr = t1st->to_chain; totr; totr = totr->to_chain) {
          if ((totr->sym >= gen->lowest_nonterm) && (totr->sym <= gen->highest_nonterm)) {
            memcpy(totr->read_set, t1st->read_set, (size_t)(gen->highest_term - gen->lowest_term + 1 + 7) / 8);
          }
        }
      }
    }
  }
  /* [S' -> .S, S] also directly reads EOF */
  for (ts = initial_state->transitions_from_state; ts; ts = ts->from_chain) {
    /* if sym matches S (first sym in root production) then we have found [S' -> .S, S] */
    if (ts->sym == gen->root_production[1]) {
      int EOF_offset = gen->eof_sym - gen->lowest_term;
      ts->read_set[EOF_offset / 8] |= (1 << (EOF_offset & 7));
      break;
    }
  }
}

static int lr_populate_reads_relations(lr_generator_t *gen) {
  /* Find all nullable non-terminal transitions, any preceeding non-terminal
   * transition is said to 'read' that non-terminal transition. */
  lr_state_t *s;
  for (s = gen->states; s; s = s->gen_chain) {
    lr_transition_t *outbound_t;
    for (outbound_t = s->transitions_from_state; outbound_t; outbound_t = outbound_t->from_chain) {
      if ((outbound_t->sym >= gen->lowest_nonterm) && (outbound_t->sym <= gen->highest_nonterm)) {
        if (gen->nonterm_is_nullable[outbound_t->sym - gen->lowest_nonterm]) {
          /* Found a nullable non-terminal that is outbound from s.
           * establish x reads outbound_t, where x is any inbound non-terminal */
          lr_transition_t *inbound_t;
          for (inbound_t = s->transitions_to_state; inbound_t; inbound_t = inbound_t->to_chain) {
            if ((inbound_t->sym >= gen->lowest_nonterm) && (inbound_t->sym <= gen->highest_nonterm)) {
              /* inbound_t 'reads' outbound_t. */
              lr_rel_t *reads_rel = (lr_rel_t *)malloc(sizeof(lr_rel_t));
              if (!reads_rel) return -1;
              reads_rel->from = inbound_t;
              reads_rel->to = outbound_t;
              reads_rel->from_chain = inbound_t->outbound_rels;
              reads_rel->to_chain = outbound_t->inbound_rels;
              inbound_t->outbound_rels = reads_rel;
              outbound_t->inbound_rels = reads_rel;
            }
          }
        }
      }
    }
  }
  return 0;
}

/* Returns -1 if A looses by caller's directive,
 *          0 if the caller has not specified this (conflict will be logged.)
 *         +1 if A wins by caller's directive.
 *         -2 if the caller has not specified this, but no memory was available to log
 */
static int lr_check_conflict(lr_generator_t *gen, lr_conflict_pair_t *cf) {
  lr_conflict_pair_t *cfd;
  lr_conflict_pair_t *new_conflict;
  /* It's not a conflict if both are the same reduction */
  if ((cf->production_a == cf->production_b) &&
      (cf->position_a == cf->position_b)) {
    /* Resolve it by keeping things as-is */
    return -1;
  }
  for (cfd = gen->conflict_resolutions; cfd; cfd = cfd->chain) {
    if ((cfd->production_a == cf->production_a) &&
        (cfd->production_b == cf->production_b) &&
        (cfd->position_a == cf->position_a) &&
        (cfd->position_b == cf->position_b)) {
      /* A wins, B looses. */
      return 1;
    }
    else if ((cfd->production_b == cf->production_a) &&
             (cfd->production_a == cf->production_b) &&
             (cfd->position_b == cf->position_a) &&
             (cfd->position_a == cf->position_b)) {
      /* B wins, A looses. */
      return -1;
    }
  }
  /* Caller has not specified.. log conflict; but only if we haven't already */
  for (cfd = gen->conflicts; cfd; cfd = cfd->chain) {
    if ((cfd->production_a == cf->production_a) &&
        (cfd->production_b == cf->production_b) &&
        (cfd->position_a == cf->position_a) &&
        (cfd->position_b == cf->position_b)) {
      /* Conflict already logged. */
      return 0;
    }
    else if ((cfd->production_b == cf->production_a) &&
             (cfd->production_a == cf->production_b) &&
             (cfd->position_b == cf->position_a) &&
             (cfd->position_a == cf->position_b)) {
      /* Conflict already logged (the other way around) */
      return 0;
    }
  }
  new_conflict = (lr_conflict_pair_t *)malloc(sizeof(lr_conflict_pair_t));
  if (!new_conflict) return -2;
  new_conflict->position_a = cf->position_a;
  new_conflict->position_b = cf->position_b;
  new_conflict->production_a = cf->production_a;
  new_conflict->production_b = cf->production_b;
  new_conflict->sym = cf->sym;
  new_conflict->chain = gen->conflicts;
  gen->conflicts = new_conflict;
  return 0;
}

static int lr_gen_reduce_from_transition(lr_generator_t *gen, lr_state_t *red_state, lr_transition_t *backtrack, int production) {
  int *row = gen->parse_table + (1 + gen->max_sym - gen->min_sym) * red_state->row;
  int lookahead_ix;
  /* Populate all lookaheads for the reduction.. */
  for (lookahead_ix = 0;
       lookahead_ix < (1 + gen->highest_term - gen->lowest_term);
       lookahead_ix++) {
    int lookahead_sym = lookahead_ix + gen->lowest_term;
    int lookahead_offset = lookahead_ix / 8;
    int lookahead_bit = lookahead_ix & 7;
    if (backtrack->read_set[lookahead_offset] & (1 << lookahead_bit)) {
      /* Reductions are the negative index of the production, minus 1. Consequently, accept
       * is -1 (as it is a reduction of production 0.) */
      if (row[lookahead_sym - gen->min_sym]) {
        /* Error, conflict */
        lr_conflict_pair_t conflict;

        conflict.position_a = gen->production_lengths[production];
        conflict.production_a = production;
        conflict.sym = lookahead_sym;

        if (row[lookahead_sym - gen->min_sym] < 0) {
          /* Our reduce conflicts with an existing reduce. */
          conflict.production_b = - 1 - row[lookahead_sym - gen->min_sym];
          conflict.position_b = gen->production_lengths[conflict.production_b];
          switch (lr_check_conflict(gen, &conflict)) {
            case -2: /* out of memory */
              return -1;
            case -1: /* A looses, B wins; keep as is. */
              break;
            case 0: /* Conflict error, and logged. */
              break;
            case 1: /* A wins, B looses; A replaces B. */
              row[lookahead_sym - gen->min_sym] = - 1 - production;
              break;
          }
        }
        else {
          /* Our reduce conflicts with an existing shift.. Find that shift's productions. */
          lr_state_t *destination;
          lr_item_t *i;
          int consensus_resolution = 0;
          int no_consensus = 0;
          for (destination = gen->states; destination; destination = destination->gen_chain) {
            if (destination->row == row[lookahead_sym - gen->min_sym]) {
              break;
            }
          }
          /* destination is the target state; locate all items whose preceeding symbol is the
           * symbol we just shifted. */
          for (i = destination->kernel_items; i; i = i->state_chain) {
            if (lookahead_sym == gen->productions[i->production][i->position + 1 - 1]) {
              /* have a matching conflict, find resolution. */
              conflict.position_b = i->position - 1;
              conflict.production_b = i->production;
              switch (lr_check_conflict(gen, &conflict)) {
                case -2: /* out of memory */
                  return -1;
                case -1: /* A looses, B wins; keep as is. */
                  if (!consensus_resolution) consensus_resolution = -1;
                  if (consensus_resolution != -1) no_consensus = 1;
                  break;
                case 0: /* Conflict error, and logged. */
                  break;
                case 1: /* A wins, B looses; A replaces B. */
                  if (!consensus_resolution) consensus_resolution = 1;
                  if (consensus_resolution != 1) no_consensus = 1;
                  break;
              }
            }
            if (no_consensus) {
              /* Loop again and report all items; caller has specified two conflicting
               * conflict resolutions. */
              for (i = destination->kernel_items; i; i = i->state_chain) {
                if (lookahead_sym == gen->productions[i->production][i->position + 1 - 1]) {
                  lr_conflict_pair_t *cf = (lr_conflict_pair_t *)malloc(sizeof(lr_conflict_pair_t));
                  if (!cf) return -1;
                  cf->position_a = conflict.position_a;
                  cf->production_a = conflict.production_a;
                  cf->position_b = i->position - 1;
                  cf->production_b = i->production;
                  cf->sym = conflict.sym;
                  cf->chain = gen->conflicts;
                  gen->conflicts = cf;
                }
              }
            }
            else {
              /* Found a concensus, caller specified we should reduce */
              if (consensus_resolution == 1) {
                row[lookahead_sym - gen->min_sym] = - 1 - production;
              }
            }
          }
        }
      }
      else {
        row[lookahead_sym - gen->min_sym] = - 1 - production;
      }
    }
  }
  return 0;
}

static int  lr_backtrack_item(lr_generator_t *gen, lr_transition_t *rel_src, lr_state_t *s, int production, int position) {
  lr_transition_t *t;
  /* Backtrack over preceeding symbol, but eliminate post-recursion if possible; note: any bug found here is likely also
   * in lr_gen_item_backtrack_parse below. */
  while (position > 0) {
    int prev_sym = gen->productions[production][position + 1 /* offset for pos */ - 1 /* prev sym */];

    /* Locate all inbound transitions for preceeding symbol; we'll use the first one for our
     * own state (removing post-recursion) and any others for recursion. */
    for (t = s->transitions_to_state; t; t = t->to_chain) {
      if (t->sym == prev_sym) {
        s = t->from;
        position--;
        t = t->to_chain;
        break;
      }
    }

    /* Now continue for recursion (we can handle a single backtrack in this for loop, but if
     * it splits off into multiple branches, other lr_backtrack_item calls will need to handle it). */
    for (; t; t = t->to_chain) {
      if (t->sym == prev_sym) {
        if (lr_backtrack_item(gen, rel_src, t->from, production, position /* already -1'ed */)) {
          /* No memory */
          return -1;
        }
      }
    }
  }

  /* Reached root of item, locate outbound transition that matches item's non-terminal reduction.
   * there will be only one as (moving forward) it is a deterministing finite automaton. */
  for (t = s->transitions_from_state; t; t = t->from_chain) {
    if (t->sym == gen->productions[production][0]) {
      /* t is the outbound transition; rel_src 'includes' t */
      lr_rel_t *includes_rel = (lr_rel_t *)malloc(sizeof(lr_rel_t));
      if (!includes_rel) return -1;
      includes_rel->from = rel_src;
      includes_rel->to = t;
      includes_rel->from_chain = rel_src->outbound_rels;
      includes_rel->to_chain = t->inbound_rels;
      rel_src->outbound_rels = includes_rel;
      t->inbound_rels = includes_rel;
      return 0;
    }
  }

  return 0;
}

static int lr_gen_item_backtrack_parse(lr_generator_t *gen, lr_state_t *reduce_state, lr_state_t *s, int production, int position) {
  lr_transition_t *t;
  
  /* The algorithm for backtracking works the same as lr_backtrack_item */
  while (position > 0) {
    int prev_sym = gen->productions[production][position + 1 - 1];

    for (t = s->transitions_to_state; t; t = t->to_chain) {
      if (t->sym == prev_sym) {
        s = t->from;
        position--;
        t = t->to_chain;
        break;
      }
    }
    for (; t; t = t->to_chain) {
      if (t->sym == prev_sym) {
        if (lr_gen_item_backtrack_parse(gen, reduce_state, t->from, production, position)) {
          return -1;
        }
      }
    }
  }

  /* Reached root of item, locate the outbound transition corresponding to the item's non-terminal reduction. */
  for (t = s->transitions_from_state; t; t = t->from_chain) {
    if (t->sym == gen->productions[production][0]) {
      /* t contains the read set for the reduction of production in reduce_state. */
      if (lr_gen_reduce_from_transition(gen, reduce_state, t, production)) {
        return -1;
      }
    }
  }

  return 0;
}

static int lr_populate_includes_relations(lr_generator_t *gen) {
  /* Find all non-terminal transitions; and for each non-terminal, locate the corresponding items.
   * For each of those items, if all the symbols /after/ the non-terminal transition are nullable,
   * then locate the item's "root transition" - the non-terminal transition that corresponds to the
   * "goto" of the item's reduction. */
  lr_state_t *s;
  for (s = gen->states; s; s = s->gen_chain) {
    lr_transition_t *t;
    for (t = s->transitions_from_state; t; t = t->from_chain) {
      if ((t->sym >= gen->lowest_nonterm) && (t->sym <= gen->highest_nonterm)) {
        /* Locate all corresponding items for this non-terminal transition, the
         * search for them starts from the destination 'to' state of the transition
         * as that is guaranteed to contain a corresponding kernel item; whereas
         * the current state s might have it as an unlisted non-kernel item. */
        lr_item_t *i;
        for (i = t->to->kernel_items; i; i = i->state_chain) {
          if (t->sym == gen->productions[i->production][i->position + 1 - 1 /* preceeding sym */]) {
            /* Verify that all subsequent symbols in the item are nullable. */
            int n;
            int item_immediately_reducable = 1;
            for (n = i->position + 1; gen->productions[i->production][n] != gen->eop_sym; ++n) {
              int tailsym = gen->productions[i->production][n];
              if ((tailsym >= gen->lowest_nonterm) && (tailsym <= gen->highest_nonterm)) {
                if (gen->nonterm_is_nullable[tailsym - gen->lowest_nonterm]) {
                  /* nullable non-terminal, this is still good. */
                  continue;
                }
              }
              /* either a terminal symbol, or non-nullable non-terminal symbol, neither of which
               * allows us to immediately reduce the production item at the current state. 
               * Therefore, t does not "include" the item's root transition. */
              item_immediately_reducable = 0;
              break;
            }
            if (item_immediately_reducable) {
              /* Found an item that can immediately reduce, now backtrack the item to find its
               * root transition(s) and establish that t "includes" that/those root transition(s).
               */
              if (lr_backtrack_item(gen, t, s, i->production, i->position - 1)) {
                return -1;
              }
            }
          }
        }
      }
    }
  }
  return 0;
}

static void lr_clear_relations(lr_generator_t *gen) {
  /* We clear the relations from the 'from' side, and null it at the 'to' side. */
  lr_state_t *s;
  for (s = gen->states; s; s = s->gen_chain) {
    lr_transition_t *t;
    for (t = s->transitions_from_state; t; t = t->from_chain) {
      lr_rel_t *r, *next_r;
      for (r = t->outbound_rels; r; r = next_r) {
        next_r = r->from_chain;
        free(r);
      }
      t->inbound_rels = NULL;
      t->outbound_rels = NULL;
      t->stack_chain = NULL;
      t->index = t->lowlink = 0;
    }
  }
  gen->index = 1;
  gen->stack = NULL;
}

/* Returns:
 * 0 - no SCC's
 * 1 - SCC's found, but only with empty read_set's
 * 2 - SCC's found, one or more with non-empty read_sets */
static int lr_propagate_rel_tarjan(lr_generator_t *gen, lr_transition_t *t) {
  int b;
  int rv = 0;
  lr_rel_t *r;
  t->index = gen->index;
  t->lowlink = gen->index;
  gen->index++;
  if (gen->stack) {
    /* Grow at head of existing stack; gen->stack is a tail pointer to a cyclic list */
    t->stack_chain = gen->stack->stack_chain;
    gen->stack->stack_chain = t;
  }
  else {
    t->stack_chain = t;
    gen->stack = t;
  }
  for (r = t->outbound_rels; r; r = r->from_chain) {
    if (!r->to->index) {
      /* index is undefined. */
      rv = lr_propagate_rel_tarjan(gen, r->to);
      if (t->lowlink > r->to->lowlink) t->lowlink = r->to->lowlink;
    }
    else if (r->to->stack_chain) {
      /* you might be wondering why t->lowlink = min(t->lowlink, r->to->index),
       * rather than min(t->lowlink, r->to->lowlink) -- my belief is that, if it
       * is in the stack, the index, if lower, is sufficient to prevent considering
       * t as an SCC (and allow fair consideration of r->to); the goal is not to 
       * ensure all nodes in an SCC to have the same lowlink. The wikipedia article about
       * the algorithm is confusing about this and I've not been able to get my hands
       * on the original article. (Wikipedia states: "each node is assigned a value v.lowlink 
       * that satisfies v.lowlink := min {v'.index: v' is reachable from v}"-- which would
       * strictly speaking not be true if r->to->lowlink is lower than r->to->index because
       * it can reach yet an earlier node which would, then, be reachable from t.) */
      if (t->lowlink > r->to->index) t->lowlink = r->to->index; 
    }
    /* OR the read_set into t */
    for (b = 0; b < (gen->highest_term - gen->lowest_term + 1 + 7) / 8; ++b) {
      t->read_set[b] |= r->to->read_set[b];
    }
  }
  if (t->lowlink == t->index) {
    lr_transition_t *popt;
    int found_nonempty = 0, found_SCC = 0;
    /* OR all masks into t first, then later we'll copy t's mask into all others. */
    for (popt = gen->stack->stack_chain; popt != t; popt = popt->stack_chain) {
      /* SCC is "non-empty" if the read sets are not identical. */
      for (b = 0; b < (gen->highest_term - gen->lowest_term + 1 + 7) / 8; ++b) {
        found_nonempty = found_nonempty || (t->read_set[b] != popt->read_set[b]);
        t->read_set[b] |= popt->read_set[b];
      }
      found_SCC = 1;
    }
    do {
      /* Pop the head off the cyclic list (which starts with a tail pointer.) and
       * copy t's mask into all others.. */
      popt = gen->stack->stack_chain;
      gen->stack->stack_chain = popt->stack_chain;
      if (gen->stack == popt) gen->stack = NULL;
      
      if (popt != t) {
        memcpy(popt->read_set, t->read_set, (gen->highest_term - gen->lowest_term + 1 + 7) / 8);
      }
    } while (popt != t);
    if (found_SCC) {
      /* only promote from 1 (empty SCC's found) to 2 (also non-empty SCC's found) */
      if (rv != 2) { rv = (found_nonempty ? 2 : 1); }
    }
  }
  return rv;
}

/* Return-value is same as for lr_propagate_rel_tarjan. */
static int lr_propagate_relations(lr_generator_t *gen) {
  lr_state_t *s;
  int r = 0;
  for (s = gen->states; s; s = s->gen_chain) {
    lr_transition_t *t;
    for (t = s->transitions_from_state; t; t = t->from_chain) {
      if (!t->index) {
        int rv = lr_propagate_rel_tarjan(gen, t);
        if (rv > r) r = rv;
      }
    }
  }
  return r;
}

void lr_cleanup(lr_generator_t *gen) {
  free(gen->productions);
  free(gen->production_lengths);

  while (gen->new_states) {
    lr_state_t *s = gen->new_states;
    gen->new_states = s->gen_chain;
    lr_destroy_state(gen, s);
  }
  while (gen->states) {
    lr_state_t *s = gen->states;
    gen->states = s->gen_chain;
    gen->nr_states--; /* sanity check */
    lr_destroy_state(gen, s);
  }
  free(gen->nonterm_is_nullable);
  free(gen->nonterm_scratchpad);
  free(gen->parse_table);
  while (gen->conflicts) {
    lr_conflict_pair_t *cp = gen->conflicts;
    gen->conflicts = cp->chain;
    free(cp);
  }
  while (gen->conflict_resolutions) {
    lr_conflict_pair_t *cp = gen->conflict_resolutions;
    gen->conflict_resolutions = cp->chain;
    free(cp);
  }
}

static int lr_generate_parse_table(lr_generator_t *gen) {
  int nr_symbols;
  lr_state_t *s;
  size_t pix;

  if (gen->max_sym < gen->min_sym) return -2;
  nr_symbols = gen->max_sym - gen->min_sym;
  if ((nr_symbols < 0) || ((nr_symbols + 1) < 0)) {
    return -2;
  }
  nr_symbols += 1;
  if ((SIZE_MAX / (size_t)nr_symbols) < gen->nr_states) {
    return -2;
  }
  size_t num_cells = ((size_t)nr_symbols) * ((size_t)gen->nr_states);
  if ((SIZE_MAX / sizeof(int)) < num_cells) {
    return -2;
  }
  gen->parse_table = (int *)malloc(sizeof(int) * num_cells);
  if (!gen->parse_table) {
    return -1;
  }
  memset(gen->parse_table, 0, sizeof(int) * num_cells);
  
  /* Pre-compute those non-terminals that have an empty production; this
   * is not the same as gen->nonterm_is_nullable as that also takes indirect
   * nullability into account; which we're not interested in here.
   * Used later on to find non-kernel item reductions of those empty reductions,
   * as they do not appear in s->kernel_items. */
  memset(gen->nonterm_scratchpad, 0, sizeof(int) * (gen->highest_nonterm - gen->lowest_nonterm + 1));

  for (pix = 0; pix < gen->nr_productions; ++pix) {
    if (gen->productions[pix][1] == gen->eop_sym) {
      /* Empty production, mark the production's non-terminal; note that we're relying on
       * production 0 to never be null (it is the synthetic S' -> S )*/
      gen->nonterm_scratchpad[gen->productions[pix][0] - gen->lowest_nonterm] = (int)pix;
    }
  }

  /* Assign actions.. */
  for (s = gen->states; s; s = s->gen_chain) {
    lr_transition_t *t;
    lr_item_t *i;
    int *row = gen->parse_table + nr_symbols * s->row;
    /* Run through the state's transitions, populate shifts, populate null reductions. */
    for (t = s->transitions_from_state; t; t = t->from_chain) {
      /* for each non-terminal
       * transition Q, determine if it is directly reducable (has a null production.)
       * If so, then implicitly it's item [Q -> .] exists and we should record a 
       * reduction for all lookaheads in the Q transition. */
      if ((t->sym >= gen->lowest_nonterm) && (t->sym <= gen->highest_nonterm)) {
        /* Non-terminal, we'll populate it's "go to" later on, for now check if it is
         * directly reducable. */
        int empty_prodix = gen->nonterm_scratchpad[t->sym - gen->lowest_nonterm];

        if (empty_prodix) {
          /* Populate all lookaheads for the reduction.. */
          if (lr_gen_reduce_from_transition(gen, s, t, empty_prodix)) {
            return -1;
          }
        }
      }
      /* Populate either goto for a non-terminal or the shift for a terminal - note that these are, in fact, 
       * the same thing */
      if (row[t->sym - gen->min_sym]) {
        /* Shift-reduce conflict.. note that it can never be a "shift shift" conflict because the transition's
         * symbols are unique for each given state. Locate all the conflicting items.. */
        lr_item_t *i;
        lr_conflict_pair_t cp;
        int consensus_resolution = 0;
        int no_consensus = 0;
        cp.position_b = gen->production_lengths[- 1 - row[t->sym - gen->min_sym]];
        cp.production_b = - 1 - row[t->sym - gen->min_sym];
        cp.sym = t->sym;
        for (i = t->to->kernel_items; i; i = i->state_chain) {
          if ((i->position > 0) && (gen->productions[i->production][i->position + 1 - 1] == t->sym)) {
            cp.position_a = i->position - 1;
            cp.production_a = i->production;
            switch (lr_check_conflict(gen, &cp)) {
              case -2: /* no memory */
                return -1;
              case -1: /* B resolves over A. */
                if (!consensus_resolution) consensus_resolution = -1;
                if (consensus_resolution != -1) no_consensus = 1;
                break;
              case 0: /* Conflict not resolved, and logged. */
                break;
              case 1: /* A resolves over B, keep as is. */
                if (!consensus_resolution) consensus_resolution = 1;
                if (consensus_resolution != 1) no_consensus = 1;
                break;
            }
          }
        }
        if (no_consensus) {
          /* Report all items as conflicting.. */
          for (i = t->to->kernel_items; i; i = i->state_chain) {
            if ((i->position > 0) && (gen->productions[i->production][i->position + 1 - 1] == t->sym)) {
              lr_conflict_pair_t *cf = (lr_conflict_pair_t *)malloc(sizeof(lr_conflict_pair_t));
              if (!cf) return -1;
              cf->position_a = i->position - 1;
              cf->production_a = i->production;
              cf->position_b = cp.position_b;
              cf->production_b = cp.production_b;
              cf->sym = t->sym;
              cf->chain = gen->conflicts;
              gen->conflicts = cf->chain;
            }
          }
        } else if (consensus_resolution == 1) {
          /* Shift wins.. */
          row[t->sym - gen->min_sym] = t->to->row;
        }
      }
      else {
        /* Populate the shift/goto, encoded as the index row. */
        row[t->sym - gen->min_sym] = t->to->row;
      }
    }

    /* Run through the state's items looking for items that are ready to reduce, and for each, locate and store their
     * lookaheads. */
    for (i = s->kernel_items; i; i = i->state_chain) {
      if (gen->productions[i->production][i->position + 1] == gen->eop_sym) {
        if (i->production != 0 /* is not the root production S'->S*/) {
          /* Reducable item. Locate all transitions that encode the "goto" (non-terminal) for this item and use their
           * read-sets for the reductions lookahead. */
          if (lr_gen_item_backtrack_parse(gen, s, s, i->production, i->position)) {
            return -1;
          }
        }
        else {
          /* Root production, encode the accept.
           * [S' -> S., EOF] should be accept. */
          if (row[gen->eof_sym - gen->min_sym]) {
            /* Accept-reduce conflict ?? */
            lr_conflict_pair_t cp;
            cp.position_a = i->position;
            cp.production_a = i->production;
            cp.position_b = gen->production_lengths[- 1 - row[gen->eof_sym - gen->min_sym]];
            cp.production_b = - 1 - row[gen->eof_sym - gen->min_sym];
            int r = lr_check_conflict(gen, &cp);
            if (r == -2) {
              /* No memory */
              return -1;
            }
            if (r == 1) {
              /* A passes; encode accept (-1) */
              row[gen->eof_sym - gen->min_sym] = -1;
            }
          }
          else {
            /* -1 encodes Accept. */
            row[gen->eof_sym - gen->min_sym] = -1;
          }
        }
      }
    }
  }
  return 0;
}

void lr_init(lr_generator_t *gen) {
  memset(gen, 0, sizeof(lr_generator_t));
}

int lr_add_conflict_resolution(lr_generator_t *gen,
                               int dominant_production, int dominant_position,
                               int yielding_production, int yielding_position) {
  lr_conflict_pair_t *cp = (lr_conflict_pair_t *)malloc(sizeof(lr_conflict_pair_t));
  if (!cp) return -1;
  cp->production_a = dominant_production; cp->position_a = dominant_position;
  cp->production_b = yielding_production; cp->position_b = yielding_position;
  cp->sym = 0;
  cp->chain = gen->conflict_resolutions;
  gen->conflict_resolutions = cp;
  return 0;
}

lr_error_t lr_gen_parser(lr_generator_t *gen, int *productions,
                         int end_of_production_sym, int end_of_grammar_sym,
                         int end_of_file_sym, int synthetic_s_sym) {
  int *pval = productions;
  int *plen;
  int **p1st;
  int min_nonterm, max_nonterm;
  int min_term, max_term;
  size_t prodix;
  int found_nullable;
  lr_state_t *initial_state;

  gen->eof_sym = end_of_file_sym;
  gen->eog_sym = end_of_grammar_sym;
  gen->eop_sym = end_of_production_sym;
  gen->synths_sym = synthetic_s_sym;

  /* Count overall number of productions (== number of gen->eop_sym's) */
  gen->nr_productions = 0;
  while (*pval != gen->eog_sym) {
    if (*pval++ == gen->eop_sym) {
      gen->nr_productions++;
    }
  }

  /* Count the root production as an extra.. */
  gen->nr_productions++;

  void *p;
  p = realloc(gen->productions, sizeof(int *) * gen->nr_productions);
  if (!p) {
    return LR_INTERNAL_ERROR;
  }
  gen->productions = (int **)p;

  p = realloc(gen->production_lengths, sizeof(int) * gen->nr_productions);
  if (!p) {
    return LR_INTERNAL_ERROR;
  }
  gen->production_lengths = (int *)p;

  /* Now fill in the productions. */
  pval = productions;
  p1st = gen->productions;

  /* Fill in the root production.. */
  *p1st++ = gen->root_production;
  if (gen->nr_productions != 1) {
    gen->root_production[0] = gen->synths_sym;
    gen->root_production[1] = *pval;
    gen->root_production[2] = gen->eop_sym;
    gen->production_lengths[0] = 1;
  }
  else /* (gen->nr_productions == 1) */ {
    /* Root production is isolated and alone, empty grammar accepts only EOF */
    gen->root_production[0] = gen->synths_sym;
    gen->root_production[1] = gen->eop_sym;
    gen->production_lengths[0] = 0;
  }

  min_nonterm = gen->synths_sym;
  max_nonterm = gen->synths_sym;
  min_term = gen->eof_sym;
  max_term = gen->eof_sym;
  plen = gen->production_lengths + 1 /* skip root */;
  /* first run populates productions and production lengths */
  while (*pval != gen->eog_sym) {
    if (*pval < min_nonterm) min_nonterm = *pval;
    if (*pval > max_nonterm) max_nonterm = *pval;
    *p1st++ = pval++;
    *plen = 0;
    while (*pval++ != gen->eop_sym) (*plen)++;
    plen++;
  }
  gen->lowest_nonterm = min_nonterm;
  gen->highest_nonterm = max_nonterm;
  if (gen->highest_nonterm < gen->lowest_nonterm) {
    return LR_INTERNAL_ERROR;
  }
  int num_nonterms = gen->highest_nonterm - gen->lowest_nonterm;
  if ((num_nonterms < 0) || ((num_nonterms + 1) < 0)) {
    return LR_INTERNAL_ERROR;
  }
  num_nonterms += 1;
  if ((SIZE_MAX / sizeof(int)) < num_nonterms) {
    return LR_INTERNAL_ERROR;
  }
  p = realloc(gen->nonterm_is_nullable, sizeof(char) * (size_t)num_nonterms);
  if (!p) {
    return LR_INTERNAL_ERROR;
  }
  gen->nonterm_is_nullable = (char *)p;
  p = realloc(gen->nonterm_scratchpad, sizeof(int) * (size_t)num_nonterms);
  if (!p) {
    return LR_INTERNAL_ERROR;
  }
  gen->nonterm_scratchpad = (int *)p;
  memset(gen->nonterm_is_nullable, 0, sizeof(char) * (size_t)num_nonterms);

  /* second run determines min and max terminals */
  for (prodix = 0; prodix < gen->nr_productions; ++prodix) {
    int idx;
    for (idx = 1; gen->productions[prodix][idx] != gen->eop_sym; ++idx) {
      int sym = gen->productions[prodix][idx];
      if ((sym < gen->lowest_nonterm) || (sym > gen->highest_nonterm)) {
        if (sym < min_term) min_term = sym;
        if (sym > max_term) max_term = sym;
      }
    }
  }
  gen->highest_term = max_term;
  gen->lowest_term = min_term;

  /* Determine overal min and max symbol */
  if (gen->lowest_nonterm < gen->lowest_term) {
    gen->min_sym = gen->lowest_nonterm;
  }
  else {
    gen->min_sym = gen->lowest_term;
  }
  if (gen->highest_nonterm > gen->highest_term) {
    gen->max_sym = gen->highest_nonterm;
  }
  else {
    gen->max_sym = gen->highest_term;
  }

  /* Determine which non-terminals are nullable */
  do {
    /* keep looking for nullable non-terminals as long as we find em. */
    found_nullable = 0;
    for (prodix = 0; prodix < gen->nr_productions; ++prodix) {
      int *p = gen->productions[prodix];
      /* non-terminal is nullable if it has a production completely void
       * of terminals and non-nullable non-terminals. */
      int n, found_non_nullable = 0;
      if (!gen->nonterm_is_nullable[p[0] - gen->lowest_nonterm]) {
        /* Might still be nullable. */
        for (n = 1; (p[n] != gen->eop_sym) && !found_non_nullable; ++n) {
          if ((p[n] >= gen->lowest_nonterm) && (p[n] <= gen->highest_nonterm)) {
            /* p[n] is a nonterminal */
            if (gen->nonterm_is_nullable[p[n] - gen->lowest_nonterm]) {
              /* nullable; continue */
            }
            else {
              /* non-nullable; this production is not nullable. */
              found_non_nullable = 1;
            }
          }
          else {
            /* terminal; this production is not nullable. */
            found_non_nullable = 1;
          }
        }
        if (!found_non_nullable) {
          /* Mark as nullable */
          gen->nonterm_is_nullable[p[0] - gen->lowest_nonterm] = 1;
          found_nullable = 1;
        }
      }
    }
  } while (found_nullable);

  /* Compute lr0 states; start with the initial state; note that lr_create_state places the state 
   * on gen->new_states, which is how lr_compute_lr0_set() automatically picks it up. */
  initial_state = lr_create_state(gen);
  if (!initial_state) {
    return LR_INTERNAL_ERROR;
  }

  /* Initial state consists of S' -> .S */
  lr_item_t *lrit = lr_find_or_create_item(gen, initial_state, 0, 0);
  if (!lrit) {
    return LR_INTERNAL_ERROR;
  }

  /* Compute the set.. Initial_state is passed for row numbering. */
  if (lr_compute_lr0_set(gen, initial_state)) {
    return LR_INTERNAL_ERROR;
  }

  /* Populate directly-reads.. Needs the initial state to indicate [S' -> .S, S] directly reads EOF */
  lr_populate_directly_reads(gen, initial_state);

  /* Find reads-relation */
  if (lr_populate_reads_relations(gen)) {
    return LR_INTERNAL_ERROR;
  }

  /* Propagate the reads-relation */
  if (lr_propagate_relations(gen)) {
    /* SCC's found in read-relation, this is not an LR(n) grammar for any n */
    return LR_NOT_LR_GRAMMAR;
  }

  lr_clear_relations(gen);

  /* Find includes-relation */
  if (lr_populate_includes_relations(gen)) {
    return LR_INTERNAL_ERROR;
  }

  /* Propagate the includes-relation */
  if (2 == lr_propagate_relations(gen)) {
    /* Non-empty SCC's found in includes-relation, this is an ambiguous grammar. */
    return LR_AMBIGUOUS_GRAMMAR;
  }

  lr_clear_relations(gen);

  /* Generate the parse table. */
  if (lr_generate_parse_table(gen)) {
    return LR_INTERNAL_ERROR;
  }

  return gen->conflicts ? LR_CONFLICTS : LR_OK;
}

