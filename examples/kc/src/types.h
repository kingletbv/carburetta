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

#ifndef TYPES_H
#define TYPES_H

#ifndef STDIO_H_INCLUDED
#define STDIO_H_INCLUDED
#include <stdio.h>
#endif

#ifndef STDINT_H_INCLUDED
#define STDINT_H_INCLUDED
#include <stdint.h>
#endif

#ifndef SITUS_H_INCLUDED
#define SITUS_H_INCLUDED
#include "situs.h"
#endif

#ifndef EXPR_NODE_TYPES_H_INCLUDED
#define EXPR_NODE_TYPES_H_INCLUDED
#include "expr_node_types.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct c_compiler;
struct expr;

#define TYPE_QUALIFIER_CONST 1
#define TYPE_QUALIFIER_RESTRICT 2
#define TYPE_QUALIFIER_VOLATILE 4

#define TYPE_BASE_HASHTABLE_SIZE 127

enum type_kind {
  tk_invalid,
  tk_char = 1,
  tk_signed_char,
  tk_short_int,
  tk_int,
  tk_long_int,
  tk_long_long_int,
  tk_bool,
  tk_unsigned_char,
  tk_unsigned_short_int,
  tk_unsigned_int,
  tk_unsigned_long_int,
  tk_unsigned_long_long_int,
  tk_float,
  tk_double,
  tk_long_double,
  tk_float_complex,
  tk_double_complex,
  tk_long_double_complex,
  tk_float_imaginary,
  tk_double_imaginary,
  tk_long_double_imaginary,
  tk_void,

  tk_enumeration,

  tk_array,
  tk_structure,
  tk_union,
  tk_function,
  tk_pointer,

  tk_qualifier
};

/* Struct or union field */
struct type_field {
  struct type_field *chain_;

  /* Identifier of the field, optional. */
  char *ident_;
  struct situs ident_loc_;

  /* Type of the field */
  struct type_node *type_;

  /* If non-zero, the number of bits in the bitfield */
  int bitfield_width_;
};

/* Enumeration constants */
struct type_enum_constant {
  struct type_enum_constant *chain_;

  /* Identifier of the constant */
  char *ident_;
  struct situs ident_loc_;

  /* Value of the constant */
  int64_t value_;

  /* Non-zero if the value_ has been initialized
   * (Typically always 1 except during construction) */
  int value_initialized_:1;
};

/* Function parameters */
struct type_param {
  struct type_param *chain_;

  /* Identifier of the parameter (optional) */
  char *ident_;
  struct situs ident_loc_;

  /* Type of the parameter (optional) */
  struct type_node *type_;

  /* If non-zero, this is the variadic ellipsis "..." */
  int is_varadic_:1;
};

struct type_node {
  /* Identifier for the type that is unique. We use this value instead of the type_node's pointer
   * for hashing to ensure consecutive runs produce the same results (thus aiding debugging and
   * reproduction of bugs.) */
  uint32_t id_;
  enum type_kind kind_;
  struct type_node *hash_chain_;
  struct type_node *chain_;

  /* The type this type derives from:
   * For kind_ == tk_array, it is the element type.
   * For kind_ == tk_function, it is the return type.
   * For kind_ == tk_pointer, it is the type pointed to.
   * For kind_ == tk_enumeration, it is the compatible integer type (6.7.2.2) an enum is implemented with. */
  struct type_node *derived_from_;

  /* For simple (builtin) types, the name of the type. Memory is a compile time constant (not owned). */
  const char *label_;

  /* For struct or union, the tagname of the struct or union. Memory owned by the
   * type_node */
  char *tag_;
  struct situs tag_loc_;

  /* For struct or union, a tail cyclic chain of all fields in the struct or union. */
  struct type_field *fields_;

  /* For function, a tail cyclic chain of all parameters */
  struct type_param *params_;

  /* For enumerations, a tail cyclic list of all enumeration constants */
  struct type_enum_constant *enum_constants_;

