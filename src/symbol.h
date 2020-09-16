#ifndef SYMBOL_H
#define SYMBOL_H

#ifndef XLTS_H_INCLUDED
#define XLTS_H_INCLUDED
#include "xlts.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define SYMBOL_TABLE_SIZE 127

typedef enum sym_type_enum {
  SYM_TERMINAL,
  SYM_NONTERMINAL
} sym_type_t;

struct symbol {
  /* Type of symbol, either terminal or non-terminal */
  sym_type_t st_;

  /* Name and source location of definition */
  struct xlts def_;

  /* Assigned ordinal for generating tables */
  int ordinal_;

  /* Next in symbol_table::terminals_ or symbol_table::non_terminals_ */
  struct symbol *next_;

  /* Next in hash table */
  struct symbol *hash_chain_;
};

struct symbol_table {
  struct symbol *hash_table_[SYMBOL_TABLE_SIZE];

  /* Terminals and non-terminals in order of declaration */
  struct symbol *terminals_;
  struct symbol *non_terminals_;
};

void symbol_table_init(struct symbol_table *st);
void symbol_table_cleanup(struct symbol_table *st);
void symbol_init(struct symbol *sym);
void symbol_cleanup(struct symbol *sym);

/* Returns NULL upon error, or the symbol that was either found or newly created.
 * To distinguish between an existing symbol and one that was created, the flag
 * *is_new is set if a new symbol was created, and cleared if an existing symbol
 * was found. Note that the existing symbol might not be of the same symtype. */
struct symbol *symbol_find_or_add(struct symbol_table *st, sym_type_t symtype, struct xlts *id, int *is_new);

/* Finds a symbol, returns NULL if the symbol could not be found. */
struct symbol *symbol_find(struct symbol_table *st, const char *id);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* SYMBOL_H */
