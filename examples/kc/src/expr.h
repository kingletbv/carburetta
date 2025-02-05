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

#ifndef EXPR_H
#define EXPR_H

#ifndef STDINT_H_INCLUDED
#define STDINT_H_INCLUDED
#include <stdint.h>
#endif

#ifndef STDIO_H_INCLUDED
#define STDIO_H_INCLUDED
#include <stdio.h>
#endif

#ifndef EXPR_NODE_TYPES_H_INCLUDED
#define EXPR_NODE_TYPES_H_INCLUDED
#include "expr_node_types.h"
#endif

#ifndef C_COMPILER_H_INCLUDED
#define C_COMPILER_H_INCLUDED
#include "c_compiler.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define EXPR_FIND_DECL_NONE_FOUND 0
#define EXPR_FIND_DECL_ONE_FOUND 1
#define EXPR_FIND_DECL_MULTIPLE_FOUND 2
#define EXPR_FIND_DECL_ERROR -1

struct expr_temp {
  /* Literal data of the node if it is a arithmetic value, or a pointer to the literal data
   * in the data section. The type of the expr node determines which field in the union (if any)
   * is the relevant one. Note that, for integer types, all sizes of integer map onto either
   * i64_ or u64_, and in the case if i64_ are sign-extended, and in the case of u64_ zero-extended.
   * The reasoning behind this is that the _actual_ implementation sizes are variable, so the maximum
   * 64 bits makes for an easy default. */
  union {
    uint64_t u64_;
    int64_t i64_;
    double d_;
    double di_;
    float f_;
    float fi_;
    struct { double d_, di_; } dc_;
    struct { float f_, fi_; } fc_;
  } v_;
};

struct expr {
  /* Type of expression node */
  enum expr_type et_;

  /* The children of this node; this forms a directed acyclic graph (the same expr node
   * may have multiple parents.) */
  struct expr *children_[3];

  /* Number of references (parents) this expr node has. */
  int refs_;

  /* The ordinal (depth-first traversal sequence number) of this expression node in the larger
   * expression. This is used to assign a temporary value slot as well as detect DAGs (references
   * back to self, which should not be re-evaluated during an evaluation.) */
  int ord_;

  /* Total number of ordinals for this node, and all its children, excluding DAGs to earlier
   * nodes. */
  int num_ords_;

  /* Type argument for the expr node types that require an explicit type. */
  struct type_node *type_arg_;

  /* Is this sub-expression used multiple times in the whole expression? */
  int is_backreferenced_ : 1;

  /* Is this expression a constant expression? Filled by expr_is_constant() */
  int is_constant_ : 1;

  /* If this is a bitfield, different rules apply (integer promotions and whether an
   * address can be taken. */
  int is_bitfield_:1;
  int bitfield_size_;

  /* Literal data of the node if it is a arithmetic value, or a pointer to the literal data 
   * in the data section. The type of the expr node determines which field in the union (if any)
   * is the relevant one. Note that, for integer types, all sizes of integer map onto either
   * i64_ or u64_, and in the case if i64_ are sign-extended, and in the case of u64_ zero-extended.
   * The reasoning behind this is that the _actual_ implementation sizes are variable, so the maximum
   * 64 bits makes for an easy default. */
  union {
    uint64_t u64_;
    int64_t i64_;
    double d_;
    double di_;
    float f_;
    float fi_;
    struct { double d_, di_; } dc_;
    struct { float f_, fi_; } fc_;
  } v_;

  /* Pointer to literal data in data_section */
  struct ds_portion *dsp_; 

  /* Pointer to the declaration */
  struct decl *decl_;

  /* Pointer to an expr used for cloning expression DAGs */
  struct expr *scratch_;
};

struct expr *expr_alloc(enum expr_type et);
void expr_free(struct expr *x);
struct expr *expr_clone(struct expr *x);
struct type_node *expr_type(struct c_compiler *cc, struct expr *src);
int expr_is_null_ptr(struct c_compiler *cc, struct expr *x, int *is_null_ptr);

