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
 
#ifndef STDLIB_H_INCLUDED
#define STDLIB_H_INCLUDED
#include <stdlib.h> /* malloc(), free() */
#endif

#ifndef STRING_H_INCLUDED
#define STRING_H_INCLUDED
#include <string.h> /* memset(), memcpy() */
#endif

#ifndef LALR_H_INCLUDED
#define LALR_H_INCLUDED
#include "lalr.h"
#endif

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
 *    visible from A if BCD are nullable; consequently we model it in our code using struct lr_rel -- a 
 *    relationship between two transitions.
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

static struct lr_state *lr_create_state(struct lr_generator *gen) {
  struct lr_state *s = (struct lr_state *)malloc(sizeof(struct lr_state));
  if (!s) return NULL;
  s->kernel_items_ = NULL;
  s->transitions_from_state_ = NULL;
  s->transitions_to_state_ = NULL;
  s->gen_chain_ = gen->new_states_;
  gen->new_states_ = s;
  return s;
}

static struct lr_item *lr_find_or_create_item(struct lr_generator *gen, struct lr_state *state, int production, int position) {
  struct lr_item **pi, *next_i;
  gen; /* unused var */
  for (pi = &state->kernel_items_; *pi; pi = &((*pi)->state_chain_)) {
    if (((*pi)->position_ == position) && ((*pi)->production_ == production)) {
      /* have it, return. */
      return *pi;
    }
    else if ((((*pi)->production_ == production) && ((*pi)->position_ > position)) ||
             ((*pi)->production_ > production)) {
      /* ascending order indicates we don't have it, insert here. */
      break;
    }
  }
  next_i = *pi;
  *pi = (struct lr_item *)malloc(sizeof(struct lr_item));
  if (!pi) {
    return NULL;
  }
  (*pi)->production_ = production;
  (*pi)->position_ = position;
  (*pi)->state_chain_ = next_i;
  return *pi;
}

/* NOTE: Always creates a new state on the other end of it. */
static struct lr_transition *lr_find_or_create_outbound_transition(struct lr_generator *gen, struct lr_state *from, int sym) {
  struct lr_transition *t;
  /* first look, if not found, then create new outbound transition *AND* state */
  for (t = from->transitions_from_state_; t; t = t->from_chain_) {
    if (t->sym_ == sym) {
      return t;
    }
  }
  t = (struct lr_transition *)malloc(sizeof(struct lr_transition) + (gen->highest_term_ - gen->lowest_term_ + 1 + 7) / 8);
  if (!t) {
    return NULL;
  }
  t->sym_ = sym;
  t->from_ = from;
  t->to_ = lr_create_state(gen);
  if (!t->to_) {
    return NULL;
  }
  t->from_chain_ = from->transitions_from_state_;
  from->transitions_from_state_ = t;
  t->to_chain_ = NULL;
  t->to_->transitions_to_state_ = t;
  t->inbound_rels_ = NULL;
  t->outbound_rels_ = NULL;
  t->stack_chain_ = NULL;
  t->index_ = t->lowlink_ = 0;
  return t;
}

static struct lr_transition *lr_find_or_create_transition(struct lr_generator *gen, struct lr_state *from, int sym, struct lr_state *to) {
  struct lr_transition *t;
  /* First look for the transition, if not found, create a new one. */
  for (t = from->transitions_from_state_; t; t = t->from_chain_) {
    if ((t->sym_ == sym) && (t->to_ == to)) {
      return t;
    }
  }
  t = (struct lr_transition *)malloc(sizeof(struct lr_transition) + (gen->highest_term_ - gen->lowest_term_ + 1 + 7) / 8);
  if (!t) {
    return NULL;
  }
  t->sym_ = sym;
  t->from_ = from;
  t->from_chain_ = from->transitions_from_state_;
  from->transitions_from_state_ = t;
  t->to_ = to;
  t->to_chain_ = to->transitions_to_state_;
  to->transitions_to_state_ = t;
  t->inbound_rels_ = NULL;
  t->outbound_rels_ = NULL;
  t->stack_chain_ = NULL;
  t->index_ = t->lowlink_ = 0;
  return t;
}

static struct lr_state *lr_find_identical_state(struct lr_generator *gen, struct lr_state *state) {
  struct lr_state *s;
  for (s = gen->states_; s; s = s->gen_chain_) {
    struct lr_item *ia, *ib;
    ia = s->kernel_items_;
    ib = state->kernel_items_;
    while (ia && ib && (ia->production_ == ib->production_) && (ia->position_ == ib->position_)) {
      ia = ia->state_chain_;
      ib = ib->state_chain_;
    }
    if ((!ia) && (!ib)) {
      /* They match */
      return s;
    }
  }
  /* No match found */
  return NULL;
}

