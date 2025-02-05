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

#ifndef STDLIB_H_INCLUDED
#define STDLIB_H_INCLUDED
#include <stdlib.h>
#endif

#ifndef STDIO_H_INCLUDED
#define STDIO_H_INCLUDED
#include <stdio.h>
#endif

#ifndef INTTYPES_H_INCLUDED
#define INTTYPES_H_INCLUDED
#include <inttypes.h>
#endif

#ifndef STRING_H_INCLUDED
#define STRING_H_INCLUDED
#include <string.h>
#endif

#ifndef ASSERT_H_INCLUDED
#define ASSERT_H_INCLUDED
#include <assert.h>
#endif

#ifndef C_COMPILER_H_INCLUDED
#define C_COMPILER_H_INCLUDED
#include "c_compiler.h"
#endif

#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED
#include "types.h"
#endif

#ifndef EXPR_H_INCLUDED
#define EXPR_H_INCLUDED
#include "expr.h"
#endif

void type_base_init(struct type_base *tb) {
  size_t n;
  for (n = 0; n < TYPE_BASE_HASHTABLE_SIZE; ++n) {
    tb->hash_table_[n] = NULL;
  }
  tb->types_ = NULL;
  tb->next_id_ = 1;

  /* 64 bit pointers */
  tb->uintptr_equivalent_ = tk_unsigned_long_long_int;
  tb->ptrdiff_equivalent_ = tk_long_long_int;

  /* Number of bits per int, long, or long long (or equivalent) */
  tb->bits_per_int_ = 32;
  tb->bits_per_long_ = 32;
  tb->bits_per_long_long_ = 64;

  /* Char is signed */
  tb->char_is_signed_ = 1;

  type_node_init(tb, &tb->char_);
  tb->char_.label_ = "char";
  tb->char_.kind_ = tk_char;
  type_node_init(tb, &tb->signed_char_);
  tb->signed_char_.label_ = "signed char";
  tb->signed_char_.kind_ = tk_signed_char;
  type_node_init(tb, &tb->short_int_);
  tb->short_int_.label_ = "short int";
  tb->short_int_.kind_ = tk_short_int;
  type_node_init(tb, &tb->int_);
  tb->int_.label_ = "int";
  tb->int_.kind_ = tk_int;
  type_node_init(tb, &tb->long_int_);
  tb->long_int_.label_ = "long int";
  tb->long_int_.kind_ = tk_long_int;
  type_node_init(tb, &tb->long_long_int_);
  tb->long_long_int_.label_ = "long long int";
  tb->long_long_int_.kind_ = tk_long_long_int;
  type_node_init(tb, &tb->bool_);
  tb->bool_.label_ = "_Bool";
  tb->bool_.kind_ = tk_bool;
  type_node_init(tb, &tb->unsigned_char_);
  tb->unsigned_char_.label_ = "unsigned char";
  tb->unsigned_char_.kind_ = tk_unsigned_char;
  type_node_init(tb, &tb->unsigned_short_int_);
  tb->unsigned_short_int_.label_ = "unsigned short int";
  tb->unsigned_short_int_.kind_ = tk_unsigned_short_int;
  type_node_init(tb, &tb->unsigned_int_);
  tb->unsigned_int_.label_ = "unsigned int";
  tb->unsigned_int_.kind_ = tk_unsigned_int;
  type_node_init(tb, &tb->unsigned_long_int_);
  tb->unsigned_long_int_.label_ = "unsigned long int";
  tb->unsigned_long_int_.kind_ = tk_unsigned_long_int;
  type_node_init(tb, &tb->unsigned_long_long_int_);
  tb->unsigned_long_long_int_.label_ = "unsigned long long int";
  tb->unsigned_long_long_int_.kind_ = tk_unsigned_long_long_int;
  type_node_init(tb, &tb->float_);
  tb->float_.label_ = "float";
  tb->float_.kind_ = tk_float;
  type_node_init(tb, &tb->double_);
  tb->double_.label_ = "double";
  tb->double_.kind_ = tk_double;
  type_node_init(tb, &tb->long_double_);
  tb->long_double_.label_ = "long double";
  tb->long_double_.kind_ = tk_long_double;
  type_node_init(tb, &tb->float_complex_);
  tb->float_complex_.label_ = "float _Complex";
  tb->float_complex_.kind_ = tk_float_complex;
  type_node_init(tb, &tb->double_complex_);
  tb->double_complex_.label_ = "double _Complex";
  tb->double_complex_.kind_ = tk_double_complex;
  type_node_init(tb, &tb->long_double_complex_);
  tb->long_double_complex_.label_ = "long double _Complex";
  tb->long_double_complex_.kind_ = tk_long_double_complex;
  type_node_init(tb, &tb->float_imaginary_);
  tb->float_imaginary_.label_ = "float _Imaginary";
  tb->float_imaginary_.kind_ = tk_float_imaginary;
  type_node_init(tb, &tb->double_imaginary_);
  tb->double_imaginary_.label_ = "double _Imaginary";
  tb->double_imaginary_.kind_ = tk_double_imaginary;
  type_node_init(tb, &tb->long_double_imaginary_);
  tb->long_double_imaginary_.label_ = "long double _Imaginary";
  tb->long_double_imaginary_.kind_ = tk_long_double_imaginary;
  type_node_init(tb, &tb->void_);
  tb->void_.label_ = "void";
  tb->void_.kind_ = tk_void;
}

void type_base_cleanup(struct type_base *tb) {
  struct type_node *t, *next;
  t = tb->types_;
  if (t) {
    next = t->chain_;
    do {
      t = next;
      next = t->chain_;

      int static_allocation = 0;
      if ((t->kind_ >= tk_char) && (t->kind_ <= tk_void)) {
        /* Builtin types should not be freed as they are members of type_base */
        static_allocation = 1;
      }

      type_node_cleanup(t);

      if (!static_allocation) {
        free(t);
      }
    } while (t != tb->types_);
  }
}

int64_t type_base_int_type_min(struct type_base *tb, enum type_kind tk) {
  if (tk == tk_char) {
    if (tb->char_is_signed_) tk = tk_signed_char;
    else tk = tk_unsigned_char;
  }
  switch (tk) {
  case tk_signed_char:
    return -128;
  case tk_short_int:
    return INT16_MIN;
  case tk_int:
    return ((int64_t)0) - (((int64_t)1) << (tb->bits_per_int_ - 1));
  case tk_long_int:
    return ((int64_t)0) - (((int64_t)1) << (tb->bits_per_long_ - 1));
  case tk_long_long_int:
    return ((int64_t)0) - (((int64_t)1) << (tb->bits_per_long_long_ - 1));
  case tk_bool:
  case tk_unsigned_char:
  case tk_unsigned_short_int:
  case tk_unsigned_int:
  case tk_unsigned_long_int:
  case tk_unsigned_long_long_int:
    return 0;
  }
  return 0;
}

uint64_t type_base_int_type_max(struct type_base *tb, enum type_kind tk) {
  if (tk == tk_char) {
    if (tb->char_is_signed_) tk = tk_signed_char;
    else tk = tk_unsigned_char;
  }
  switch (tk) {
  case tk_signed_char:
    return (((uint64_t)1) << (8 - 1)) - 1;
  case tk_short_int:
    return (((uint64_t)1) << (16 - 1)) - 1;
  case tk_int:
    return (((uint64_t)1) << (tb->bits_per_int_ - 1)) - 1;
  case tk_long_int:
    return (((uint64_t)1) << (tb->bits_per_long_ - 1)) - 1;
  case tk_long_long_int:
    return (((uint64_t)1) << (tb->bits_per_long_long_ - 1)) - 1;
  case tk_bool:
    return 1;
  case tk_unsigned_char:
    return (((uint64_t)1) << 8) - 1;
  case tk_unsigned_short_int:
    return (((uint64_t)1) << 16) - 1;
  case tk_unsigned_int:
    return (((uint64_t)1) << tb->bits_per_int_) - 1;
  case tk_unsigned_long_int:
    return (((uint64_t)1) << tb->bits_per_long_) - 1;
  case tk_unsigned_long_long_int:
    return ((uint64_t)0) - 1;
  }
  return 0;
}


int type_kind_is_complex(enum type_kind tk) {
  switch (tk) {
    case tk_long_double_complex:
    case tk_double_complex:
    case tk_float_complex:
      return 1;
  }
  return 0;
}