/* Decays arrays and function types to pointers. */
int expr_pointer_decay(struct c_compiler *cc, struct expr **px);

/* Converts the type of the node (enum expr_type) to a string, for debugging */
const char *expr_node_type_to_str(struct expr *x);

/* Prints the hierarchy of expr node types as a oneliner. */
void expr_debug_print(struct c_compiler *cc, struct expr *x);

/* Prints the value of the (already evaluated) expression
 * Returns non-zero if no value could be printed (e.g. unprintable type or
 * a failure,) or zero upon success.
 * In the case of no success, a message will be printed. */
int expr_print_value(struct c_compiler *cc, struct expr *x, struct expr_temp *temps);

/* Prints the value of the temp whose type matches that of the expression x
 * Returns non-zero if no value could be printed (e.g. unprintable type or
 * a failure,) or zero upon success.
 * In the case of no success, a message will be printed. */
int expr_print_temp(struct c_compiler *cc, struct expr *x, struct expr_temp *temp);

/* Converts the value in expression src to the type to. Both src and to must be arithmetically
 * typed (eg. an integer type, a floating type, or one of the complex/imaginary types, or an enumeration
 * that has been fully evaluated (eg. has a compatible integer type.)).
 * Upon success, returns the expression tree that contains src, and converts it to the desired 'to' type.
 * Upon failure, returns NULL; src will be unaffected. */
struct expr *expr_convert_arithmetic_type(struct c_compiler *cc, struct type_node *to, struct expr *src);

/* Like expr_convert_arithmetic_type, but includes pointers */
struct expr *expr_convert_type(struct c_compiler *cc, struct type_node *to, struct expr *src);

/* Returns the arithmetic type kind of the expression, if, however, the kind is enum, then the
 * compatibile integer type of the enumeration will be returned instead. */
enum type_kind expr_arith_type_kind_no_enum(struct c_compiler *cc, struct expr *x);

void expr_prepare(struct expr *x);
int expr_eval(struct c_compiler *cc, struct expr_temp *result, struct expr *x, int is_constant_expr, struct decl *nulled_out_decl, void *local_base, void *param_base, void *return_value_ptr);

int expr_constant_evaluation(struct c_compiler *cc, struct expr_temp *result, struct expr *x);

int expr_integer_promotions(struct c_compiler *cc, struct expr **expr);
int expr_usual_arithmetic_conversions(struct c_compiler *cc, struct expr **left_expr, struct expr **right_expr);

int expr_mul(struct c_compiler *cc, struct expr **dst, struct situs *left_loc, struct expr **left, struct situs *op_loc, struct situs *right_loc, struct expr **right);
int expr_div(struct c_compiler *cc, struct expr **dst, struct situs *left_loc, struct expr **left, struct situs *op_loc, struct situs *right_loc, struct expr **right);
int expr_rem(struct c_compiler *cc, struct expr **dst, struct situs *left_loc, struct expr **left, struct situs *op_loc, struct situs *right_loc, struct expr **right);

int expr_shift_left(struct c_compiler *cc, struct expr **dst, struct situs *left_loc, struct expr **left, struct situs *op_loc, struct situs *right_loc, struct expr **right);
int expr_shift_right(struct c_compiler *cc, struct expr **dst, struct situs *left_loc, struct expr **left, struct situs *op_loc, struct situs *right_loc, struct expr **right);

int expr_less_than(struct c_compiler *cc, struct expr **dst, struct situs *left_loc, struct expr **left, struct situs *op_loc, struct situs *right_loc, struct expr **right);
int expr_greater_than(struct c_compiler *cc, struct expr **dst, struct situs *left_loc, struct expr **left, struct situs *op_loc, struct situs *right_loc, struct expr **right);
int expr_less_than_or_equal(struct c_compiler *cc, struct expr **dst, struct situs *left_loc, struct expr **left, struct situs *op_loc, struct situs *right_loc, struct expr **right);
int expr_greater_than_or_equal(struct c_compiler *cc, struct expr **dst, struct situs *left_loc, struct expr **left, struct situs *op_loc, struct situs *right_loc, struct expr **right);

