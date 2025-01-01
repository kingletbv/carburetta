/* Copyright 2020-2025 Kinglet B.V.
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

#ifndef NFA_H_INCLUDED
#define NFA_H_INCLUDED
#include "nfa.h"
#endif

#ifndef XLALR_H_INCLUDED
#define XLALR_H_INCLUDED
#include "xlalr.h"
#endif

#define USE_PRECOMPILED_PARSETABLE 1
#define WRITE_PARSETABLE_TO_FILE 0

#define DEF_SYMS     \
  xx(NT_END)         \
  xx(RULE_END)       \
  xx(GRAMMAR_END)    \
                     \
  xx(SYM_CHAR)       \
  xx(SYM_LPAREN)     \
  xx(SYM_RPAREN)     \
  xx(SYM_LSQBRACKET) \
  xx(SYM_RSQBRACKET) \
  xx(SYM_DOT)        \
  xx(SYM_DOLLAR)     \
  xx(SYM_CARET)      \
  xx(SYM_DASH)       \
  xx(SYM_BAR)        \
  xx(SYM_ASTERISK)   \
  xx(SYM_PLUS)       \
  xx(SYM_QUESTIONMARK)\
  xx(INPUT_END)      \
                     \
  xx(REG_EXP)        \
  xx(REG_TERM)       \
  xx(REG_ELM)        \
  xx(REG_SYM)        \
  xx(REG_RANGE)      \
  xx(REG_RANGE_ELM)  \
  xx(SYNTH_S)

typedef enum sym_enum {
#define xx(s) s,
  DEF_SYMS
#undef xx
} sym_t;

struct nfa_input_stream {
  const char *input_pointer;
};

struct nfa_parse_context {
  struct nfa_input_stream is;
  struct nfa nfa;
};

struct sym_data_node_struct {
  size_t initial, final;
};

struct sym_char_set_struct {
  /* 256 bits bitmap[bit >> 6] & (1 << 0x3F) */
  uint64_t bitmap[4];
};

union sym_data_union {
  struct sym_data_node_struct nfa;
  int             sym;
  struct sym_char_set_struct  set;
};

struct sym_stack {
  size_t pos;
  size_t num_stack_allocated; /* size in elements of states and syms */
  int *states;
  union sym_data_union *syms;
};


static void make_empty_trans(struct nfa *nfa, size_t from, size_t to);
static void make_trans(struct nfa *nfa, size_t from, char c, size_t to);
static size_t make_node(struct nfa *nfa);
static void *arealloc(void *mem, size_t count, size_t size);
static int is_in_set(char c, const char *str);

/* grammar[]  are only used when generating new tables */
#if !USE_PRECOMPILED_PARSETABLE
static int grammar[] = {
  REG_EXP, NT_END, REG_TERM, RULE_END,
  REG_EXP, NT_END, REG_EXP, SYM_BAR, REG_TERM, RULE_END,

  REG_TERM, NT_END, RULE_END,
  REG_TERM, NT_END, REG_TERM, REG_ELM, RULE_END,

  REG_ELM, NT_END, REG_SYM, RULE_END,
  REG_ELM, NT_END, REG_SYM, SYM_ASTERISK, RULE_END,
  REG_ELM, NT_END, REG_SYM, SYM_PLUS, RULE_END,
  REG_ELM, NT_END, REG_SYM, SYM_QUESTIONMARK, RULE_END,

  REG_SYM, NT_END, SYM_CHAR, RULE_END,
  REG_SYM, NT_END, SYM_DOT, RULE_END,
  REG_SYM, NT_END, SYM_CARET, RULE_END,
  REG_SYM, NT_END, SYM_DOLLAR, RULE_END,
  REG_SYM, NT_END, SYM_LPAREN, REG_EXP, SYM_RPAREN, RULE_END,
  REG_SYM, NT_END, SYM_LSQBRACKET, REG_RANGE, SYM_RSQBRACKET, RULE_END,
  REG_SYM, NT_END, SYM_LSQBRACKET, SYM_CARET, REG_RANGE, SYM_RSQBRACKET, RULE_END,

  REG_RANGE, NT_END, REG_RANGE, REG_RANGE_ELM, RULE_END,
  REG_RANGE, NT_END, REG_RANGE_ELM, RULE_END,

  REG_RANGE_ELM, NT_END, SYM_CHAR, RULE_END,
  REG_RANGE_ELM, NT_END, SYM_CHAR, SYM_DASH, SYM_CHAR, RULE_END,

  GRAMMAR_END
};
#endif

