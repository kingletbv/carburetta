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

#ifndef TYPESTR_H
#define TYPESTR_H

#ifndef SNIPPET_H_INCLUDED
#define SNIPPET_H_INCLUDED
#include "snippet.h"
#endif

#ifndef STDDEF_H_INCLUDED
#define STDDEF_H_INCLUDED
#include <stddef.h> /* size_t */
#endif

#ifndef STDINT_H_INCLUDED
#define STDINT_H_INCLUDED
#include <stdint.h> /* uint64_t */
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define TYPESTR_TABLE_SIZE 127

struct typestr {
  struct typestr *hash_chain_;
  uint64_t hash_;

  /* Sequence of tokens that make up the type description, may
   * contain a single "$" TOK_SPECIAL_IDENT to denote the position
   * at which the identifier would appear. If no "$" placeholder
   * exists, one is implied to exist at the end of the snippet.
   * Note that a "$" placeholder may not explicitly exist at the
   * end of the snippet as this would remove the uniqueness of
   * a type definition. */
  struct snippet typestr_snippet_;
  
  int ordinal_;

  struct snippet constructor_snippet_;
  struct snippet destructor_snippet_;
};

struct typestr_table {
  struct typestr *hash_table_[TYPESTR_TABLE_SIZE];

  /* Array of pointers for all typestrs, index of array corresponds to typestr ordinal. */
  size_t num_typestrs_;
  size_t num_typestrs_allocated_;
  struct typestr **typestrs_; /* NOTE: Array of pointers, not objects, so we don't disturb the hash lists */
};

void typestr_table_init(struct typestr_table *tt);
void typestr_table_cleanup(struct typestr_table *tt);

struct typestr *typestr_find_or_add(struct typestr_table *tt, const struct snippet *typestr_snippet, int *is_new);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* TYPESTR_H */