int type_kind_is_imaginary(enum type_kind tk) {
  switch (tk) {
    case tk_long_double_imaginary:
    case tk_double_imaginary:
    case tk_float_imaginary:
      return 1;
  }
  return 0;
}

int type_kind_is_signed(struct type_base *tb, enum type_kind tk) {
  switch (tk) {
    case tk_char:
      return !!tb->char_is_signed_;
    case tk_signed_char:
    case tk_short_int:
    case tk_int:
    case tk_long_int:
    case tk_long_long_int:
    case tk_float:
    case tk_double:
    case tk_long_double:
    case tk_float_complex:
    case tk_double_complex:
    case tk_long_double_complex:
    case tk_float_imaginary:
    case tk_double_imaginary:
    case tk_long_double_imaginary:
      return 1;
  }
  return 0;
}

enum type_kind type_kind_get_real_type(enum type_kind tk) {
  switch (tk) {
  case tk_long_double_complex:
  case tk_long_double_imaginary:
    return tk_long_double;
  case tk_double_complex:
  case tk_double_imaginary:
    return tk_double;
  case tk_float_complex:
  case tk_float_imaginary:
    return tk_float;
  }
  return tk;
}

enum type_kind type_kind_get_type_in_domain(enum type_kind same_domain, enum type_kind real_type) {
  switch (real_type) {
    case tk_long_double:
    case tk_long_double_complex:
    case tk_long_double_imaginary:
      if (type_kind_is_complex(same_domain)) {
        return tk_long_double_complex;
      }
      else if (type_kind_is_imaginary(same_domain)) {
        return tk_long_double_imaginary;
      }
      return real_type;
    case tk_double:
    case tk_double_complex:
    case tk_double_imaginary:
      if (type_kind_is_complex(same_domain)) {
        return tk_double_complex;
      }
      else if (type_kind_is_imaginary(same_domain)) {
        return tk_double_imaginary;
      }
      return real_type;
    case tk_float:
    case tk_float_complex:
    case tk_float_imaginary:
      if (type_kind_is_complex(same_domain)) {
        return tk_float_complex;
      }
      else if (type_kind_is_imaginary(same_domain)) {
        return tk_float_imaginary;
      }
      return real_type;
  }
  return real_type;
}

enum type_kind type_kind_get_unsigned_kind(enum type_kind tk) {
  switch (tk) {
  case tk_char: 
  case tk_signed_char: return tk_unsigned_char;
  case tk_short_int: return tk_unsigned_short_int;
  case tk_int: return tk_unsigned_int;
  case tk_long_int: return tk_unsigned_long_int;
  case tk_long_long_int: return tk_unsigned_long_long_int;
  }
  return tk;
}

void type_node_init(struct type_base *tb, struct type_node *tn) {
  tn->id_ = tb->next_id_++;
  tn->kind_ = tk_invalid;
  tn->hash_chain_ = NULL;
  if (tb->types_) {
    tn->chain_ = tb->types_->chain_;
    tb->types_->chain_ = tn;
  }
  else {
    tn->chain_ = tn;
  }
  tb->types_ = tn;
  tn->derived_from_ = NULL;
  tn->label_ = NULL;
  tn->tag_ = NULL;
  situs_init(&tn->tag_loc_);
  tn->fields_ = NULL;
  tn->params_ = NULL;
  tn->enum_constants_ = NULL;
  tn->qualifiers_ = 0;
  tn->is_variable_length_array_ = 0;
  tn->arguments_unknown_ = 0;
  tn->is_identifier_list_ = 0;
  tn->is_function_def_ = 0;
  tn->array_size_ = 0;
  tn->array_size_expr_ = NULL;
}

struct type_node *type_node_alloc(struct type_base *tb) {
  struct type_node *tn = (struct type_node *)malloc(sizeof(struct type_node));
  if (!tn) return NULL;
  type_node_init(tb, tn);
  return tn;
}

void type_node_cleanup(struct type_node *tn) {
  if (tn->tag_) free(tn->tag_);
  if (tn->fields_) type_field_free_chain(tn->fields_);
  if (tn->params_) type_param_free_chain(tn->params_);
  if (tn->enum_constants_) type_enum_constant_free_chain(tn->enum_constants_);
  if (tn->array_size_expr_) expr_free(tn->array_size_expr_);
  situs_cleanup(&tn->tag_loc_);
}

struct type_node *type_base_simple(struct type_base *tb, enum type_kind tk) {
  switch (tk) {
    case tk_char: return &tb->char_;
    case tk_signed_char: return &tb->signed_char_;
    case tk_short_int: return &tb->short_int_;
    case tk_int: return &tb->int_;
    case tk_long_int: return &tb->long_int_;
    case tk_long_long_int: return &tb->long_long_int_;
    case tk_bool: return &tb->bool_;
    case tk_unsigned_char: return &tb->unsigned_char_;
    case tk_unsigned_short_int: return &tb->unsigned_short_int_;
    case tk_unsigned_int: return &tb->unsigned_int_;
    case tk_unsigned_long_int: return &tb->unsigned_long_int_;
    case tk_unsigned_long_long_int: return &tb->unsigned_long_long_int_;
    case tk_float: return &tb->float_;
    case tk_double: return &tb->double_;
    case tk_long_double: return &tb->long_double_;
    case tk_float_complex: return &tb->float_complex_;
    case tk_double_complex: return &tb->double_complex_;
    case tk_long_double_complex: return &tb->long_double_complex_;
    case tk_float_imaginary: return &tb->float_imaginary_;
    case tk_double_imaginary: return &tb->double_imaginary_;
    case tk_long_double_imaginary: return &tb->long_double_imaginary_;
    case tk_void: return &tb->void_;
  }
  /* Not one of the simple types.. */
  return NULL;
}

int type_node_is_scalar_type(struct type_node *node) {
  while (node->kind_ == tk_qualifier) {
    node = node->derived_from_;
  }
  switch (node->kind_) {
    case tk_char:
    case tk_signed_char:
    case tk_short_int:
    case tk_int:
    case tk_long_int:
    case tk_long_long_int:
    case tk_bool:
    case tk_unsigned_char:
    case tk_unsigned_short_int:
    case tk_unsigned_int:
    case tk_unsigned_long_int:
    case tk_unsigned_long_long_int:
    case tk_float:
    case tk_double:
    case tk_long_double:
    case tk_float_complex:
    case tk_double_complex:
    case tk_long_double_complex:
    case tk_float_imaginary:
    case tk_double_imaginary:
    case tk_long_double_imaginary:

    case tk_enumeration:

    case tk_pointer:
      return 1;
  }
  return 0;
}

int type_node_is_arithmetic_type(struct type_node *node) {
  while (node->kind_ == tk_qualifier) {
    node = node->derived_from_;
  }
  switch (node->kind_) {
    case tk_char:
    case tk_signed_char:
    case tk_short_int:
    case tk_int:
    case tk_long_int:
    case tk_long_long_int:
    case tk_bool:
    case tk_unsigned_char:
    case tk_unsigned_short_int:
    case tk_unsigned_int:
    case tk_unsigned_long_int:
    case tk_unsigned_long_long_int:
    case tk_float:
    case tk_double:
    case tk_long_double:
    case tk_float_complex:
    case tk_double_complex:
    case tk_long_double_complex:
    case tk_float_imaginary:
    case tk_double_imaginary:
    case tk_long_double_imaginary:

    case tk_enumeration:

      return 1;
  }
  return 0;
}

int type_node_is_real_type(struct type_node *node) {
  while (node->kind_ == tk_qualifier) {
    node = node->derived_from_;
  }
  switch (node->kind_) {
    case tk_char:
    case tk_signed_char:
    case tk_short_int:
    case tk_int:
    case tk_long_int:
    case tk_long_long_int:
    case tk_bool:
    case tk_unsigned_char:
    case tk_unsigned_short_int:
    case tk_unsigned_int:
    case tk_unsigned_long_int:
    case tk_unsigned_long_long_int:
    case tk_float:
    case tk_double:
    case tk_long_double:

    case tk_enumeration:

      return 1;
  }
  return 0;
}

int type_node_is_integer_type(struct type_node *node) {
  while (node->kind_ == tk_qualifier) {
    node = node->derived_from_;
  }
  switch (node->kind_) {
    case tk_char:
    case tk_signed_char:
    case tk_short_int:
    case tk_int:
    case tk_long_int:
    case tk_long_long_int:
    case tk_bool:
    case tk_unsigned_char:
    case tk_unsigned_short_int:
    case tk_unsigned_int:
    case tk_unsigned_long_int:
    case tk_unsigned_long_long_int:

    case tk_enumeration:

      return 1;
  }
  return 0;
}

