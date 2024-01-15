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

#ifndef STDIO_H_INCLUDED
#define STDIO_H_INCLUDED
#include <stdio.h>
#endif

#ifndef STDLIB_H_INCLUDED
#define STDLIB_H_INCLUDED
#include <stdlib.h>
#endif

#ifndef STDINT_H_INCLUDED
#define STDINT_H_INCLUDED
#include <stdint.h>
#endif

#ifndef CTYPE_H_INCLUDED
#define CTYPE_H_INCLUDED
#include <ctype.h>
#endif

#ifndef STRING_H_INCLUDED
#define STRING_H_INCLUDED
#include <string.h>
#endif

#ifndef ASSERT_H_INCLUDED
#define ASSERT_H_INCLUDED
#include <assert.h>
#endif

#ifndef INTTYPES_H_INCLUDED
#define INTTYPES_H_INCLUDED
#include <inttypes.h>
#endif

#ifndef MUL_H_INCLUDED
#define MUL_H_INCLUDED
#include "mul.h"
#endif

#include "scanner.h"

#include "nfa.h"
#include "dfa.h"

static void *arealloc(void *mem, size_t count, size_t size) {
  size_t size_to_alloc;
  if (multiply_size_t(count, size, NULL, &size_to_alloc)) {
    return NULL; /* overflow */
  }
  return realloc(mem, size_to_alloc);
}

void sc_scanner_init(struct sc_scanner *sc) {
  sc->num_states = 0;
  sc->transition_table = NULL;
  sc->actions = NULL;
}

void sc_scanner_cleanup(struct sc_scanner *sc) {
  if (sc->transition_table) free(sc->transition_table);
  if (sc->actions) free(sc->actions);
}

int sc_scanner_compile(struct sc_scanner *sc, uintptr_t default_action, size_t num_rules, const struct sc_scan_rule *rules) {
  int result;
  size_t n;
  struct nfa cumulative_nfa;
  size_t *final_rule_nfa_states = (size_t *)arealloc(NULL, num_rules, sizeof(size_t));
  if (!final_rule_nfa_states) {
    return -1;
  }
  nfa_init(&cumulative_nfa);
  for (n = 0; n < num_rules; ++n) {
    struct nfa rule_nfa;
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

  struct dfa dfa;
  dfa_init(&dfa);
  dfa.num_nfa_nodes = cumulative_nfa.num_nfa_nodes;
  struct dfa_node *start = dfa_make(&dfa, &cumulative_nfa, cumulative_nfa.start_nfa);
  
  size_t num_dfas = 0;
  struct dfa_node *dfa_node;
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
  sc->actions = (struct sc_action *)arealloc(NULL, num_dfas, sizeof(struct sc_action));
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

static void sc_dump_row(struct sc_scanner *sc, size_t state) {
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
    printf(" ==> %" PRIxPTR "\n", sc->actions[state].action);
  }
  else {
    printf("\n");
  }
}

void sc_scanner_dump(struct sc_scanner *sc) {
  size_t n;
  for (n = 1; n < sc->num_states; ++n) {
    sc_dump_row(sc, n);
  }
}

void sc_scanner_write_to_c_file(struct sc_scanner *sc, FILE *fp, const char *transition_table_id, const char *state_actions_id, const char *scanner_id) {
  size_t n;
  fprintf(fp, "static size_t %s[] = {\n", transition_table_id);
  for (n = 0; n < sc->num_states; ++n) {
    int c;
    for (c = 0; c < 256; ++c) {
      fprintf(fp, "%s%zu", c ? ", " : "  ", sc->transition_table[256 * n + (size_t)c]);
    }
    fprintf(fp, "%s\n", (n != (sc->num_states - 1)) ? "," : "");
  }
  fprintf(fp, "};\n");
  fprintf(fp, "static struct sc_action %s[] = {\n", state_actions_id);
  for (n = 0; n < sc->num_states; ++n) {
    fprintf(fp, "  {%" PRIuPTR ",%" PRIuPTR "}%s", sc->actions[n].action, sc->actions[n].variant, n == (sc->num_states - 1) ? "\n" : ",\n");
  }
  fprintf(fp, "};\n");
  fprintf(fp, "static struct sc_scanner %s = { %u, %u, %s, %s, %u };\n", scanner_id,
          (unsigned int)sc->num_states, (unsigned int)sc->start_state, transition_table_id, state_actions_id, (unsigned int)sc->default_action);
}
