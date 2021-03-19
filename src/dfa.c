/* Copyright 2020-2021 Kinglet B.V.
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

#ifndef DFA_H_INCLUDED
#define DFA_H_INCLUDED
#include "dfa.h"
#endif

#ifndef NFA_H_INCLUDED
#define NFA_H_INCLUDED
#include "nfa.h"
#endif

static void *arealloc(void *mem, size_t count, size_t size) {
  size_t size_to_alloc;
  if (multiply_size_t(count, size, NULL, &size_to_alloc)) {
    return NULL; /* overflow */
  }
  return realloc(mem, size_to_alloc);
}

void dfa_init(struct dfa *dfa) {
  dfa->first_dfa = NULL;
  dfa->next_dfa_num = 0;
  dfa->num_nfa_nodes = 0;
}

void dfa_cleanup(struct dfa *dfa) {
  while (dfa->first_dfa) {
    struct dfa_node *dfa_node = dfa->first_dfa;
    dfa->first_dfa = dfa_node->next_dfa;
    free(dfa_node);
  }
}

static void dfa_reset(struct dfa *dfa) {
  dfa_cleanup(dfa);
  dfa_init(dfa);
}

static struct dfa_node *dfa_make_node(struct dfa *dfa) {
  struct dfa_node *dfa_node = (struct dfa_node *)arealloc(NULL, 1, sizeof(struct dfa_node) + sizeof(uint64_t) * (((dfa->num_nfa_nodes + 63) / 64) - 1));
  memset(dfa_node->moves, 0, sizeof(struct dfa_node *) * 256);
  memset(dfa_node->nfa_map, 0, sizeof(uint64_t) * ((dfa->num_nfa_nodes + 63) / 64));
  dfa_node->next_dfa = NULL;
  dfa_node->num = 0;
  return dfa_node;
}

static void dfa_record_node(struct dfa *dfa, struct dfa_node *node) {
  node->next_dfa = dfa->first_dfa;
  dfa->first_dfa = node;
  node->num = dfa->next_dfa_num++;
}

static void dfa_free_unrecorded_node(struct dfa *dfa, struct dfa_node *node) {
  free(node);
}

int dfa_node_has_nfa(struct dfa_node *dfa_node, size_t nfa_node_index) {
  return !!(dfa_node->nfa_map[nfa_node_index / 64] & ((uint64_t)1) << (nfa_node_index & 63));
}

static int dfa_add_nfa(struct dfa_node *dfa_node, size_t nfa_node_index) {
  int already_had_nfa = dfa_node_has_nfa(dfa_node, nfa_node_index);
  dfa_node->nfa_map[nfa_node_index / 64] |= ((uint64_t)1) << (nfa_node_index & 63);
  return already_had_nfa;
}

static void dfa_closure(struct dfa *dfa, struct nfa *nfa, struct dfa_node *dfa_node) {
  size_t nfa_node_index;
  int progress_was_made;
  do {
    progress_was_made = 0;
    for (nfa_node_index = 0; nfa_node_index < dfa->num_nfa_nodes; ++nfa_node_index) {
      if (dfa_node_has_nfa(dfa_node, nfa_node_index)) {
        struct nfa_node *nfa_node = nfa->nfa_nodes + nfa_node_index;
        struct nfa_trans *trans;
        for (trans = nfa_node->empty_move; trans; trans = trans->next_trans) {
          if (!dfa_add_nfa(dfa_node, trans->node)) {
            progress_was_made = 1;
          }
        }
      }
    }
  } while (progress_was_made);
}

/* dfa_node should not yet be "recorded" or it'll find itself */
static struct dfa_node *dfa_find_identical(struct dfa *dfa, struct dfa_node *dfa_node) {
  struct dfa_node *alt;
  for (alt = dfa->first_dfa; alt; alt = alt->next_dfa) {
    if (!memcmp(alt->nfa_map, dfa_node->nfa_map, sizeof(uint64_t) * ((dfa->num_nfa_nodes + 63) / 64))) {
      return alt;
    }
  }
  return NULL;
}

/* Note: returns NULL if the transition is not permitted by the NFA. */
struct dfa_node *dfa_transition(struct dfa *dfa, struct nfa *nfa, char c, struct dfa_node *from) {
  size_t nfa_node_index;
  struct dfa_node *dfa_node = from->moves[(uint8_t)c];

