#ifndef SCANNER_H
#define SCANNER_H

#ifndef STDIO_H_INCLUDED
#define STDIO_H_INCLUDED
#include <stdio.h>
#endif

#ifndef STDINT_H_INCLUDED
#define STDINT_H_INCLUDED
#include <stdint.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct sc_scanner sc_scanner_t;
typedef struct sc_scan_rule sc_scan_rule_t;
typedef struct sc_action sc_action_t;

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
  sc_action_t *actions;
  
  /* Recognize those states with no rules being matches */
  size_t default_action;
};

struct sc_action {
  /* Field typically used to store the primary action, eg. the token
   * value that is matched.
   * If this equals default_action, there is no action for the corresponding state,
   * the valid actions must therefore not be equal to default_action.
   * default_action is passed as an argument to sc_scanner_compile and
   * stored in sc_scanner_t::default_action */
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
  sc_action_t action;
};

void sc_scanner_init(sc_scanner_t *sc);
void sc_scanner_cleanup(sc_scanner_t *sc);

int sc_scanner_compile(sc_scanner_t *sc, uintptr_t default_action, size_t num_rules, const sc_scan_rule_t *rules);
void sc_scanner_dump(sc_scanner_t *sc);

void sc_scanner_write_to_c_file(sc_scanner_t *sc, FILE *fp);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* SCANNER_H */
