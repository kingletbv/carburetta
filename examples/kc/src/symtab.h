/* Copyright 2023-2025 Kinglet B.V.
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

#ifndef SYMTAB_H
#define SYMTAB_H

#ifndef STDINT_H_INCLUDED
#define STDINT_H_INCLUDED
#include <stdint.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct sym {
  uint64_t hash_value_;
  char *ident_; /* ident_ storage is allocated as part of sym, free()'ing the sym frees the ident_ with it. */
  int is_red_ : 1;
  struct sym *left_, *right_;
  struct sym *next_, *prev_;
};

struct symtab {
  struct sym *root_;
  
  /* All symbols in the table as a cyclic list in order of declaration, points
   * to the first sym declared */
  struct sym *seq_;
};

void st_init(struct symtab *st);

/* Note that st_cleanup detaches all syms from the symtab, but does not clean up the individual
 * syms, as it does not own the link and does not have an awareness of what structure the sym
 * is embedded in. */
void st_cleanup(struct symtab *st);

struct sym *st_find_or_insert(struct symtab *st, const char *value_key, int *s_is_new, size_t sym_size);
struct sym *st_find(struct symtab *st, const char *value_key);

int st_remove(struct symtab *st, struct sym *s);

int st_check_sanity(struct symtab *st);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* SYMTAB_H */
