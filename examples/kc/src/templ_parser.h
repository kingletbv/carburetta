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

#ifndef TEMPL_PARSER_H
#define TEMPL_PARSER_H

#ifndef STDIO_H_INCLUDED
#define STDIO_H_INCLUDED
#include <stdio.h>
#endif

#ifndef STDINT_H_INCLUDED
#define STDINT_H_INCLUDED
#include <stdint.h>
#endif

#ifndef STMT_H_INCLUDED
#define STMT_H_INCLUDED
#include "stmt.h"
#endif

#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED
#include "types.h"
#endif

#ifndef EXPR_H_INCLUDED
#define EXPR_H_INCLUDED
#include "expr.h"
#endif

#ifndef DATA_SECTION_H_INCLUDED
#define DATA_SECTION_H_INCLUDED
#include "data_section.h"
#endif

#ifndef PARTIAL_TYPE_SPECIFIERS_H_INCLUDED
#define PARTIAL_TYPE_SPECIFIERS_H_INCLUDED
#include "partial_type_specifiers.h"
#endif

#ifndef FUNC_DEF_H_INCLUDED
#define FUNC_DEF_H_INCLUDED
#include "func_def.h"
#endif


#ifdef __cplusplus
extern "C" {
#endif

enum templ_input_kind {
  TIK_MEMORY,
  TIK_FILE
};

enum templ_declarator_scaffold_kind {
  TDSK_ARRAY,
  TDSK_FUNCTION,
  TDSK_POINTER,
  TDSK_QUALIFIER
};

enum templ_initializer_kind {
  TINK_LITERAL,
  TINK_COMPOUND
};

enum templ_initializer_designation_kind {
  TINDK_INDEX,
  TINDK_IDENT
};

struct templ_context {
  int is_typedefname_permitted_:1;
  struct stmt *block_;
  struct stmt *current_switch_parent_;
  struct stmt *current_break_parent_;
  struct stmt *current_continue_parent_;
  struct func_def *current_func_;
  struct type_node *decl_spec_type_;
  int decl_spec_fs_; /* function specifier */
  int decl_spec_sc_; /* storage class (SC_xx, e.g. SC_STATIC) */
};

union templ_input_source {
  FILE *fp_;
  void *bytes_;
};

struct templ_input {
  enum templ_input_kind kind_;
  union templ_input_source source_;
  size_t length_;
};

struct templ_declarator_entry {
  struct templ_declarator_entry *chain_; /* chain of siblings in the same declaration. */

  struct templ_declarator_scaffold *tds_;

  char *ident_;
  struct situs ident_loc_;

  int bitfield_size_;
};

/* Temporary scaffold structure used during parsing declarators. The declarator part of the language
 * is structured such that the declaration mimicks the use of that type. For instance, char x[4][5] is
 * an array of 4 arrays to 5 characters. This matches how we'd parse it as an expression (with the [4]
 * being evaluated first) but is inverse from how we parse it as a declaration (with the first evaluation
 * theoretically corresponding to the innermost type, evaluated last.)
 * We build up a temporary scaffold in the parser, and then convert it into a realized type when we combine
 * it with the type specifier ("char" in the example above.)
 */
struct templ_declarator_scaffold {
  enum templ_declarator_scaffold_kind kind_;

  /* Cyclic chain of declarators, points to the super type.
   * eg. if chain_ points to an array type, then this is the element type of that array.
   * This is probably the inverse from the way one might expect it, but makes it possible to build
   * the actual type more easily. In the earlier example, the type char x[4][5] ("array of 4 arrays of
   * 5 characters") is represented as [5]->[4], the innermost type coming first. Building the type
   * from the bottom up is then done by forward stepping through the chain. */
  struct templ_declarator_scaffold *chain_;

  /* Parameters if kind_ is TDSK_FUNCTION */
  struct templ_declarator_scaffold_field *fields_;

  /* Qualifiers (combination of TYPE_QUALIFIER_CONST, TYPE_QUALIFIER_RESTRICT, TYPE_QUALIFIER_VOLATILE)
   * present when kind_ is TDSK_QUALIFIER */
  int qualifiers_;

  /* Array size expression, valid only if kind_ == TDSK_ARRAY. This is NULL if the array is an incomplete
   * type. */
  struct expr *array_size_;

  /* If non-zero, the array is a static array (compiler may assume the array is present in full.)
   * Valid only in argument lists. */
  int is_static_array_:1;

  /* If non-zero, the array is a variable length array (eg. "int x[*]" the '*' indicates this is
   * a variable length array, even though the length expression is not known at this time.)
   * Valid only in argument lists of function declarations. */
  int is_variable_length_array_:1;

  /* If non-zero, the fields_ member being NULL signifies the arguments are unknown, not that it is
   * known that there are no arguments. */
  int has_unknown_arguments_:1;

  /* If non-zero, parameters are listed in the function declaration by only their identifiers, and
   * no type information. */
  int identifiers_only_:1;
};

struct templ_declarator_scaffold_field {
  /* Chain of siblings */
  struct templ_declarator_scaffold_field *chain_;

  /* Identifier of the parameter. */
  char *ident_;
  struct situs ident_loc_;

  /* Type of the parameter */
  struct type_node *type_;

  /* If non-zero, this is not a field but the variadic ellipsis (...) */
  int is_variadic_:1;
};

struct templ_initializer {
  enum templ_initializer_kind kind_;

  struct situs loc_;

  union {
    struct expr *literal_exp_;
    struct templ_initializer_list_node *compound_nodes_;
  } u_;
};

struct templ_initializer_list_node {
  struct templ_initializer_list_node *chain_;
  struct templ_initializer_designation *designators_;
  struct templ_initializer *initializer_;
};

struct templ_initializer_designation {
  enum templ_initializer_designation_kind kind_;
  struct templ_initializer_designation *chain_;
  struct situs loc_;
  union {
    struct expr *index_exp_;
    char *ident_str_;
  } u_;
};

void templ_context_init(struct templ_context *cx);
void templ_context_cleanup(struct templ_context *cx);

void report_error(struct situs *loc, const char *fmt, ...);

struct templ_declarator_scaffold *templ_ds_array(struct templ_declarator_scaffold *super_type, struct expr *array_size_expr, int is_static_array, int is_variable_length_array);
struct templ_declarator_scaffold *templ_ds_qualifier(struct templ_declarator_scaffold *super_type, int qualifiers);
struct templ_declarator_scaffold *templ_ds_function(struct templ_declarator_scaffold *super_type, struct templ_declarator_scaffold_field *params, int identifiers_only);
struct templ_declarator_scaffold *templ_ds_pointer(struct templ_declarator_scaffold *super_type);
struct templ_declarator_scaffold *templ_ds_join(struct templ_declarator_scaffold *front, struct templ_declarator_scaffold *back);
void templ_declarator_scaffold_free_chain(struct templ_declarator_scaffold *tdsf);

struct type_node *templ_type_node_realize(struct c_compiler *cc, pts_type_specifier_t pts, struct type_node *specifier_type, struct situs *spec_loc, int type_qualifier, struct templ_declarator_scaffold *declarator, int *fatality);

struct templ_declarator_scaffold_field *templ_dsf_alloc(struct templ_declarator_scaffold_field *siblings, char *ident, struct situs *ident_loc, struct type_node *tn);
void templ_dsf_free_chain(struct templ_declarator_scaffold_field *dsf);
struct templ_declarator_scaffold_field *templ_dsf_join(struct templ_declarator_scaffold_field *front, struct templ_declarator_scaffold_field *back);
int templ_dsf_are_arguments_known(struct templ_declarator_scaffold_field *arguments);
int templ_dsf_is_isolated_void(struct templ_declarator_scaffold_field *arguments);
struct templ_declarator_scaffold_field *templ_dsf_realize(struct c_compiler *cc, pts_type_specifier_t pts, struct type_node *specifier, struct situs *spec_loc, int type_qualifier, struct templ_declarator_scaffold *declarator, char *ident, struct situs *ident_loc, int *fatality);
struct type_field *templ_type_field_realize(struct c_compiler *cc, pts_type_specifier_t pts, struct type_node *specifier, struct situs *spec_loc, int type_qualifier, struct templ_declarator_scaffold *declarator, char *ident, struct situs *ident_loc, int bitfield_size, int *fatality);

struct templ_declarator_entry *templ_de_alloc(struct templ_declarator_entry *siblings, struct templ_declarator_scaffold *tds, char *ident, struct situs *ident_loc, int bitfield_size);
void templ_de_free_chain(struct templ_declarator_entry *de);
struct templ_declarator_entry *templ_de_join(struct templ_declarator_entry *front, struct templ_declarator_entry *back);

struct type_node *templ_realize_type(struct c_compiler *cc, struct type_node *specifier, struct templ_declarator_scaffold *tds, int *fatality /* set to 1 upon NULL return and memory failure */);

struct type_node *templ_specifier_realize(struct c_compiler *cc, pts_type_specifier_t pts, struct type_node *specifier, struct situs *spec_loc, int type_qualifier);

struct templ_initializer *templ_initializer_literal(struct expr *x, struct situs *loc);
struct templ_initializer *templ_initializer_compound(struct templ_initializer_list_node *nodes, struct situs *loc);
void                      templ_initializer_free(struct templ_initializer *tin);

struct templ_initializer_list_node *templ_initializer_list_node(struct templ_initializer_designation *designators, struct templ_initializer *initializer);
struct templ_initializer_list_node *templ_initializer_list_node_join(struct templ_initializer_list_node *front, struct templ_initializer_list_node *back);
void                                templ_initializer_list_node_free(struct templ_initializer_list_node *tinln);

struct templ_initializer_designation *templ_initializer_designation_index(struct expr *index, struct situs *loc);
struct templ_initializer_designation *templ_initializer_designation_ident(const char *ident, struct situs *loc);
struct templ_initializer_designation *templ_initializer_designation_join(struct templ_initializer_designation *front, struct templ_initializer_designation *back);
void                                  templ_initializer_designation_free(struct templ_initializer_designation *tind);

struct decl_initializer *templ_initializer_realize(struct c_compiler *cc, struct templ_initializer *tin, struct type_node *tn, uint64_t *top_array_observed_length);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* TEMPL_PARSER_H */