  if (dfa_node) {
    /* Already have it */
    return dfa_node;
  }

  for (nfa_node_index = 0; nfa_node_index < dfa->num_nfa_nodes; ++nfa_node_index) {
    if (dfa_node_has_nfa(from, nfa_node_index)) {
      struct nfa_node *nfa_node = nfa->nfa_nodes + nfa_node_index;
      struct nfa_trans *trans;
      trans = nfa_node->moves[(uint8_t)c];
      if (trans) {
        if (!dfa_node) {
          dfa_node = dfa_make_node(dfa);
        }
        do {
          dfa_add_nfa(dfa_node, trans->node);

          trans = trans->next_trans;
        } while (trans);
      }
    }
  }

  if (dfa_node) {
    struct dfa_node *alt;
    dfa_closure(dfa, nfa, dfa_node);

    /* Check if dfa_node is a duplicate, if so, replace it with the original */
    alt = dfa_find_identical(dfa, dfa_node);
    if (alt) {
      dfa_free_unrecorded_node(dfa, dfa_node);
      dfa_node = alt;
    }
    else {
      /* This is a new DFA state that does not exist yet, record it as final
       * so future DFA candidate states can be compared to it. */
      dfa_record_node(dfa, dfa_node);
    }
  }

  from->moves[(uint8_t)c] = dfa_node;

  return dfa_node;
}


struct dfa_node *dfa_make(struct dfa *dfa, struct nfa *nfa, size_t nfa_start) {
  struct dfa_node *dfa_node = dfa_make_node(dfa);
  struct dfa_node *alt;
  struct dfa_node *dfa_start;
  dfa_add_nfa(dfa_node, nfa_start);
  dfa_closure(dfa, nfa, dfa_node);
  alt = dfa_find_identical(dfa, dfa_node);
  if (alt) {
    dfa_free_unrecorded_node(dfa, dfa_node);
    return alt;
  }

  struct dfa_node *sentinel = dfa->first_dfa;
  /* This pushes dfa_node to the head of dfa->first_dfa, sentinel
   * is now the first value from before we added anything */
  dfa_record_node(dfa, dfa_node);
  dfa_start = dfa_node;
  do {
    struct dfa_node *next_sentinel = dfa->first_dfa;

    for (dfa_node = dfa->first_dfa; dfa_node != sentinel; dfa_node = dfa_node->next_dfa) {
      int c;
      for (c = 0; c < 256; ++c) {
        /* If the transition is valid and it leads to a state we do not
         * already have, dfa_transition will create a new DFA state and 
         * prepend it to dfa->first_dfa. Consequently, "next_sentinel"
         * will point to the first DFA state that this sequence of
         * dfa_transition() calls hasn't created. Alternatively, if no
         * new DFA nodes are needed, next_sentinel will equal dfa->first_dfa,
         * and we are done. */
        dfa_transition(dfa, nfa, (char)c, dfa_node);
      }
    }

    sentinel = next_sentinel;
  } while (sentinel != dfa->first_dfa);

  return dfa_start;
}

static void print_char(int c) {
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

void dfa_dump_node(struct dfa *odfa, struct dfa_node *d) {
  int c;
  printf("#%2d: ", (int)d->num);
  for (c = 0; c < 256; ++c) {
    if (d->moves[c]) {
      /* Check if this is the first instance of this move.. */
      struct dfa_node *dst = d->moves[c];
      int q;
      int is_first = 1;
      for (q = 0; q < c; ++q) {
        if (d->moves[q] == dst) {
          /* Already have a move to this state, no need to dump it again.. */
          is_first = 0;
          break;
        }
      }
      if (is_first) {
        int count = 0;
        for (q = c; q < 256; ++q) {
          if (d->moves[q] == dst) {
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
            if (d->moves[q] == dst) {
              int range_end = q + 1;
              while ((range_end < 256) && (d->moves[range_end] == dst)) {
                ++range_end;
              }
              if (range_end == (q + 1)) {
                /* Single character, write out the single char. */
                print_char(q);
              }
              else {
                /* Range of characters */
                print_char(q);
                printf("-");
                print_char(range_end - 1);
              }
              q = range_end - 1; /* Loop will immediately increment. */
            }
          }
          printf("]");
        }
        else {
          /* Single isolated character */
          print_char(c);
        }
        printf(":#%d ", (int)dst->num);
      }
    }
  }
  printf("\n");
}