int expr_add(struct c_compiler *cc, struct expr **dst, struct situs *left_loc, struct expr **left, struct situs *op_loc, struct situs *right_loc, struct expr **right);
int expr_sub(struct c_compiler *cc, struct expr **dst, struct situs *left_loc, struct expr **left, struct situs *op_loc, struct situs *right_loc, struct expr **right);
int expr_unary_plus(struct c_compiler *cc, struct expr **dst, struct situs *op_loc, struct situs *opd_loc, struct expr **operand);
int expr_unary_minus(struct c_compiler *cc, struct expr **dst, struct situs *op_loc, struct situs *opd_loc, struct expr **operand);
int expr_bitwise_complement(struct c_compiler *cc, struct expr **dst, struct situs *op_loc, struct situs *opd_loc, struct expr **operand);
int expr_bitwise_and(struct c_compiler *cc, struct expr **dst, struct situs *left_loc, struct expr **left, struct situs *op_loc, struct situs *right_loc, struct expr **right);
int expr_bitwise_xor(struct c_compiler *cc, struct expr **dst, struct situs *left_loc, struct expr **left, struct situs *op_loc, struct situs *right_loc, struct expr **right);
int expr_bitwise_or(struct c_compiler *cc, struct expr **dst, struct situs *left_loc, struct expr **left, struct situs *op_loc, struct situs *right_loc, struct expr **right);

int expr_logical_not(struct c_compiler *cc, struct expr **dst, struct situs *op_loc, struct situs *opd_loc, struct expr **operand);
int expr_logical_and(struct c_compiler *cc, struct expr **dst, struct situs *left_loc, struct expr **left, struct situs *op_loc, struct situs *right_loc, struct expr **right);
int expr_logical_or(struct c_compiler *cc, struct expr **dst, struct situs *left_loc, struct expr **left, struct situs *op_loc, struct situs *right_loc, struct expr **right);
int expr_indirection(struct c_compiler *cc, struct expr **dst, struct situs *op_loc, struct situs *opd_loc, struct expr **operand);
int expr_post_inc(struct c_compiler *cc, struct expr **dst, struct situs *op_loc, struct situs *opd_loc, struct expr **operand);
int expr_post_dec(struct c_compiler *cc, struct expr **dst, struct situs *op_loc, struct situs *opd_loc, struct expr **operand);
int expr_pre_inc(struct c_compiler *cc, struct expr **dst, struct situs *op_loc, struct situs *opd_loc, struct expr **operand);
int expr_pre_dec(struct c_compiler *cc, struct expr **dst, struct situs *op_loc, struct situs *opd_loc, struct expr **operand);
int expr_address_of(struct c_compiler *cc, struct expr **dst, struct situs *op_loc, struct situs *opd_loc, struct expr **operand);
int expr_cast(struct c_compiler *cc, struct expr **dst, struct situs *type_loc, struct type_node *ptype, struct situs *val_loc, struct expr **val);
int expr_assign(struct c_compiler *cc, struct expr **dst, struct situs *left_loc, struct expr **left, struct situs *op_loc, struct situs *right_loc, struct expr **right);
int expr_assign_mul(struct c_compiler *cc, struct expr **dst, struct situs *left_loc, struct expr **left, struct situs *op_loc, struct situs *right_loc, struct expr **right);
int expr_assign_div(struct c_compiler *cc, struct expr **dst, struct situs *left_loc, struct expr **left, struct situs *op_loc, struct situs *right_loc, struct expr **right);
int expr_assign_rem(struct c_compiler *cc, struct expr **dst, struct situs *left_loc, struct expr **left, struct situs *op_loc, struct situs *right_loc, struct expr **right);
int expr_assign_add(struct c_compiler *cc, struct expr **dst, struct situs *left_loc, struct expr **left, struct situs *op_loc, struct situs *right_loc, struct expr **right);
int expr_assign_sub(struct c_compiler *cc, struct expr **dst, struct situs *left_loc, struct expr **left, struct situs *op_loc, struct situs *right_loc, struct expr **right);
int expr_assign_shift_left(struct c_compiler *cc, struct expr **dst, struct situs *left_loc, struct expr **left, struct situs *op_loc, struct situs *right_loc, struct expr **right);
int expr_assign_shift_right(struct c_compiler *cc, struct expr **dst, struct situs *left_loc, struct expr **left, struct situs *op_loc, struct situs *right_loc, struct expr **right);
int expr_assign_bitwise_and(struct c_compiler *cc, struct expr **dst, struct situs *left_loc, struct expr **left, struct situs *op_loc, struct situs *right_loc, struct expr **right);
int expr_assign_bitwise_xor(struct c_compiler *cc, struct expr **dst, struct situs *left_loc, struct expr **left, struct situs *op_loc, struct situs *right_loc, struct expr **right);
int expr_assign_bitwise_or(struct c_compiler *cc, struct expr **dst, struct situs *left_loc, struct expr **left, struct situs *op_loc, struct situs *right_loc, struct expr **right);