static int reduce(struct nfa_parse_context *ctx, int production, union sym_data_union *syms) {
  size_t i, f;
  int n, begin_range, end_range;
  switch (production) {
    case 1: /* REG_EXP, NT_END, REG_TERM, RULE_END, */
      /* REG-EXP == REG-TERM, no changes, keep */
      return 1;
    case 2: /* REG_EXP, NT_END, REG_EXP, SYM_BAR, REG_TERM, RULE_END, */
      /* REG-EXP == REG_EXP | REG_TERM */
      i = make_node(&ctx->nfa); f = make_node(&ctx->nfa);
      make_empty_trans(&ctx->nfa, i, syms[0].nfa.initial);
      make_empty_trans(&ctx->nfa, i, syms[2].nfa.initial);
      make_empty_trans(&ctx->nfa, syms[0].nfa.final, f);
      make_empty_trans(&ctx->nfa, syms[2].nfa.final, f);
      syms[0].nfa.initial = i;
      syms[0].nfa.final = f;
      return 1;

    case 3: /* REG_TERM, NT_END, RULE_END, */
      i = make_node(&ctx->nfa); f = make_node(&ctx->nfa);
      make_empty_trans(&ctx->nfa, i, f);
      syms[0].nfa.initial = i;
      syms[0].nfa.final = f;
      return 1;
    case 4: /* REG_TERM, NT_END, REG_TERM, REG_ELM, RULE_END, */
      /* XXX: Find better way of fusing, we're likely creating needless
       *      nodes here.. Reason for not doing it now is that in the
       *      current data-structure setup, we have no way of knowing
       *      what transitions are inbound to syms[1].initial. */
      make_empty_trans(&ctx->nfa, syms[0].nfa.final, syms[1].nfa.initial);
      syms[0].nfa.initial = syms[0].nfa.initial;
      syms[0].nfa.final = syms[1].nfa.final;
      return 1;

    case 5: /* REG_ELM, NT_END, REG_SYM, RULE_END, */
      /* REG-ELM == REG-SYM, no changes, keep */
      return 1;
    case 6: /* REG_ELM, NT_END, REG_SYM, SYM_ASTERISK, RULE_END, */
      i = make_node(&ctx->nfa); f = make_node(&ctx->nfa);
      make_empty_trans(&ctx->nfa, i, f);
      make_empty_trans(&ctx->nfa, syms[0].nfa.final, syms[0].nfa.initial);
      make_empty_trans(&ctx->nfa, i, syms[0].nfa.initial);
      make_empty_trans(&ctx->nfa, syms[0].nfa.final, f);
      syms[0].nfa.initial = i;
      syms[0].nfa.final = f;
      return 1;
    case 7: /* REG_ELM, NT_END, REG_SYM, SYM_PLUS, RULE_END, */
      i = syms[0].nfa.initial;
      f = make_node(&ctx->nfa);
      make_empty_trans(&ctx->nfa, syms[0].nfa.final, f);
      make_empty_trans(&ctx->nfa, syms[0].nfa.final, syms[0].nfa.initial);
      syms[0].nfa.initial = i;
      syms[0].nfa.final = f;
      return 1;
    case 8: /* REG_ELM, NT_END, REG_SYM, SYM_QUESTIONMARK, RULE_END, */
      i = make_node(&ctx->nfa); f = make_node(&ctx->nfa);
      make_empty_trans(&ctx->nfa, i, f);
      make_empty_trans(&ctx->nfa, i, syms[0].nfa.initial);
      make_empty_trans(&ctx->nfa, syms[0].nfa.final, f);
      syms[0].nfa.initial = i;
      syms[0].nfa.final = f;
      return 1;

    case 9: /* REG_SYM, NT_END, SYM_CHAR, RULE_END, */
      /* REG-SYM == SYM-CHAR */
      i = make_node(&ctx->nfa); f = make_node(&ctx->nfa);
      make_trans(&ctx->nfa, i, (char)syms[0].sym, f);
      syms[0].nfa.initial = i;
      syms[0].nfa.final = f;
      return 1;
    case 10: /* REG_SYM, NT_END, SYM_DOT, RULE_END, */
      i = make_node(&ctx->nfa); f = make_node(&ctx->nfa);
      for (n = 0; n < 256; n++) {
        if (n == 0xA) continue;
        make_trans(&ctx->nfa, i, (char)n, f);
      }
      syms[0].nfa.initial = i;
      syms[0].nfa.final = f;
      return 1;

    case 11: /* REG_SYM, NT_END, SYM_CARET, RULE_END, */
      /* XXX: Figure out how to do start-of-string Caret */
      return 0;
    case 12: /* REG_SYM, NT_END, SYM_DOLLAR, RULE_END, */
      /* XXX: Figure out how to do end-of-string Dollar */
      return 0;
    case 13: /* REG_SYM, NT_END, SYM_LPAREN, REG_EXP, SYM_RPAREN, RULE_END, */
      syms[0].nfa = syms[1].nfa;
      return 1;
    case 14: /* REG_SYM, NT_END, SYM_LSQBRACKET, REG_RANGE, SYM_RSQBRACKET, RULE_END, */
      i = make_node(&ctx->nfa); f = make_node(&ctx->nfa);
      for (n = 0; n < 256; ++n) {
        if (syms[1].set.bitmap[n >> 6] & (((uint64_t)1) << (n & 0x3F))) {
          make_trans(&ctx->nfa, i, (char)n, f);
        }
      }
      syms[0].nfa.initial = i;
      syms[0].nfa.final = f;
      return 1;
    case 15: /* REG_SYM, NT_END, SYM_LSQBRACKET, SYM_CARET, REG_RANGE, SYM_RSQBRACKET, RULE_END, */
      for (n = 0; n < 4; ++n) {
        syms[2].set.bitmap[n] ^= ~(uint64_t)0;
      }
      i = make_node(&ctx->nfa); f = make_node(&ctx->nfa);
      for (n = 0; n < 256; ++n) {
        if (syms[2].set.bitmap[n >> 6] & (((uint64_t)1) << (n & 0x3F))) {
          make_trans(&ctx->nfa, i, (char)n, f);
        }
      }
      syms[0].nfa.initial = i;
      syms[0].nfa.final = f;
      return 1;

    case 16: /* REG_RANGE, NT_END, REG_RANGE, REG_RANGE_ELM, RULE_END, */
      for (n = 0; n < 4; ++n) {
        syms[0].set.bitmap[n] |= syms[1].set.bitmap[n];
      }
      return 1;
    case 17: /* REG_RANGE, NT_END, REG_RANGE_ELM, RULE_END, */
      /* REG_RANGE == REG_RANGE_ELM */
      return 1;
    case 18: /* REG_RANGE_ELM, NT_END, SYM_CHAR, RULE_END, */
      n = syms[0].sym;
      memset(&syms[0].set, 0, sizeof(struct sym_char_set_struct));
      syms[0].set.bitmap[n >> 6] = ((uint64_t)1) << (n & 0x3F);
      return 1;
    case 19: /* REG_RANGE_ELM, NT_END, SYM_CHAR, SYM_DASH, SYM_CHAR, RULE_END, */
      begin_range = syms[0].sym;
      end_range = syms[2].sym;
      if (begin_range > end_range) {
        n = end_range;
        end_range = begin_range;
        begin_range = n;
      }
      memset(&syms[0].set, 0, sizeof(struct sym_char_set_struct));
      for (n = begin_range; n <= end_range; ++n) {
        syms[0].set.bitmap[n >> 6] |= ((uint64_t)1) << (n & 0x3F);
      }
      return 1;
    default:
      assert(0 && "unknown state");
      return 0;
  }
}