int type_node_is_pointer_type(struct type_node *node) {
  while (node->kind_ == tk_qualifier) {
    node = node->derived_from_;
  }
  return (node->kind_ == tk_pointer);
}

int type_node_is_struct_or_union(struct type_node *node) {
  while (node->kind_ == tk_qualifier) {
    node = node->derived_from_;
  }
  return (node->kind_ == tk_structure) || (node->kind_ == tk_union);
}

int type_node_is_array(struct type_node *node) {
  while (node->kind_ == tk_qualifier) {
    node = node->derived_from_;
  }
  return (node->kind_ == tk_array);
}

int type_node_is_function(struct type_node *node) {
  while (node->kind_ == tk_qualifier) {
    node = node->derived_from_;
  }
  return (node->kind_ == tk_function);
}

static int hash_qualifiers(enum type_kind tk, struct type_node *base, int qualifiers) {
  uint32_t v = (uint32_t)tk;
  v = (v << 7) | (v >> (31 & -7));
  v += base->id_;
  v = (v << 7) | (v >> (31 & -7));
  v += qualifiers;
  return (int)(v % TYPE_BASE_HASHTABLE_SIZE);
}

struct type_node *type_base_qualifier(struct type_base *tb, struct type_node *base, int qualifiers) {
  if (!qualifiers) return base;
  if (!base) return NULL; /* passthrough errors */
  if (base->kind_ == tk_qualifier) {
    qualifiers |= base->qualifiers_;
    base = base->derived_from_;
  }
  struct type_node *tn;
  int hash = hash_qualifiers(tk_qualifier, base, qualifiers);
  tn = tb->hash_table_[hash];
  if (tn) {
    do {
      tn = tn->hash_chain_;

      if ((tn->kind_ == tk_qualifier) && (tn->qualifiers_ == qualifiers) && (tn->derived_from_ == base)) {
        /* Found existing match */
        return tn;
      }
    } while (tn != tb->hash_table_[hash]);
  }

  /* No match found, create new */
  tn = type_node_alloc(tb);
  if (!tn) return NULL;
  tn->kind_ = tk_qualifier;
  tn->qualifiers_ = qualifiers;
  tn->derived_from_ = base;
  
  if (tb->hash_table_[hash]) {
    tn->hash_chain_ = tb->hash_table_[hash]->hash_chain_;
    tb->hash_table_[hash]->hash_chain_ = tn;
  }
  else {
    tn->hash_chain_ = tn;
  }
  tb->hash_table_[hash] = tn;

  return tn;
}

struct type_node *type_node_unqualified(struct type_node *tn) {
  if (!tn) return NULL;
  if (tn->kind_ == tk_qualifier) {
    return tn->derived_from_;
  }
  return tn;
}

static int hash_ptr(struct type_node *base) {
  uint32_t v = (uint32_t)tk_pointer;
  v = (v << 7) | (v >> (31 & -7));
  v += base->id_;
  return (int)(v % TYPE_BASE_HASHTABLE_SIZE);
}

struct type_node *type_base_pointer(struct type_base *tb, struct type_node *base) {
  struct type_node *tn;
  if (!base) return NULL; /* passthrough errors */
  int hash = hash_ptr(base);
  tn = tb->hash_table_[hash];
  if (tn) {
    do {
      tn = tn->hash_chain_;

      if ((tn->kind_ == tk_pointer) && (tn->derived_from_ == base)) {
        /* Found it */
        return tn;
      }
    } while (tn != tb->hash_table_[hash]);
  }

  /* No match found, create new */
  tn = type_node_alloc(tb);
  if (!tn) return NULL;
  tn->kind_ = tk_pointer;
  tn->derived_from_ = base;

  if (tb->hash_table_[hash]) {
    tn->hash_chain_ = tb->hash_table_[hash]->hash_chain_;
    tb->hash_table_[hash]->hash_chain_ = tn;
  }
  else {
    tn->hash_chain_ = tn;
  }
  tb->hash_table_[hash] = tn;
  return tn;
}

static int hash_fixed_array(struct type_node *element, uint64_t array_size) {
  uint32_t v = (uint32_t)tk_array;
  v = (v << 7) | (v >> (31 & -7));
  v += element->id_;
  v = (v << 7) | (v >> (31 & -7));
  v += (uint32_t)array_size;
  v = (v << 7) | (v >> (31 & -7));
  v += (uint32_t)(array_size >> 32);
  return (int)(v % TYPE_BASE_HASHTABLE_SIZE);
}

struct type_node *type_base_array(struct c_compiler *cc, struct type_node *element, struct expr *array_size_expr) {
  int r;
  struct type_node *tn;
  uint64_t array_size = 0;
  if (array_size_expr && expr_is_constant(array_size_expr)) {
    struct expr_temp array_size_result;
    expr_prepare(array_size_expr);
    r = expr_constant_evaluation(cc, &array_size_result, array_size_expr);
    if (r) {
      return NULL;
    }
    array_size = array_size_result.v_.u64_;
  }
  int hash = hash_fixed_array(element, array_size);
  if (array_size) {
    /* Complete array type (we have a size); find existing type. */
    tn = cc->tb_.hash_table_[hash];
    if (tn) {
      do {
        tn = tn->hash_chain_;

        if ((tn->kind_ == tk_array) && (tn->derived_from_ == element) && (tn->array_size_ == array_size)) {
          /* Found it */
          return tn;
        }

      } while (tn != cc->tb_.hash_table_[hash]);
    }
  }

  /* No match found or incomplete type, create new */
  struct expr *array_size_clone = expr_clone(array_size_expr);
  if (array_size_expr && !array_size_clone) {
    cc_no_memory(cc);
    return NULL;
  }
  tn = type_node_alloc(&cc->tb_);
  if (!tn) {
    expr_free(array_size_clone);
    return NULL;
  }
  tn->kind_ = tk_array;
  tn->derived_from_ = element;
  tn->array_size_ = array_size;
  tn->array_size_expr_ = array_size_clone;

  if (array_size) {
    /* Type not incomplete, add to hash */
    if (cc->tb_.hash_table_[hash]) {
      tn->hash_chain_ = cc->tb_.hash_table_[hash]->hash_chain_;
      cc->tb_.hash_table_[hash]->hash_chain_ = tn;
    }
    else {
      tn->hash_chain_ = tn;
    }
    cc->tb_.hash_table_[hash] = tn;
  }

  return tn;
}

struct type_node *type_base_array_fixed_length(struct c_compiler *cc, struct type_node *elm_type, uint64_t array_length) {
  struct expr *array_length_expr = NULL;
  int max_bits = 0;
  enum expr_type const_expr_type = ET_INVALID;
  switch (cc->tb_.uintptr_equivalent_) {
    case tk_unsigned_int:
      const_expr_type = ET_C_UNSIGNED_INT;
      max_bits = cc->tb_.bits_per_int_;
      break;
    case tk_unsigned_long_int:
      const_expr_type = ET_C_UNSIGNED_LONG_INT;
      max_bits = cc->tb_.bits_per_long_;
      break;
    case tk_unsigned_long_long_int:
      const_expr_type = ET_C_UNSIGNED_LONG_LONG_INT;
      max_bits = cc->tb_.bits_per_long_long_;
      break;
    default:
      assert(0 && "Cannot work with uintptr type");
      break;
  }
  switch (max_bits) {
    case 64:
      /* all values valid */
      break;
    case 32:
      if (0xFFFFFFFF00000000ULL & array_length) {
        fprintf(stderr, "Error, array length does not fit %d bits", max_bits);
        return NULL;
      }
      break;
    case 16:
      if (0xFFFFFFFFFFFF0000ULL & array_length) {
        fprintf(stderr, "Error, array length does not fit %d bits", max_bits);
        return NULL;
      }
      break;
  }

  array_length_expr = expr_alloc(const_expr_type);
  if (!array_length_expr) {
    cc_no_memory(cc);
    return NULL;
  }
  array_length_expr->v_.u64_ = array_length;

  struct type_node *tn = type_base_array(cc, elm_type, array_length_expr);
  if (!tn) {
    cc_no_memory(cc);
    expr_free(array_length_expr);
    return NULL;
  }
  return tn;
}


struct type_node *type_base_alloc_struct_or_union(struct type_base *tb, enum type_kind struct_or_union) {
  /* Aggregate types are always allocated anew as they are always unique */
  struct type_node *tn;
  tn = type_node_alloc(tb);
  if (!tn) return NULL;
  assert((struct_or_union == tk_structure) || (struct_or_union == tk_union));
  tn->kind_ = struct_or_union;
  return tn;
}

