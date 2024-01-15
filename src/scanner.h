/* Copyright 2020-2024 Kinglet B.V.
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

#ifndef SCANNER_H
#define SCANNER_H

#ifndef STDIO_H_INCLUDED
#define STDIO_H_INCLUDED
#include <stdio.h>
#endif

#ifndef STDDEF_H_INCLUDED
#define STDDEF_H_INCLUDED
#include <stddef.h> /* size_t */
#endif

#ifndef STDINT_H_INCLUDED
#define STDINT_H_INCLUDED
#include <stdint.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct sc_scanner {
  /* Number of states for the scanner, including the dummy
   * 0 state. */
  size_t num_states;

  /* State at which to start scanning */
  size_t start_state;

  /* Transition table, each row is 256 cells (one for each
   * possible input byte) - there are num_states + 1 rows.
   * For any given state index as row, and any given column
   * as the next input byte from that state, the cell gives
   * the state to transition to upon accepting the input.
   * State 0 is unused, it's row can be ignored. The value 
   * 0 in a cell indicates that byte is not acceptable input 
   * in the given state. */
  size_t *transition_table;

  /* Actions for each state */
  struct sc_action *actions;
  
  /* Recognize those states with no rules being matches */
  size_t default_action;
};

struct sc_action {
  /* Field typically used to store the primary action, eg. the token
   * value that is matched.
   * If this equals default_action, there is no action for the corresponding state,
   * the valid actions must therefore not be equal to default_action.
   * default_action is passed as an argument to sc_scanner_compile and
   * stored in sc_scanner::default_action */
  uintptr_t action;

  /* Field typically used to store the variant of the primary action,
   * for instance, it may be helpful to distinguish the matching
   * of 123 (decimals) vs. 0123 (octals) vs. 0x123 (hexadecimals) as, even
   * though the corresponding action would be the same (they're integers),
   * the parsing needed to convert the value to its binary representation
   * is different. If action equals default_action, the value of the
   * variant is undefined. */
  uintptr_t variant;
};

struct sc_scan_rule {
  const char *regexp;

  /* Action if the regexp rule is matched before any other in
  * the rules table. */
  struct sc_action action;
};

void sc_scanner_init(struct sc_scanner *sc);
void sc_scanner_cleanup(struct sc_scanner *sc);

int sc_scanner_compile(struct sc_scanner *sc, uintptr_t default_action, size_t num_rules, const struct sc_scan_rule *rules);
void sc_scanner_dump(struct sc_scanner *sc);

void sc_scanner_write_to_c_file(struct sc_scanner *sc, FILE *fp, const char *transition_table_id, const char *state_actions_id, const char *scanner_id);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* SCANNER_H */