static int is_in_set(char c, const char *str) {
  while (*str) {
    if (c == *str) return 1;
    str++;
  }
  return 0;
}

static void *arealloc(void *mem, size_t count, size_t size) {
  size_t size_to_alloc;
  if (multiply_size_t(count, size, NULL, &size_to_alloc)) {
    return NULL; /* overflow */
  }
  return realloc(mem, size_to_alloc);
}

static size_t make_node(struct nfa *nfa) {
  size_t node;
  if (nfa->free_nfa != ~(size_t)0) {
    node = nfa->free_nfa;
    nfa->free_nfa = ~(size_t)0;
  }
  else {
    if (nfa->num_nfa_nodes == nfa->num_nfa_nodes_allocated) {
      nfa->num_nfa_nodes_allocated += nfa->num_nfa_nodes_allocated;
      if (!nfa->num_nfa_nodes_allocated) {
        nfa->num_nfa_nodes_allocated = 16;
      }
      nfa->nfa_nodes = (struct nfa_node *)arealloc(nfa->nfa_nodes, nfa->num_nfa_nodes_allocated, sizeof(struct nfa_node));
    }
    node = nfa->num_nfa_nodes++;
  }
  memset(nfa->nfa_nodes + node, 0, sizeof(struct nfa_node));
  return node;
}