struct type_node *type_base_alloc_enum(struct type_base *tb) {
  struct type_node *tn;
  tn = type_node_alloc(tb);
  if (!tn) return NULL;
  tn->kind_ = tk_enumeration;
  tn->derived_from_ = type_base_simple(tb, tk_int);
  return tn;
}

struct type_node *type_base_alloc_func(struct type_base *tb, struct type_node *return_type) {
  struct type_node *tn;
  tn = type_node_alloc(tb);
  if (!tn) return NULL;
  tn->kind_ = tk_function;
  tn->derived_from_ = return_type;
  return tn;
}

struct type_field *type_field_alloc(char *ident, struct situs *ident_loc, struct type_node *tn, int bitfield_size) {
  struct type_field *tf = (struct type_field *)malloc(sizeof(struct type_field));
  if (!tf) return NULL;
  tf->chain_ = tf;
  tf->ident_ = ident;
  situs_init(&tf->ident_loc_);
  if (situs_clone(&tf->ident_loc_, ident_loc)) {
    free(tf);
    return NULL;
  }
  tf->type_ = tn;
  tf->bitfield_width_ = bitfield_size;

  return tf;
}

void type_field_free_chain(struct type_field *tf) {
  struct type_field *f = tf, *next;
  if (f) {
    next = f->chain_;
    do {
      f = next;
      next = f->chain_;

      if (f->ident_) free(f->ident_);
      situs_cleanup(&f->ident_loc_);
      free(f);

    } while (f != tf);
  }
}

struct type_field *type_field_join(struct type_field *front, struct type_field *back) {
  if (!front) return back;
  if (!back) return front;
  struct type_field *front_head = front->chain_;
  struct type_field *front_tail = front;
  struct type_field *back_head = back->chain_;
  struct type_field *back_tail = back;

  back_tail->chain_ = front_head;
  front_tail->chain_ = back_head;
  return back_tail;
}

struct type_param *type_param_alloc(char *ident, struct situs *ident_loc, struct type_node *tn, int is_variadic) {
  char *dup_ident = NULL;
  if (ident) {
    dup_ident = strdup(ident);
    if (!dup_ident) {
      return NULL;
    }
  }
  struct type_param *tp = (struct type_param *)malloc(sizeof(struct type_param));
  if (!tp) {
    if (dup_ident) free(dup_ident);
    return NULL;
  }
  tp->chain_ = tp;
  tp->ident_ = dup_ident;
  situs_init(&tp->ident_loc_);
  if (situs_clone(&tp->ident_loc_, ident_loc)) {
    free(tp);
    if (dup_ident) free(dup_ident);
    return NULL;
  }
  tp->type_ = tn;
  tp->is_varadic_ = !!is_variadic;

  return tp;
}

void type_param_free_chain(struct type_param *tp) {
  struct type_param *p = tp, *next;
  if (p) {
    next = p->chain_;
    do {
      p = next;
      next = p->chain_;

      if (p->ident_) free(p->ident_);
      situs_cleanup(&p->ident_loc_);
      free(p);

    } while (p != tp);
  }
}

struct type_param *type_param_join(struct type_param *front, struct type_param *back) {
  struct type_param *front_head = front->chain_;
  struct type_param *front_tail = front;
  struct type_param *back_head = back->chain_;
  struct type_param *back_tail = back;
  
  back_tail->chain_ = front_head;
  front_tail->chain_ = back_head;
  return back_tail;
}

struct type_param *type_param_clone_list(struct type_param *src) {
  struct type_param *tp;
  struct type_param *r = NULL;
  struct type_param *cl;
  if (src) {
    tp = src;
    do {
      tp = tp->chain_;

      cl = type_param_alloc(tp->ident_, &tp->ident_loc_, tp->type_, tp->is_varadic_);
      if (!cl) {
        type_param_free_chain(r);
        return NULL;
      }
      r = type_param_join(r, cl);
    } while (tp != src);
  }

  return r;
}

struct type_enum_constant *type_enum_constant_alloc(const char *ident, struct situs *ident_loc) {
  char *id = strdup(ident);
  if (!id) return NULL;
  struct type_enum_constant *tec = (struct type_enum_constant *)malloc(sizeof(struct type_enum_constant));
  if (!tec) {
    free(id);
    return NULL;
  }
  tec->chain_ = tec;
  tec->ident_ = id;
  situs_init(&tec->ident_loc_);
  if (situs_clone(&tec->ident_loc_, ident_loc)) {
    free(tec);
    free(id);
    return NULL;
  }
  tec->value_ = 0;
  tec->value_initialized_ = 0;
  return tec;
}

void type_enum_constant_free_chain(struct type_enum_constant *tec) {
  struct type_enum_constant *ec = tec, *next;
  if (ec) {
    next = ec->chain_;
    do {
      ec = next;
      next = ec->chain_;

      if (ec->ident_) free(ec->ident_);
      situs_cleanup(&ec->ident_loc_);
      free(ec);

    } while (ec != tec);
  }
}

struct type_enum_constant *type_enum_constant_join(struct type_enum_constant *front, struct type_enum_constant *back) {
  struct type_enum_constant *front_head = front->chain_;
  struct type_enum_constant *front_tail = front;
  struct type_enum_constant *back_head = back->chain_;
  struct type_enum_constant *back_tail = back;

  back_tail->chain_ = front_head;
  front_tail->chain_ = back_head;
  return back_tail;
}

static void type_node_print_specifier(FILE *fp, struct type_node *tn) {
  struct type_node *pn = tn;
  struct type_node *qn = NULL;

  if (!tn) {
    fprintf(fp, "(null)");
    return;
  }

  if ((tn->kind_ == tk_qualifier) && tn->derived_from_ && (tn->derived_from_->label_)) {
    pn = tn->derived_from_;
    qn = tn;
  }
  if (qn) {
    if (qn->qualifiers_ & TYPE_QUALIFIER_CONST) fprintf(fp, "const ");
    if (qn->qualifiers_ & TYPE_QUALIFIER_RESTRICT) fprintf(fp, "restrict ");
    if (qn->qualifiers_ & TYPE_QUALIFIER_VOLATILE) fprintf(fp, "volatile ");
  }
  if (pn->label_) {
    fprintf(fp, "%s", pn->label_);
  }
  else {
    /* No label implies not a simple type yet. */
    /* XXX: Tail recursion unnecessary.. */
    type_node_print_specifier(fp, tn->derived_from_);
  }
}

int type_node_is_compatible(struct type_node *tn, struct type_node *with) {
  if (tn == with) return 1;

  if (tn->kind_ != with->kind_) {
    return 0;
  }

  if (tn->kind_ == tk_qualifier) {
    if (tn->qualifiers_ != with->qualifiers_) {
      return 0;
    }
    return type_node_is_compatible(tn->derived_from_, with->derived_from_);
  }

  if (tn->kind_ == tk_pointer) {
    return type_node_is_compatible(tn->derived_from_, with->derived_from_);
  }

  if (tn->kind_ == tk_array) {
    if (tn->is_variable_length_array_ || with->is_variable_length_array_) {
      /* Variable length arrays (eg. non-constant expression or [*]) are always
       * compatible with both incomplete types ([]) or constant arrays ([6]). /*/
      return type_node_is_compatible(tn->derived_from_, with->derived_from_);
    }
    if (tn->array_size_ == with->array_size_) {
      return type_node_is_compatible(tn->derived_from_, with->derived_from_);
    }
    if (!tn->array_size_ || !with->array_size_) {
      /* One or both of the arrays are incomplete types (no assigned size) */
      return type_node_is_compatible(tn->derived_from_, with->derived_from_);
    }
    /* Array sizes are constant expressions, they are defined, and mismatch. */
    return 0;
  }

  if (tn->kind_ == tk_function) {
    /* now what... 
     * XXX: Do function compatibility. */

  }

  return 0;
  /* XXX: This is incorrect!! Works for things like simple types and objects, fails for things like functions and arrays.
    Additional  rules  fordetermining  whether  two types  are  compatible  are  described  in  6.7.2  for  type  specifiers,in  6.7.3  for  type  qualifiers, 
    and  in  6.7.5  for  declarators.46)
    
    6.7.5.3-15 - Function declarators and prototype compatibility.
  */
}

