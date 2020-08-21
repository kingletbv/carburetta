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

#ifndef XLALR_H_INCLUDED
#define XLALR_H_INCLUDED
#include "xlalr.h"
#endif

#ifndef TOKENIZER_H_INCLUDED
#define TOKENIZER_H_INCLUDED
#include "tokenizer.h"
#endif

#ifndef KLT_LOGGER_H_INCLUDED
#define KLT_LOGGER_H_INCLUDED
#define KLT_LOG_MODULE "prd"
#include "klt_logger.h"
#endif

#define PRD_SYMBOL_ENUM \
  xx(NT_END) \
  xx(RULE_END) \
  xx(GRAMMAR_END) \
\
  xx(PRD_IDENT) \
  xx(PRD_COLON) \
  xx(PRD_SEMICOLON) \
  xx(PRD_TOKEN) \
  xx(PRD_PAR_OPEN) \
  xx(PRD_PAR_CLOSE) \
  xx(PRD_CUBRACE_OPEN) \
  xx(PRD_CUBRACE_CLOSE) \
\
  xx(INPUT_END) \
\
  xx(PRD_GRAMMAR) \
  xx(PRD_RULE) \
  xx(PRD_PRODUCTION) \
  xx(PRD_ACTION_SEQUENCE) \
  xx(PRD_STMT_ACTION) \
  xx(PRD_COMP_ACTION) \
  xx(PRD_NONTERMINAL) \
\
  xx(SYNTH_S)

typedef enum prd_symbol_enum {
#define xx(ident) ident,
  PRD_SYMBOL_ENUM
#undef xx
} prd_symbol_t;

static int g_grammar_[] = {
  PRD_GRAMMAR, NT_END, RULE_END,
  PRD_GRAMMAR, NT_END, PRD_PRODUCTION, PRD_GRAMMAR, RULE_END,

  PRD_PRODUCTION, NT_END, PRD_NONTERMINAL, PRD_COLON, PRD_RULE, PRD_SEMICOLON, RULE_END,
  PRD_PRODUCTION, NT_END, PRD_NONTERMINAL, PRD_COLON, PRD_RULE, PRD_STMT_ACTION, PRD_SEMICOLON, RULE_END,
  PRD_PRODUCTION, NT_END, PRD_NONTERMINAL, PRD_COLON, PRD_RULE, PRD_COMP_ACTION, PRD_SEMICOLON, RULE_END,

  PRD_NONTERMINAL, NT_END, PRD_IDENT, RULE_END,

  PRD_RULE, NT_END, RULE_END,
  PRD_RULE, NT_END, PRD_RULE, PRD_IDENT, RULE_END,

  PRD_STMT_ACTION, NT_END, RULE_END,
  PRD_STMT_ACTION, NT_END, PRD_STMT_ACTION, PRD_IDENT, RULE_END,
  PRD_STMT_ACTION, NT_END, PRD_STMT_ACTION, PRD_COLON, RULE_END,
  PRD_STMT_ACTION, NT_END, PRD_STMT_ACTION, PRD_TOKEN, RULE_END,
  PRD_STMT_ACTION, NT_END, PRD_STMT_ACTION, PRD_PAR_OPEN, PRD_ACTION_SEQUENCE, PRD_PAR_CLOSE, RULE_END,
  PRD_STMT_ACTION, NT_END, PRD_STMT_ACTION, PRD_CUBRACE_OPEN, PRD_ACTION_SEQUENCE, PRD_CUBRACE_CLOSE, RULE_END,

  PRD_COMP_ACTION, NT_END, PRD_CUBRACE_OPEN, PRD_ACTION_SEQUENCE, PRD_CUBRACE_CLOSE, RULE_END,

  PRD_ACTION_SEQUENCE, NT_END, RULE_END,
  PRD_ACTION_SEQUENCE, NT_END, PRD_ACTION_SEQUENCE, PRD_IDENT, RULE_END,
  PRD_ACTION_SEQUENCE, NT_END, PRD_ACTION_SEQUENCE, PRD_COLON, RULE_END,
  PRD_ACTION_SEQUENCE, NT_END, PRD_ACTION_SEQUENCE, PRD_SEMICOLON, RULE_END,
  PRD_ACTION_SEQUENCE, NT_END, PRD_ACTION_SEQUENCE, PRD_TOKEN, RULE_END,
  PRD_ACTION_SEQUENCE, NT_END, PRD_ACTION_SEQUENCE, PRD_PAR_OPEN, PRD_ACTION_SEQUENCE, PRD_PAR_CLOSE, RULE_END,
  PRD_ACTION_SEQUENCE, NT_END, PRD_ACTION_SEQUENCE, PRD_CUBRACE_OPEN, PRD_ACTION_SEQUENCE, PRD_CUBRACE_CLOSE, RULE_END,


  GRAMMAR_END
};

static int prd_get_parsetable(int **parse_table, size_t **production_lengths, int **production_syms,
                              int *minimum_sym, size_t *num_columns, size_t *num_rows, size_t *num_productions) {
  xlr_gen_t xlalr_gen;
  xlr_init(&xlalr_gen);
  xlr_error_t err;
  err = xlr_generate(&xlalr_gen, g_grammar_, RULE_END, GRAMMAR_END, INPUT_END, NT_END, SYNTH_S);
  if (err != XLR_OK) {
    return err;
  }
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
        (*parse_table)[*num_columns * row + col] = -action->production - 1;
      }
    }
  }
  for (row = 0; row < *num_productions; ++row) {
    (*production_lengths)[row] = xlalr_gen.productions[row].production_length;
    (*production_syms)[row] = xlalr_gen.productions[row].reduction_syms[0];
  }
  xlr_cleanup(&xlalr_gen);

  return err;
}


static int minimum_sym;
static size_t num_columns;
static size_t num_rows;
static size_t num_productions;
static int *parse_table = NULL;
static size_t *production_lengths = NULL;
static int *production_syms = NULL;

int prd_init(void) {
  int res;
  res = prd_get_parsetable(&parse_table, &production_lengths, &production_syms, &minimum_sym, &num_columns, &num_rows, &num_productions);
  if (res != XLR_OK) {
    LOGERROR("Failed to generate parse table.\n");
    return res;
  }

  return 0;
}

void prd_cleanup(void) {
  if (parse_table) free(parse_table);
  if (production_lengths) free(production_lengths);
  if (production_syms) free(production_syms);
}
