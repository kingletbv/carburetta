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

#ifndef LALR_H
#define LALR_H

#ifndef STDDEF_H_INCLUDED
#define STDDEF_H_INCLUDED
#include <stddef.h> /* size_t */
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* LALR(1) parser & table generator */

/* Format for passing in:
 * int*; an array of integers.
 * The array is ended using value -2.
 * The array consists of productions, each production is one or more integers, followed by a -1
 * value to mark the end of the production. The last production in the overall array is ended with
 * -1, followed by aforementioned -2 (to indicate no further productions follow.)
 * The first integer of a production is the non-terminal symbol to which the production reduces,
 * the following integers (if any) refer to terminal or non-terminal symbols that make up the
 * production.
 * Terminals and non-terminals occupy the same enumeration space; (though non-terminals should
 * perhaps be sequential) -- right now, if a symbol does not have an associated production that
 * reduces to it, it is assumed to be a terminal.
 * The first production always describes the Start non-terminal, whose reduction equals acceptance
 * of the grammar. It is possible for multiple productions to reduce to the same Start non-terminal.
 */

typedef enum lr_error_enum {
  /* Everything went well, parser is available. */
  LR_OK,
  /* The Grammar passed in is not LR(n) for any N */
  LR_NOT_LR_GRAMMAR,
  /* The Grammar passed in is ambiguous */
  LR_AMBIGUOUS_GRAMMAR,
  /* Shift-reduce or reduce-reduce conflicts were detected. */
  LR_CONFLICTS,
  /* Memory allocation or overflow failure */
  LR_INTERNAL_ERROR
} lr_error_t;

struct lr_item {
  int production_, position_;

  struct lr_item *state_chain_;
};

struct lr_state {
  int row_;
  struct lr_item *kernel_items_;

  struct lr_transition *transitions_from_state_;
  struct lr_transition *transitions_to_state_;

  struct lr_state *gen_chain_;
};

struct lr_rel {
  struct lr_transition *from_;
  struct lr_transition *to_;

  struct lr_rel *from_chain_;
  struct lr_rel *to_chain_;
};

struct lr_transition {
  int sym_;

  struct lr_transition *from_chain_;
  struct lr_transition *to_chain_;

  struct lr_state *from_;
  struct lr_state *to_;

  struct lr_rel *outbound_rels_;
  struct lr_rel *inbound_rels_;

  /* index and lowlink for Tarjan's Strongly Connected Components algorithm */
  int index_, lowlink_;

  /* Stack position for Tarjan's algorithm, as the stack is set up
   * as a cyclic list, we can also check whether the transition is
   * on the stack by checking if the stack chain is not NULL */
  struct lr_transition *stack_chain_;

  /* read_set is of length 1 + gen->highest_term - gen->lowest_term,
   * for all non-terminals. */
  unsigned char read_set_[1];
};

struct lr_conflict_pair {
  struct lr_conflict_pair *chain_;
  /* production and position A, if this is a conflict resolution,
   * this will be chosen over B. To specify a reduction, place
   * position at the end (length) of the production. */
  int production_a_;
  int position_a_;

  /* production and position B, if this is a conflict resolution,
   * it will be replaced by A. To specify a reduction, place
   * position at the end (length) of the production. */
  int production_b_;
  int position_b_;

  /* For informational purposes, the symbol on which the conflict 
   * occurred. Ignored for conflict resolutions. */
  int sym_;
};

struct lr_generator {
  /* All productions; including S' -> S */
  size_t nr_productions_;
  int **productions_;

  /* Length of each production, eg. S' -> S would have length 1. */
  int *production_lengths_;
  
  /* root production S' -> S, where
   * S is the non-terminal the first
   * real production reduces to. */
  int root_production_[3];

  /* Various caller-defined constants..*/
  int synths_sym_; /* S' synthetic root production non-terminal symbol */
  int eop_sym_;    /* marks the end of a production */
  int eog_sym_;    /* marks the end of a grammar */
  int eof_sym_;    /* end-of-file synthetic terminal symbol */

  /* All states whose item sets are complete, linked through
   * lr_state::gen_chain. */
  struct lr_state *states_;
  int nr_states_;
  