  /* Combination of TYPE_QUALIFIER_XXX flags, valid *only* if kind_ == tk_qualifier */
  int qualifiers_; 

  /* For arrays (kind == tk_array), if non-zero, the array is a variable length array 
   * (eg. "int x[*]" the '*' indicates this is a variable length array, even though 
   * the length expression is not known at this time.)
   * Valid only in argument lists of function declarations. */
  int is_variable_length_array_:1;

  /* For functions (kind == tk_function), if non-zero, then the function has no
   * known arguments. This helps distinguish the case params_ == NULL. If arguments_unknown_,
   * then params_ == NULL because the arguments are not known. If not arguments_unknown_ and
   * params_ == NULL, then we know there are no arguments. */
  int arguments_unknown_:1;

  /* For functions (kind == tk_function), if non-zero, then the function is an old
   * style K&R function definition where the params_ contain identifiers only (and no
   * types.) Also true if arguments_unknown_ is true. */
  int is_identifier_list_ : 1;

  /* For functions (kind == tk_function), if non-zero, the function type declarator is
   * part of a function definition. If zero, it is part of a prototype declaration. */
  int is_function_def_ : 1;

  /* For arrays (kind_ == tk_array,) the fixed array size (in count of elements)
   * If this is 0, the array is an incomplete type */
  uint64_t array_size_;

  /* For arrays (kind_ == tk_array,) the expression of the array size, or NULL if the
   * array size is not specified. If this is a constant expression, then the actual
   * array_size_ value is also in array_size_. */
  struct expr *array_size_expr_;

};

struct type_base {
  struct type_node *hash_table_[TYPE_BASE_HASHTABLE_SIZE];

  struct type_node *types_;
  uint32_t next_id_;
  
  /* Configuration */

  /* type_kind for an unsigned int the size of pointers */
  enum type_kind uintptr_equivalent_;

  /* type_kind for a signed int the size of pointers (for pointer subtraction etc.) */
  enum type_kind ptrdiff_equivalent_;

  /* number of bits per tk_int; used for determining if an unsigned bitfield can fit
   * in an int during the integer promotions (C99 6.3.1.1-2) */
  int bits_per_int_;
  int bits_per_long_;
  int bits_per_long_long_;

  /* Is "char" a signed type ? */
  int char_is_signed_:1;

  /* Is a string literal a const array ? */
  int string_literals_are_const_:1;

  struct type_node char_;
  struct type_node signed_char_;
  struct type_node short_int_;
  struct type_node int_;
  struct type_node long_int_;
  struct type_node long_long_int_;
  struct type_node bool_;
  struct type_node unsigned_char_;
  struct type_node unsigned_short_int_;
  struct type_node unsigned_int_;
  struct type_node unsigned_long_int_;
  struct type_node unsigned_long_long_int_;
  struct type_node float_;
  struct type_node double_;
  struct type_node long_double_;
  struct type_node float_complex_;
  struct type_node double_complex_;
  struct type_node long_double_complex_;
  struct type_node float_imaginary_;
  struct type_node double_imaginary_;
  struct type_node long_double_imaginary_;
  struct type_node void_;
};

void type_base_init(struct type_base *tb);
void type_base_cleanup(struct type_base *tb);

int64_t type_base_int_type_min(struct type_base *tb, enum type_kind tk);
uint64_t type_base_int_type_max(struct type_base *tb, enum type_kind tk);

int type_kind_is_complex(enum type_kind tk);
int type_kind_is_imaginary(enum type_kind tk);
int type_kind_is_signed(struct type_base *tb, enum type_kind tk);
enum type_kind type_kind_get_real_type(enum type_kind tk);
enum type_kind type_kind_get_type_in_domain(enum type_kind same_domain, enum type_kind real_type);
enum type_kind type_kind_get_unsigned_kind(enum type_kind tk);
void type_kind_classify_integer_type(enum type_kind integerkind, int *is_signed, int *rank);