static void make_trans(struct nfa *nfa, size_t from, char c, size_t to) {
  struct nfa_trans *trans = (struct nfa_trans *)malloc(sizeof(struct nfa_trans));
  trans->next_trans = nfa->nfa_nodes[from].moves[(uint8_t)c];
  nfa->nfa_nodes[from].moves[(uint8_t)c] = trans;
  trans->node = to;
}

static void make_empty_trans(struct nfa *nfa, size_t from, size_t to) {
  struct nfa_trans *trans = (struct nfa_trans *)malloc(sizeof(struct nfa_trans));
  trans->next_trans = nfa->nfa_nodes[from].empty_move;
  nfa->nfa_nodes[from].empty_move = trans;
  trans->node = to;
}

static char peek(struct nfa_parse_context *ctx) {
  return *ctx->is.input_pointer;
}

static void pop(struct nfa_parse_context *ctx) {
  ctx->is.input_pointer++;
}

static int scan_token(struct nfa_parse_context *ctx, char *c) {
  char inc = peek(ctx);
  int token = 0;
  switch (inc) {
    case '(': token = SYM_LPAREN;
              break;
    case ')': token = SYM_RPAREN;
              break;
    case '[': token = SYM_LSQBRACKET;
              break;
    case ']': token = SYM_RSQBRACKET;
              break;
    case '.': token = SYM_DOT;
              break;
    case '$': token = SYM_DOLLAR;
              break;
    case '^': token = SYM_CARET;
              break;
    case '-': token = SYM_DASH;
              break;
    case '|': token = SYM_BAR;
              break;
    case '*': token = SYM_ASTERISK;
              break;
    case '+': token = SYM_PLUS;
              break;
    case '?': token = SYM_QUESTIONMARK;
              break;
  }
  if (token) {
    pop(ctx);
    *c = inc;
    return token;
  }

  if (is_in_set(inc, "{}[](),.^$*|?+:-")) {
    /* Reserved character */
    return 0;
  }
  if (inc == '\\') {
    /* Escape sequence */
    pop(ctx); inc = peek(ctx);
    char esc = 0;
    switch (inc) {
      case 'a':
        esc = '\a';
        break;
      case 'b':
        esc = '\b';
        break;
      case 'f':
        esc = '\f';
        break;
      case 'n':
        esc = '\n';
        break;
      case 'r':
        esc = '\r';
        break;
      case 't':
        esc = '\t';
        break;
      case 'v':
        esc = '\v';
        break;
    }
    if (!esc) {
      if (is_in_set(inc, "{}[](),.^$*|?+:;-\\\'\" ")) {
        esc = inc;
      }
    }
    if (esc) {
      pop(ctx);
      *c = esc;
      return SYM_CHAR;
    }
    if (inc == 'x') {
      /* Hex escape */
      pop(ctx); inc = peek(ctx);
      if (((inc >= '0') && (inc <= '9')) ||
          ((inc >= 'a') && (inc <= 'f')) ||
          ((inc >= 'A') && (inc <= 'F'))) {
        /* Starts with hex, we're good. */
      }
      else {
        /* Hex escape does not follow with hex digits, problem. */
        /* XXX: Consider throwing error */
        return 0;
      }
      while (((inc >= '0') && (inc <= '9')) ||
             ((inc >= 'a') && (inc <= 'f')) ||
             ((inc >= 'A') && (inc <= 'F'))) {
        if (esc & 0xF0) {
          /* XXX: Overflow, consider throwing error */
          return 0;
        }
        if ((inc >= '0') && (inc <= '9')) {
          esc = (esc << 4) | (inc - '0');
        }
        else if ((inc >= 'a') && (inc <= 'f')) {
          esc = (esc << 4) | ((inc - 'a') + 0xA);
        }
        else if ((inc >= 'A') && (inc <= 'F')) {
          esc = (esc << 4) | ((inc - 'A') + 0xA);
        }
        pop(ctx); inc = peek(ctx);
      }
      *c = esc;
      return SYM_CHAR;
    }
    else if ((inc >= '0') && (inc <= '7')) {
      while ((inc >= '0') && (inc <= '7')) {
        if (0xE0 & esc) {
          /* XXX: Overflow, consider throwing error */
          return 0;
        }
        esc = (esc << 3) | (inc - '0');
        pop(ctx); inc = peek(ctx);
      }
      *c = esc;
      return SYM_CHAR;
    }
    /* Illicit escape */
    /* XXX: Throw error */
    return 0;
  }
  if ((inc >= 0) && (inc <= 127) && isprint(inc)) {
    *c = inc;
    pop(ctx);
    return SYM_CHAR;
  }
  if (is_in_set(inc, " \t\v\r\n\b\a\f")) {
    /* Whitespace tolerated */
    *c = inc;
    pop(ctx);
    return SYM_CHAR;
  }
  /* Non-ASCII (>127 or UTF-8) character most likely */
  /* XXX: Throw error */
  return 0;
}

