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

#ifndef STDLIB_H_INCLUDED
#define STDLIB_H_INCLUDED
#include <stdlib.h>
#endif

#ifndef STDINT_H_INCLUDED
#define STDINT_H_INCLUDED
#include <stdint.h>
#endif

#ifndef STRING_H_INCLUDED
#define STRING_H_INCLUDED
#include <string.h>
#endif

#ifndef XLTS_H_INCLUDED
#define XLTS_H_INCLUDED
#include "xlts.h"
#endif

#ifndef ASSERT_H_INCLUDED
#define ASSERT_H_INCLUDED
#include <assert.h>
#endif

#ifndef SYMBOL_H_INCLUDED
#define SYMBOL_H_INCLUDED
#include "symbol.h"
#endif


void symbol_table_init(struct symbol_table *st) {
  memset(st->hash_table_, 0, sizeof(st->hash_table_));
  st->non_terminals_ = NULL;
  st->terminals_ = NULL;
}

void symbol_table_cleanup(struct symbol_table *st) {
  struct symbol *sym, *next;
  sym = st->non_terminals_;
  if (sym) {
    next = sym->next_; /* next == head of list */
    do {
      sym = next;
      next = sym->next_;

      symbol_cleanup(sym);
      free(sym);

    } while (sym != st->non_terminals_); /* while not last */
    st->non_terminals_ = NULL;
  }
  sym = st->terminals_;
  if (sym) {
    next = sym->next_;
    do {
      sym = next;
      next = sym->next_;

      symbol_cleanup(sym);
      free(sym);

    } while (sym != st->terminals_);
    st->terminals_ = NULL;
  }
}


void symbol_init(struct symbol *sym) {
  sym->st_ = SYM_UNDEFINED;
  xlts_init(&sym->def_);
  sym->ordinal_ = 0;
  sym->next_ = NULL;
  sym->hash_chain_ = NULL;
  snippet_init(&sym->type_snippet_);
  sym->assigned_type_ = NULL;
}

void symbol_cleanup(struct symbol *sym) {
  snippet_cleanup(&sym->type_snippet_);
  xlts_cleanup(&sym->def_);
}

static uint8_t sym_standardize_char(uint8_t c) {
  if (c == '-') c = '_';
  if ((c >= 'a') && (c <= 'z')) c = c + 'A' - 'a';
  return c;
}

static int sym_hash(const char *id) {
  uint64_t hash_value = 0;
  while (*id) {
    uint8_t c = sym_standardize_char(*(uint8_t *)id);
    /* Dashes convert to underscores, letters are all upper-cased, to ensure the symbols are unique *after* hashing */
    hash_value = (hash_value << 7) | (hash_value >> ((-7) & 63));
    hash_value += c;
    id++;
  }
  return (int)(hash_value % SYMBOL_TABLE_SIZE);
}

static int sym_cmp(const char *left, const char *right) {
  while (*left && *right) {
    uint8_t left_value = sym_standardize_char((uint8_t)*left);
    uint8_t right_value = sym_standardize_char((uint8_t)*right);
    if (left_value < right_value) return -1;
    if (left_value > right_value) return 1;
    left++;
    right++;
  }
  if (*left) {
    return 1;
  }
  if (*right) {
    return -1;
  }
  return 0;
}

struct symbol *symbol_find_or_add(struct symbol_table *st, sym_type_t symtype, struct xlts *id, int *is_new) {
  int idx = sym_hash(id->translated_);
  struct symbol *sym, *last;
  sym = last = st->hash_table_[idx];
  if (sym) {
    do {
      sym = sym->hash_chain_;

      if (!sym_cmp(sym->def_.translated_, id->translated_)) {
        *is_new = 0;
        return sym;
      }

    } while (sym != last);
  }

  sym = (struct symbol *)malloc(sizeof(struct symbol));
  if (!sym) {
    return NULL;
  }
  symbol_init(sym);
  sym->st_ = symtype;
  sym->hash_chain_ = last ? last->hash_chain_ : sym;
  if (last) {
    last->hash_chain_ = sym;
  }
  st->hash_table_[idx] = sym;
  sym->ordinal_ = 0;
  xlts_append(&sym->def_, id);
  if (symtype == SYM_NONTERMINAL) {
    if (st->non_terminals_) {
      sym->next_ = st->non_terminals_->next_;
      st->non_terminals_->next_ = sym;
    }
    else {
      sym->next_ = sym;
    }
    st->non_terminals_ = sym;
  }
  else /* symtype == SYM_TERMINAL */ {
    assert(symtype == SYM_TERMINAL);
    if (st->terminals_) {
      sym->next_ = st->terminals_->next_;
      st->terminals_->next_ = sym;
    }
    else {
      sym->next_ = sym;
    }
    st->terminals_ = sym;
  }

  *is_new = 1;
  return sym;
}


struct symbol *symbol_find(struct symbol_table *st, const char *id) {
  int idx = sym_hash(id);
  struct symbol *sym, *last;
  sym = last = st->hash_table_[idx];
  if (sym) {

    do {
      sym = sym->hash_chain_;
      
      if (!sym_cmp(sym->def_.translated_, id)) {
        return sym;
      }

    } while (sym != last);
  }

  return NULL; /* not found */
}

struct symbol *symbol_find_by_ordinal(struct symbol_table *st, int n) {
  struct symbol *sym;
  sym = st->terminals_;
  if (sym) {
    do {
      if (sym->ordinal_ == n) return sym;

      sym = sym->next_;
    } while (sym != st->terminals_);
  }

  sym = st->non_terminals_;
  if (sym) {
    do {
      if (sym->ordinal_ == n) return sym;

      sym = sym->next_;
    } while (sym != st->non_terminals_);
  }
  return NULL;
}