struct type_node *type_base_simple(struct type_base *tb, enum type_kind tk);
struct type_node *type_base_qualifier(struct type_base *tb, struct type_node *base, int qualifiers);
struct type_node *type_base_pointer(struct type_base *tb, struct type_node *base);
struct type_node *type_base_array(struct c_compiler *cc, struct type_node *element, struct expr *array_size_expr);
struct type_node *type_base_array_fixed_length(struct c_compiler *cc, struct type_node *elm_type, uint64_t array_length);

struct type_node *type_base_alloc_struct_or_union(struct type_base *tb, enum type_kind struct_or_union);
struct type_node *type_base_alloc_enum(struct type_base *tb);
struct type_node *type_base_alloc_func(struct type_base *tb, struct type_node *return_type);

struct type_enum_constant *type_enum_constant_alloc(const char *ident, struct situs *ident_loc);
void type_enum_constant_free_chain(struct type_enum_constant *tec);
struct type_enum_constant *type_enum_constant_join(struct type_enum_constant *front, struct type_enum_constant *back);

struct type_field *type_field_alloc(char *ident, struct situs *ident_loc, struct type_node *tn, int bitfield_size);
void type_field_free_chain(struct type_field *tf);
struct type_field *type_field_join(struct type_field *front, struct type_field *back);

struct type_param *type_param_alloc(char *ident, struct situs *input_loc, struct type_node *tn, int is_variadic);
void type_param_free_chain(struct type_param *tp);
struct type_param *type_param_join(struct type_param *front, struct type_param *back);
struct type_param *type_param_clone_list(struct type_param *src);

void type_node_init(struct type_base *tb, struct type_node *typ);
void type_node_cleanup(struct type_node *typ);

int type_node_is_scalar_type(struct type_node *node);
int type_node_is_arithmetic_type(struct type_node *node);
int type_node_is_real_type(struct type_node *node);
int type_node_is_integer_type(struct type_node *node);
int type_node_is_pointer_type(struct type_node *node);
int type_node_is_struct_or_union(struct type_node *node);
int type_node_is_array(struct type_node *node);
int type_node_is_function(struct type_node *node);

struct type_node *type_node_unqualified(struct type_node *tn);

struct type_node *type_node_integer_promotion(struct type_base *tb, struct type_node *et);
struct type_node *type_node_default_arg_promotion(struct type_base *tb, struct type_node *tn);
struct type_node *type_node_composite(struct c_compiler *cc, struct type_node *t1, struct type_node *t2);
int type_node_is_compatible(struct type_node *tn, struct type_node *with);

/* Prints the type as if it were a declaration. ident is the identifier, optional and may be NULL if purely
 * the type is to be printed. */
void type_node_print_type_decl(FILE *fp, struct type_node *tn, const char *ident);

enum type_kind type_node_arith_type_kind_no_enum(struct type_base *tb, struct type_node *tn);

uint64_t type_node_align(struct type_base *tb, struct type_node *tn);
uint64_t type_node_size(struct type_base *tb, struct type_node *tn);
struct expr *type_node_struct_offset_expr(struct c_compiler *cc, struct type_node *tn_struct, struct type_field *offset_field);
struct expr *type_node_size_expr(struct c_compiler *cc, struct type_node *tn);
void type_node_uintptr_expr(struct type_base *tb, enum expr_type *uintptr_and, enum expr_type *uintptr_add, enum expr_type *uintptr_sub, enum expr_type *uintptr_c, enum expr_type *uintptr_mul, enum expr_type *uintptr_less_than);
void type_node_ptrdiff_expr(struct type_base *tb, enum expr_type *ptrdiff_and, enum expr_type *ptrdiff_add, enum expr_type *ptrdiff_sub, enum expr_type *ptrdiff_c, enum expr_type *ptrdiff_mul, enum expr_type *ptrdiff_less_than);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* TYPES_H */