static void push_state(struct sym_stack *stack, int state) {
  if (stack->num_stack_allocated == stack->pos) {
    size_t new_num_stack_allocated;
    if (!stack->num_stack_allocated) {
      new_num_stack_allocated = 16;
    }
    else {
      new_num_stack_allocated = stack->num_stack_allocated * 2;
    }
    void *p;

    p = arealloc(stack->syms, new_num_stack_allocated, sizeof(union sym_data_union));
    if (!p) return;
    stack->syms = (union sym_data_union *)p;

    p = arealloc(stack->states, new_num_stack_allocated, sizeof(int));
    if (!p) return;
    stack->states = (int *)p;

    stack->num_stack_allocated = new_num_stack_allocated;
  }
  stack->states[stack->pos++] = state;
}

static int top_state(struct sym_stack *stack) {
  assert(stack->pos && "Invalid stack position, should never be empty");
  return stack->states[stack->pos - 1];
}

static void popn_state(struct sym_stack *stack, size_t num_pops) {
  assert((stack->pos > num_pops) && "Cannot pop down to zero or below");
  stack->pos -= num_pops;
}

void nfa_init(struct nfa *nfa) {
  nfa->nfa_nodes = NULL;
  nfa->num_nfa_nodes = nfa->num_nfa_nodes_allocated = 0;
  nfa->free_nfa = ~0;
  nfa->start_nfa = 0;
  nfa->stop_nfa = 0;
}

void nfa_cleanup(struct nfa *nfa) {
  if (nfa->nfa_nodes) {
    free(nfa->nfa_nodes);
  }
}

static struct nfa_trans *nfa_clone_transition_chain(size_t dst_offset, struct nfa_trans *src) {
  if (!src) {
    return NULL;
  }
  struct nfa_trans *trans = (struct nfa_trans *)malloc(sizeof(struct nfa_trans));
  trans->node = src->node + dst_offset;
  trans->next_trans = nfa_clone_transition_chain(dst_offset, src->next_trans);
  return trans;
}

