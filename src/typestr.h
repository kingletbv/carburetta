/* Copyright 2020-2021 Kinglet B.V.
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
  /* Non-zero if this is a symbol type (a type associated with
   * a particular set of symbols, but not all.) */
  int is_symbol_type_:1;

  /* If non-zero, the constructor will not be considered complete
   * if the code snippet exits the parse or scan functions.
   * RAII refers to Resource Acquisition Is Initialization and implies
   * that during construction, an exception may be thrown. If an
   * exception is thrown, any partial construction is unwound and
   * the exception passes transparently through Carburetta's generated
   * C-style code. Because the type is not constructed, it is important
   * that the destructor is *only* called if construction completes
   * normally (i.e. execution of the constructor completes to the end.)
   * The counter example, is_raii_constructor_==0, handles the case
   * where the caller is responsible for part of the construction. In
   * such a scenario, returning out of the parse of scan function is
   * a sign that the constructor completed, not failed, and any 
   * subsequent cleanup must called the destructor. */
  int is_raii_constructor_:1;

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
  struct snippet token_action_snippet_;
};

struct typestr_table {
  /* Array of pointers for all typestrs, index of array corresponds to typestr ordinal. */
  size_t num_typestrs_;
  size_t num_typestrs_allocated_;
  struct typestr **typestrs_; /* NOTE: Array of pointers, not objects, so we don't disturb the hash lists */
};

void typestr_table_init(struct typestr_table *tt);
void typestr_table_cleanup(struct typestr_table *tt);

struct typestr *typestr_add(struct typestr_table *tt, const struct snippet *typestr_snippet);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* TYPESTR_H */