int expr_equality(struct c_compiler *cc, struct expr **dst, struct situs *left_loc, struct expr **left, struct situs *op_loc, struct situs *right_loc, struct expr **right);
int expr_inequality(struct c_compiler *cc, struct expr **dst, struct situs *left_loc, struct expr **left, struct situs *op_loc, struct situs *right_loc, struct expr **right);

int expr_sequence(struct c_compiler *cc, struct expr **dst, struct situs *left_loc, struct expr **left, struct situs *op_loc, struct situs *right_loc, struct expr **right);

/* Evaluates to int 0 if false, and int 1 if true. */
int expr_cond_is_true(struct c_compiler *cc, struct expr **dst, struct situs *loc, struct expr **val);

int expr_condition(struct c_compiler *cc, struct expr **dst, struct situs *cond_loc, struct expr **cond, struct situs *true_loc, struct expr **true_expr, struct situs *false_loc, struct expr **false_expr);
int expr_array_index(struct c_compiler *cc, struct expr **dst, struct situs *arr_loc, struct expr **arr, struct situs *op_loc, struct situs *index_loc, struct expr **index);
int expr_function_call(struct c_compiler *cc, struct expr **dst, struct situs *func_loc, struct expr **func, struct situs *op_loc, struct situs *args_loc, struct expr **args);

int expr_ptr_member_access(struct c_compiler *cc, struct expr **dst, struct situs *exp_loc, struct expr **exp, struct situs *op_loc, struct situs *field_loc, const char *field_ident, const char *operator_str);
int expr_lvalue_member_access(struct c_compiler *cc, struct expr **dst, struct situs *exp_loc, struct expr **exp, struct situs *op_loc, struct situs *field_loc, const char *field_ident);

/* Returns non-zero if the expression is a constant expression, or zero if there is a variable part to the expression. */
int expr_is_constant(struct expr *x);

/* Returns the decl of an expression if that expression is an address constant (see C99 6.6 Constant expressions paragraph 9).
 * Return values may be one of EXPR_FIND_DECL_xxx:
 * EXPR_FIND_DECL_NONE_FOUND: no decl was found, the expression x is otherwise a valid constant.
 * EXPR_FIND_DECL_ONE_FOUND: a single decl was found and has been stored in *addressed. x is a valid address constant.
 * EXPR_FIND_DECL_MULTIPLE_FOUND: multiple decl's were found in this expression, x is not a valid address constant.
 * EXPR_FIND_DECL_ERROR: x is not a valid constant. */
int expr_find_decl_addressed(struct c_compiler *cc, struct expr *x, struct decl **addressed);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* EXPR_H */