int nfa_merge_nfas(struct nfa *dst, struct nfa *src, size_t *new_src_end_node) {
  size_t new_num_dst_nfa_nodes = dst->num_nfa_nodes + src->num_nfa_nodes;
  if (new_num_dst_nfa_nodes < dst->num_nfa_nodes) {
    /* overflow */
    return -2;
  }
  if (new_num_dst_nfa_nodes > dst->num_nfa_nodes_allocated) {
    size_t new_dst_num_nodes_allocated;
    if (dst->num_nfa_nodes_allocated) {
      new_dst_num_nodes_allocated = 2 * dst->num_nfa_nodes_allocated;
      if (new_dst_num_nodes_allocated < dst->num_nfa_nodes_allocated) {
        /* overflow */
        return -2;
      }
      if (new_dst_num_nodes_allocated < new_num_dst_nfa_nodes) {
        new_dst_num_nodes_allocated = new_num_dst_nfa_nodes;
      }
    }
    else {
      new_dst_num_nodes_allocated = new_num_dst_nfa_nodes;
    }
    void *p = arealloc(dst->nfa_nodes, new_dst_num_nodes_allocated, sizeof(struct nfa_node));
    if (!p) {
      /* no mem */
      return -1;
    }
    dst->nfa_nodes = (struct nfa_node *)p;
    dst->num_nfa_nodes_allocated = new_dst_num_nodes_allocated;
  }
  /* dst is now guaranteed to have enough space to fit all source NFAs;
   * copy the two together, this also involves duplicating the transitions.. */
  size_t n;
  for (n = 0; n < src->num_nfa_nodes; ++n) {
    struct nfa_node *dst_node = &dst->nfa_nodes[dst->num_nfa_nodes + n];
    struct nfa_node *src_node = &src->nfa_nodes[n];
    dst_node->empty_move = nfa_clone_transition_chain(dst->num_nfa_nodes, src_node->empty_move);
    int c;
    for (c = 0; c < 256; ++c) {
      dst_node->moves[c] = nfa_clone_transition_chain(dst->num_nfa_nodes, src_node->moves[c]);
    }
  }

  if (!dst->num_nfa_nodes) {
    /* dst was an empty NFA, so this is a straight-out duplication with
     * no need to join up transitions */
    dst->start_nfa = src->start_nfa;
    dst->stop_nfa = src->stop_nfa;

    *new_src_end_node = dst->stop_nfa;

    dst->num_nfa_nodes = src->num_nfa_nodes;

    /* Already done! */
    return 0;
  }

  /* Determine clones equivalent to src's start and end in dst.. */
  size_t start_of_clones = src->start_nfa + dst->num_nfa_nodes;
  *new_src_end_node = src->stop_nfa + dst->num_nfa_nodes;

  dst->num_nfa_nodes += src->num_nfa_nodes;

  /* Destination now has valid clones for all source nodes and their transitions, next, make
   * sure the clones can be reached */
  make_empty_trans(dst, dst->start_nfa, start_of_clones);

  /* Done! */
  return 0;
}

#if !USE_PRECOMPILED_PARSETABLE
static int nfa_get_parsetable(int **parse_table, size_t **production_lengths, int **production_syms,
                              int *minimum_sym, size_t *num_columns, size_t *num_rows, size_t *num_productions) {
  struct xlr_gen xlalr_gen;
  xlr_init(&xlalr_gen);
  xlr_error_t err;
  err = xlr_generate(&xlalr_gen, grammar, RULE_END, GRAMMAR_END, INPUT_END, NT_END, SYNTH_S);
  assert((err == XLR_OK) && "Parser grammar is static and should always compile");

  size_t row, col;
  *minimum_sym = xlalr_gen.min_s;
  *num_columns = 1 + xlalr_gen.max_s - xlalr_gen.min_s;
  *num_rows = xlalr_gen.nr_states;
  *num_productions = xlalr_gen.nr_productions;
  *parse_table = (int *)malloc(*num_columns * *num_rows * sizeof(int));
  *production_lengths = (size_t *)malloc(*num_productions * sizeof(size_t));
  *production_syms = (int *)malloc(*num_productions * sizeof(int));
  for (row = 0; row < *num_rows; ++row) {
    for (col = 0; col < *num_columns; ++col) {
      xlr_action_t *action = xlalr_gen.action_table[*num_columns * row + col];
      if (!action) {
        /* 0 = no action, syntax error */
        (*parse_table)[*num_columns * row + col] = 0;
      }
      else if (action->action == XLR_SHIFT) {
        /* positive = shift + 1 */
        (*parse_table)[*num_columns * row + col] = action->state->ordinal + 1;
      }
      else if (action->action == XLR_REDUCE) {
        /* negative = -production - 1 */
        (*parse_table)[*num_columns * row + col] = -action->production - 1;
      }
    }
  }
  for (row = 0; row < *num_productions; ++row) {
    (*production_lengths)[row] = xlalr_gen.productions[row].production_length;
    (*production_syms)[row] = xlalr_gen.productions[row].reduction_syms[0];
  }
  xlr_cleanup(&xlalr_gen);
#if WRITE_PARSETABLE_TO_FILE
  FILE *fp = fopen("parsetable.c", "wb");
  fprintf(fp, "static int minimum_sym = %d;\n", *minimum_sym);
  fprintf(fp, "static size_t num_columns = %u;\n", (unsigned int)(*num_columns));
  fprintf(fp, "static int parse_table[] = {\n");
  for (row = 0; row < *num_rows; ++row) {
    for (col = 0; col < *num_columns; ++col) {
      fprintf(fp, "%s%d", col ? ", " : "  ", (*parse_table)[(*num_columns) * row + col]);
    }
    fprintf(fp, "%s\n", (row != ((*num_rows) - 1)) ? "," : "");
  }
  fprintf(fp, "};\n");
  fprintf(fp, "static size_t production_lengths[] = {\n");
  for (row = 0; row < *num_productions; ++row) {
    fprintf(fp, "  %d%s", (*production_lengths)[row], row == ((*num_productions) - 1) ? "\n" : ",\n");
  }
  fprintf(fp, "};\n");
  fprintf(fp, "static int production_syms[] = {\n");
  for (row = 0; row < *num_productions; ++row) {
    fprintf(fp, "  %d%s", (*production_syms)[row], row == ((*num_productions) - 1) ? "\n" : ",\n");
  }
  fprintf(fp, "};\n");
  fclose(fp);
#endif

  return 0;
}

