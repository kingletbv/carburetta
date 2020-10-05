#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include <inttypes.h>

#include "scanner.h"

#include "nfa.h"
#include "dfa.h"

static void *arealloc(void *mem, size_t count, size_t size) {
  if (((count >= (((size_t)1) << sizeof(size_t)*4)) ||
       (size >= (((size_t)1) << (sizeof(size_t)*4)))) &&
      (((size_t)-1)/(count) < size)) {
    return NULL;
  }
  return realloc(mem, count * size);
}

void sc_scanner_init(sc_scanner_t *sc) {
  sc->num_states = 0;
  sc->transition_table = NULL;
  sc->actions = NULL;
}

void sc_scanner_cleanup(sc_scanner_t *sc) {
  if (sc->transition_table) free(sc->transition_table);
  if (sc->actions) free(sc->actions);
}

int sc_scanner_compile(sc_scanner_t *sc, uintptr_t default_action, size_t num_rules, const sc_scan_rule_t *rules) {
  int result;
  size_t n;
  nfa_t cumulative_nfa;
  size_t *final_rule_nfa_states = arealloc(NULL, num_rules, sizeof(size_t));
  if (!final_rule_nfa_states) {
    return -1;
  }
  nfa_init(&cumulative_nfa);
  for (n = 0; n < num_rules; ++n) {
    nfa_t rule_nfa;
    nfa_init(&rule_nfa);
    result = nfa_parse_regexp(&rule_nfa, rules[n].regexp);
    if (!result) {
      result = nfa_merge_nfas(&cumulative_nfa, &rule_nfa, &final_rule_nfa_states[n]);
    }
    nfa_cleanup(&rule_nfa);
    if (result) {
      break;
    }
  }

  if (result) {
    free(final_rule_nfa_states);
    return result;
  }

  dfa_t dfa;
  dfa_init(&dfa);
  dfa.num_nfa_nodes = cumulative_nfa.num_nfa_nodes;
  dfa_node_t *start = dfa_make(&dfa, &cumulative_nfa, cumulative_nfa.start_nfa);
  
  size_t num_dfas = 0;
  dfa_node_t *dfa_node;
  for (dfa_node = dfa.first_dfa; dfa_node; dfa_node = dfa_node->next_dfa) {
    num_dfas++;
  }
  /* Add in dummy state to get the total number of states */
  num_dfas++;
  if (sc->actions) {
    free(sc->actions);
    sc->actions = NULL;
  }
  if (sc->transition_table) {
    free(sc->transition_table);
    sc->transition_table = NULL;
  }
  sc->num_states = num_dfas;
  sc->actions = (sc_action_t *)arealloc(NULL, num_dfas, sizeof(sc_action_t));
  if (!sc->actions) {
    return -1;
  }
  sc->transition_table = (size_t *)arealloc(NULL, num_dfas, 256 * sizeof(size_t));
  if (!sc->transition_table) {
    return -1;
  }
  for (dfa_node = dfa.first_dfa; dfa_node; dfa_node = dfa_node->next_dfa) {
    /* plus 1 for the dummy state at 0, you'll see the +1 correction a few times */
    size_t state_index = dfa_node->num + 1;
    size_t *row = sc->transition_table + 256 * state_index;
    for (n = 0; n < 256; ++n) {
      row[n] = (dfa_node->moves[n]) ? dfa_node->moves[n]->num + 1 : 0;
    }
    for (n = 0; n < num_rules; ++n) {
      if (dfa_node_has_nfa(dfa_node, final_rule_nfa_states[n])) {
        /* Found match for this state. */
        sc->actions[state_index] = rules[n].action;
        break;
      }
    }
    if (n == num_rules) {
      /* No match for this state, apply default action */
      sc->actions[state_index].action = default_action;
      sc->actions[state_index].variant = 0;
    }
  }
  /* Set first row to zeroes */
  memset(sc->transition_table, 0, 256 * sizeof(size_t));
  sc->actions[0].action = 0;
  sc->actions[0].variant = 0;

  /* Set start state to begin scanning from.. */
  sc->start_state = start->num + 1;

  sc->default_action = default_action;

  return 0;
}

static void sc_print_char(int c) {
  if (((c >= 'a') && (c <= 'z')) ||
      ((c >= 'A') && (c <= 'Z')) ||
      ((c >= '0') && (c <= '9')) ||
      (c == '_')) {
    printf("%c", c);
  }
  else {
    printf("\\x%02X", c);
  }
}

static void sc_dump_row(sc_scanner_t *sc, size_t state) {
  int c;
  size_t *row = sc->transition_table + 256 * state;
  printf("#%3d: ", (int)state);

  for (c = 0; c < 256; ++c) {
    if (row[c]) {
      /* Check if this is the first instance of this move.. */
      size_t dst = row[c];
      int q;
      int is_first = 1;
      for (q = 0; q < c; ++q) {
        if (row[q] == dst) {
          /* Already have a move to this state, no need to dump it again.. */
          is_first = 0;
          break;
        }
      }
      if (is_first) {
        int count = 0;
        for (q = c; q < 256; ++q) {
          if (row[q] == dst) {
            count++;
            if (count > 1) {
              break;
            }
          }
        }
        if (count > 1) {
          /* Make character class */
          printf("[");
          for (q = c; q < 256; ++q) {
            if (row[q] == dst) {
              int range_end = q + 1;
              while ((range_end < 256) && (row[range_end] == dst)) {
                ++range_end;
              }
              if (range_end == (q + 1)) {
                /* Single character, write out the single char. */
                sc_print_char(q);
              }
              else {
                /* Range of characters */
                sc_print_char(q);
                printf("-");
                sc_print_char(range_end - 1);
              }
              q = range_end - 1; /* Loop will immediately increment. */
            }
          }
          printf("]");
        }
        else {
          /* Single isolated character */
          sc_print_char(c);
        }
        printf(":#%d ", (int)dst);
      }
    }
  }
  if (sc->actions[state].action != sc->default_action) {
    printf(" ==> %"PRIxPTR"\n", sc->actions[state].action);
  }
  else {
    printf("\n");
  }
}

void sc_scanner_dump(sc_scanner_t *sc) {
  size_t n;
  for (n = 1; n < sc->num_states; ++n) {
    sc_dump_row(sc, n);
  }
}

void sc_scanner_write_to_c_file(sc_scanner_t *sc, FILE *fp) {
  size_t n;
  fprintf(fp, "static size_t scanner_transition_table[] = {\n");
  for (n = 0; n < sc->num_states; ++n) {
    int c;
    for (c = 0; c < 256; ++c) {
      fprintf(fp, "%s%zu", c ? ", " : "  ", sc->transition_table[256 * n + (size_t)c]);
    }
    fprintf(fp, "%s\n", (n != (sc->num_states - 1)) ? "," : "");
  }
  fprintf(fp, "};\n");
  fprintf(fp, "static sc_action_t scanner_state_actions[] = {\n");
  for (n = 0; n < sc->num_states; ++n) {
    fprintf(fp, "  {%"PRIuPTR",%"PRIuPTR"}%s", sc->actions[n].action, sc->actions[n].variant, n == (sc->num_states - 1) ? "\n" : ",\n");
  }
  fprintf(fp, "};\n");
  fprintf(fp, "static sc_scanner_t scanner = { %u, %u, scanner_transition_table, scanner_state_actions, %u };\n",
          (unsigned int)sc->num_states, (unsigned int)sc->start_state, (unsigned int)sc->default_action);
}
