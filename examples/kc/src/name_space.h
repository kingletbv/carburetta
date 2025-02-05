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

#ifndef NAME_SPACE_H
#define NAME_SPACE_H

#ifndef SYMTAB_H_INCLUDED
#define SYMTAB_H_INCLUDED
#include "symtab.h"
#endif

#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED
#include "types.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct name_space {
  struct name_space *parent_;

  struct symtab ordinary_idents_;
  struct symtab struct_tags_;
  struct symtab union_tags_;
  struct symtab enum_tags_;
  struct symtab stmt_labels_;
};

struct ns_label_sym {
  struct sym sym_;
  struct stmt *labeled_stmt_;
};

struct ns_tagged_type_sym {
  struct sym sym_;
  struct type_node *type_;
};

void ns_init(struct name_space *ns);
void ns_cleanup(struct name_space *ns);

struct sym *ns_find_ordinary_ident(struct name_space *ns, const char *id);
struct ns_tagged_type_sym *ns_find_struct(struct name_space *ns, const char *id);
struct ns_tagged_type_sym *ns_find_union(struct name_space *ns, const char *id);
struct ns_tagged_type_sym *ns_find_enum(struct name_space *ns, const char *id);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NAME_SPACE_H */