/* Build a table.
 * For each Each row is the From-type. Each column is the To-type (indexed by type kind_).
 * Each cell indicates:
 * 1) The conversion instruction to apply
 * 2) The result type of the conversion instruction
 * This way, we can convert by stepping through the table until we get to the destination
 * type. */

void type_kind_classify_integer_type(enum type_kind integerkind, int *is_signed, int *rank) {
  *is_signed = -1;
  *rank = -1;
  switch (integerkind) {
    case tk_signed_char:
      *rank = 0;
      *is_signed = 1;
      break;
    case tk_short_int:
      *rank = 1;
      *is_signed = 1;
      break;
    case tk_int:
      *rank = 2;
      *is_signed = 1;
      break;
    case tk_long_int:
      *rank = 3;
      *is_signed = 1;
      break;
    case tk_long_long_int:
      *rank = 4;
      *is_signed = 1;
      break;
    case tk_unsigned_char:
      *rank = 0;
      *is_signed = 0;
      break;
    case tk_unsigned_short_int:
      *rank = 1;
      *is_signed = 0;
      break;
    case tk_unsigned_int:
      *rank = 2;
      *is_signed = 0;
      break;
    case tk_unsigned_long_int:
      *rank = 3;
      *is_signed = 0;
      break;
    case tk_unsigned_long_long_int:
      *rank = 4;
      *is_signed = 0;
      break;
  }
  return;
}


struct type_node *type_node_integer_promotion(struct type_base *tb, struct type_node *et) {
  /* WARNING: Integer promotion logic *also* exists in expr_integer_promotions() this logic is
   *          replicated and expanded to include bitfield considerations. */
  if (type_node_is_integer_type(et)) {
    int is_signed, rank;
    /* et can be enum, check derived_from if that's the case (compatible integer type then defines rank). */
    enum type_kind et_kind = et->kind_;
    if (et_kind == tk_enumeration) {
      /* Use compatible integer type */
      et_kind = et->derived_from_->kind_;
    }
    type_kind_classify_integer_type(et_kind, &is_signed, &rank);
    int dummy, int_rank;
    type_kind_classify_integer_type(tk_int, &dummy, &int_rank);
    if (rank <= int_rank) {
      return type_base_simple(tb, tk_int);
    }
    else /* rank > int_rank */ {
      /* All other types are unchanged by the integer promotions */
      return et;
    }
  }
  else {
    /* All other types are unchanged by the integer promotions */
    return et;
  }
}

struct type_node *type_node_default_arg_promotion(struct type_base *tb, struct type_node *tn) {
  tn = type_node_integer_promotion(tb, tn);
  if (!tn) return NULL;
  if (tn->kind_ == tk_float) {
    return type_base_simple(tb, tk_double);
  }
  return tn;
}

static int type_node_are_params_default_promotion_compatible(struct type_base *tb, struct type_param *tps) {
  struct type_param *tp;
  tp = tps;
  if (tp) {
    do {
      tp = tp->chain_;

      if (tp->is_varadic_) break;

      if (!type_node_is_compatible(tp->type_, type_node_default_arg_promotion(tb, tp->type_))) {
        /* not compatible.. */
        return 0;
      }
    } while (tp != tps);
  }
  return 1; /* compatible */
}

struct type_node *type_node_composite(struct c_compiler *cc, struct type_node *t1, struct type_node *t2) {
  if (!t1 || !t2) return NULL;

  /* 6.2.7-3 */
  if (t1 == t2) return t1;

  if (t1->kind_ != t2->kind_) return NULL;

  if (t1->kind_ == tk_qualifier) {
    if (t1->qualifiers_ != t2->qualifiers_) {
      return NULL;
    }
    return type_base_qualifier(&cc->tb_, type_node_composite(cc, t1->derived_from_, t2->derived_from_), t1->qualifiers_);
  }

  if (t1->kind_ == tk_pointer) {
    return type_base_pointer(&cc->tb_, type_node_composite(cc, t1->derived_from_, t2->derived_from_));
  }

  if (t1->kind_ == tk_array) {
    if (!t1->is_variable_length_array_ && t1->array_size_) {
      return type_base_array(cc, t1->derived_from_, t1->array_size_expr_);
    }
    if (!t2->is_variable_length_array_ && t2->array_size_) {
      return type_base_array(cc, t2->derived_from_, t2->array_size_expr_);
    }
    if (t1->is_variable_length_array_ || t2->is_variable_length_array_) {
      struct type_node *tr = type_base_array(cc, type_node_composite(cc, t1->derived_from_, t2->derived_from_), 0);
      if (tr) tr->is_variable_length_array_ = 1;
      return tr;
    }
    return type_base_array(cc, type_node_composite(cc, t1->derived_from_, t2->derived_from_), 0);
  }

  if (t1->kind_ == tk_function) {
    if (t1->arguments_unknown_ && t2->arguments_unknown_) {
      struct type_node *tr = type_base_alloc_func(&cc->tb_, type_node_composite(cc, t1->derived_from_, t2->derived_from_));
      tr->arguments_unknown_ = 1;
      return tr;
    }
    if (t1->arguments_unknown_ && !t2->arguments_unknown_) {
      if (!type_node_are_params_default_promotion_compatible(&cc->tb_, t1->params_)) {
        /* Argument types not compatible with default argument promoted equivalents */
        return NULL;
      }
      struct type_node *tr = type_base_alloc_func(&cc->tb_, type_node_composite(cc, t1->derived_from_, t2->derived_from_));
      tr->arguments_unknown_ = 0;
      if (t1->params_) {
        tr->params_ = type_param_clone_list(t1->params_);
        if (!tr->params_) {
          return NULL;
        }
      }
      return tr;
    }
    if (t2->arguments_unknown_ && !t1->arguments_unknown_) {
      if (!type_node_are_params_default_promotion_compatible(&cc->tb_, t2->params_)) {
        /* Argument types not compatible with default argument promoted equivalents */
        return NULL;
      }
      struct type_node *tr = type_base_alloc_func(&cc->tb_, type_node_composite(cc, t1->derived_from_, t2->derived_from_));
      tr->arguments_unknown_ = 0;
      if (t2->params_) {
        tr->params_ = type_param_clone_list(t2->params_);
        if (!tr->params_) {
          return NULL;
        }
      }
      return tr;
    }
    
    size_t t1c = 0;
    struct type_param *tp = t1->params_;
    do {
      tp = tp->chain_;

      t1c++;
    } while (tp != t1->params_);

    size_t t2c = 0;
    tp = t2->params_;
    do {
      tp = tp->chain_;

      t2c++;
    } while (tp != t2->params_);
  
    /* both parameter lists (arguments) are known, form new param list as composite of each individual parameter */
    if (t1->params_ && t2->params_ && !t1->is_identifier_list_ && !t2->is_identifier_list_) {
      if (t1->params_->is_varadic_ != t2->params_->is_varadic_) {
        /* Mismatch in existence of variadic ellipsis terminator. */
        return NULL;
      }
      /* Parameter type lists agree in the use of ellipsis terminators.. */
      if (t1c != t2c) {
        /* Parameter type lists do not agree in the number of parameters */
        return NULL;
      }
      struct type_param *tp1, *tp2;
      tp1 = t1->params_;
      tp2 = t2->params_;
      struct type_param *tpc = NULL;
      do {
        tp1 = tp1->chain_;
        tp2 = tp2->chain_;

        if ((tp1 == t1->params_) && (tp1->is_varadic_)) {
          /* Already checked for agreement in use of ellipsis, this is the last argument, and it is an ellipsis, accept this */
          break;
        }

        if (tp1->is_varadic_ || tp2->is_varadic_) {
          /* Variadic ellipsis somewhere in the middle.. ? */
          type_param_free_chain(tpc);
          return NULL;
        }

        tp = type_param_alloc(tp1->ident_ ? tp1->ident_ : tp2->ident_, tp1->ident_ ? &tp1->ident_loc_ : &tp2->ident_loc_, type_node_composite(cc, tp1->type_, tp2->type_), 0);
        if (!tp) {
          type_param_free_chain(tpc);
          return NULL;
        }
        tpc = type_param_join(tpc, tp);

      } while (tp1 != t1->params_);
      struct type_node *tr = type_base_alloc_func(&cc->tb_, type_node_composite(cc, t1->derived_from_, t2->derived_from_));
      if (!tr) {
        type_param_free_chain(tpc);
        return NULL;
      }
      tr->params_ = tpc;
      return tr;
    }

    /* t1 is a function definition with an identifier list and t2 is a parameter type list */
    if (t1->is_function_def_ && t1->is_identifier_list_ && !t2->is_identifier_list_) {
      /* t2 may not be variadic */
      if (t2->params_ && t2->params_->is_varadic_) {
        return NULL;
      }

      /* Number of arguments must agree */
      if (t1c != t2c) {
        return NULL;
      }

      /* Each argument must be compatible with default argument promoted equivalents */
      if (!type_node_are_params_default_promotion_compatible(&cc->tb_, t2->params_)) {
        return NULL;
      }

      struct type_node *tr = type_base_alloc_func(&cc->tb_, type_node_composite(cc, t1->derived_from_, t2->derived_from_));
      tr->arguments_unknown_ = 0;
      if (t2->params_) {
        tr->params_ = type_param_clone_list(t2->params_);
        if (!tr->params_) {
          return NULL;
        }
      }
      return tr;
    }

    /* t2 is a function definition with an identifier list and t1 is a parameter type list */
    if (t2->is_function_def_ && t2->is_identifier_list_ && !t1->is_identifier_list_) {
      /* t1 may not be variadic */
      if (t1->params_ && t1->params_->is_varadic_) {
        return NULL;
      }

      /* Number of arguments must agree */
      if (t1c != t2c) {
        return NULL;
      }

      /* Each argument must be compatible with default argument promoted equivalents */
      if (!type_node_are_params_default_promotion_compatible(&cc->tb_, t1->params_)) {
        return NULL;
      }

      struct type_node *tr = type_base_alloc_func(&cc->tb_, type_node_composite(cc, t1->derived_from_, t2->derived_from_));
      tr->arguments_unknown_ = 0;
      if (t1->params_) {
        tr->params_ = type_param_clone_list(t1->params_);
        if (!tr->params_) {
          return NULL;
        }
      }
      return tr;
    }

    // XXX: Make a definitive list of all the cases:
    // - Prototype or Not a Prototype
    //   void fun(void) : prototype with no arguments
    //   void fun() : Not a Prototype with unknown arguments
    // - Function Definition or Not a Function Definition
    //   void fun() { } : Function definition with no arguments
    //   void fun() : Not a function definition with unknown arguments.



    if (t1->params_ || t2->params_) {
      /* Incompatible param list */
      return NULL;
    }
    /* Known param lists and we know there are no params. */
    struct type_node *tr = type_base_alloc_func(&cc->tb_, type_node_composite(cc, t1->derived_from_, t2->derived_from_));
    tr->arguments_unknown_ = 0;
    return tr;
  }

  

