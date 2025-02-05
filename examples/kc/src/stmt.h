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

#ifndef STMT_H
#define STMT_H

#ifndef SITUS_H_INCLUDED
#define SITUS_H_INCLUDED
#include "situs.h"
#endif

#ifndef SWITCH_H_INCLUDED
#define SWITCH_H_INCLUDED
#include "switch.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct c_compiler;
struct expr;

enum stmt_type {
  ST_LABEL,
  ST_CASE,
  ST_DEFAULT,
  ST_BLOCK,
  ST_EXPR,
  ST_IF,
  ST_IF_ELSE,
  ST_SWITCH,
  ST_WHILE,
  ST_DO_WHILE,
  ST_FOR,
  ST_GOTO,
  ST_CONTINUE,
  ST_BREAK,
  ST_RETURN,
  ST_NOP
};

struct stmt {
  enum stmt_type type_;
  struct situs location_; /* location for issuing errors (eg. typically the keyword) */

  struct stmt *parent_;
  struct stmt *next_, *prev_;

  struct stmt *parent_block_; /* parent block context when allocating ST_BLOCK preemptively in anticipation of its use */
  
  struct stmt *child0_; /* true branch for if, if-else, children for others, declaration for "for (decl; exp; exp)" */
  struct stmt *child1_; /* false branch for if-else */
  struct situs label_range_; /* location for the label, if there is one. */
  char *label_;         /* label for both label and goto */

  struct name_space *ns_; /* namespace of block if type_ == ST_BLOCK. */

  /* expression is absent if start_ == end_ (eg. both "for (;;)" and "return;" have absent expressions) */
  struct expr *expr0_;
  struct expr *expr1_;
  struct expr *expr2_;

  /* Pointer to the switch-stmt that is a parent of this statement, or NULL if this is not a 
   * ST_CASE, ST_DEFAULT or ST_SWITCH statement, or there is no parent. */
  struct stmt *parent_switch_;

  /* Same as parent_switch but for break statements (meaning it is impacted by switches and loops) */
  struct stmt *break_parent_;

  /* Same as parent_switch but for continue statements (meaning it is impacted only by loops) */
  struct stmt *continue_parent_;

  /* All case statements linked up to this switch (this must be a ST_SWITCH). */
  struct switch_case_map cases_;

  /* Goto destination statement (this must be an ST_GOTO.) */
  struct stmt *goto_destination_;
};

struct stmt *stmt_alloc(struct c_compiler *cc, enum stmt_type st, struct situs *location);
void stmt_free(struct stmt *s);
void stmt_set_parent(struct stmt *s, struct stmt *new_parent);

int stmt_prepare(struct c_compiler *cc, struct stmt *root);
int stmt_exec(struct c_compiler *cc, struct stmt *root, void *local_base, void *param_base, void *return_value_ptr);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* STMT_H */