static int nfa_release_parsetable(int **parse_table, size_t **production_lengths, int **production_syms) {
  if (*parse_table) {
    free(*parse_table);
    *parse_table = NULL;
  }
  if (*production_lengths) {
    free(*production_lengths);
    *production_lengths = NULL;
  }
  if (*production_syms) {
    free(*production_syms);
    *production_syms = NULL;
  }
  return 0;
}
#else
static int minimum_sym = 3;
static size_t num_columns = 21;
static int parse_table[] = {
  -4, -4, 0, -4, 0, -4, -4, -4, 0, -4, 0, 0, 0, -4, 2, 3, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0,
  5, 6, -2, 7, 0, 8, 9, 10, 0, -2, 0, 0, 0, -2, 0, 0, 11, 12, 0, 0, 0,
  -4, -4, -4, -4, 0, -4, -4, -4, 0, -4, 0, 0, 0, -4, 0, 13, 0, 0, 0, 0, 0,
  -10, -10, -10, -10, 0, -10, -10, -10, 0, -10, -10, -10, -10, -10, 0, 0, 0, 0, 0, 0, 0,
  -4, -4, -4, -4, 0, -4, -4, -4, 0, -4, 0, 0, 0, 0, 14, 3, 0, 0, 0, 0, 0,
  15, 0, 0, 0, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 17, 18, 0,
  -11, -11, -11, -11, 0, -11, -11, -11, 0, -11, -11, -11, -11, -11, 0, 0, 0, 0, 0, 0, 0,
  -13, -13, -13, -13, 0, -13, -13, -13, 0, -13, -13, -13, -13, -13, 0, 0, 0, 0, 0, 0, 0,
  -12, -12, -12, -12, 0, -12, -12, -12, 0, -12, -12, -12, -12, -12, 0, 0, 0, 0, 0, 0, 0,
  -5, -5, -5, -5, 0, -5, -5, -5, 0, -5, 0, 0, 0, -5, 0, 0, 0, 0, 0, 0, 0,
  -6, -6, -6, -6, 0, -6, -6, -6, 0, -6, 19, 20, 21, -6, 0, 0, 0, 0, 0, 0, 0,
  5, 6, -3, 7, 0, 8, 9, 10, 0, -3, 0, 0, 0, -3, 0, 0, 11, 12, 0, 0, 0,
  0, 0, 22, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  -19, 0, 0, 0, -19, 0, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 24, 18, 0,
  15, 0, 0, 0, 25, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 26, 0,
  -18, 0, 0, 0, -18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  -7, -7, -7, -7, 0, -7, -7, -7, 0, -7, 0, 0, 0, -7, 0, 0, 0, 0, 0, 0, 0,
  -8, -8, -8, -8, 0, -8, -8, -8, 0, -8, 0, 0, 0, -8, 0, 0, 0, 0, 0, 0, 0,
  -9, -9, -9, -9, 0, -9, -9, -9, 0, -9, 0, 0, 0, -9, 0, 0, 0, 0, 0, 0, 0,
  -14, -14, -14, -14, 0, -14, -14, -14, 0, -14, -14, -14, -14, -14, 0, 0, 0, 0, 0, 0, 0,
  27, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  15, 0, 0, 0, 28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 26, 0,
  -15, -15, -15, -15, 0, -15, -15, -15, 0, -15, -15, -15, -15, -15, 0, 0, 0, 0, 0, 0, 0,
  -17, 0, 0, 0, -17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  -20, 0, 0, 0, -20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  -16, -16, -16, -16, 0, -16, -16, -16, 0, -16, -16, -16, -16, -16, 0, 0, 0, 0, 0, 0, 0
};
static size_t production_lengths[] = {
1,
1,
3,
0,
2,
1,
2,
2,
2,
1,
1,
1,
1,
3,
3,
4,
2,
1,
1,
3
};
static int production_syms[] = {
23,
17,
17,
18,
18,
19,
19,
19,
19,
20,
20,
20,
20,
20,
20,
20,
21,
21,
22,
22
};
#endif