  assert(0 && "Unhandled condition");
  return NULL;
}

static void type_node_print_declarator_prefix(FILE *fp, struct type_node *tn) {
  struct type_node *pn = tn;
  struct type_node *qn = NULL;
  struct type_node *derived_from = tn->derived_from_;

  if ((tn->kind_ == tk_qualifier) && tn->derived_from_ && (tn->derived_from_->kind_ == tk_pointer)) {
    /* Qualifier-on-pointer is handled as a single thing. */
    derived_from = tn->derived_from_->derived_from_;
    pn = tn->derived_from_;
    qn = tn;
  }

  if (derived_from) {
    type_node_print_declarator_prefix(fp, derived_from);
  }

  if (pn->kind_ == tk_pointer) {
    /* Check if we need parentheses */
    if (derived_from) {
      if ((derived_from->kind_ == tk_array) ||
        (derived_from->kind_ == tk_function)) {
        /* Inner type is an array of a function, consequently, we need
        * parentheses to avoid this being seen as "function returning pointer"
        * or "array containing pointer", instead, we should see it as 
        * "pointer to function" or "pointer to array". */
        fprintf(fp, "(");
      }
    }

    if (!qn) {
      fprintf(fp, "*");
    }
    else {
      fprintf(fp, "* ");
      if (qn->qualifiers_ & TYPE_QUALIFIER_CONST) fprintf(fp, "const ");
      if (qn->qualifiers_ & TYPE_QUALIFIER_RESTRICT) fprintf(fp, "restrict ");
      if (qn->qualifiers_ & TYPE_QUALIFIER_VOLATILE) fprintf(fp, "volatile ");
    }
  }
}

static void type_node_print_declarator_suffix(FILE *fp, struct type_node *tn) {
  if (tn->kind_ == tk_pointer) {
    /* Check if we need parentheses */
    if (tn->derived_from_) {
      if ((tn->derived_from_->kind_ == tk_array) ||
        (tn->derived_from_->kind_ == tk_function)) {
        fprintf(fp, ")");
      }
    }
  }
  struct type_node *derived_from = tn->derived_from_;

  if (tn->kind_ == tk_array || 
    ((tn->kind_ == tk_qualifier) && tn->derived_from_ && (tn->derived_from_->kind_ == tk_array))) {
    fprintf(fp, "[");
    struct type_node *ta; /* array type */
    struct type_node *tq; /* qualifier type or NULL */
    if (tn->kind_ == tk_qualifier) {
      ta = tn->derived_from_;
      tq = tn;
      derived_from = ta->derived_from_;
    }
    else {
      ta = tn;
      tq = NULL;
    }
    if (tq && (tq->qualifiers_ & TYPE_QUALIFIER_CONST)) fprintf(fp, "const");
    if (tq && (tq->qualifiers_ & TYPE_QUALIFIER_CONST) && (tq->qualifiers_ & (TYPE_QUALIFIER_RESTRICT | TYPE_QUALIFIER_VOLATILE))) fprintf(fp, " ");
    if (tq && (tq->qualifiers_ & TYPE_QUALIFIER_RESTRICT)) fprintf(fp, "restrict");
    if (tq && (tq->qualifiers_ & (TYPE_QUALIFIER_CONST | TYPE_QUALIFIER_RESTRICT)) && (tq->qualifiers_ & TYPE_QUALIFIER_VOLATILE)) fprintf(fp, " ");
    if (tq && (tq->qualifiers_ & TYPE_QUALIFIER_VOLATILE)) fprintf(fp, "volatile");

    if (tn->array_size_) {
      if (tq && tq->qualifiers_) fprintf(fp, " ");
      fprintf(fp, "%" PRIu64 "", tn->array_size_);
    }
    else if (tn->is_variable_length_array_) {
      if (tq && tq->qualifiers_) fprintf(fp, " ");
      fprintf(fp, "*");
    }
    fprintf(fp, "]");
  }

  if (derived_from) type_node_print_declarator_suffix(fp, derived_from);
}

void type_node_print_type_decl(FILE *fp, struct type_node *tn, const char *ident) {
  type_node_print_specifier(fp, tn);
  fprintf(fp, " ");
  type_node_print_declarator_prefix(fp, tn);
  if (ident) fprintf(fp, "%s", ident);
  type_node_print_declarator_suffix(fp, tn);
}

enum type_kind type_node_arith_type_kind_no_enum(struct type_base *tb, struct type_node *tn) {
  while (tn->kind_ == tk_enumeration) {
    tn = tn->derived_from_;
  }
  return tn->kind_;
}

static uint64_t type_node_struct_union_align(struct type_base *tb, struct type_node *tn) {
  uint64_t current_align = 0;
  struct type_field *field = tn->fields_;
  if (field) {
    do {
      field = field->chain_;

      uint64_t field_align = type_node_align(tb, field->type_);
      if (field_align > current_align) {
        current_align = field_align;
      }

    } while (field != tn->fields_);
  }
  return current_align;
}

uint64_t type_node_align(struct type_base *tb, struct type_node *tn) {
  if (!tn) return 0;
  tn = type_node_unqualified(tn);
  enum type_kind tk = tn->kind_;
  if (tk == tk_enumeration) {
    tk = type_node_arith_type_kind_no_enum(tb, tn);
  }
  if (tk == tk_pointer) {
    tk = tb->uintptr_equivalent_;
  }
  switch (tk) {
    case tk_invalid: return 0;
    case tk_char:
    case tk_signed_char:
      return 1;
    case tk_short_int:
      return 2;
    case tk_int:
      return (uint64_t)(tb->bits_per_int_ / 8);
    case tk_long_int:
      return (uint64_t)(tb->bits_per_long_ / 8);
    case tk_long_long_int:
      return (uint64_t)(tb->bits_per_long_long_ / 8);
    case tk_bool:
      return 1;
    case tk_unsigned_char:
      return 1;
    case tk_unsigned_short_int:
      return 2;
    case tk_unsigned_int:
      return (uint64_t)(tb->bits_per_int_ / 8);
    case tk_unsigned_long_int:
      return (uint64_t)(tb->bits_per_long_ / 8);
    case tk_unsigned_long_long_int:
      return (uint64_t)(tb->bits_per_long_long_ / 8);
    case tk_float:
      return 4;
    case tk_double:
    case tk_long_double:
      return 8;
    case tk_float_complex:
      return 8;
    case tk_double_complex:
    case tk_long_double_complex:
      return 16;
    case tk_float_imaginary:
      return 4;
    case tk_double_imaginary:
    case tk_long_double_imaginary:
      return 8;
    case tk_void:
      return 0;
    case tk_enumeration:
      assert(0 && "Should've been filtered earlier");
      return 0;
    case tk_array:
      return type_node_align(tb, tn->derived_from_);
    case tk_structure:
    case tk_union:
      return type_node_struct_union_align(tb, tn);
    case tk_function:
      return 0;
    case tk_pointer:
      assert(0 && "Should've been filtered earlier");
      return 0;
    case tk_qualifier:
      assert(0 && "Should've been filtered earlier");
      return 0;
    default:
      assert(0 && "Unhandled case");
      return 0;
  }
}