  /* All states as they are being constructed, inside lr_closure()
   * these states may have partial item sets and have not yet been
   * matched for duplicates. Any new state created through 
   * lr_create_state() is linked into this chain (and only into
   * lr_generator::states after lr_closure()).*/
  struct lr_state *new_states_;

  /* range of nonterminal ordinals; highest_nonterm is the last
   * non-terminal symbol and always equals S' (i.e. it is 
   * synthetically generated.) */
  int lowest_nonterm_;
  int highest_nonterm_;

  /* range of terminals; highest_term is the last terminal symbol,
   * lowest_term the first. */
  int lowest_term_;
  int highest_term_;

  /* range of symbols. */
  int min_sym_;
  int max_sym_;

  /* byte-array from lowest_nonterm to highest_nonterm,
   * indicating whether the nonterm is nullable. */
  char *nonterm_is_nullable_;

  /* int-array, again from lowest_nonterm to highest_nonterm,
   * used for closure computation book-keeping. */
  int *nonterm_scratchpad_;

  /* Stack of transitions for Tarjan's SCC algorithm */
  struct lr_transition *stack_;

  /* Current index for Tarjan's SCC algorithm */
  int index_;

  /* Parse table under construction; as many rows as there are 
   * states, first row is guaranteed to be the initial state, as 
   * many columns as there are symbols (eg. max(highest_term, 
   * highest_nonterm) - min(lowest_term, lowest_nonterm) .)*/
  int *parse_table_;

  /* All conflicts found during parser table generation. */
  struct lr_conflict_pair *conflicts_;

  /* All caller specified resolutions of conflicts, with 'a'
   * taking precedence over 'b'. */
  struct lr_conflict_pair *conflict_resolutions_;
};

/* Initializes a generator for use. After this, you are expected to call
 * lr_add_conflict_resolution zero or more times and call lr_gen_parser
 * to generate the actual parser. Following lr_gen_parser, you are expected
 * to lr_cleanup(), it is not permissable to re-use the same generator without
 * cleanup. */
void lr_init(struct lr_generator *gen);

/* Adds a conflict resolution, should the dominant item and the yielding item
 * face a shift/reduce or reduce/reduce conflict, the dominant item has priority.
 * An item consists of both a production index (the ordinal of the production
 * as passed in to lr_gen_parser's productions, counted from 1) and a position
 * in to that production. A position at the length of the production indicates
 * a reduction, rather than a shift.
 * Returns zero upon success, or non-zero upon memory failure */
int lr_add_conflict_resolution(struct lr_generator *gen,
                               int dominant_production, int dominant_position,
                               int yielding_production, int yielding_position);