int nfa_parse_regexp(struct nfa *nfa, const char *regexp) {
  int res;
#if !USE_PRECOMPILED_PARSETABLE
  int minimum_sym;
  size_t num_columns;
  size_t num_rows;
  size_t num_productions;
  int *parse_table = NULL;
  size_t *production_lengths = NULL;
  int *production_syms = NULL;
  res = nfa_get_parsetable(&parse_table, &production_lengths, &production_syms, &minimum_sym, &num_columns, &num_rows, &num_productions);
#endif

  struct nfa_parse_context ctx;
  ctx.is.input_pointer = regexp;
  ctx.nfa.nfa_nodes = NULL;
  ctx.nfa.num_nfa_nodes = ctx.nfa.num_nfa_nodes_allocated = 0;
  ctx.nfa.free_nfa = ~0;
 
  struct sym_stack stack;
  stack.pos = 0;
  stack.num_stack_allocated = 0;
  stack.states = NULL;
  stack.syms = NULL;
  push_state(&stack, 0);
  memset(&stack.syms[stack.pos - 1], 0, sizeof(union sym_data_union));

  int failed = 0;
  int not_done = 1;
  size_t start_nfa = ~0, end_nfa = ~0;
  while (not_done) {
    char c;

    if (peek(&ctx)) {
      res = scan_token(&ctx, &c);
    }
    else {
      res = INPUT_END;
      not_done = 0;
    }

    if (!res) {
      /* Error scanning! */
      failed = -2;
      break;
    }

    int current_state = top_state(&stack);
    int action = parse_table[num_columns * current_state + (res - minimum_sym)];
    if (!action) {
      /* Syntax error */
      failed = -1;
      break;
    }

    while (action < 0) {
      int production = -action - 1;
      size_t prod_length = production_lengths[production];
      int nonterminal = production_syms[production];
      popn_state(&stack, prod_length);

      if (0 == production) {
        /* Synthetic S production reduced! We're done */
        start_nfa = stack.syms[stack.pos].nfa.initial;
        end_nfa = stack.syms[stack.pos].nfa.final;
        failed = 0;
        not_done = 0;
        break;
      }

      if (!reduce(&ctx, production, stack.syms + stack.pos)) {
        /* Semantic error */
        failed = -1;
        not_done = 0;
        break;
      }

      current_state = top_state(&stack);
      action = parse_table[num_columns * current_state + (nonterminal - minimum_sym)];
      assert(action && "Cannot shift an already reduced nonterminal??");
      if (!action) {
        failed = -1;
        not_done = 0;
        break;
      }
      assert((action > 0) && "reduced non-terminal should always shift!");
      if (action < 0) {
        failed = -1;
        not_done = 0;
        break;
      }
      
      push_state(&stack, action - 1 /* action for a shift is ordinal + 1 */);

      current_state = top_state(&stack); /* == action->state->ordinal */
      action = parse_table[num_columns * current_state + (res - minimum_sym)];

      if (!action) {
        /* illegal action, syntax error */
        failed = -1;
        not_done = 0;
        break;
      }
    }
    if (!not_done) {
      break;
    }
    if (action > 0 /* == shift action */) {
      push_state(&stack, action - 1);
      stack.syms[stack.pos - 1].sym = (int)(uint8_t)c;
    }
    else {
      /* Illegal action, syntax error. */
      failed = -1;
      not_done = 0;
      break;
    }
  }

  nfa_cleanup(nfa);
  *nfa = ctx.nfa;
  if (stack.states) {
    free(stack.states);
  }
  if (stack.syms) {
    free(stack.syms);
  }
  nfa->start_nfa = start_nfa;
  nfa->stop_nfa = end_nfa;

#if !USE_PRECOMPILED_PARSETABLE
  nfa_release_parsetable(&parse_table, &production_lengths, &production_syms);
#endif

  return failed;
}
