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

#ifndef FUNC_DEF_H
#define FUNC_DEF_H

#ifndef DECL_H_INCLUDED
#define DECL_H_INCLUDED
#include "decl.h"
#endif

#ifndef STMT_H_INCLUDED
#define STMT_H_INCLUDED
#include "stmt.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct c_compiler;

struct func_def {
  struct decl d_;

  struct stmt *body_;

  uint64_t stackframe_size_required_;
  uint64_t parameter_size_required_;
};

struct func_def *func_def_alloc(struct name_space *ns, int *already_exists, const char *ident, struct situs *ident_loc, sc_storage_class_t sc, int function_specifiers, struct type_node *tn);

/* Assigns decl::local_offset_ for all local variables in the function and, in doing so, determines fd->stackframe_size_required_ */
int func_def_realize_locals(struct c_compiler *cc, struct func_def *fd);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* FUNC_DEF_H */