/* lr_gen_parser
 * 
 * Generates the parser using the generator passed in gen, which has been 
 * initialized using lr_init() and has had zero or more conflict resolutions
 * added using lr_add_conflict_resolution().
 * Parameters:
 * productions : an array of integer specifying the productions; more on this below.
 * end_of_production_sym : the integer value used for indicating the end of a production
 *                         in the array passed in productions.
 * end_of_grammar_sym : the integer value used to indicate no other productions follow
 *                      in the array passed in productions.
 * end_of_file_sym : the terminal symbol value that will be used to indicate the end of
 *                   input.
 * synthetic_s_sym : the non-terminal symbol value that lr_gen_parser() will use to 
 *                   construct the root production (S' -> S, where S' is synthetic_s_sym.
 *
 * Return values:
 * LR_OK : The parser was succesfully generated, the table can be found in 
 *         gen->parse_table. The first column of the table corresponds to symbol
 *         gen->min_sym, the last column of the table corresponds to symbol gen->max_sym.
 *         The width of the table generated is therefore (gen->max_sym - gen->min_sym + 1).
 *         The number of rows in the table corresponds to the number of states, which can
 *         be found in gen->nr_states. Parsing should start at row 0.
 *         Each cell may hold four types of values ("cell_value") :
 *         larger than 0: shift -- parser should shift the symbol onto the stack and move
 *                        to the state indicated by (cell_value). Note that this encoding
 *                        means you can never "shift" to row 0; which is correct since that
 *                        row corresponds to the closure of (S' -> .S) which can only have
 *                        a viable prefix of null.
 *         equal to 0: error -- parser should process the error; note that you can scan
 *                     the current row for non-zero cells to discover the inputs that would
 *                     have been acceptable at this point.
 *         equal to -1: accept -- parsing is complete, all input has been accepted.
 *         less than -1: reduce -- the production indexed by (- 1 - cell_value) is to be 
 *                       reduced, pop gen->production_lengths[- 1 - cell_value] symbols off
 *                       the stack and process the action associated with production
 *                       (- 1 - cell_value), which is a 1-based index into the productions
 *                       you passed in to the generator. Next, the goto should be executed
 *                       by "shifting the non-terminal" you just generated and pushing state
 *                       (row[top_of_stack])[gen->productions[- 1 - cell_value]] onto the stack,
 *                       where (row[top_of_stack]) is the pointer arithmetic to get the row
 *                       of the state that is on top of the stack after popping the number of
 *                       symbols off of the stack.
 *
 * LR_NOT_LR_GRAMMAR: The grammar is not a LR(n) grammar for any n, and should be examined.
 *                    Unfortunately more detailed error information is not available at this,
 *                    however, the error is unlikely and should be relatively apparent when
 *                    writing the grammar.
 *
 * LR_AMBIGUOUS_GRAMMAR: The grammar is ambiguous and cannot generate a LALR(1) parser.
 *
 * LR_CONFLICTS: Shift-reduce and/or reduce-reduce conflicts were encountered for which no
 *               resolution was passed in using lr_add_conflict_resolution(). You should
 *               examine the linked list in gen->conflicts for more information about which
 *               conflicts occurred.
 *
 * Remarks:
 * The array passed in to productions specifies the grammar to be converted into a LALR
 * parsing table. Each production in the grammar consists of a series of integers, where
 * the first integer specifies the non-terminal the production reduces to, subsequent
 * integers specify the (terminal and non-terminal) symbols that make up the production,
 * until an integer of value "end_of_production_sym" is encountered, which terminates the 
 * production (but is obviously not a part of it.)
 * For instance, the grammar:
 * NT_LIST -> NT_LIST COMMA ID
 * NT_LIST -> ID
 * with terminals {ID, COMMA} and non-terminal {NT_LIST} can be specified using the
 * integer array:
 * int grammar[] = {NT_LIST, NT_LIST, COMMA, ID, END_OF_PRODUCTION,
 *                  NT_LIST, ID, END_OF_PRODUCTION,
 *                  END_OF_GRAMMAR};
 * where END_OF_PRODUCTION corresponds to the value passed in for end_of_production_sym and
 * END_OF_GRAMMAR corresponds to the value passed in for end_of_grammar_sym.
 *
 * The productions are internally numbered in order, starting from 1; therefore, the 
 * production "NT_LIST -> NT_LIST COMMA ID" is production number 1, and "NT_LIST -> ID" is
 * production number 2. There is a synthetic production S' -> S that corresponds to production
 * number 0 (this may be relevant in the unlikely scenario that you get a conflict on it.)
 * For the synthetic production S' -> S, the value nonterminal S' is equal to synthetic_s_sym,
 * this value should therefore be in the range of non-terminals. The root non-terminal S used
 * as the body of the entire grammar is defined to be the non-terminal that the first production
 * reduces to.
 * 
 * Requirements on the terminals and non-terminals passed in.
 *
 * The parser generater distinguishes between terminals and non-terminals by checking whether
 * the symbol is greater than or equal to the smallest non-terminal or smaller-than or equal
 * to the largest non-terminal. Therefore, it is important that all non-terminals are in one 
 * range and that no terminals occupy the same range. Consequently, even though they do not
 * appear in your grammar, the end_of_file_sym terminal should not occupy the non-terminal
 * range, and the synthetic_s_sym terminal should occupy the non-terminal range.
 *
 * Error recovery
 *
 * If the parser you wish to generate should handle error recovery using an error symbol, then
 * ensure that the error symbol is treated as a terminal, and is in that range. The parser
 * generator itself is indifferent about error terminals and will treat it as any other 
 * terminal symbol.
 *
 */
lr_error_t lr_gen_parser(struct lr_generator *gen, int *productions,
                         int end_of_production_sym, int end_of_grammar_sym,
                         int end_of_file_sym, int synthetic_s_sym);

void lr_cleanup(struct lr_generator *gen);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* LALR_H */
