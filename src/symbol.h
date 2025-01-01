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

#ifndef SYMBOL_H
#define SYMBOL_H

#ifndef XLTS_H_INCLUDED
#define XLTS_H_INCLUDED
#include "xlts.h"
#endif

#ifndef SNIPPET_H_INCLUDED
#define SNIPPET_H_INCLUDED
#include "snippet.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define SYMBOL_TABLE_SIZE 127

typedef enum sym_type_enum {
  SYM_UNDEFINED,
  SYM_TERMINAL,
  SYM_NONTERMINAL
} sym_type_t;

struct symbol {
  /* Type of symbol, either terminal or non-terminal */
  sym_type_t st_;

  /* Name and source location of definition */
  struct xlts def_;

  /* Assigned symbol ordinal for generating tables */
  int ordinal_;

  /* Next in symbol_table::terminals_ or symbol_table::non_terminals_ */
  struct symbol *next_;

  /* Next in hash table */
  struct symbol *hash_chain_;

  /* Snippet describing target language specific type string assigned to symbol.
   * The snippet may hold a single "$" TOK_SPECIAL_IDENT as a placeholder for
   * the declarator identifier of such a type string. If no such a placeholder
   * is found, the placeholder for the identifier is implied at the end. */
  struct snippet type_snippet_;

  /* Assigned type; representing both the type (as type_snippet) and its field
   * value in the union of symbol data on the stack. */
  struct typestr *assigned_type_;
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

/* Finds a symbol by its ordinal value (note: inefficient O(n) implementation!) */
struct symbol *symbol_find_by_ordinal(struct symbol_table *st, int n);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* SYMBOL_H */
