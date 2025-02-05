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

#ifndef SWITCH_H
#define SWITCH_H

#ifndef STDINT_H_INCLUDED
#define STDINT_H_INCLUDED
#include <stdint.h>
#endif


#ifdef __cplusplus
extern "C" {
#endif

struct stmt; /* forward declaration from "stmt.h" */

struct switch_case_map {
  struct switch_case *first_; /* first case in next_/prev_ chain */
  struct switch_case *root_;  /* root of red-black tree */
  struct switch_case *default_; /* default case ; cannot exist in RB tree but does exist in chain.*/
};

struct switch_case {
  uint64_t case_value_;
  int is_red_ : 1;
  struct switch_case *left_, *right_;
  struct switch_case *next_, *prev_;

  struct stmt *case_stmt_;
};

void switch_case_map_init(struct switch_case_map *scm);
void switch_case_map_cleanup(struct switch_case_map *scm);

/* Allocate and insert a new switch_case in the switch_case_map scm
 * NOTE: IF the case value already exists, then the *existing* switch_case is returned.
 *       Caller should check that switch_case::case_stmt_ equals case_stmt. */
struct switch_case *switch_alloc_case(struct switch_case_map *scm, struct stmt *case_stmt, uint64_t case_value);
struct switch_case *switch_alloc_default(struct switch_case_map *scm, struct stmt *default_stmt);

void switch_free_case(struct switch_case_map *scm, uint64_t case_value);

struct switch_case *switch_find_case(struct switch_case_map *scm, uint64_t case_value);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* SWITCH_H */