static uint64_t type_node_struct_size(struct type_base *tb, struct type_node *tn) {
  uint64_t current_val = 0;
  uint64_t current_offset = 0;
  uint64_t current_align = 0;
  struct type_field *field = tn->fields_;
  if (field) {
    do {
      field = field->chain_;

      uint64_t field_align = type_node_align(tb, field->type_);
      if (field_align > current_align) {
        current_align = field_align;
      }

      uint64_t field_size = type_node_size(tb, field->type_);
      if (field_size > current_val) {
        current_val = field_size;
      }

      current_offset = (current_offset + (field_align - 1)) & ~(field_align - 1);

      current_offset += field_size;

    } while (field != tn->fields_);
  }

  /* Now ensure the total struct size is aligned to its biggest alignment */
  current_offset = (current_offset + (current_align - 1)) & ~(current_align - 1);

  return current_offset;
}

static uint64_t type_node_union_size(struct type_base *tb, struct type_node *tn) {
  uint64_t current_val = 0;
  struct type_field *field = tn->fields_;
  if (field) {
    do {
      field = field->chain_;

      uint64_t field_size = type_node_size(tb, field->type_);
      if (field_size > current_val) {
        current_val = field_size;
      }

    } while (field != tn->fields_);
  }
  return current_val;
}

uint64_t type_node_size(struct type_base *tb, struct type_node *tn) {
  if (!tn) return 0;
  tn = type_node_unqualified(tn);
  enum type_kind tk = tn->kind_;
  if (tk == tk_enumeration) {
    tk = type_node_arith_type_kind_no_enum(tb, tn);
  }
  if (tk == tk_pointer) {
    tk = tb->uintptr_equivalent_;
  }
  switch (tk) {
    case tk_invalid: return 0;
    case tk_char:
    case tk_signed_char:
      return 1;
    case tk_short_int:
      return 2;
    case tk_int:
      return (uint64_t)(tb->bits_per_int_ / 8);
    case tk_long_int:
      return (uint64_t)(tb->bits_per_long_ / 8);
    case tk_long_long_int:
      return (uint64_t)(tb->bits_per_long_long_ / 8);
    case tk_bool:
      return 1;
    case tk_unsigned_char:
      return 1;
    case tk_unsigned_short_int:
      return 2;
    case tk_unsigned_int:
      return (uint64_t)(tb->bits_per_int_ / 8);
    case tk_unsigned_long_int:
      return (uint64_t)(tb->bits_per_long_ / 8);
    case tk_unsigned_long_long_int:
      return (uint64_t)(tb->bits_per_long_long_ / 8);
    case tk_float:
      return 4;
    case tk_double:
    case tk_long_double:
      return 8;
    case tk_float_complex:
      return 8;
    case tk_double_complex:
    case tk_long_double_complex:
      return 16;
    case tk_float_imaginary:
      return 4;
    case tk_double_imaginary:
    case tk_long_double_imaginary:
      return 8;
    case tk_void:
      return 0;
    case tk_enumeration:
      assert(0 && "Should've been filtered earlier");
      return 0;
    case tk_array:
      /* Cannot overflow */
      return tn->array_size_ * type_node_size(tb, tn->derived_from_);
    case tk_structure:
      return type_node_struct_size(tb, tn);
    case tk_union:
      return type_node_union_size(tb, tn);
    case tk_function:
      return 0;
    case tk_pointer:
      assert(0 && "Should've been filtered earlier");
      return 0;
    case tk_qualifier:
      assert(0 && "Should've been filtered earlier");
      return 0;
    default:
      assert(0 && "Unhandled case");
      return 0;
  }
}

void type_node_uintptr_expr(struct type_base *tb, enum expr_type *uintptr_and, enum expr_type *uintptr_add, enum expr_type *uintptr_sub, enum expr_type *uintptr_c, enum expr_type *uintptr_mul, enum expr_type *uintptr_less_than) {
  switch (tb->uintptr_equivalent_) {
    case tk_unsigned_int: 
      *uintptr_and = ET_BITWISE_AND_UI;
      *uintptr_add = ET_ADD_UI;
      *uintptr_sub = ET_SUB_UI;
      *uintptr_c = ET_C_UNSIGNED_INT;
      *uintptr_mul = ET_MUL_UI;
      *uintptr_less_than = ET_LESS_THAN_UI;
      return;
    case tk_unsigned_long_int: 
      *uintptr_and = ET_BITWISE_AND_ULI;
      *uintptr_add = ET_ADD_ULI;
      *uintptr_sub = ET_SUB_ULI;
      *uintptr_c = ET_C_UNSIGNED_LONG_INT;
      *uintptr_mul = ET_MUL_ULI;
      *uintptr_less_than = ET_LESS_THAN_ULI;
      return;
    case tk_unsigned_long_long_int:
      *uintptr_and = ET_BITWISE_AND_ULLI;
      *uintptr_add = ET_ADD_ULLI;
      *uintptr_sub = ET_SUB_ULLI;
      *uintptr_c = ET_C_UNSIGNED_LONG_LONG_INT;
      *uintptr_mul = ET_MUL_ULLI;
      *uintptr_less_than = ET_LESS_THAN_ULLI;
      return;
  }
  assert(0 && "Cannot work with uintptr type");
}

void type_node_ptrdiff_expr(struct type_base *tb, enum expr_type *ptrdiff_and, enum expr_type *ptrdiff_add, enum expr_type *ptrdiff_sub, enum expr_type *ptrdiff_c, enum expr_type *ptrdiff_mul, enum expr_type *ptrdiff_less_than) {
  switch (tb->ptrdiff_equivalent_) {
    case tk_int: 
      *ptrdiff_and = ET_BITWISE_AND_SI;
      *ptrdiff_add = ET_ADD_SI;
      *ptrdiff_sub = ET_SUB_SI;
      *ptrdiff_c = ET_C_INT;
      *ptrdiff_mul = ET_MUL_SI;
      *ptrdiff_less_than = ET_LESS_THAN_SI;
      return;
    case tk_long_int: 
      *ptrdiff_and = ET_BITWISE_AND_SLI;
      *ptrdiff_add = ET_ADD_SLI;
      *ptrdiff_sub = ET_SUB_SLI;
      *ptrdiff_c = ET_C_LONG_INT;
      *ptrdiff_mul = ET_MUL_SLI;
      *ptrdiff_less_than = ET_LESS_THAN_SLI;
      return;
    case tk_long_long_int:
      *ptrdiff_and = ET_BITWISE_AND_SLLI;
      *ptrdiff_add = ET_ADD_SLLI;
      *ptrdiff_sub = ET_SUB_SLLI;
      *ptrdiff_c = ET_C_LONG_LONG_INT;
      *ptrdiff_mul = ET_MUL_SLLI;
      *ptrdiff_less_than = ET_LESS_THAN_SLLI;
      return;
  }
  assert(0 && "Cannot work with uintptr type");
}

static struct expr *type_node_const_expr(struct c_compiler *cc, enum expr_type et_op, uint64_t val) {
  struct expr *x;
  x = expr_alloc(et_op);
  if (!x) {
    cc_no_memory(cc);
    return NULL;
  }
  x->v_.u64_ = val;
  return x;
}

