/* Copyright 2020-2023 Kinglet B.V.
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

#ifndef MODE_H
#define MODE_H

#ifndef XLTS_H_INCLUDED
#define XLTS_H_INCLUDED
#include "xlts.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define MODE_TABLE_SIZE 31

struct rex_mode;

struct mode {
  /* Name and source location of definition */
  struct xlts def_;

  /* Next in mode_table::modes_ */
  struct mode *next_;

  /* Next in hash table */
  struct mode *hash_chain_;

  /* Regular expression scantable builder's equivalent mode
   * (Not owned by struct mode) */
  struct rex_mode *rex_mode_;
};

struct mode_table {
  struct mode *mode_table_[MODE_TABLE_SIZE];
  struct mode *modes_;
};

void mode_table_init(struct mode_table *mt);
void mode_table_cleanup(struct mode_table *mt);
void mode_init(struct mode *m);
void mode_cleanup(struct mode *m);

struct mode *mode_find_or_add(struct mode_table *mt, struct xlts *id, int *is_new);
struct mode *mode_find(struct mode_table *mt, const char *id);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* MODE_H */
