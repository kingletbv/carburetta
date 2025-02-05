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

#ifndef STDIO_H_INCLUDED
#define STDIO_H_INCLUDED
#include <stdio.h>
#endif

#ifndef STDLIB_H_INCLUDED
#define STDLIB_H_INCLUDED
#include <stdlib.h>
#endif

#ifndef STRING_H_INCLUDED
#define STRING_H_INCLUDED
#include <string.h>
#endif

#ifndef HELPERS_H_INCLUDED
#define HELPERS_H_INCLUDED
#include "helpers.h"
#endif

#ifndef KLT_LOGGER_H_INCLUDED
#define KLT_LOGGER_H_INCLUDED
#define KLT_LOG_MODULE "symtab"
#include "klt_logger.h"
#endif

#ifndef NAME_SPACE_H_INCLUDED
#define NAME_SPACE_H_INCLUDED
#include "name_space.h"
#endif


void ns_init(struct name_space *ns) {
  ns->parent_ = NULL;

  st_init(&ns->ordinary_idents_);
  st_init(&ns->struct_tags_);
  st_init(&ns->union_tags_);
  st_init(&ns->enum_tags_);
  st_init(&ns->stmt_labels_);
}

void ns_cleanup(struct name_space *ns) {
  st_cleanup(&ns->ordinary_idents_);
  st_cleanup(&ns->struct_tags_);
  st_cleanup(&ns->union_tags_);
  st_cleanup(&ns->enum_tags_);
  st_cleanup(&ns->stmt_labels_);
}

struct sym *ns_find_ordinary_ident(struct name_space *ns, const char *id) {
  struct sym *s;
  do {
    s = st_find(&ns->ordinary_idents_, id);
    if (s) return s;
    ns = ns->parent_;
  } while (ns);

  return NULL;
}

struct ns_tagged_type_sym *ns_find_struct(struct name_space *ns, const char *id) {
  struct sym *s;
  do {
    s = st_find(&ns->struct_tags_, id);
    if (s) return (struct ns_tagged_type_sym *)s;
    ns = ns->parent_;
  } while (ns);

  return NULL;
}

struct ns_tagged_type_sym *ns_find_union(struct name_space *ns, const char *id) {
  struct sym *s;
  do {
    s = st_find(&ns->union_tags_, id);
    if (s) return (struct ns_tagged_type_sym *)s;
    ns = ns->parent_;
  } while (ns);

  return NULL;
}

struct ns_tagged_type_sym *ns_find_enum(struct name_space *ns, const char *id) {
  struct sym *s;
  do {
    s = st_find(&ns->enum_tags_, id);
    if (s) return (struct ns_tagged_type_sym *)s;
    ns = ns->parent_;
  } while (ns);

  return NULL;
}