static struct expr *type_node_binop_expr(struct c_compiler *cc, enum expr_type et_op, struct expr *lvalue, struct expr *rvalue) {
  if ((!lvalue) || (!rvalue)) {
    /* Bubble errors upwards, freeing any live branches */
    expr_free(lvalue);
    expr_free(rvalue);
    return NULL;
  }
  struct expr *x;
  x = expr_alloc(et_op);
  if (!x) {
    cc_no_memory(cc);
    expr_free(lvalue);
    expr_free(rvalue);
    return NULL;
  }
  x->children_[0] = lvalue;
  x->children_[1] = rvalue;
  return x;
}

static struct expr *type_node_cond_expr(struct c_compiler *cc, struct expr *cond, struct expr *lvalue, struct expr *rvalue) {
  if ((!cond) || (!lvalue) || (!rvalue)) {
    /* Bubble errors upwards, freeing any live branches */
    expr_free(cond);
    expr_free(lvalue);
    expr_free(rvalue);
    return NULL;
  }
  struct expr *x;
  x = expr_alloc(ET_CONDITION);
  if (!x) {
    cc_no_memory(cc);
    expr_free(cond);
    expr_free(lvalue);
    expr_free(rvalue);
    return NULL;
  }
  x->children_[0] = cond;
  x->children_[1] = lvalue;
  x->children_[2] = rvalue;
  return x;
}

struct expr *type_node_struct_offset_expr(struct c_compiler *cc, struct type_node *tn_struct, struct type_field *offset_field) {
  struct expr *current_offset = NULL;
  uint64_t current_align = 0;

  if (!tn_struct) return NULL; /* trapdoor for errors */
  assert(tn_struct->kind_ == tk_structure);

  enum expr_type et_and, et_add, et_sub, et_c, et_mul, et_lt;
  type_node_uintptr_expr(&cc->tb_, &et_and, &et_add, &et_sub, &et_c, &et_mul, &et_lt);
  struct type_field *field = tn_struct->fields_;
  if (field) {
    do {
      field = field->chain_;

      uint64_t field_align = type_node_align(&cc->tb_, field->type_);
      if (field_align > current_align) {
        current_align = field_align;
      }

      if (current_offset) {
        /* First pad offset to field's alignment.... */
        /* current_offset = current_offset + (field_align - 1) */
        current_offset = type_node_binop_expr(cc, et_add, current_offset, type_node_const_expr(cc, et_c, field_align - 1));
        /* current_offset = current_offset & ~(field_align - 1) */
        current_offset = type_node_binop_expr(cc, et_and, current_offset, type_node_const_expr(cc, et_c, ~(field_align - 1)));
      }

      if (field == offset_field) {
        /* Align to field and bail out */
        if (!current_offset && (field == tn_struct->fields_->chain_)) {
          /* Must return 0 if the current_offset has not yet been determined; this is only true if this is the first
           * field, otherwise we're propagating a failure (and should return NULL.) */
          return type_node_const_expr(cc, et_c, 0);
        }
        return current_offset;
      }

      struct expr *field_size = type_node_size_expr(cc, field->type_);

      if (current_offset) {
        /* Add size of field */
        current_offset = type_node_binop_expr(cc, et_add, current_offset, field_size);
      }
      else {
        current_offset = field_size;
      }

    } while (field != tn_struct->fields_);
  }

  /* offset_field NULL or not inside struct, the latter is a problem. */
  assert(!offset_field && "offset_field not found in struct");

  /* No field was matched, ensure the total struct size is aligned to its biggest alignment */
  /* current_offset = current_offset + (current_align - 1) */
  current_offset = type_node_binop_expr(cc, et_add, current_offset, type_node_const_expr(cc, et_c, current_align - 1));
  /* current_offset = current_offset & ~(current_align - 1) */
  current_offset = type_node_binop_expr(cc, et_and, current_offset, type_node_const_expr(cc, et_c, ~(current_align - 1)));

  return current_offset;
}

static struct expr *type_node_struct_size_expr(struct c_compiler *cc, struct type_node *tn) {
  return type_node_struct_offset_expr(cc, tn, NULL);
}

static struct expr *type_node_union_size_expr(struct c_compiler *cc, struct type_node *tn) {
  struct expr *current_val = NULL;
  if (!tn) return NULL;
  struct type_field *field = tn->fields_;
  enum expr_type et_and, et_add, et_sub, et_c, et_mul, et_lt;
  type_node_uintptr_expr(&cc->tb_, &et_and, &et_add, &et_sub, &et_c, &et_mul, &et_lt);
  if (field) {
    do {
      field = field->chain_;

      struct expr *field_size = type_node_size_expr(cc, field->type_);
      if (!current_val) {
        current_val = field_size;
      }
      else {
          /* Construct a DAG for the expression "current_val < field_size ? field_size : current_val"
           * This is a DAG, the expr struct ensures each branch is evaluated only once. */
        struct expr *less_than = type_node_binop_expr(cc, et_lt, current_val, field_size);
        if (less_than) {
          /* less_than successfully existing implies both current_val and field_size are valid */
          current_val->refs_++;
          field_size->refs_++;
        }

        current_val = type_node_cond_expr(cc, less_than, field_size, current_val);
      }
    } while (field != tn->fields_);
  }
  return current_val;
}

struct expr *type_node_size_expr(struct c_compiler *cc, struct type_node *tn) {
  if (!tn) return NULL;

  enum expr_type et_and, et_add, et_sub, et_c, et_mul, et_lt;
  type_node_uintptr_expr(&cc->tb_, &et_and, &et_add, &et_sub, &et_c, &et_mul, &et_lt);

  tn = type_node_unqualified(tn);
  enum type_kind tk = tn->kind_;
  if (tk == tk_enumeration) {
    tk = type_node_arith_type_kind_no_enum(&cc->tb_, tn);
  }
  if (tk == tk_pointer) {
    tk = cc->tb_.uintptr_equivalent_;
  }

  switch (tk) {
    case tk_invalid: return 0;
    case tk_char:
    case tk_signed_char:
      return type_node_const_expr(cc, et_c, 1);
    case tk_short_int:
      return type_node_const_expr(cc, et_c, 2);
    case tk_int:
      return type_node_const_expr(cc, et_c, (uint64_t)(cc->tb_.bits_per_int_ / 8));
    case tk_long_int:
      return type_node_const_expr(cc, et_c, (uint64_t)(cc->tb_.bits_per_long_ / 8));
    case tk_long_long_int:
      return type_node_const_expr(cc, et_c, (uint64_t)(cc->tb_.bits_per_long_long_ / 8));
    case tk_bool:
      return type_node_const_expr(cc, et_c, 1);
    case tk_unsigned_char:
      return type_node_const_expr(cc, et_c, 1);
    case tk_unsigned_short_int:
      return type_node_const_expr(cc, et_c, 2);
    case tk_unsigned_int:
      return type_node_const_expr(cc, et_c, (uint64_t)(cc->tb_.bits_per_int_ / 8));
    case tk_unsigned_long_int:
      return type_node_const_expr(cc, et_c, (uint64_t)(cc->tb_.bits_per_long_ / 8));
    case tk_unsigned_long_long_int:
      return type_node_const_expr(cc, et_c, (uint64_t)(cc->tb_.bits_per_long_long_ / 8));
    case tk_float:
      return type_node_const_expr(cc, et_c, 4);
    case tk_double:
    case tk_long_double:
      return type_node_const_expr(cc, et_c, 8);
    case tk_float_complex:
      return type_node_const_expr(cc, et_c, 8);
    case tk_double_complex:
    case tk_long_double_complex:
      return type_node_const_expr(cc, et_c, 16);
    case tk_float_imaginary:
      return type_node_const_expr(cc, et_c, 4);
    case tk_double_imaginary:
    case tk_long_double_imaginary:
      return type_node_const_expr(cc, et_c, 8);
    case tk_void:
      return 0;
    case tk_enumeration:
      assert(0 && "Should've been filtered earlier");
      return 0;
    case tk_array: {
      if (!tn->array_size_expr_) return NULL;
      struct expr *array_size = expr_clone(tn->array_size_expr_);
      if (!array_size) return NULL;
      return type_node_binop_expr(cc, et_mul, array_size, type_node_size_expr(cc, tn->derived_from_));
    }
    case tk_structure:
      return type_node_struct_size_expr(cc, tn);
    case tk_union:
      return type_node_union_size_expr(cc, tn);
    case tk_function:
      return 0;
    case tk_pointer:
      assert(0 && "Should've been filtered earlier");
      return 0;
    case tk_qualifier:
      assert(0 && "Should've been filtered earlier");
      return 0;
    default:
      assert(0 && "Unhandled case");
      return 0;
  }

}