static int lr_closure(struct lr_generator *gen, struct lr_state *state) {
  struct lr_item *i;
  for (i = state->kernel_items_; i; i = i->state_chain_) {
    int sym = gen->productions_[i->production_][i->position_ + 1 /* [0] is production reduction sym */ ];
    if (sym == gen->eop_sym_) {
      /* Reduction */
    }
    else if ((sym >= gen->lowest_nonterm_) && (sym <= gen->highest_nonterm_)) {
      size_t prodix;
      int nt_sym;
      int have_unprocessed_nonterminals;

      /* Non-terminal */
      struct lr_transition *t = lr_find_or_create_outbound_transition(gen, state, sym);
      if (!t) return -1;
      struct lr_item *new_item = lr_find_or_create_item(gen, t->to_, i->production_, i->position_ + 1);
      if (!new_item) return -1;

      /* Find non-kernel items for non-terminal transition and add their initial transitions. */
      memset(gen->nonterm_scratchpad_, 0, sizeof(int) * (gen->highest_nonterm_ - gen->lowest_nonterm_ + 1));
      /* Flag non-terminal for processing.. */
      gen->nonterm_scratchpad_[sym - gen->lowest_nonterm_] = 1;
      have_unprocessed_nonterminals = 1;
      while (have_unprocessed_nonterminals) {
        have_unprocessed_nonterminals = 0;
        for (nt_sym = gen->lowest_nonterm_; nt_sym <= gen->highest_nonterm_; nt_sym++) {
          /* Check if non-terminal is to be processed.. */
          if (gen->nonterm_scratchpad_[nt_sym - gen->lowest_nonterm_] == 1) {
            /* Mark non-terminal as processed. */
            gen->nonterm_scratchpad_[nt_sym - gen->lowest_nonterm_] = 2;

            /* Process all first transitions for the non-terminal */
            for (prodix = 0; prodix < gen->nr_productions_; prodix++) {
              if (gen->productions_[prodix][0] == nt_sym) {
                int first_sym = gen->productions_[prodix][1];
                if (first_sym == gen->eop_sym_) {
                  /* null production */
                }
                else {
                  /* Add transition */
                  struct lr_transition *first_t = lr_find_or_create_outbound_transition(gen, state, first_sym);
                  struct lr_item *first_new_item = lr_find_or_create_item(gen, first_t->to_, (int)prodix, 1);
                  first_new_item; /* touch first_new_item so it does not go unreferenced. */

                  if ((first_sym >= gen->lowest_nonterm_) && (first_sym <= gen->highest_nonterm_)) {
                    /* First symbol is a non-terminal, meaning we'll need to process the first symbols
                     * for its productions as well, unless we've already done so. */
                    if (gen->nonterm_scratchpad_[first_sym - gen->lowest_nonterm_] == 0) {
                      gen->nonterm_scratchpad_[first_sym - gen->lowest_nonterm_] = 1;
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
      struct lr_transition *t = lr_find_or_create_outbound_transition(gen, state, sym);
      if (!t) return -1;
      struct lr_item *new_item = lr_find_or_create_item(gen, t->to_, i->production_, i->position_ + 1);
      if (!new_item) return -1;
    }
  }
  return 0;
}

static void lr_destroy_state(struct lr_generator *gen, struct lr_state *moriturus) {
  /* Drop all inbound and outbound transitions.. */
  struct lr_transition *t, *nt;
  struct lr_item *i, *ni;
  /* touch gen so it does not go unreferenced. */
  gen;
  for (t = moriturus->transitions_to_state_; t; t = nt) {
    struct lr_state *ds = t->from_;
    struct lr_transition **pt;
    for (pt = &(ds->transitions_from_state_); *pt; pt = &((*pt)->from_chain_)) {
      if (*pt == t) {
        *pt = t->from_chain_;
        break;
      }
    }
    nt = t->to_chain_;
    free(t);
  }
  for (t = moriturus->transitions_from_state_; t; t = nt) {
    struct lr_state *ds = t->to_;
    struct lr_transition **pt;
    for (pt = &(ds->transitions_to_state_); *pt; pt = &((*pt)->to_chain_)) {
      if (*pt == t) {
        *pt = t->to_chain_;
        break;
      }
    }
    nt = t->from_chain_;
    free(t);
  }

  /* Drop all items */
  for (i = moriturus->kernel_items_; i; i = ni) {
    ni = i->state_chain_;
    free(i);
  }
  free(moriturus);
}

static int lr_merge_states(struct lr_generator *gen, struct lr_state *from, struct lr_state *to) {
  /* Merge all inbound transitions to 'from' into 'to'. */
  struct lr_transition *t, *t2;
  for (t = from->transitions_to_state_; t; t = t->to_chain_) {
    /* .. lr_find_or_create_transition checks for dupes. */
    t2 = lr_find_or_create_transition(gen, t->from_, t->sym_, to);
    if (!t2) return -1;
  }
  return 0;
}

static int lr_compute_lr0_set(struct lr_generator *gen, struct lr_state *initial_state) {
  int next_index = 1;
  struct lr_state *s;

  while (gen->new_states_) {
    struct lr_state *dup;
    s = gen->new_states_;
    gen->new_states_ = s->gen_chain_;
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
      s->gen_chain_ = gen->states_;
      gen->states_ = s;
      gen->nr_states_++;
      if (lr_closure(gen, s)) {
        return -1;
      }
    }
  }

  /* Assign rows; makes debugging easier to do it now than to wait until parse table generation.*/
  for (s = gen->states_; s; s = s->gen_chain_) {
    if (s == initial_state) {
      s->row_ = 0;
    }
    else {
      s->row_ = next_index++;
    }
  }
  return 0;
}

static void lr_populate_directly_reads(struct lr_generator *gen, struct lr_state *initial_state) {
  struct lr_state *s;
  struct lr_transition *ts;
  for (s = gen->states_; s; s = s->gen_chain_) {
    struct lr_transition *t1st, *totr;
    /* First populate a single transition, then copy its directly-read
     * terminals into any other transitions. */
    t1st = s->transitions_to_state_;
    if (t1st) {
      if ((t1st->sym_ >= gen->lowest_nonterm_) && (t1st->sym_ <= gen->highest_nonterm_)) {
        struct lr_transition *tout;
        memset(t1st->read_set_, 0, (size_t)(gen->highest_term_ - gen->lowest_term_ + 1 + 7) / 8);
        
        for (tout = s->transitions_from_state_; tout; tout = tout->from_chain_) {
          if ((tout->sym_ < gen->lowest_nonterm_) || (tout->sym_ > gen->highest_nonterm_)) {
            int term_offset = tout->sym_ - gen->lowest_term_;
            t1st->read_set_[term_offset / 8] |= (1 << (term_offset & 7));
          }
        }

        /* now locate any subsequent non-terminal transitions to state s and copy 
         * the findings for the first non-terminal transition into them. */
        for (totr = t1st->to_chain_; totr; totr = totr->to_chain_) {
          if ((totr->sym_ >= gen->lowest_nonterm_) && (totr->sym_ <= gen->highest_nonterm_)) {
            memcpy(totr->read_set_, t1st->read_set_, (size_t)(gen->highest_term_ - gen->lowest_term_ + 1 + 7) / 8);
          }
        }
      }
    }
  }
  /* [S' -> .S, S] also directly reads EOF */
  for (ts = initial_state->transitions_from_state_; ts; ts = ts->from_chain_) {
    /* if sym matches S (first sym in root production) then we have found [S' -> .S, S] */
    if (ts->sym_ == gen->root_production_[1]) {
      int EOF_offset = gen->eof_sym_ - gen->lowest_term_;
      ts->read_set_[EOF_offset / 8] |= (1 << (EOF_offset & 7));
      break;
    }
  }
}

static int lr_populate_reads_relations(struct lr_generator *gen) {
  /* Find all nullable non-terminal transitions, any preceeding non-terminal
   * transition is said to 'read' that non-terminal transition. */
  struct lr_state *s;
  for (s = gen->states_; s; s = s->gen_chain_) {
    struct lr_transition *outbound_t;
    for (outbound_t = s->transitions_from_state_; outbound_t; outbound_t = outbound_t->from_chain_) {
      if ((outbound_t->sym_ >= gen->lowest_nonterm_) && (outbound_t->sym_ <= gen->highest_nonterm_)) {
        if (gen->nonterm_is_nullable_[outbound_t->sym_ - gen->lowest_nonterm_]) {
          /* Found a nullable non-terminal that is outbound from s.
           * establish x reads outbound_t, where x is any inbound non-terminal */
          struct lr_transition *inbound_t;
          for (inbound_t = s->transitions_to_state_; inbound_t; inbound_t = inbound_t->to_chain_) {
            if ((inbound_t->sym_ >= gen->lowest_nonterm_) && (inbound_t->sym_ <= gen->highest_nonterm_)) {
              /* inbound_t 'reads' outbound_t. */
              struct lr_rel *reads_rel = (struct lr_rel *)malloc(sizeof(struct lr_rel));
              if (!reads_rel) return -1;
              reads_rel->from_ = inbound_t;
              reads_rel->to_ = outbound_t;
              reads_rel->from_chain_ = inbound_t->outbound_rels_;
              reads_rel->to_chain_ = outbound_t->inbound_rels_;
              inbound_t->outbound_rels_ = reads_rel;
              outbound_t->inbound_rels_ = reads_rel;
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
static int lr_check_conflict(struct lr_generator *gen, struct lr_conflict_pair *cf) {
  struct lr_conflict_pair *cfd;
  struct lr_conflict_pair *new_conflict;
  /* It's not a conflict if both are the same reduction */
  if ((cf->production_a_ == cf->production_b_) &&
      (cf->position_a_ == cf->position_b_)) {
    /* Resolve it by keeping things as-is */
    return -1;
  }
  for (cfd = gen->conflict_resolutions_; cfd; cfd = cfd->chain_) {
    if ((cfd->production_a_ == cf->production_a_) &&
        (cfd->production_b_ == cf->production_b_) &&
        (cfd->position_a_ == cf->position_a_) &&
        (cfd->position_b_ == cf->position_b_)) {
      /* A wins, B looses. */
      return 1;
    }
    else if ((cfd->production_b_ == cf->production_a_) &&
             (cfd->production_a_ == cf->production_b_) &&
             (cfd->position_b_ == cf->position_a_) &&
             (cfd->position_a_ == cf->position_b_)) {
      /* B wins, A looses. */
      return -1;
    }
  }
  /* Caller has not specified.. log conflict; but only if we haven't already */
  for (cfd = gen->conflicts_; cfd; cfd = cfd->chain_) {
    if ((cfd->production_a_ == cf->production_a_) &&
        (cfd->production_b_ == cf->production_b_) &&
        (cfd->position_a_ == cf->position_a_) &&
        (cfd->position_b_ == cf->position_b_)) {
      /* Conflict already logged. */
      return 0;
    }
    else if ((cfd->production_b_ == cf->production_a_) &&
             (cfd->production_a_ == cf->production_b_) &&
             (cfd->position_b_ == cf->position_a_) &&
             (cfd->position_a_ == cf->position_b_)) {
      /* Conflict already logged (the other way around) */
      return 0;
    }
  }
  new_conflict = (struct lr_conflict_pair *)malloc(sizeof(struct lr_conflict_pair));
  if (!new_conflict) return -2;
  new_conflict->position_a_ = cf->position_a_;
  new_conflict->position_b_ = cf->position_b_;
  new_conflict->production_a_ = cf->production_a_;
  new_conflict->production_b_ = cf->production_b_;
  new_conflict->sym_ = cf->sym_;
  new_conflict->chain_ = gen->conflicts_;
  gen->conflicts_ = new_conflict;
  return 0;
}

static int lr_gen_reduce_from_transition(struct lr_generator *gen, struct lr_state *red_state, struct lr_transition *backtrack, int production) {
  int *row = gen->parse_table_ + (1 + gen->max_sym_ - gen->min_sym_) * red_state->row_;
  int lookahead_ix;
  /* Populate all lookaheads for the reduction.. */
  for (lookahead_ix = 0;
       lookahead_ix < (1 + gen->highest_term_ - gen->lowest_term_);
       lookahead_ix++) {
    int lookahead_sym = lookahead_ix + gen->lowest_term_;
    int lookahead_offset = lookahead_ix / 8;
    int lookahead_bit = lookahead_ix & 7;
    if (backtrack->read_set_[lookahead_offset] & (1 << lookahead_bit)) {
      /* Reductions are the negative index of the production, minus 1. Consequently, accept
       * is -1 (as it is a reduction of production 0.) */
      if (row[lookahead_sym - gen->min_sym_]) {
        /* Error, conflict */
        struct lr_conflict_pair conflict;

        conflict.position_a_ = gen->production_lengths_[production];
        conflict.production_a_ = production;
        conflict.sym_ = lookahead_sym;

        if (row[lookahead_sym - gen->min_sym_] < 0) {
          /* Our reduce conflicts with an existing reduce. */
          conflict.production_b_ = - 1 - row[lookahead_sym - gen->min_sym_];
          conflict.position_b_ = gen->production_lengths_[conflict.production_b_];
          switch (lr_check_conflict(gen, &conflict)) {
            case -2: /* out of memory */
              return -1;
            case -1: /* A looses, B wins; keep as is. */
              break;
            case 0: /* Conflict error, and logged. */
              break;
            case 1: /* A wins, B looses; A replaces B. */
              row[lookahead_sym - gen->min_sym_] = - 1 - production;
              break;
          }
        }
        else {
          /* Our reduce conflicts with an existing shift.. Find that shift's productions. */
          struct lr_state *destination;
          struct lr_item *i;
          int consensus_resolution = 0;
          int no_consensus = 0;
          for (destination = gen->states_; destination; destination = destination->gen_chain_) {
            if (destination->row_ == row[lookahead_sym - gen->min_sym_]) {
              break;
            }
          }
          /* destination is the target state; locate all items whose preceeding symbol is the
           * symbol we just shifted. */
          for (i = destination->kernel_items_; i; i = i->state_chain_) {
            if (lookahead_sym == gen->productions_[i->production_][i->position_ + 1 - 1]) {
              /* have a matching conflict, find resolution. */
              conflict.position_b_ = i->position_ - 1;
              conflict.production_b_ = i->production_;
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
              for (i = destination->kernel_items_; i; i = i->state_chain_) {
                if (lookahead_sym == gen->productions_[i->production_][i->position_ + 1 - 1]) {
                  struct lr_conflict_pair *cf = (struct lr_conflict_pair *)malloc(sizeof(struct lr_conflict_pair));
                  if (!cf) return -1;
                  cf->position_a_ = conflict.position_a_;
                  cf->production_a_ = conflict.production_a_;
                  cf->position_b_ = i->position_ - 1;
                  cf->production_b_ = i->production_;
                  cf->sym_ = conflict.sym_;
                  cf->chain_ = gen->conflicts_;
                  gen->conflicts_ = cf;
                }
              }
            }
            else {
              /* Found a concensus, caller specified we should reduce */
              if (consensus_resolution == 1) {
                row[lookahead_sym - gen->min_sym_] = - 1 - production;
              }
            }
          }
        }
      }
      else {
        row[lookahead_sym - gen->min_sym_] = - 1 - production;
      }
    }
  }
  return 0;
}

static int  lr_backtrack_item(struct lr_generator *gen, struct lr_transition *rel_src, struct lr_state *s, int production, int position) {
  struct lr_transition *t;
  /* Backtrack over preceeding symbol, but eliminate post-recursion if possible; note: any bug found here is likely also
   * in lr_gen_item_backtrack_parse below. */
  while (position > 0) {
    int prev_sym = gen->productions_[production][position + 1 /* offset for pos */ - 1 /* prev sym */];

    /* Locate all inbound transitions for preceeding symbol; we'll use the first one for our
     * own state (removing post-recursion) and any others for recursion. */
    for (t = s->transitions_to_state_; t; t = t->to_chain_) {
      if (t->sym_ == prev_sym) {
        s = t->from_;
        position--;
        t = t->to_chain_;
        break;
      }
    }

    /* Now continue for recursion (we can handle a single backtrack in this for loop, but if
     * it splits off into multiple branches, other lr_backtrack_item calls will need to handle it). */
    for (; t; t = t->to_chain_) {
      if (t->sym_ == prev_sym) {
        if (lr_backtrack_item(gen, rel_src, t->from_, production, position /* already -1'ed */)) {
          /* No memory */
          return -1;
        }
      }
    }
  }

  /* Reached root of item, locate outbound transition that matches item's non-terminal reduction.
   * there will be only one as (moving forward) it is a deterministing finite automaton. */
  for (t = s->transitions_from_state_; t; t = t->from_chain_) {
    if (t->sym_ == gen->productions_[production][0]) {
      /* t is the outbound transition; rel_src 'includes' t */
      struct lr_rel *includes_rel = (struct lr_rel *)malloc(sizeof(struct lr_rel));
      if (!includes_rel) return -1;
      includes_rel->from_ = rel_src;
      includes_rel->to_ = t;
      includes_rel->from_chain_ = rel_src->outbound_rels_;
      includes_rel->to_chain_ = t->inbound_rels_;
      rel_src->outbound_rels_ = includes_rel;
      t->inbound_rels_ = includes_rel;
      return 0;
    }
  }

  return 0;
}

static int lr_gen_item_backtrack_parse(struct lr_generator *gen, struct lr_state *reduce_state, struct lr_state *s, int production, int position) {
  struct lr_transition *t;
  
  /* The algorithm for backtracking works the same as lr_backtrack_item */
  while (position > 0) {
    int prev_sym = gen->productions_[production][position + 1 - 1];

    for (t = s->transitions_to_state_; t; t = t->to_chain_) {
      if (t->sym_ == prev_sym) {
        s = t->from_;
        position--;
        t = t->to_chain_;
        break;
      }
    }
    for (; t; t = t->to_chain_) {
      if (t->sym_ == prev_sym) {
        if (lr_gen_item_backtrack_parse(gen, reduce_state, t->from_, production, position)) {
          return -1;
        }
      }
    }
  }

  /* Reached root of item, locate the outbound transition corresponding to the item's non-terminal reduction. */
  for (t = s->transitions_from_state_; t; t = t->from_chain_) {
    if (t->sym_ == gen->productions_[production][0]) {
      /* t contains the read set for the reduction of production in reduce_state. */
      if (lr_gen_reduce_from_transition(gen, reduce_state, t, production)) {
        return -1;
      }
    }
  }

  return 0;
}

static int lr_populate_includes_relations(struct lr_generator *gen) {
  /* Find all non-terminal transitions; and for each non-terminal, locate the corresponding items.
   * For each of those items, if all the symbols /after/ the non-terminal transition are nullable,
   * then locate the item's "root transition" - the non-terminal transition that corresponds to the
   * "goto" of the item's reduction. */
  struct lr_state *s;
  for (s = gen->states_; s; s = s->gen_chain_) {
    struct lr_transition *t;
    for (t = s->transitions_from_state_; t; t = t->from_chain_) {
      if ((t->sym_ >= gen->lowest_nonterm_) && (t->sym_ <= gen->highest_nonterm_)) {
        /* Locate all corresponding items for this non-terminal transition, the
         * search for them starts from the destination 'to' state of the transition
         * as that is guaranteed to contain a corresponding kernel item; whereas
         * the current state s might have it as an unlisted non-kernel item. */
        struct lr_item *i;
        for (i = t->to_->kernel_items_; i; i = i->state_chain_) {
          if (t->sym_ == gen->productions_[i->production_][i->position_ + 1 - 1 /* preceeding sym */]) {
            /* Verify that all subsequent symbols in the item are nullable. */
            int n;
            int item_immediately_reducable = 1;
            for (n = i->position_ + 1; gen->productions_[i->production_][n] != gen->eop_sym_; ++n) {
              int tailsym = gen->productions_[i->production_][n];
              if ((tailsym >= gen->lowest_nonterm_) && (tailsym <= gen->highest_nonterm_)) {
                if (gen->nonterm_is_nullable_[tailsym - gen->lowest_nonterm_]) {
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
              if (lr_backtrack_item(gen, t, s, i->production_, i->position_ - 1)) {
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

static void lr_clear_relations(struct lr_generator *gen) {
  /* We clear the relations from the 'from' side, and null it at the 'to' side. */
  struct lr_state *s;
  for (s = gen->states_; s; s = s->gen_chain_) {
    struct lr_transition *t;
    for (t = s->transitions_from_state_; t; t = t->from_chain_) {
      struct lr_rel *r, *next_r;
      for (r = t->outbound_rels_; r; r = next_r) {
        next_r = r->from_chain_;
        free(r);
      }
      t->inbound_rels_ = NULL;
      t->outbound_rels_ = NULL;
      t->stack_chain_ = NULL;
      t->index_ = t->lowlink_ = 0;
    }
  }
  gen->index_ = 1;
  gen->stack_ = NULL;
}

/* Returns:
 * 0 - no SCC's
 * 1 - SCC's found, but only with empty read_set's
 * 2 - SCC's found, one or more with non-empty read_sets */
static int lr_propagate_rel_tarjan(struct lr_generator *gen, struct lr_transition *t) {
  int b;
  int rv = 0;
  struct lr_rel *r;
  t->index_ = gen->index_;
  t->lowlink_ = gen->index_;
  gen->index_++;
  if (gen->stack_) {
    /* Grow at head of existing stack; gen->stack is a tail pointer to a cyclic list */
    t->stack_chain_ = gen->stack_->stack_chain_;
    gen->stack_->stack_chain_ = t;
  }
  else {
    t->stack_chain_ = t;
    gen->stack_ = t;
  }
  for (r = t->outbound_rels_; r; r = r->from_chain_) {
    if (!r->to_->index_) {
      /* index is undefined. */
      rv = lr_propagate_rel_tarjan(gen, r->to_);
      if (t->lowlink_ > r->to_->lowlink_) t->lowlink_ = r->to_->lowlink_;
    }
    else if (r->to_->stack_chain_) {
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
      if (t->lowlink_ > r->to_->index_) t->lowlink_ = r->to_->index_; 
    }
    /* OR the read_set into t */
    for (b = 0; b < (gen->highest_term_ - gen->lowest_term_ + 1 + 7) / 8; ++b) {
      t->read_set_[b] |= r->to_->read_set_[b];
    }
  }
  if (t->lowlink_ == t->index_) {
    struct lr_transition *popt;
    int found_nonempty = 0, found_SCC = 0;
    /* OR all masks into t first, then later we'll copy t's mask into all others. */
    for (popt = gen->stack_->stack_chain_; popt != t; popt = popt->stack_chain_) {
      /* SCC is "non-empty" if the read sets are not identical. */
      for (b = 0; b < (gen->highest_term_ - gen->lowest_term_ + 1 + 7) / 8; ++b) {
        found_nonempty = found_nonempty || (t->read_set_[b] != popt->read_set_[b]);
        t->read_set_[b] |= popt->read_set_[b];
      }
      found_SCC = 1;
    }
    do {
      /* Pop the head off the cyclic list (which starts with a tail pointer.) and
       * copy t's mask into all others.. */
      popt = gen->stack_->stack_chain_;
      gen->stack_->stack_chain_ = popt->stack_chain_;
      if (gen->stack_ == popt) gen->stack_ = NULL;
      
      if (popt != t) {
        memcpy(popt->read_set_, t->read_set_, (gen->highest_term_ - gen->lowest_term_ + 1 + 7) / 8);
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
static int lr_propagate_relations(struct lr_generator *gen) {
  struct lr_state *s;
  int r = 0;
  for (s = gen->states_; s; s = s->gen_chain_) {
    struct lr_transition *t;
    for (t = s->transitions_from_state_; t; t = t->from_chain_) {
      if (!t->index_) {
        int rv = lr_propagate_rel_tarjan(gen, t);
        if (rv > r) r = rv;
      }
    }
  }
  return r;
}

void lr_cleanup(struct lr_generator *gen) {
  free(gen->productions_);
  free(gen->production_lengths_);

  while (gen->new_states_) {
    struct lr_state *s = gen->new_states_;
    gen->new_states_ = s->gen_chain_;
    lr_destroy_state(gen, s);
  }
  while (gen->states_) {
    struct lr_state *s = gen->states_;
    gen->states_ = s->gen_chain_;
    gen->nr_states_--; /* sanity check */
    lr_destroy_state(gen, s);
  }
  free(gen->nonterm_is_nullable_);
  free(gen->nonterm_scratchpad_);
  free(gen->parse_table_);
  while (gen->conflicts_) {
    struct lr_conflict_pair *cp = gen->conflicts_;
    gen->conflicts_ = cp->chain_;
    free(cp);
  }
  while (gen->conflict_resolutions_) {
    struct lr_conflict_pair *cp = gen->conflict_resolutions_;
    gen->conflict_resolutions_ = cp->chain_;
    free(cp);
  }
}

static int lr_generate_parse_table(struct lr_generator *gen) {
  int nr_symbols;
  struct lr_state *s;
  size_t pix;

  if (gen->max_sym_ < gen->min_sym_) return -2;
  nr_symbols = gen->max_sym_ - gen->min_sym_;
  if ((nr_symbols < 0) || ((nr_symbols + 1) < 0)) {
    return -2;
  }
  nr_symbols += 1;
  if ((SIZE_MAX / (size_t)nr_symbols) < gen->nr_states_) {
    return -2;
  }
  size_t num_cells = ((size_t)nr_symbols) * ((size_t)gen->nr_states_);
  if ((SIZE_MAX / sizeof(int)) < num_cells) {
    return -2;
  }
  gen->parse_table_ = (int *)malloc(sizeof(int) * num_cells);
  if (!gen->parse_table_) {
    return -1;
  }
  memset(gen->parse_table_, 0, sizeof(int) * num_cells);
  
  /* Pre-compute those non-terminals that have an empty production; this
   * is not the same as gen->nonterm_is_nullable as that also takes indirect
   * nullability into account; which we're not interested in here.
   * Used later on to find non-kernel item reductions of those empty reductions,
   * as they do not appear in s->kernel_items. */
  memset(gen->nonterm_scratchpad_, 0, sizeof(int) * (gen->highest_nonterm_ - gen->lowest_nonterm_ + 1));

  for (pix = 0; pix < gen->nr_productions_; ++pix) {
    if (gen->productions_[pix][1] == gen->eop_sym_) {
      /* Empty production, mark the production's non-terminal; note that we're relying on
       * production 0 to never be null (it is the synthetic S' -> S )*/
      gen->nonterm_scratchpad_[gen->productions_[pix][0] - gen->lowest_nonterm_] = (int)pix;
    }
  }

  /* Assign actions.. */
  for (s = gen->states_; s; s = s->gen_chain_) {
    struct lr_transition *t;
    struct lr_item *i;
    int *row = gen->parse_table_ + nr_symbols * s->row_;
    /* Run through the state's transitions, populate shifts, populate null reductions. */
    for (t = s->transitions_from_state_; t; t = t->from_chain_) {
      /* for each non-terminal
       * transition Q, determine if it is directly reducable (has a null production.)
       * If so, then implicitly it's item [Q -> .] exists and we should record a 
       * reduction for all lookaheads in the Q transition. */
      if ((t->sym_ >= gen->lowest_nonterm_) && (t->sym_ <= gen->highest_nonterm_)) {
        /* Non-terminal, we'll populate it's "go to" later on, for now check if it is
         * directly reducable. */
        int empty_prodix = gen->nonterm_scratchpad_[t->sym_ - gen->lowest_nonterm_];

        if (empty_prodix) {
          /* Populate all lookaheads for the reduction.. */
          if (lr_gen_reduce_from_transition(gen, s, t, empty_prodix)) {
            return -1;
          }
        }
      }
      /* Populate either goto for a non-terminal or the shift for a terminal - note that these are, in fact, 
       * the same thing */
      if (row[t->sym_ - gen->min_sym_]) {
        /* Shift-reduce conflict.. note that it can never be a "shift shift" conflict because the transition's
         * symbols are unique for each given state. Locate all the conflicting items.. */
        struct lr_item *i;
        struct lr_conflict_pair cp;
        int consensus_resolution = 0;
        int no_consensus = 0;
        cp.position_b_ = gen->production_lengths_[- 1 - row[t->sym_ - gen->min_sym_]];
        cp.production_b_ = - 1 - row[t->sym_ - gen->min_sym_];
        cp.sym_ = t->sym_;
        for (i = t->to_->kernel_items_; i; i = i->state_chain_) {
          if ((i->position_ > 0) && (gen->productions_[i->production_][i->position_ + 1 - 1] == t->sym_)) {
            cp.position_a_ = i->position_ - 1;
            cp.production_a_ = i->production_;
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
          for (i = t->to_->kernel_items_; i; i = i->state_chain_) {
            if ((i->position_ > 0) && (gen->productions_[i->production_][i->position_ + 1 - 1] == t->sym_)) {
              struct lr_conflict_pair *cf = (struct lr_conflict_pair *)malloc(sizeof(struct lr_conflict_pair));
              if (!cf) return -1;
              cf->position_a_ = i->position_ - 1;
              cf->production_a_ = i->production_;
              cf->position_b_ = cp.position_b_;
              cf->production_b_ = cp.production_b_;
              cf->sym_ = t->sym_;
              cf->chain_ = gen->conflicts_;
              gen->conflicts_ = cf->chain_;
            }
          }
        } else if (consensus_resolution == 1) {
          /* Shift wins.. */
          row[t->sym_ - gen->min_sym_] = t->to_->row_;
        }
      }
      else {
        /* Populate the shift/goto, encoded as the index row. */
        row[t->sym_ - gen->min_sym_] = t->to_->row_;
      }
    }

    /* Run through the state's items looking for items that are ready to reduce, and for each, locate and store their
     * lookaheads. */
    for (i = s->kernel_items_; i; i = i->state_chain_) {
      if (gen->productions_[i->production_][i->position_ + 1] == gen->eop_sym_) {
        if (i->production_ != 0 /* is not the root production S'->S*/) {
          /* Reducable item. Locate all transitions that encode the "goto" (non-terminal) for this item and use their
           * read-sets for the reductions lookahead. */
          if (lr_gen_item_backtrack_parse(gen, s, s, i->production_, i->position_)) {
            return -1;
          }
        }
        else {
          /* Root production, encode the accept.
           * [S' -> S., EOF] should be accept. */
          if (row[gen->eof_sym_ - gen->min_sym_]) {
            /* Accept-reduce conflict ?? */
            struct lr_conflict_pair cp;
            cp.position_a_ = i->position_;
            cp.production_a_ = i->production_;
            cp.position_b_ = gen->production_lengths_[- 1 - row[gen->eof_sym_ - gen->min_sym_]];
            cp.production_b_ = - 1 - row[gen->eof_sym_ - gen->min_sym_];
            int r = lr_check_conflict(gen, &cp);
            if (r == -2) {
              /* No memory */
              return -1;
            }
            if (r == 1) {
              /* A passes; encode accept (-1) */
              row[gen->eof_sym_ - gen->min_sym_] = -1;
            }
          }
          else {
            /* -1 encodes Accept. */
            row[gen->eof_sym_ - gen->min_sym_] = -1;
          }
        }
      }
    }
  }
  return 0;
}

void lr_init(struct lr_generator *gen) {
  memset(gen, 0, sizeof(struct lr_generator));
}

int lr_add_conflict_resolution(struct lr_generator *gen,
                               int dominant_production, int dominant_position,
                               int yielding_production, int yielding_position) {
  struct lr_conflict_pair *cp = (struct lr_conflict_pair *)malloc(sizeof(struct lr_conflict_pair));
  if (!cp) return -1;
  cp->production_a_ = dominant_production; cp->position_a_ = dominant_position;
  cp->production_b_ = yielding_production; cp->position_b_ = yielding_position;
  cp->sym_ = 0;
  cp->chain_ = gen->conflict_resolutions_;
  gen->conflict_resolutions_ = cp;
  return 0;
}

lr_error_t lr_gen_parser(struct lr_generator *gen, int *productions,
                         int end_of_production_sym, int end_of_grammar_sym,
                         int end_of_file_sym, int synthetic_s_sym) {
  int *pval = productions;
  int *plen;
  int **p1st;
  int min_nonterm, max_nonterm;
  int min_term, max_term;
  size_t prodix;
  int found_nullable;
  struct lr_state *initial_state;

  gen->eof_sym_ = end_of_file_sym;
  gen->eog_sym_ = end_of_grammar_sym;
  gen->eop_sym_ = end_of_production_sym;
  gen->synths_sym_ = synthetic_s_sym;

  /* Count overall number of productions (== number of gen->eop_sym's) */
  gen->nr_productions_ = 0;
  while (*pval != gen->eog_sym_) {
    if (*pval++ == gen->eop_sym_) {
      gen->nr_productions_++;
    }
  }

  /* Count the root production as an extra.. */
  gen->nr_productions_++;

  void *p;
  p = realloc(gen->productions_, sizeof(int *) * gen->nr_productions_);
  if (!p) {
    return LR_INTERNAL_ERROR;
  }
  gen->productions_ = (int **)p;

  p = realloc(gen->production_lengths_, sizeof(int) * gen->nr_productions_);
  if (!p) {
    return LR_INTERNAL_ERROR;
  }
  gen->production_lengths_ = (int *)p;

  /* Now fill in the productions. */
  pval = productions;
  p1st = gen->productions_;

  /* Fill in the root production.. */
  *p1st++ = gen->root_production_;
  if (gen->nr_productions_ != 1) {
    gen->root_production_[0] = gen->synths_sym_;
    gen->root_production_[1] = *pval;
    gen->root_production_[2] = gen->eop_sym_;
    gen->production_lengths_[0] = 1;
  }
  else /* (gen->nr_productions == 1) */ {
    /* Root production is isolated and alone, empty grammar accepts only EOF */
    gen->root_production_[0] = gen->synths_sym_;
    gen->root_production_[1] = gen->eop_sym_;
    gen->production_lengths_[0] = 0;
  }

  min_nonterm = gen->synths_sym_;
  max_nonterm = gen->synths_sym_;
  min_term = gen->eof_sym_;
  max_term = gen->eof_sym_;
  plen = gen->production_lengths_ + 1 /* skip root */;
  /* first run populates productions and production lengths */
  while (*pval != gen->eog_sym_) {
    if (*pval < min_nonterm) min_nonterm = *pval;
    if (*pval > max_nonterm) max_nonterm = *pval;
    *p1st++ = pval++;
    *plen = 0;
    while (*pval++ != gen->eop_sym_) (*plen)++;
    plen++;
  }
  gen->lowest_nonterm_ = min_nonterm;
  gen->highest_nonterm_ = max_nonterm;
  if (gen->highest_nonterm_ < gen->lowest_nonterm_) {
    return LR_INTERNAL_ERROR;
  }
  int num_nonterms = gen->highest_nonterm_ - gen->lowest_nonterm_;
  if ((num_nonterms < 0) || ((num_nonterms + 1) < 0)) {
    return LR_INTERNAL_ERROR;
  }
  num_nonterms += 1;
  if ((SIZE_MAX / sizeof(int)) < num_nonterms) {
    return LR_INTERNAL_ERROR;
  }
  p = realloc(gen->nonterm_is_nullable_, sizeof(char) * (size_t)num_nonterms);
  if (!p) {
    return LR_INTERNAL_ERROR;
  }
  gen->nonterm_is_nullable_ = (char *)p;
  p = realloc(gen->nonterm_scratchpad_, sizeof(int) * (size_t)num_nonterms);
  if (!p) {
    return LR_INTERNAL_ERROR;
  }
  gen->nonterm_scratchpad_ = (int *)p;
  memset(gen->nonterm_is_nullable_, 0, sizeof(char) * (size_t)num_nonterms);

  /* second run determines min and max terminals */
  for (prodix = 0; prodix < gen->nr_productions_; ++prodix) {
    int idx;
    for (idx = 1; gen->productions_[prodix][idx] != gen->eop_sym_; ++idx) {
      int sym = gen->productions_[prodix][idx];
      if ((sym < gen->lowest_nonterm_) || (sym > gen->highest_nonterm_)) {
        if (sym < min_term) min_term = sym;
        if (sym > max_term) max_term = sym;
      }
    }
  }
  gen->highest_term_ = max_term;
  gen->lowest_term_ = min_term;

  /* Determine overal min and max symbol */
  if (gen->lowest_nonterm_ < gen->lowest_term_) {
    gen->min_sym_ = gen->lowest_nonterm_;
  }
  else {
    gen->min_sym_ = gen->lowest_term_;
  }
  if (gen->highest_nonterm_ > gen->highest_term_) {
    gen->max_sym_ = gen->highest_nonterm_;
  }
  else {
    gen->max_sym_ = gen->highest_term_;
  }

  /* Determine which non-terminals are nullable */
  do {
    /* keep looking for nullable non-terminals as long as we find em. */
    found_nullable = 0;
    for (prodix = 0; prodix < gen->nr_productions_; ++prodix) {
      int *p = gen->productions_[prodix];
      /* non-terminal is nullable if it has a production completely void
       * of terminals and non-nullable non-terminals. */
      int n, found_non_nullable = 0;
      if (!gen->nonterm_is_nullable_[p[0] - gen->lowest_nonterm_]) {
        /* Might still be nullable. */
        for (n = 1; (p[n] != gen->eop_sym_) && !found_non_nullable; ++n) {
          if ((p[n] >= gen->lowest_nonterm_) && (p[n] <= gen->highest_nonterm_)) {
            /* p[n] is a nonterminal */
            if (gen->nonterm_is_nullable_[p[n] - gen->lowest_nonterm_]) {
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
          gen->nonterm_is_nullable_[p[0] - gen->lowest_nonterm_] = 1;
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
  struct lr_item *lrit = lr_find_or_create_item(gen, initial_state, 0, 0);
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

  return gen->conflicts_ ? LR_CONFLICTS : LR_OK;
}

