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

#ifndef ASSERT_H_INCLUDED
#define ASSERT_H_INCLUDED
#include <assert.h>
#endif

#ifndef MATH_H_INCLUDED
#define MATH_H_INCLUDED
#include <math.h>
#endif

#ifndef INTTYPES_H_INCLUDED
#define INTTYPES_H_INCLUDED
#include <inttypes.h>
#endif

#ifndef STRING_H_INCLUDED
#define STRING_H_INCLUDED
#include <string.h>
#endif

#ifndef C_COMPILER_H_INCLUDED
#define C_COMPILER_H_INCLUDED
#include "c_compiler.h"
#endif

#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED
#include "types.h"
#endif

#ifndef DATA_SECTION_H_INCLUDED
#define DATA_SECTINO_H_INCLUDED
#include "data_section.h"
#endif

#ifndef EXPR_H_INCLUDED
#define EXPR_H_INCLUDED
#include "expr.h"
#endif

#ifndef DECL_H_INCLUDED
#define DECL_H_INCLUDED
#include "decl.h"
#endif

#ifndef DYNAMIC_RUNTIME_LINKING_H_INCLUDED
#define DYNAMIC_RUNTIME_LINKING_H_INCLUDED
#include "dynamic_runtime_linking.h"
#endif

#ifndef INVOKE_X64_INCLUDED
#define INVOKE_X64_INCLUDED
#include "invoke_x64.h"
#endif

int expr_pointer_decay(struct c_compiler *cc, struct expr **px) {
  /* 6.3.2.1-3 (arrays) and 6.3.2.1-4 (functions) decay into their pointers,
   * applies to almost all operator types, save for a few noted exceptions (sizeof
   * unary &, ++, --, left of a ., or an assignment operator.) */
  if (!px || !*px) return 0;

  struct type_node *tn = expr_type(cc, *px);
  if (!tn) {
    cc_error(cc, "unable to discover type\n");
    return -1;
  }
  int qualifiers = tn->kind_ == tk_qualifier ? tn->qualifiers_ : 0;
  tn = type_node_unqualified(tn);
  if (tn->kind_ == tk_array) {
    /* Pop the indirection that converts the data pointer to an array to an array, and 
     * cast to a pointer to the array element, re-applying the qualifiers (if any.) */
    if ((*px)->et_ != ET_INDIRECTION_PTR) {
      cc_printf(cc, "Internal Error: Array is not an indirected pointer\n");
      return -1;
    }
    struct expr *ptr_exp = (*px)->children_[0];
    /* left is parent of ptr_exp; we now drop the reference so parent frees while ptr child remains */
    struct expr *x = expr_alloc(ET_CVPTR2PTR);
    if (!x) {
      cc_no_memory(cc);
      return -1;
    }

    x->children_[0] = ptr_exp;
    ptr_exp->refs_++; /* expr_free() of *left parent decrements this back down by one; the ++ therefore ensures its survival */
    expr_free(*px);
    *px = x;
    struct type_node *tn_ptr_to_arr_elm = type_base_pointer(&cc->tb_, tn->derived_from_);
    tn_ptr_to_arr_elm = type_base_qualifier(&cc->tb_, tn_ptr_to_arr_elm, qualifiers);
    if (!tn_ptr_to_arr_elm) {
      cc_no_memory(cc);
      return -1;
    }
    x->type_arg_ = tn_ptr_to_arr_elm;
    return 0;
  }
  else if (tn->kind_ == tk_function) {
    /* Pop the indirection and return the underlying node */
    if ((*px)->et_ != ET_INDIRECTION_PTR) {
      cc_printf(cc, "Internal Error: Function is not an indirected pointer\n");
      return -1;
    }
    struct expr *ptr_exp = (*px)->children_[0];
    ptr_exp->refs_++;
    expr_free(*px);
    *px = ptr_exp;
    return 0;
  }
  else {
    /* Keep as-is, no decay to pointer */
    return 0;
  }

  return 0;
}

static enum expr_type expr_get_next_arithmetic_conversion_step(struct c_compiler *cc, enum type_kind current, enum type_kind destination) {
  if (current == destination) {
    return ET_NOP;
  }

  /* Check for Imaginary to imaginary & complex number conversions */
  if ((current == tk_float_imaginary) || (current == tk_double_imaginary) || (current == tk_long_double_imaginary)) {
    if ((destination == tk_float_imaginary) || (destination == tk_double_imaginary) || (destination == tk_long_double_imaginary)) {
      /* Imaginary up or down conversions */
      if (current == tk_float_imaginary) {
        if ((destination == tk_double_imaginary) || (destination == tk_double_imaginary)) {
          return ET_CVFI2DI;
        }
      }
      else if (current == tk_double_imaginary) {
        if (destination == tk_float_imaginary) {
          return ET_CVDI2FI;
        }
        else if (destination == tk_long_double_imaginary) {
          return ET_CVDI2LDI;
        }
      }
      else if (current == tk_long_double_imaginary) {
        if ((destination == tk_float_imaginary) || (destination == tk_double_imaginary)) {
          return ET_CVLDI2DI;
        }
      }
    }
    else if ((destination == tk_float_complex) || (destination == tk_double_complex) || (destination == tk_long_double_complex)) {
      if (current == tk_float_imaginary) {
        return ET_CVFI2FC;
      }
      else if (current == tk_double_imaginary) {
        return ET_CVDI2DC;
      }
      else if (current == tk_long_double_imaginary) {
        return ET_CVLDI2LDC;
      }
    }
    else {
      /* Current is imaginary and Destination is neither Imaginary nor Complex, convert to Real,
       * later conversions will take the Real to the final destination.
       * While this conversion from Imaginary to Real is equivalent to the Real number 0., we still
       * have explicit conversion expr types nodes for analysis. */
      if (current == tk_float_imaginary) {
        return ET_CVFI2F;
      }
      else if (current == tk_double_imaginary) {
        return ET_CVDI2D;
      }
      else if (current == tk_long_double_imaginary) {
        return ET_CVLDI2LD;
      }
    }
  }
  else if ((current == tk_float_complex) || (current == tk_double_complex) || (current == tk_long_double_complex)) {
    if ((destination == tk_float_complex) || (destination == tk_double_complex) || (destination == tk_long_double_complex)) {
      /* Complex up or down conversions */
      if (current == tk_float_complex) {
        if ((destination == tk_double_complex) || (destination == tk_long_double_complex)) {
          return ET_CVFC2DC;
        }
      }
      else if (current == tk_double_complex) {
        if (destination == tk_float_complex) {
          return ET_CVDC2FC;
        }
        else if (destination == tk_long_double_complex) {
          return ET_CVDC2LDC;
        }
      }
      else if (current == tk_long_double_complex) {
        if ((destination == tk_float_complex) || (destination == tk_double_complex)) {
          return ET_CVLDC2DC;
        }
      }
    }
    else if ((destination == tk_float_imaginary) || (destination == tk_double_imaginary) || (destination == tk_long_double_imaginary)) {
      if (current == tk_float_complex) {
        return ET_CVFC2FI;
      }
      else if (current == tk_double_complex) {
        return ET_CVDC2DI;
      }
      else if (current == tk_long_double_complex) {
        return ET_CVLDC2LDI;
      }
    }
    else {
      /* Current is Complex and Destination is neither Imaginary nor Complex, convert to Real,
       * later conversions will take the Real type to its final destination. */
      if (current == tk_float_complex) {
        return ET_CVFC2F;
      }
      else if (current == tk_double_complex) {
        return ET_CVDC2D;
      }
      else if (current == tk_long_double_complex) {
        return ET_CVLDC2LD;
      }
    }
  }

  /* Handled all current == imaginary or complex, situations. Current definitely not a complex type. */

  if ((destination == tk_float_imaginary) || (destination == tk_double_imaginary) || (destination == tk_long_double_imaginary)) {
    /* Destination is Imaginary but the current is real. Check if we can make the direct conversion, if not, set a new destination
     * so we get closer to the point where we can make a direct conversion. */
    if (destination == tk_float_imaginary) {
      if (current == tk_float) {
        return ET_CVF2FI;
      }
      else {
        destination = tk_float;
      }
    }
    else if (destination == tk_double_imaginary) {
      if (current == tk_double) {
        return ET_CVD2DI;
      }
      else {
        destination = tk_double;
      }
    }
    else if (destination == tk_long_double_imaginary) {
      if (current == tk_long_double) {
        return ET_CVLD2LDI;
      }
      else {
        destination = tk_long_double;
      }
    }
  }

  if ((destination == tk_float_complex) || (destination == tk_double_complex) || (destination == tk_long_double_complex)) {
    /* Destination is Complex but the current is real. Check if we can make the direct conversion. If not, set a new destination
     * so we get closer to the point where we can make a direct conversion. */
    if (destination == tk_float_complex) {
      if (current == tk_float) {
        return ET_CVF2FC;
      }
      else {
        destination = tk_float;
      }
    }
    else if (destination == tk_double_complex) {
      if (current == tk_double) {
        return ET_CVD2DC;
      }
      else {
        destination = tk_double;
      }
    }
    else if (destination == tk_long_double_complex) {
      if (current == tk_long_double) {
        return ET_CVLD2LDC;
      }
      else {
        destination = tk_long_double;
      }
    }
  }

  /* All complex types handled. */

  /* Handle the "Bool" type, conversions to bool have unique expression nodes to reflect
   * that (x == 0) is the conversion. This makes it distinct from unsigned char. */
  if (current == tk_bool) {
    /* Convert current to unsigned char */
    return ET_CVB2UC;
  }
  else if (destination == tk_bool) {
    switch (current) {
    case tk_unsigned_char:
      return ET_CVUC2B;
    case tk_signed_char:
      return ET_CVSC2B;
    case tk_unsigned_short_int:
      return ET_CVUS2B;
    case tk_short_int:
      return ET_CVSS2B;
    case tk_unsigned_int:
      return ET_CVUI2B;
    case tk_int:
      return ET_CVSI2B;
    case tk_unsigned_long_int:
      return ET_CVULI2B;
    case tk_long_int:
      return ET_CVSLI2B;
    case tk_unsigned_long_long_int:
      return ET_CVULLI2B;
    case tk_long_long_int:
      return ET_CVSLLI2B;
    case tk_float:
      return ET_CVF2B;
    case tk_double:
      return ET_CVD2B;
    case tk_long_double:
      return ET_CVLD2B;
    }
    /* Type unknown, cannot be a complex/imaginary type as we've already handled those,
     * and conversions to Bool should all have direct equivalents. */
    return ET_INVALID;
  }

  /* Handle the "char" type, which is either signed or unsigned, and cannot be handled
   * by the rest of the logic until that is determined. */
  if (current == tk_char) {
    if (destination == tk_unsigned_char) {
      return ET_CVC2UC;
    }
    else if (destination == tk_signed_char) {
      return ET_CVC2SC;
    }
    else {
      /* Convert away from tk_char to the signed or unsigned equivalent and then continue conversions from there */
      if (cc->tb_.char_is_signed_) {
        /* Char type is configured as being signed. */
        return ET_CVC2SC;
      }
      else {
        /* Char type is configured as being unsigned. */
        return ET_CVC2UC;
      }
    }
  }
  if (destination == tk_char) {
    if (current == tk_unsigned_char) {
      return ET_CVUC2C;
    }
    else if (current == tk_signed_char) {
      return ET_CVSC2C;
    }
    else {
      /* Destination is char, and current is not a char type.
      * Aim conversion towards either signed or unsigned char, depending
      * on the signed'ness of the char type. */
      if (cc->tb_.char_is_signed_) {
        destination = tk_signed_char;
      }
      else {
        destination = tk_unsigned_char;
      }
      /* Fall through and let regular processing take care of the rest. */
    }
  }

  /* Check for floating point to floating point conversions */
  if (current == tk_float) {
    if ((destination == tk_double) || (destination == tk_long_double)) {
      return ET_CVF2D; /* long double will go again */
    }
  }
  else if (current == tk_double) {
    if (destination == tk_float) {
      return ET_CVD2F;
    }
    else if (destination == tk_long_double) {
      return ET_CVD2LD;
    }
  }
  else if (current == tk_long_double) {
    if ((destination == tk_float) || (destination == tk_double)) {
      return ET_CVLD2D; /* float will go again */
    }
  }

  /* Handle conversions *from* floating types. The key thing is we have to figure out what
   * integer we first convert it to -- this integer has to be the lowest rank integer of rank 
   * equal or greater than the final integer, that can directly qualify. */
  if ((current == tk_float) || (current == tk_double) || (current == tk_long_double)) {
    if (current == tk_long_double) {
      /* Downconvert long doubles, we don't convert those directly and the representation
       * is the same as doubles; but in all cases we first need to downconvert to doubles
       * before we can do anything meaningful. */
      return ET_CVLD2D;
    }
    /* Classify the destination type. If both current and destination are floating types, then
     * we would never get here as those conversions have already been handled above. */
    int destination_is_signed;
    int destination_rank;
    type_kind_classify_integer_type(destination, &destination_is_signed, &destination_rank);
    int dummy, LL_rank, L_rank;
    type_kind_classify_integer_type(tk_unsigned_long_int, &dummy, &L_rank);
    type_kind_classify_integer_type(tk_unsigned_long_long_int, &dummy, &LL_rank);

    if (destination_is_signed) {
      if (destination_rank < L_rank) {
        if (current == tk_float) {
          return ET_CVF2SLI;
        }
        else /* (current == tk_double) */ {
          return ET_CVD2SLI;
        }
      }
      else if (destination_rank < LL_rank) {
        if (current == tk_float) {
          return ET_CVF2SLLI;
        }
        else /* (current == tk_double) */ {
          return ET_CVD2SLLI;
        }
      }
      else {
        /* Destination has rank greater than LL, which must be impossible */
        return ET_INVALID;
      }
    }
    else /* (!destination_signed) */ {
      if (destination_rank < L_rank) {
        if (current == tk_float) {
          return ET_CVF2ULI;
        }
        else /* (current == tk_double) */ {
          return ET_CVD2ULI;
        }
      }
      else if (destination_rank < LL_rank) {
        if (current == tk_float) {
          return ET_CVF2ULLI;
        }
        else /* (current == tk_double) */ {
          return ET_CVD2ULLI;
        }
      }
      else {
        /* Destination has rank greater than LL, which must be impossible */
        return ET_INVALID;
      }
    }
  }

  int current_is_signed;
  int current_rank;
  type_kind_classify_integer_type(current, &current_is_signed, &current_rank);

  /* If going to a floating point type, see if we can convert directly. If we cannot convert directly,
   * then we will first have to do some integer conversions before we reach a state where we can
   * convert to a floating type. Consequently, either convert now, or determine the new (integer)
   * destination that can convert eventually. */
  if ((destination == tk_double) || (destination == tk_float) || (destination == tk_long_double)) {
    /* Conversions for Float, or "not-float" - there are no direct-to-long-double conversions, these
     * are up-converted from double (as they are the same type, implementation-wise, this cuts down
     * on the number of conversions.) */
    if (current == tk_long_int) {
      return (destination == tk_float) ? ET_CVSLI2F : ET_CVSLI2D;
    }
    else if (current == tk_long_long_int) {
      return (destination == tk_float) ? ET_CVSLLI2F : ET_CVSLLI2D;
    }
    else if (current == tk_unsigned_long_int) {
      return (destination == tk_float) ? ET_CVULI2F : ET_CVULI2D;
    }
    else if (current == tk_unsigned_long_long_int) {
      return (destination == tk_float) ? ET_CVULLI2F : ET_CVULLI2D;
    }
    else {
      /* Not ready for straight up conversion yet, change destination
       * to an intermediate goal that will fit all bits, so we're sure to go the right way */
      int dummy, LL_rank, L_rank;
      type_kind_classify_integer_type(tk_unsigned_long_int, &dummy, &L_rank);
      type_kind_classify_integer_type(tk_unsigned_long_long_int, &dummy, &LL_rank);
      if (current_is_signed) {
        if (current_rank <= L_rank) {
          /* Convert to long int first, before conversion to double or float */
          destination = tk_long_int;
        }
        else if (current_rank <= LL_rank) {
          /* Convert to long long int first, before conversion to double or float */
          destination = tk_long_long_int;
        }
        else {
          /* Impossibly high rank, higher than tk_unsigned_long_long_int ?? */
          return ET_INVALID;
        }
      }
      else {
        if (current_rank < L_rank) {
          /* Convert to unsigned long int first, before conversiont to double or float */
          destination = tk_unsigned_long_int;
        }
        else if (current_rank < LL_rank) {
          /* Convert to long long int first, before conversion to double or float */
          destination = tk_long_long_int;
        }
        else {
          /* Impossibly high rank, over long long int. */
          return ET_INVALID;
        }
      }
    }
  }

  int destination_is_signed;
  int destination_rank;
  type_kind_classify_integer_type(destination, &destination_is_signed, &destination_rank);

  if (destination_rank > current_rank) {
    /* Ascend from current */
    switch (current) {
    case tk_signed_char:
      return ET_CVSC2SS;
    case tk_short_int:
      return ET_CVSS2SI;
    case tk_int:
      return ET_CVSI2SLI;
    case tk_long_int:
      return ET_CVSLI2SLLI;
    case tk_long_long_int:
      return ET_INVALID; /* Cannot ascend from largest type (top rank) */
    case tk_unsigned_char:
      return ET_CVUC2US;
    case tk_unsigned_short_int:
      return ET_CVUS2UI;
    case tk_unsigned_int:
      return ET_CVUI2ULI;
    case tk_unsigned_long_int:
      return ET_CVULI2ULLI;
    case tk_unsigned_long_long_int:
      return ET_INVALID; /* Cannot ascend from largest type (top rank) */
    }
  }
  else if (destination_rank < current_rank) {
    /* Descend from current */
    switch (current) {
    case tk_signed_char:
      return ET_INVALID; /* Cannot descend from smallest type (bottom rank) */
    case tk_short_int:
      return ET_CVSS2SC;
    case tk_int:
      return ET_CVSI2SS;
    case tk_long_int:
      return ET_CVSLI2SI;
    case tk_long_long_int:
      return ET_CVSLLI2SLI;
    case tk_unsigned_char:
      return ET_INVALID; /* Cannot descend from smallest type (bottom rank) */
    case tk_unsigned_short_int:
      return ET_CVUS2UC;
    case tk_unsigned_int:
      return ET_CVUI2US;
    case tk_unsigned_long_int:
      return ET_CVULI2UI;
    case tk_unsigned_long_long_int:
      return ET_CVULLI2ULI;
    }
  }
  else if (current_is_signed != destination_is_signed) {
    /* We need to switch signs */
    switch (current) {
      case tk_signed_char:
        return ET_CVSC2UC;
      case tk_short_int:
        return ET_CVSS2US;
      case tk_int:
        return ET_CVSI2UI;
      case tk_long_int:
        return ET_CVSLI2ULI;
      case tk_long_long_int:
        return ET_CVSLLI2ULLI;
      case tk_unsigned_char:
        return ET_CVUC2SC;
      case tk_unsigned_short_int:
        return ET_CVUS2SS;
      case tk_unsigned_int:
        return ET_CVUI2SI;
      case tk_unsigned_long_int:
        return ET_CVULI2SLI;
      case tk_unsigned_long_long_int:
        return ET_CVULLI2SLLI;
    }
  }
  
 
  /* If the rank is identical, and the sign is identical, then the type_kinds
   * must be identical. This implies we should not get here, so the case is invalid.*/
  /* If we get here, then everything about the types is identical, except, the types
   * are not identical. Which should be impossible. The case is invalid */
  assert(0 && "Everything about the types is identical except the types are not identical (?)");
  return ET_INVALID;
}

struct expr *expr_alloc(enum expr_type et) {
  struct expr *x = (struct expr *)malloc(sizeof(struct expr));
  if (!x) return NULL;
  x->et_ = et;
  size_t n;
  for (n = 0; n < (sizeof(x->children_) / sizeof(*x->children_)); ++n) {
    x->children_[n] = NULL;
  }
  x->refs_ = 1;
  x->ord_ = x->num_ords_ = 0;
  x->type_arg_ = NULL;
  x->is_backreferenced_ = 0;
  x->is_constant_ = 0;
  x->is_bitfield_ = 0;
  x->bitfield_size_ = 0;
  memset(&x->v_, 0, sizeof(x->v_));
  x->dsp_ = NULL;
  x->decl_ = NULL;
  x->scratch_ = NULL;
  return x;
}

void expr_free(struct expr *x) {
  if (!x) return;
  if (--x->refs_) return; /* refcount not yet at zero */
  size_t n;
  for (n = 0; n < (sizeof(x->children_) / sizeof(*x->children_)); ++n) {
    expr_free(x->children_[n]);
  }
  free(x);
}

static void expr_clear_scratch(struct expr *x) {
  size_t n;
  if (!x) return;
  if (!x->scratch_) return; /* branch already cleared */
  x->scratch_ = NULL;
  for (n = 0; n < (sizeof(x->children_) / sizeof(*x->children_)); ++n) {
    if (x->children_[n]) {
      expr_clear_scratch(x->children_[n]);
    }
  }
}

static struct expr *expr_clone_impl(int *failed, struct expr *x) {
  size_t n;
  if (!x) return NULL;
  if (x->scratch_) return x->scratch_;
  struct expr *clone = expr_alloc(x->et_);
  if (!clone) {
    *failed = 1;
    return NULL;
  }
  clone->et_ = x->et_;
  clone->refs_ = 0;
  clone->ord_ = x->ord_;
  clone->num_ords_ = x->num_ords_;
  clone->type_arg_ = x->type_arg_;
  clone->is_backreferenced_ = x->is_backreferenced_;
  clone->is_constant_ = x->is_constant_;
  clone->is_bitfield_ = x->is_bitfield_;
  clone->bitfield_size_ = x->bitfield_size_;
  clone->v_ = x->v_;
  clone->dsp_ = x->dsp_;
  clone->decl_ = x->decl_;

  x->scratch_ = clone;
  for (n = 0; n < (sizeof(x->children_) / sizeof(*x->children_)); ++n) {
    struct expr *child = expr_clone_impl(failed, x->children_[n]);
    if (*failed) {
      expr_free(clone);
      return NULL;
    }
    if (child) {
      child->refs_++;
    }
    clone->children_[n] = child;
  }
  return clone;
}

struct expr *expr_clone(struct expr *x) {
  int failed = 0;
  expr_clear_scratch(x);
  struct expr *clone = expr_clone_impl(&failed, x);
  if (failed) {
    expr_free(clone);
    clone = NULL;
  }
  expr_clear_scratch(x);
  if (clone) {
    clone->refs_++;
  }
  return clone;
}

static struct expr *expr_create_array_size_expr(struct c_compiler *cc, struct ds_portion *dsp, size_t element_size) {
  struct expr *array_size_expr = NULL;
  switch (cc->tb_.uintptr_equivalent_) {
    case tk_unsigned_int:
      array_size_expr = expr_alloc(ET_C_UNSIGNED_INT);
      break;
    case tk_unsigned_long_int:
      array_size_expr = expr_alloc(ET_C_UNSIGNED_LONG_INT);
      break;
    case tk_unsigned_long_long_int:
      array_size_expr = expr_alloc(ET_C_UNSIGNED_LONG_LONG_INT);
      break;
    default:
      assert(0 && "Cannot work with uintptr type");
      break;
  }
  if (!array_size_expr) {
    cc_no_memory(cc);
    return NULL;
  }
  array_size_expr->v_.u64_ = (uint64_t)dsp->size_ / element_size;
  return array_size_expr;
}

struct type_node *expr_type(struct c_compiler *cc, struct expr *src) {
  if (!src) return NULL;
  /* Get the type of the expression */
  if (src->et_ == ET_CVFUN2PTR) {
    struct type_node *tn = expr_type(cc, src->children_[0]);
    if (!tn) {
      return NULL;
    }
    if (type_node_unqualified(tn)->kind_ != tk_function) {
      return NULL;
    }
    return type_base_pointer(&cc->tb_, tn);
  }
  else if (src->et_ == ET_CVE2CIT) {
    /* Type of the (only) argument must be the enum */
    struct type_node *enum_type = expr_type(cc, src->children_[0]);
    if (!enum_type) {
      return NULL;
    }
    if (enum_type->kind_ != tk_enumeration) {
      return NULL;
    }
    /* Return the enumeration's Compatible Integer Type */
    return enum_type->derived_from_;
  }
  else if (src->et_ == ET_CVCIT2E) {
    /* Type becomes the enumeration */
    return src->type_arg_;
  }
  else if ((src->et_ == ET_C_STRING_LIT) || (src->et_ == ET_C_WIDE_STRING_LIT)) {
    struct ds_portion *dsp = src->dsp_;
    struct expr *array_size_expr = expr_create_array_size_expr(cc, dsp, (src->et_ == ET_C_STRING_LIT) ? sizeof(uint8_t) : sizeof(uint16_t));
    struct type_node *elm_type = type_base_simple(&cc->tb_, (src->et_ == ET_C_STRING_LIT) ? tk_char : tk_unsigned_short_int);
    if (!elm_type) {
      expr_free(array_size_expr);
      return NULL;
    }
    if (cc->tb_.string_literals_are_const_) {
      elm_type = type_base_qualifier(&cc->tb_, elm_type, TYPE_QUALIFIER_CONST);
    }
    struct type_node *tn = type_base_array(cc, elm_type, array_size_expr);
    if (!tn) {
      expr_free(array_size_expr);
      return NULL;
    }
    tn = type_base_pointer(&cc->tb_, tn);
    if (!tn) {
      expr_free(array_size_expr);
      return NULL;
    }
    return tn;
  }
  else if (src->et_ == ET_ADD_PTR_UINTPTR) {
    return expr_type(cc, src->children_[0]);
  }
  else if (src->et_ == ET_SUB_PTR) {
    return type_base_simple(&cc->tb_, cc->tb_.ptrdiff_equivalent_);
  }
  else if (src->et_ == ET_INDIRECTION_PTR) {
    /* Type of value pointed to. */
    struct type_node *ptr_type = expr_type(cc, src->children_[0]);
    if (!ptr_type) {
      return NULL;
    }
    ptr_type = type_node_unqualified(ptr_type);
    if (ptr_type->kind_ != tk_pointer) {
      /* Very strange node configuration; ET_INDIRECTION_PTR child must always be a pointer. */
      assert(0 && "Indirection with non-pointer as child");
      return NULL;
    }
    /* Remove the pointer, return the type pointed to, including any and all its qualifiers. */
    return ptr_type->derived_from_;
  }
  else if (src->et_ == ET_CVPTR2UINTPTR) {
    return type_base_simple(&cc->tb_, cc->tb_.uintptr_equivalent_);
  }
  else if (src->et_ == ET_CVUINTPTR2PTR) {
    return src->type_arg_;
  }
  else if (src->et_ == ET_CVPTR2PTR) {
    return src->type_arg_;
  }
  else if (src->et_ == ET_STORE_PTR) {
    /* Value type the type of the left ptr side indirected (left side must be "pointer to pointer") */
    struct type_node *ptr_type = expr_type(cc, src->children_[0]);
    if (ptr_type->kind_ != tk_pointer) {
      /* This is .. unexpected, any ET_STORE_xxx have been a pointer as first argument */
      assert(0 && "Pointer expected on first argument to ET_STORE_PTR");
      return NULL;
    }
    return ptr_type->derived_from_;
  }
  else if (src->et_ == ET_SEQ) {
    return expr_type(cc, src->children_[1]);
  }
  else if ((src->et_ == ET_ADDRESS_E) || (src->et_ == ET_ADDRESS_G) || (src->et_ == ET_ADDRESS_P) || (src->et_ == ET_ADDRESS_L)) {
    struct type_node *tn_variable_type = src->decl_->type_;
    if (tn_variable_type && src->decl_->is_passed_by_pointer_) {
      /* Passed by pointer, meaning this value will be a pointer to a pointer. */
      tn_variable_type = type_base_pointer(&cc->tb_, tn_variable_type);
    }

    if (tn_variable_type) {
      /* Now make a pointer to the variable type */
      return type_base_pointer(&cc->tb_, tn_variable_type);
    }
    /* Something very wrong here. */
    return NULL;
  }
  else if (src->et_ == ET_ADDRESS_R) {
    /* Return type from function is in type_arg; derive a pointer to it */
    /* XXX: Should take ABI into account when returning structs. (Structs returnvalues may be passed as a pointer parameter to that struct memory in the caller) */
    return type_base_pointer(&cc->tb_, src->type_arg_);
  }
  else if (src->et_ == ET_CONDITION) {
    return src->type_arg_;
  }
  else if (src->et_ == ET_FUNCTION_CALL) {
    /* First child is pointer to function - pointer - to function. Not the function designator directly. */
    struct type_node *func_type = expr_type(cc, src->children_[0]);
    if (!func_type) return NULL;
    func_type = type_node_unqualified(func_type);
    if (func_type->kind_ != tk_pointer) return NULL;
    func_type = type_node_unqualified(func_type->derived_from_);
    if (!func_type || (func_type->kind_ != tk_function)) return NULL;
    return func_type->derived_from_;
  }
#define xx(id, num_args, type_of) \
  case id: return type_base_simple(&cc->tb_, type_of);
  switch (src->et_) {
    ENUM_EXPR_TYPES
  }
#undef xx
  return NULL;
}

const char *expr_node_type_to_str(struct expr *x) {
  if (!x) return "null";
#define xx(id, num_args, type_of) case id: return #id;
  switch (x->et_) {
    ENUM_EXPR_TYPES
  }
#undef xx
  return "??";
}

int expr_is_null_ptr(struct c_compiler *cc, struct expr *x, int *is_null_ptr) {
  /* Either an integer constant value 0 or such a value cast to (void *). */
  *is_null_ptr = 0;
  struct type_node *tn = expr_type(cc, x);
  if (!tn) return 0;
  if (type_node_is_pointer_type(tn)) {
    if (type_node_unqualified(type_node_unqualified(tn)->derived_from_)->kind_ != tk_void) {
      /* NULL pointers only valid for type void*. */
      return 0;
    }
  }
  else if (!type_node_is_integer_type(tn)) {
    /* NULL pointers only valid for integer types or void* */
    return 0;
  }

  int r;
  struct expr_temp ptr_result;
  r = expr_constant_evaluation(cc, &ptr_result, x);
  if (r) {
    return r;
  }
  
  /* Evaluation result is in i64 (given as it is an integer type, and i64_ and u64_ are in the same union) */
  *is_null_ptr = !ptr_result.v_.i64_;

  return 0;
}

struct expr *expr_convert_type(struct c_compiler *cc, struct type_node *to, struct expr *src) {
  struct type_node *from = expr_type(cc, src);
  if (!from) return NULL;

  if (from == to) return src; /* Already there */

  int from_is_ptr = type_node_is_pointer_type(from);
  int to_is_ptr = type_node_is_pointer_type(to);

  if (from_is_ptr && to_is_ptr) {
    /* Direct conversion */
    struct expr *x = expr_alloc(ET_CVPTR2PTR);
    if (!x) {
      return NULL;
    }
    x->type_arg_ = to;
    x->children_[0] = src;
    return x;
  }

  if (from_is_ptr) {
    /* First convert away from pointer */
    struct expr *x = expr_alloc(ET_CVPTR2UINTPTR);
    if (!x) {
      return NULL;
    }
    x->children_[0] = src;
    struct expr *r = expr_convert_arithmetic_type(cc, to, x);
    if (!r) {
      /* Clean up x nicely if we're not returning it. */
      x->children_[0] = NULL;
      expr_free(x);
      return NULL;
    }
    return r;
  }

  if (to_is_ptr) {
    /* Convert to uintptr first before we convert to pointer; do the allocation
     * of the final step first so we know it won't fail half-way. */
    struct expr *x = expr_alloc(ET_CVUINTPTR2PTR);
    if (!x) return NULL;
    
    struct expr *r = expr_convert_arithmetic_type(cc, type_base_simple(&cc->tb_, cc->tb_.uintptr_equivalent_), src);
    if (!r) {
      expr_free(x);
      return NULL;
    }
    x->children_[0] = r;
    x->type_arg_ = to;
    return x;
  }

  /* Not to or from a type, convert arithmetically.. */

  /* XXX: Arrays, Functions, Structs, Unions! There is much more to be done here!!! */
  return expr_convert_arithmetic_type(cc, to, src);
}

struct expr *expr_convert_arithmetic_type(struct c_compiler *cc, struct type_node *to, struct expr *src) {
  struct type_node *from = expr_type(cc, src);
  if (!from) return NULL;

  from = type_node_unqualified(from);

  if (from == to) return src; /* Already there.. */

  struct expr *root = NULL;

  struct expr *current_expr;
  enum type_kind current_kind;
  if (from->kind_ == tk_enumeration) {
    /* Convert away from dynamically typed enum towards compatible integer type */
    current_expr = expr_alloc(ET_CVE2CIT);
    current_kind = from->derived_from_->kind_;
    root = current_expr; /* this assigns src to children_[0] as a final step.. */
  }
  else {
    current_expr = NULL;
    current_kind = from->kind_;
  }

  enum type_kind goal_kind;
  if (to->kind_ == tk_enumeration) {
    /* To convert to the enumeration we must convert to its compatible integer type */
    goal_kind = to->derived_from_->kind_;
  }
  else {
    goal_kind = to->kind_;
  }

  while (current_kind != goal_kind) {
    enum expr_type next_expr_node = expr_get_next_arithmetic_conversion_step(cc, current_kind, goal_kind);
    if (next_expr_node == ET_NOP) {
      assert(0 && "Unexpected NOP when not yet at destination");
      break;
    }
    else if (next_expr_node == ET_INVALID) {
      expr_free(current_expr);
      return NULL;
    }
    else {
      struct expr *x = expr_alloc(next_expr_node);
      x->children_[0] = current_expr;
      if (!root) root = x; /* this assigns src to children_[0] as a final step.. */
      current_expr = x;
      current_kind = expr_arith_type_kind_no_enum(cc, x);
    }
  }

  if (to->kind_ == tk_enumeration) {
    /* We are at the compatible integer type, what remains is conversion to the enum */
    struct expr *last = expr_alloc(ET_CVCIT2E);
    last->children_[0] = current_expr;
    last->type_arg_ = to;
    if (!root) root = last;
    current_expr = last;
  }

  /* Completed, socket the source expression into our conversion chain
   * (We could not do this previously as, in case of failure, that would cause expr_free() to
   * free the src expression tree as well.) */
  if (root) {
    root->children_[0] = src;
  }

  return current_expr;
}

int expr_integer_promotions(struct c_compiler *cc, struct expr **expr) {
  /* C99 6.3.1.1-2 */
  /* NOTE: The size of a bitfield is important here, if a bitfield's full range can be represented
   *       by an int, then it should be promoted to an int, otherwise it should be converted to an
   *       unsigned int. */

  struct type_node *et = expr_type(cc, *expr);
  if (type_node_is_integer_type(et)) {
    int is_signed, rank;
    /* et can be enum, check derived_from if that's the case (compatible integer type then defines rank). */
    enum type_kind et_kind  = et->kind_;
    if (et_kind == tk_enumeration) {
      /* Use compatible integer type */
      et_kind = et->derived_from_->kind_;
    }
    type_kind_classify_integer_type(et_kind, &is_signed, &rank);
    int dummy, int_rank;
    type_kind_classify_integer_type(tk_int, &dummy, &int_rank);
    int range_fits_int = 0;
    if (rank < int_rank) {
      range_fits_int = 1;
    }
    else if (rank == int_rank) {
      if (!is_signed) {
        /* Unsigned int; this can fit only if it is a bitfield of less than the number of bits in an int */
        if ((*expr)->is_bitfield_ && ((*expr)->bitfield_size_ < cc->tb_.bits_per_int_)) {
          range_fits_int = 1;
        }
      }
      else {
        range_fits_int = 1;
      }
    }
    else /* rank > int_rank */ {
      /* All other types are unchanged by the integer promotions */
      return 0;
    }
    enum type_kind promoted_type;
    if (range_fits_int) {
      promoted_type = tk_int;
    }
    else {
      promoted_type = tk_unsigned_int;
    }
    struct expr *as_int = expr_convert_arithmetic_type(cc, type_base_simple(&cc->tb_, promoted_type), *expr);
    if (as_int) {
      *expr = as_int;
      return 0;
    }
    cc_error(cc, "Internal error, failed to convert integer type to %s\n", type_base_simple(&cc->tb_, promoted_type)->label_);
    return -1;
  }
  else {
    /* All other types are unchanged by the integer promotions */
    return 0;
  }
}

/* From C99 appendix G */
#ifdef _MSC_VER
#pragma fp_contract(off)
#else
#pragma STDC FP_CONTRACT OFF
#endif
static void complex_mul_d(double *r, double *ri, double z, double zi, double w, double wi) {
  double a, b, c, d, ac, bd, ad, bc, x, y;
  a = z; b = zi;
  c = w; d = wi;
  ac = a * c;   bd = b * d;
  ad = a * d;   bc = b * c;
  x = ac - bd;  y = ad + bc;
  if (isnan(x) && isnan(y)) {
    /* Recover infinities that computed as NaN + iNaN */
    int recalc = 0;
    if (isinf(a) || isinf(b)) {
      /* Z is infinite; "Box" the infinity and change NaNs in the other factor to 0 */
      a = copysign(isinf(a) ? 1. : 0., a);
      b = copysign(isinf(b) ? 1. : 0., b);
      if (isnan(c)) c = copysign(0., c);
      if (isnan(d)) d = copysign(0., d);
      recalc = 1;
    }
    if (isinf(c) || isinf(d)) {
      /* W is infinite; "Box" the infinity and change NaNs in the other factor to 0 */
      c = copysign(isinf(c) ? 1. : 0., c);
      d = copysign(isinf(d) ? 1. : 0., d);
      if (isnan(a)) a = copysign(0., a);
      if (isnan(b)) b = copysign(0., b);
      recalc = 1;
    }
    if (!recalc && (isinf(ac) || isinf(bd) || isinf(ad) || isinf(bc))) {
      /* Recover infinities from overflow by changing NaNs to 0 */
      if (isnan(a)) a = copysign(0., a);
      if (isnan(b)) b = copysign(0., b);
      if (isnan(c)) c = copysign(0., c);
      if (isnan(d)) d = copysign(0., d);
      recalc = 1;
    }
    if (recalc) {
      x = INFINITY * (a * c - b * d);
      y = INFINITY * (a * d + b * c);
    }
  }
  *r = x;
  *ri = y;
}

static void complex_mul_f(float *r, float *ri, float z, float zi, float w, float wi) {
  float a, b, c, d, ac, bd, ad, bc, x, y;
  a = z; b = zi;
  c = w; d = wi;
  ac = a * c;   bd = b * d;
  ad = a * d;   bc = b * c;
  x = ac - bd;  y = ad + bc;
  if (isnan(x) && isnan(y)) {
    /* Recover infinities that computed as NaN + iNaN */
    int recalc = 0;
    if (isinf(a) || isinf(b)) {
      /* Z is infinite; "Box" the infinity and change NaNs in the other factor to 0 */
      a = copysignf(isinf(a) ? 1.f : 0.f, a);
      b = copysignf(isinf(b) ? 1.f : 0.f, b);
      if (isnan(c)) c = copysignf(0.f, c);
      if (isnan(d)) d = copysignf(0.f, d);
      recalc = 1;
    }
    if (isinf(c) || isinf(d)) {
      /* W is infinite; "Box" the infinity and change NaNs in the other factor to 0 */
      c = copysignf(isinf(c) ? 1.f : 0.f, c);
      d = copysignf(isinf(d) ? 1.f : 0.f, d);
      if (isnan(a)) a = copysignf(0.f, a);
      if (isnan(b)) b = copysignf(0.f, b);
      recalc = 1;
    }
    if (!recalc && (isinf(ac) || isinf(bd) || isinf(ad) || isinf(bc))) {
      /* Recover infinities from overflow by changing NaNs to 0 */
      if (isnan(a)) a = copysignf(0.f, a);
      if (isnan(b)) b = copysignf(0.f, b);
      if (isnan(c)) c = copysignf(0.f, c);
      if (isnan(d)) d = copysignf(0.f, d);
      recalc = 1;
    }
    if (recalc) {
      x = INFINITY * (a * c - b * d);
      y = INFINITY * (a * d + b * c);
    }
  }
  *r = x;
  *ri = y;
}

static void complex_div_d(double *r, double *ri, double z, double zi, double w, double wi) {
  double a, b, c, d, logbw, denom, x, y;
  int ilogbw = 0;
  a = z; b = zi;
  c = w; d = wi;
  logbw = logb(fmax(fabs(c), fabs(d)));
  if (isfinite(logbw)) {
    ilogbw = (int)logbw;
    c = scalbn(c, -ilogbw); d = scalbn(d, -ilogbw);
  }
  denom = c * c + d * d;
  x = scalbn((a * c + b * d) / denom, -ilogbw);
  y = scalbn((b * c - a * d) / denom, -ilogbw);
  /* Recover infinities and zero that computed as NaN + iNaN; the only cases are: non-zero/zero,
   * infinite/finite, and finite/infinite */
  if (isnan(x) && isnan(y)) {
    if ((denom == 0.) && (!isnan(a) || !isnan(b))) {
      x = copysign(INFINITY, c) * a;
      y = copysign(INFINITY, c) * b;
    }
    else if ((isinf(a) || isinf(b)) &&
      (isfinite(c) && isfinite(d))) {
      a = copysign(isinf(a) ? 1. : 0., a);
      b = copysign(isinf(b) ? 1. : 0., b);
      x = INFINITY * (a * c + b * d);
      y = INFINITY * (b * c - a * d);
    }
    else if (isinf(logbw) && isfinite(a) && isfinite(b)) {
      c = copysign(isinf(c) ? 1. : 0., c);
      d = copysign(isinf(d) ? 1. : 0., d);
      x = 0. * (a * c + b * d);
      y = 0. * (b * c - a * d);
    }
  }
  *r = x;
  *ri = y;
}

static void complex_div_f(float *r, float *ri, float z, float zi, float w, float wi) {
  float a, b, c, d, logbw, denom, x, y;
  int ilogbw = 0;
  a = z; b = zi;
  c = w; d = wi;
  logbw = logbf(fmaxf(fabsf(c), fabsf(d)));
  if (isfinite(logbw)) {
    ilogbw = (int)logbw;
    c = scalbnf(c, -ilogbw); d = scalbnf(d, -ilogbw);
  }
  denom = c * c + d * d;
  x = scalbnf((a * c + b * d) / denom, -ilogbw);
  y = scalbnf((b * c - a * d) / denom, -ilogbw);
  /* Recover infinities and zero that computed as NaN + iNaN; the only cases are: non-zero/zero,
   * infinite/finite, and finite/infinite */
  if (isnan(x) && isnan(y)) {
    if ((denom == 0.) && (!isnan(a) || !isnan(b))) {
      x = copysignf(INFINITY, c) * a;
      y = copysignf(INFINITY, c) * b;
    }
    else if ((isinf(a) || isinf(b)) &&
      (isfinite(c) && isfinite(d))) {
      a = copysignf(isinf(a) ? 1.f : 0.f, a);
      b = copysignf(isinf(b) ? 1.f : 0.f, b);
      x = INFINITY * (a * c + b * d);
      y = INFINITY * (b * c - a * d);
    }
    else if (isinf(logbw) && isfinite(a) && isfinite(b)) {
      c = copysignf(isinf(c) ? 1.f : 0.f, c);
      d = copysignf(isinf(d) ? 1.f : 0.f, d);
      x = 0.f * (a * c + b * d);
      y = 0.f * (b * c - a * d);
    }
  }
  *r = x;
  *ri = y;
}

static int expr_num_operands(struct expr *x) {
  if (!x) return 0;
  int num_operands = 0;
  switch (x->et_) {
#define xx(id, num_opds, type_of) case id: num_operands = num_opds; break;
    ENUM_EXPR_TYPES
#undef xx
  }
  return num_operands;
}

static void expr_prep_reset(struct expr *x) {
  if (!x) { return; }
  x->ord_ = 0;
  x->num_ords_ = 0;
  x->is_backreferenced_ = 0;
  if (x->children_[0]) expr_prep_reset(x->children_[0]);
  if (x->children_[1]) expr_prep_reset(x->children_[1]);
  if (x->children_[2]) expr_prep_reset(x->children_[2]);
}

static void expr_prep_visit(struct expr *x, int *pp_ordinal) {
  if (!x) { return; }
  if (!x->ord_) {
    /* Ordinal not yet assigned (not a DAG back-reference.) */
    int base_ord = *pp_ordinal;
    x->ord_ = (*pp_ordinal)++;
    if (x->children_[0]) expr_prep_visit(x->children_[0], pp_ordinal);
    if (x->children_[1]) expr_prep_visit(x->children_[1], pp_ordinal);
    if (x->children_[2]) expr_prep_visit(x->children_[2], pp_ordinal);
    x->num_ords_ = (*pp_ordinal) - base_ord;
  }
  else {
    /* Ordinal already assigned */
    assert((x->ord_ < *pp_ordinal) && "backreference does not precede node??");
    x->is_backreferenced_ = 1;
  }
}

void expr_prepare(struct expr *x) {
  /* Start the count at 1, this allows us to discover (on debug) when
   * we failed to initialize the ordinals. */
  int ordinal = 1;
  expr_prep_reset(x);
  expr_prep_visit(x, &ordinal);
}

static int expr_invoke_function_call(struct c_compiler *cc, struct expr *x, struct expr_temp *temps) {
  return invoke_x64(cc, x, temps);
}

static int expr_eval_impl(struct c_compiler *cc, struct expr *x, struct expr_temp *temps, int parent_ordinal, int is_constant_expr, struct decl *nulled_out_decl, void *local_base, void *param_base, void *return_value_ptr) {
  int r;
  if (!x) return -1;

  /* Only execute the tree once, as the tree may actually be a DAG */
  if (x->ord_ < parent_ordinal) return 0;

  if ((x->et_ == ET_LOGICAL_AND) || (x->et_ == ET_LOGICAL_OR)) {
    r = expr_eval_impl(cc, x->children_[0], temps, x->ord_, is_constant_expr, nulled_out_decl, local_base, param_base, return_value_ptr);
    if (r) return r;

    if (temps[x->children_[0]->ord_].v_.i64_) {
      /* First branch evaluates to True */
      if (x->et_ == ET_LOGICAL_AND) {
        /* Evaluation equals that of second branch */
        r = expr_eval_impl(cc, x->children_[1], temps, x->ord_, is_constant_expr, nulled_out_decl, local_base, param_base, return_value_ptr);
        if (r) return r;
        temps[x->ord_] = temps[x->children_[1]->ord_];
        return 0;
      }
      else if (x->et_ == ET_LOGICAL_OR) {
        /* Evaluate to True without evaluating second branch */
        temps[x->ord_] = temps[x->children_[0]->ord_];
        return 0;
      }
    }
    else {
      /* First branch evaluates to False */
      if (x->et_ == ET_LOGICAL_AND) {
        /* Evaluation equals False without evaluating second branch */
        temps[x->ord_].v_.i64_ = 0;
        return 0;
      }
      else if (x->et_ == ET_LOGICAL_OR) {
        /* Evaluation equals that of second branch */
        r = expr_eval_impl(cc, x->children_[1], temps, x->ord_, is_constant_expr, nulled_out_decl, local_base, param_base, return_value_ptr);
        if (r) return r;
        temps[x->ord_] = temps[x->children_[1]->ord_];
        return 0;
      }
    }
  }
  else if (x->et_ == ET_CONDITION) {
    r = expr_eval_impl(cc, x->children_[0], temps, x->ord_, is_constant_expr, nulled_out_decl, local_base, param_base, return_value_ptr);
    if (r) return r;

    int select = temps[x->children_[0]->ord_].v_.i64_ ? 1 : 2;

    r = expr_eval_impl(cc, x->children_[select], temps, x->ord_, is_constant_expr, nulled_out_decl, local_base, param_base, return_value_ptr);
    if (r) return r;

    temps[x->ord_].v_ = temps[x->children_[select]->ord_].v_;
    
    return 0;
  }

  int num_operands = expr_num_operands(x);
  if (x->et_ == ET_FUNCTION_CALL) {
    /* NULL is a valid second argument (would normally be ET_FUNCTION_CALL_ARG_LIST but can be NULL if no arguments.) */
    if (!x->children_[1]) num_operands--;
  }
  int n;
  int at_sibling_ord = x->ord_ + 1;

  for (n = 0; n < num_operands; ++n) {
    r = expr_eval_impl(cc, x->children_[n], temps, at_sibling_ord, is_constant_expr, nulled_out_decl, local_base, param_base, return_value_ptr);
    struct expr_temp *xs = temps + x->children_[n]->ord_;
    if (x->children_[n] && (x->children_[n]->ord_ >= at_sibling_ord)) {
      /* This was not a back-reference, proceed with increment. */
      at_sibling_ord += x->children_[n]->num_ords_;
    }
    if (r) return r;
  }

  if (cc->fatal_error_) {
    return -1;
  }

  enum impl_type {
    i8, i16, i32, i64,
    u8, u16, u32, u64,
    ld, d, f,
    fi, di, ldi,
    fc, dc, ldc,
    b, enum_type, enum_cit,
    ptr,
    none
  } load[3], store, c, si, sli, slli, ui, uli, ulli, uintptr, indi;

  enum operator {
    nop,
    mul,
    div,
    rem,
    add,
    sub,
    imm_si,
    imm_sli,
    imm_slli,
    imm_ui,
    imm_uli,
    imm_ulli,
    imm_d,
    imm_f,
    imm_usi,
    subptr_ptr,
    neg,
    compl,
    indir,
    shl,
    shr,
    lt,  /* signed less than */
    ult, /* unsigned less than */
    ltf,
    ltd,
    gt,
    ugt,
    gtf,
    gtd,
    lte,
    ulte,
    ltef,
    lted,
    gte,
    ugte,
    gtef,
    gted,
    storeldc,
    storeldi,
    storedc,
    storedi,
    storefc,
    storefi,
    storeld,
    stored,
    storef,
    storeslli,
    storesli,
    storesi,
    storess,
    storesc,
    storeulli,
    storeuli,
    storeui,
    storeus,
    storeuc,
    storec,
    storeb,
    storeptr,
    copy,
    clear,
    copy2ndchild,
    extern_addr,
    global_addr,
    local_addr,
    param_addr,
    return_value_addr,
    bitwise_and,
    bitwise_or,
    bitwise_xor,
    equ_dc,
    equ_fc,
    equ_u,
    equ_i,
    neq_dc,
    neq_fc,
    neq_u,
    neq_i,
    func
  } op;

  c = cc->tb_.char_is_signed_ ? i8 : u8;

  if (cc->tb_.bits_per_int_ == 16) {
    si = i16;
    ui = u16;
  }
  else if (cc->tb_.bits_per_int_ == 32) {
    si = i32;
    ui = u32;
  }
  else if (cc->tb_.bits_per_int_ == 64) {
    si = i64;
    ui = u64;
  }
  else {
    assert(0 && "Unsupported bits per int");
  }

  if (cc->tb_.bits_per_long_ == 16) {
    sli = i16;
    uli = u16;
  }
  else if (cc->tb_.bits_per_long_ == 32) {
    sli = i32;
    uli = u32;
  }
  else if (cc->tb_.bits_per_long_ == 64) {
    sli = i64;
    uli = u64;
  }
  else {
    assert(0 && "Unsupported bits per int");
  }

  if (cc->tb_.bits_per_long_long_ == 16) {
    slli = i16;
    ulli = u16;
  }
  else if (cc->tb_.bits_per_long_long_ == 32) {
    slli = i32;
    ulli = u32;
  }
  else if (cc->tb_.bits_per_long_long_ == 64) {
    slli = i64;
    ulli = u64;
  }
  else {
    assert(0 && "Unsupported bits per int");
  }

  if (cc->tb_.uintptr_equivalent_ == tk_unsigned_short_int) {
    uintptr = u16;
  }
  else if (cc->tb_.uintptr_equivalent_ == tk_unsigned_int) {
    uintptr = ui;
  }
  else if (cc->tb_.uintptr_equivalent_ == tk_unsigned_long_int) {
    uintptr = uli;
  }
  else if (cc->tb_.uintptr_equivalent_ == tk_unsigned_long_long_int) {
    uintptr = ulli;
  }
  else {
    assert(0 && "Unsupported uintptr_equivalent");
  }

  double operands_d[3];
  double operands_di[3];
  int64_t operands_i[3];
  uint64_t operands_u[3];
  float operands_f[3];
  float operands_fi[3];


  /* Goal: i64_ should reflect the value of the field to the extent possible,
   *       e.g. for an "unsigned char", the range in the i64_ should be 0..255
   *       (it should not sign extend from 8 bits.)
   *       for a "signed char", the range in the i64_ should be -128..127, so there
   *       it *should* sign extend from 8 bits up to all 64 bits.
   * For operations that, when compiled, would be a no-op (eg. conversion
   * from "unsigned char" to "signed char" for instance, is a no-op, both
   * being bytes), there is therefore work to be done when evaluating as
   * a constant (sign extension.)
   * This also implies that operations which, ordinarily, would require
   * an instruction (setting / clearing extension bits) now become somewhat
   * implicit. We nevertheless encode them (eg. int16_t to int32_t extension)
   * so each evaluation of an expression normalizes on the data. */

  enum expr_type et = x->et_;

  switch (et) {
#define xx(id, opt, load1_val, load2_val, load3_val, store_val) case id: op = opt; load[0] = load1_val; load[1] = load2_val; load[2] = load3_val; store = store_val; break;
    xx(ET_CVC2SC, nop, c, none, none, i8)
    xx(ET_CVSC2C, nop, i8, none, none, c)
    xx(ET_CVC2UC, nop, c, none, none, u8)
    xx(ET_CVUC2C, nop, u8, none, none, c)
    xx(ET_CVSC2UC, nop, i8, none, none, u8)
    xx(ET_CVUC2SC, nop, u8, none, none, i8)
    xx(ET_CVSC2SS, nop, i8, none, none, i16)
    xx(ET_CVSS2SC, nop, i16, none, none, i8)
    xx(ET_CVUC2US, nop, i8, none, none, i16)
    xx(ET_CVUS2UC, nop, i16, none, none, i8)
    xx(ET_CVSS2US, nop, i16, none, none, u16)
    xx(ET_CVUS2SS, nop, u16, none, none, i16)
    xx(ET_CVSS2SI, nop, i16, none, none, si)
    xx(ET_CVSI2SS, nop, si, none, none, i16)
    xx(ET_CVUS2UI, nop, u16, none, none, ui)
    xx(ET_CVUI2US, nop, ui, none, none, u16)
    xx(ET_CVSI2UI, nop, si, none, none, ui)
    xx(ET_CVUI2SI, nop, ui, none, none, si)
    xx(ET_CVSI2SLI, nop, si, none, none, sli)
    xx(ET_CVSLI2SI, nop, sli, none, none, si)
    xx(ET_CVUI2ULI, nop, ui, none, none, uli)
    xx(ET_CVULI2UI, nop, uli, none, none, ui)
    xx(ET_CVSLI2ULI, nop, sli, none, none, uli)
    xx(ET_CVULI2SLI, nop, uli, none, none, sli)
    xx(ET_CVSLI2SLLI, nop, sli, none, none, slli)
    xx(ET_CVSLLI2SLI, nop, slli, none, none, sli)
    xx(ET_CVULI2ULLI, nop, uli, none, none, ulli)
    xx(ET_CVULLI2ULI, nop, ulli, none, none, uli)
    xx(ET_CVSLLI2ULLI, nop, slli, none, none, ulli)
    xx(ET_CVULLI2SLLI, nop, ulli, none, none, slli)
    xx(ET_CVULI2F, nop, uli, none, none, f)
    xx(ET_CVULLI2F, nop, ulli, none, none, f)
    xx(ET_CVULI2D, nop, uli, none, none, d)
    xx(ET_CVULLI2D, nop, ulli, none, none, d)
    xx(ET_CVSLI2F, nop, sli, none, none, f)
    xx(ET_CVSLLI2F, nop, slli, none, none, f)
    xx(ET_CVSLI2D, nop, sli, none, none, d)
    xx(ET_CVSLLI2D, nop, slli, none, none, d)
    xx(ET_CVF2ULI, nop, f, none, none, uli)
    xx(ET_CVF2ULLI, nop, f, none, none, ulli)
    xx(ET_CVD2ULI, nop, d, none, none, uli)
    xx(ET_CVD2ULLI, nop, d, none, none, ulli)
    xx(ET_CVF2SLI, nop, f, none, none, sli)
    xx(ET_CVF2SLLI, nop, f, none, none, slli)
    xx(ET_CVD2SLI, nop, d, none, none, sli)
    xx(ET_CVD2SLLI, nop, d, none, none, slli)
    xx(ET_CVF2D, nop, f, none, none, d)
    xx(ET_CVD2F, nop, d, none, none, f)
    xx(ET_CVD2LD, nop, d, none, none, ld)
    xx(ET_CVLD2D, nop, ld, none, none, d)
    xx(ET_CVFI2DI, nop, fi, none, none, di)
    xx(ET_CVDI2FI, nop, di, none, none, fi)
    xx(ET_CVDI2LDI, nop, di, none, none, ldi)
    xx(ET_CVLDI2DI, nop, ldi, none, none, di)
    xx(ET_CVFI2FC, nop, fi, none, none, fc)
    xx(ET_CVFC2FI, nop, fc, none, none, fi)
    xx(ET_CVDI2DC, nop, di, none, none, dc)
    xx(ET_CVDC2DI, nop, dc, none, none, di)
    xx(ET_CVLDI2LDC, nop, ldi, none, none, ldc)
    xx(ET_CVLDC2LDI, nop, ldc, none, none, ldi)
    xx(ET_CVFI2F, nop, fi, none, none, f)
    xx(ET_CVF2FI, nop, f, none, none, fi)
    xx(ET_CVDI2D, nop, di, none, none, d)
    xx(ET_CVD2DI, nop, d, none, none, di)
    xx(ET_CVLDI2LD, nop, ldi, none, none, ld)
    xx(ET_CVLD2LDI, nop, ld, none, none, ldi)
    xx(ET_CVFC2DC, nop, fc, none, none, dc)
    xx(ET_CVDC2FC, nop, dc, none, none, fc)
    xx(ET_CVDC2LDC, nop, dc, none, none, ldc)
    xx(ET_CVLDC2DC, nop, ldc, none, none, dc)
    xx(ET_CVFC2F, nop, fc, none, none, f)
    xx(ET_CVF2FC, nop, f, none, none, fc)
    xx(ET_CVDC2D, nop, dc, none, none, d)
    xx(ET_CVD2DC, nop, d, none, none, dc)
    xx(ET_CVLDC2LD, nop, ldc, none, none, ld)
    xx(ET_CVLD2LDC, nop, ld, none, none, ldc)
    xx(ET_CVB2UC, nop, b, none, none, u8)
    xx(ET_CVUC2B, nop, u8, none, none, b)
    xx(ET_CVSC2B, nop, i8, none, none, b)
    xx(ET_CVUS2B, nop, u16, none, none, b)
    xx(ET_CVSS2B, nop, i16, none, none, b)
    xx(ET_CVUI2B, nop, ui, none, none, b)
    xx(ET_CVSI2B, nop, si, none, none, b)
    xx(ET_CVULI2B, nop, uli, none, none, b)
    xx(ET_CVSLI2B, nop, sli, none, none, b)
    xx(ET_CVULLI2B, nop, ulli, none, none, b)
    xx(ET_CVSLLI2B, nop, slli, none, none, b)
    xx(ET_CVF2B, nop, f, none, none, b)
    xx(ET_CVD2B, nop, d, none, none, b)
    xx(ET_CVLD2B, nop, ld, none, none, b)
    xx(ET_CVFUN2PTR, nop, ptr, none, none, ptr)
    xx(ET_CVE2CIT, nop, enum_type, none, none, enum_cit)
    xx(ET_CVCIT2E, nop, enum_cit, none, none, enum_type)
    xx(ET_CVPTR2UINTPTR, nop, ptr, none, none, uintptr)
    xx(ET_CVUINTPTR2PTR, nop, uintptr, none, none, ptr)
    xx(ET_CVPTR2PTR, nop, ptr, none, none, ptr)

    xx(ET_MUL_LDC_LD, mul, ldc, ld, none, ldc)
    xx(ET_MUL_LDC_LDC, mul, ldc, ldc, none, ldc)
    xx(ET_MUL_LDC_LDI, mul, ldc, ldi, none, ldc)
    xx(ET_MUL_DC_D, mul, dc, d, none, dc)
    xx(ET_MUL_DC_DC, mul, dc, dc, none, dc)
    xx(ET_MUL_DC_DI, mul, dc, di, none, dc)
    xx(ET_MUL_FC_F, mul, fc, f, none, f)
    xx(ET_MUL_FC_FC, mul, fc, fc, none, fc)
    xx(ET_MUL_FC_FI, mul, fc, fi, none, fc)
    xx(ET_MUL_LDI_LD, mul, ldi, ld, none, ldi)
    xx(ET_MUL_LDI_LDI, mul, ldi, ldi, none, ld)
    xx(ET_MUL_DI_D, mul, di, d, none, di)
    xx(ET_MUL_DI_DI, mul, di, di, none, d)
    xx(ET_MUL_FI_F, mul, fi, f, none, fi)
    xx(ET_MUL_FI_FI, mul, fi, fi, none, f)
    xx(ET_MUL_LD, mul, ld, ld, none, ld)
    xx(ET_MUL_D, mul, d, d, none, d)
    xx(ET_MUL_F, mul, f, f, none, f)
    xx(ET_MUL_SLLI, mul, slli, slli, none, slli)
    xx(ET_MUL_SLI, mul, sli, sli, none, sli)
    xx(ET_MUL_SI, mul, si, si, none, si)
    xx(ET_MUL_ULLI, mul, ulli, ulli, none, ulli)
    xx(ET_MUL_ULI, mul, uli, uli, none, uli)
    xx(ET_MUL_UI, mul, ui, ui, none, ui)

    xx(ET_DIV_LDC_LD, div, ldc, ld, none, ldc)
    xx(ET_DIV_LD_LDC, div, ld, ldc, none, ldc)
    xx(ET_DIV_LDC_LDC, div, ldc, ldc, none, ldc)
    xx(ET_DIV_LDC_LDI, div, ldc, ldi, none, ldc)
    xx(ET_DIV_LDI_LDC, div, ldi, ldc, none, ldc)
    xx(ET_DIV_DC_D, div, dc, d, none, dc)
    xx(ET_DIV_D_DC, div, d, dc, none, dc)
    xx(ET_DIV_DC_DC, div, dc, dc, none, dc)
    xx(ET_DIV_DC_DI, div, dc, di, none, dc)
    xx(ET_DIV_DI_DC, div, di, dc, none, dc)
    xx(ET_DIV_FC_F, div, fc, f, none, fc)
    xx(ET_DIV_F_FC, div, f, fc, none, fc)
    xx(ET_DIV_FC_FC, div, fc, fc, none, fc)
    xx(ET_DIV_FC_FI, div, fc, fi, none, fc)
    xx(ET_DIV_FI_FC, div, fi, fc, none, fc)
    xx(ET_DIV_LDI_LD, div, ldi, ld, none, ldi)
    xx(ET_DIV_LD_LDI, div, ld, ldi, none, ldi)
    xx(ET_DIV_LDI_LDI, div, ldi, ldi, none, ld)
    xx(ET_DIV_DI_D, div, di, d, none, di)
    xx(ET_DIV_D_DI, div, d, di, none, di)
    xx(ET_DIV_DI_DI, div, di, di, none, d)
    xx(ET_DIV_FI_F, div, fi, f, none, fi)
    xx(ET_DIV_F_FI, div, f, fi, none, fi)
    xx(ET_DIV_FI_FI, div, fi, fi, none, f)
    xx(ET_DIV_LD, div, ld, ld, none, ld)
    xx(ET_DIV_D, div, d, d, none, d)
    xx(ET_DIV_F, div, f, f, none, f)
    xx(ET_DIV_SLLI, div, slli, slli, none, slli)
    xx(ET_DIV_SLI, div, sli, sli, none, sli)
    xx(ET_DIV_SI, div, si, si, none, si)
    xx(ET_DIV_ULLI, div, ulli, ulli, none, ulli)
    xx(ET_DIV_ULI, div, uli, uli, none, uli)
    xx(ET_DIV_UI, div, ui, ui, none, ui)

    xx(ET_REM_SLLI, rem, slli, slli, none, slli)
    xx(ET_REM_SLI, rem, sli, sli, none, sli)
    xx(ET_REM_SI, rem, si, si, none, si)
    xx(ET_REM_ULLI, rem, ulli, ulli, none, ulli)
    xx(ET_REM_ULI, rem, uli, uli, none, uli)
    xx(ET_REM_UI, rem, ui, ui, none, ui)

    xx(ET_ADD_LDC_LD, add, ldc, ld, none, ldc)
    xx(ET_ADD_LDC_LDC, add, ldc, ldc, none, ldc)
    xx(ET_ADD_LDC_LDI, add, ldc, ldi, none, ldc)
    xx(ET_ADD_DC_D, add, dc, d, none, dc)
    xx(ET_ADD_DC_DC, add, dc, dc, none, dc)
    xx(ET_ADD_DC_DI, add, dc, di, none, dc)
    xx(ET_ADD_FC_F, add, fc, f, none, fc)
    xx(ET_ADD_FC_FC, add, fc, fc, none, fc)
    xx(ET_ADD_FC_FI, add, fc, fi, none, fc)
    xx(ET_ADD_LDI_LD, add, ldi, ld, none, ldc)
    xx(ET_ADD_LDI_LDI, add, ldi, ldi, none, ldi)
    xx(ET_ADD_DI_D, add, di, d, none, dc)
    xx(ET_ADD_DI_DI, add, di, di, none, di)
    xx(ET_ADD_FI_F, add, fi, f, none, fc)
    xx(ET_ADD_FI_FI, add, fi, fi, none, fi)
    xx(ET_ADD_LD, add, ld, ld, none, ld)
    xx(ET_ADD_D, add, d, d, none, d)
    xx(ET_ADD_F, add, f, f, none, f)
    xx(ET_ADD_SLLI, add, slli, slli, none, slli)
    xx(ET_ADD_SLI, add, sli, sli, none, sli)
    xx(ET_ADD_SI, add, si, si, none, si)
    xx(ET_ADD_ULLI, add, ulli, ulli, none, ulli)
    xx(ET_ADD_ULI, add, uli, uli, none, uli)
    xx(ET_ADD_UI, add, ui, ui, none, ui)

    xx(ET_SUB_LDC_LD, sub, ldc, ld, none, ldc)
    xx(ET_SUB_LD_LDC, sub, ld, ldc, none, ldc)
    xx(ET_SUB_LDC_LDC, sub, ldc, ldc, none, ldc)
    xx(ET_SUB_LDC_LDI, sub, ldc, ldi, none, ldc)
    xx(ET_SUB_LDI_LDC, sub, ldi, ldc, none, ldc)
    xx(ET_SUB_DC_D, sub, dc, d, none, dc)
    xx(ET_SUB_D_DC, sub, d, dc, none, dc)
    xx(ET_SUB_DC_DC, sub, dc, dc, none, dc)
    xx(ET_SUB_DC_DI, sub, dc, di, none, dc)
    xx(ET_SUB_DI_DC, sub, di, dc, none, dc)
    xx(ET_SUB_FC_F, sub, fc, f, none, fc)
    xx(ET_SUB_F_FC, sub, f, fc, none, fc)
    xx(ET_SUB_FC_FC, sub, fc, fc, none, fc)
    xx(ET_SUB_FC_FI, sub, fc, fi, none, fc)
    xx(ET_SUB_FI_FC, sub, fi, fc, none, fc)
    xx(ET_SUB_LDI_LD, sub, ldi, ld, none, ldc)
    xx(ET_SUB_LD_LDI, sub, ld, ldi, none, ldc)
    xx(ET_SUB_LDI_LDI, sub, ldi, ldi, none, ldi)
    xx(ET_SUB_DI_D, sub, di, d, none, dc)
    xx(ET_SUB_D_DI, sub, d, di, none, dc)
    xx(ET_SUB_DI_DI, sub, di, di, none, di)
    xx(ET_SUB_FI_F, sub, fi, f, none, fc)
    xx(ET_SUB_F_FI, sub, f, fi, none, fc)
    xx(ET_SUB_FI_FI, sub, fi, fi, none, fi)
    xx(ET_SUB_LD, sub, ld, ld, none, ld)
    xx(ET_SUB_D, sub, d, d, none, d)
    xx(ET_SUB_F, sub, f, f, none, f)
    xx(ET_SUB_SLLI, sub, slli, slli, none, slli)
    xx(ET_SUB_SLI, sub, sli, sli, none, sli)
    xx(ET_SUB_SI, sub, si, si, none, si)
    xx(ET_SUB_ULLI, sub, ulli, ulli, none, ulli)
    xx(ET_SUB_ULI, sub, uli, uli, none, uli)
    xx(ET_SUB_UI, sub, ui, ui, none, ui)

    /* Constants already carry their value */
    xx(ET_C_INT, imm_si, none, none, none, si)
    xx(ET_C_LONG_INT, imm_sli, none, none, none, sli)
    xx(ET_C_LONG_LONG_INT, imm_slli, none, none, none, slli)
    xx(ET_C_UNSIGNED_INT, imm_ui, none, none, none, ui)
    xx(ET_C_UNSIGNED_LONG_INT, imm_uli, none, none, none, uli)
    xx(ET_C_UNSIGNED_LONG_LONG_INT, imm_ulli, none, none, none, ulli)
    xx(ET_C_DOUBLE, imm_d, none, none, none, d)
    xx(ET_C_FLOAT, imm_f, none, none, none, f)
    xx(ET_C_STRING_LIT, global_addr, none, none, none, none)
    xx(ET_C_WIDE_STRING_LIT, global_addr, none, none, none, none)
    xx(ET_C_UNSIGNED_SHORT_INT, imm_usi, none, none, none, u16)

    /* Pointer arithmetic is separate from regular arithmetic because
     * the unit quantity is the size of the objects pointed to. */
    xx(ET_ADD_PTR_UINTPTR, add, ptr, uintptr, none, ptr)
    xx(ET_SUB_PTR_UINTPTR, add, ptr, uintptr, none, ptr)
    xx(ET_SUB_PTR, subptr_ptr, ptr, ptr, uintptr, ptr)

    xx(ET_UNARY_PLUS_LDC, nop, ldc, none, none, ldc)
    xx(ET_UNARY_PLUS_LDI, nop, ldi, none, none, ldi)
    xx(ET_UNARY_PLUS_DC, nop, dc, none, none, dc)
    xx(ET_UNARY_PLUS_DI, nop, di, none, none, di)
    xx(ET_UNARY_PLUS_FC, nop, fc, none, none, fc)
    xx(ET_UNARY_PLUS_FI, nop, fi, none, none, fi)
    xx(ET_UNARY_PLUS_LD, nop, ld, none, none, ld)
    xx(ET_UNARY_PLUS_D, nop, d, none, none, d)
    xx(ET_UNARY_PLUS_F, nop, f, none, none, f)
    xx(ET_UNARY_PLUS_SLLI, nop, slli, none, none, slli)
    xx(ET_UNARY_PLUS_SLI, nop, sli, none, none, sli)
    xx(ET_UNARY_PLUS_SI, nop, si, none, none, si)
    xx(ET_UNARY_PLUS_ULLI, nop, ulli, none, none, ulli)
    xx(ET_UNARY_PLUS_ULI, nop, uli, none, none, uli)
    xx(ET_UNARY_PLUS_UI, nop, ui, none, none, ui)

    xx(ET_UNARY_MINUS_LDC, neg, ldc, none, none, ldc)
    xx(ET_UNARY_MINUS_LDI, neg, ldi, none, none, ldi)
    xx(ET_UNARY_MINUS_DC, neg, dc, none, none, dc)
    xx(ET_UNARY_MINUS_DI, neg, di, none, none, di)
    xx(ET_UNARY_MINUS_FC, neg, fc, none, none, fc)
    xx(ET_UNARY_MINUS_FI, neg, fi, none, none, fi)
    xx(ET_UNARY_MINUS_LD, neg, ld, none, none, ld)
    xx(ET_UNARY_MINUS_D, neg, d, none, none, d)
    xx(ET_UNARY_MINUS_F, neg, f, none, none, f)
    xx(ET_UNARY_MINUS_SLLI, neg, slli, none, none, slli)
    xx(ET_UNARY_MINUS_SLI, neg, sli, none, none, sli)
    xx(ET_UNARY_MINUS_SI, neg, si, none, none, si)
    xx(ET_UNARY_MINUS_ULLI, neg, ulli, none, none, ulli)
    xx(ET_UNARY_MINUS_ULI, neg, uli, none, none, uli)
    xx(ET_UNARY_MINUS_UI, neg, ui, none, none, ui)

    xx(ET_BITWISE_COMPLEMENT_SLLI, compl, slli, none, none, slli)
    xx(ET_BITWISE_COMPLEMENT_SLI, compl, sli, none, none, sli)
    xx(ET_BITWISE_COMPLEMENT_SI, compl, si, none, none, si)
    xx(ET_BITWISE_COMPLEMENT_ULLI, compl, ulli, none, none, ulli)
    xx(ET_BITWISE_COMPLEMENT_ULI, compl, uli, none, none, uli)
    xx(ET_BITWISE_COMPLEMENT_UI, compl, ui, none, none, ui)

    xx(ET_INDIRECTION_PTR, indir, ptr, none, none, none)

    /* Taking i8 as the right side value for the shifts, this copies the x86 SHR instruction
     * and fits the mold of undefined behavior in the spec. */
    xx(ET_SHIFT_LEFT_SLLI, shl, slli, i8, none, slli)
    xx(ET_SHIFT_LEFT_SLI, shl, sli, i8, none, sli)
    xx(ET_SHIFT_LEFT_SI, shl, si, i8, none, si)
    xx(ET_SHIFT_LEFT_ULLI, shl, ulli, i8, none, ulli)
    xx(ET_SHIFT_LEFT_ULI, shl, uli, i8, none, uli)
    xx(ET_SHIFT_LEFT_UI, shl, ui, i8, none, ui)

    xx(ET_SHIFT_RIGHT_SLLI, shr, slli, i8, none, slli)
    xx(ET_SHIFT_RIGHT_SLI, shr, sli, i8, none, sli)
    xx(ET_SHIFT_RIGHT_SI, shr, si, i8, none, si)
    xx(ET_SHIFT_RIGHT_ULLI, shr, ulli, i8, none, ulli)
    xx(ET_SHIFT_RIGHT_ULI, shr, uli, i8, none, uli)
    xx(ET_SHIFT_RIGHT_UI, shr, ui, i8, none, ui)

    xx(ET_LESS_THAN_PTR, ult, ptr, ptr, none, si)
    xx(ET_LESS_THAN_LD, ltd, ld, ld, none, si)
    xx(ET_LESS_THAN_D, ltd, d, d, none, si)
    xx(ET_LESS_THAN_F, ltf, f, f, none, si)
    xx(ET_LESS_THAN_SLLI, lt, slli, slli, none, si)
    xx(ET_LESS_THAN_SLI, lt, sli, sli, none, si)
    xx(ET_LESS_THAN_SI, lt, si, si, none, si)
    xx(ET_LESS_THAN_ULLI, lt, ulli, ulli, none, si)
    xx(ET_LESS_THAN_ULI, lt, uli, uli, none, si)
    xx(ET_LESS_THAN_UI, lt, ui, ui, none, si)

    xx(ET_GREATER_THAN_PTR, ugt, ptr, ptr, none, si)
    xx(ET_GREATER_THAN_LD, gtd, ld, ld, none, si)
    xx(ET_GREATER_THAN_D, gtd, d, d, none, si)
    xx(ET_GREATER_THAN_F, gtf, f, f, none, si)
    xx(ET_GREATER_THAN_SLLI, gt, slli, slli, none, si)
    xx(ET_GREATER_THAN_SLI, gt, sli, sli, none, si)
    xx(ET_GREATER_THAN_SI, gt, si, si, none, si)
    xx(ET_GREATER_THAN_ULLI, gt, ulli, ulli, none, si)
    xx(ET_GREATER_THAN_ULI, gt, uli, uli, none, si)
    xx(ET_GREATER_THAN_UI, gt, ui, ui, none, si)

    xx(ET_LESS_THAN_OR_EQUAL_PTR, ulte, ptr, ptr, none, si)
    xx(ET_LESS_THAN_OR_EQUAL_LD, lted, ld, ld, none, si)
    xx(ET_LESS_THAN_OR_EQUAL_D, lted, d, d, none, si)
    xx(ET_LESS_THAN_OR_EQUAL_F, ltef, f, f, none, si)
    xx(ET_LESS_THAN_OR_EQUAL_SLLI, lte, slli, slli, none, si)
    xx(ET_LESS_THAN_OR_EQUAL_SLI, lte, sli, sli, none, si)
    xx(ET_LESS_THAN_OR_EQUAL_SI, lte, si, si, none, si)
    xx(ET_LESS_THAN_OR_EQUAL_ULLI, lte, ulli, ulli, none, si)
    xx(ET_LESS_THAN_OR_EQUAL_ULI, lte, uli, uli, none, si)
    xx(ET_LESS_THAN_OR_EQUAL_UI, lte, ui, ui, none, si)

    xx(ET_GREATER_THAN_OR_EQUAL_PTR, ugte, ptr, ptr, none, si)
    xx(ET_GREATER_THAN_OR_EQUAL_LD, gted, ld, ld, none, si)
    xx(ET_GREATER_THAN_OR_EQUAL_D, gted, d, d, none, si)
    xx(ET_GREATER_THAN_OR_EQUAL_F, gtef, f, f, none, si)
    xx(ET_GREATER_THAN_OR_EQUAL_SLLI, gte, slli, slli, none, si)
    xx(ET_GREATER_THAN_OR_EQUAL_SLI, gte, sli, sli, none, si)
    xx(ET_GREATER_THAN_OR_EQUAL_SI, gte, si, si, none, si)
    xx(ET_GREATER_THAN_OR_EQUAL_ULLI, gte, ulli, ulli, none, si)
    xx(ET_GREATER_THAN_OR_EQUAL_ULI, gte, uli, uli, none, si)
    xx(ET_GREATER_THAN_OR_EQUAL_UI, gte, ui, ui, none, si)

    xx(ET_STORE_LDC, storeldc, ptr, ldc, none, ldc)
    xx(ET_STORE_LDI, storeldi, ptr, ldi, none, ldi)
    xx(ET_STORE_DC, storedc, ptr, dc, none, dc)
    xx(ET_STORE_DI, storedi, ptr, di, none, di)
    xx(ET_STORE_FC, storefc, ptr, fc, none, fc)
    xx(ET_STORE_FI, storefi, ptr, fi, none, fi)
    xx(ET_STORE_LD, storeld, ptr, ld, none, ld)
    xx(ET_STORE_D, stored, ptr, d, none, d)
    xx(ET_STORE_F, storef, ptr, f, none, f)
    xx(ET_STORE_SLLI, storeslli, ptr, slli, none, slli)
    xx(ET_STORE_SLI, storesli, ptr, sli, none, sli)
    xx(ET_STORE_SI, storesi, ptr, si, none, si)
    xx(ET_STORE_SS, storess, ptr, i16, none, i16)
    xx(ET_STORE_SC, storesc, ptr, i8, none, i8)
    xx(ET_STORE_ULLI, storeulli, ptr, ulli, none, ulli)
    xx(ET_STORE_ULI, storeuli, ptr, uli, none, uli)
    xx(ET_STORE_UI, storeui, ptr, ui, none, ui)
    xx(ET_STORE_US, storeus, ptr, u16, none, u16)
    xx(ET_STORE_UC, storeuc, ptr, u8, none, u8)
    xx(ET_STORE_C, storec, ptr, c, none, c)
    xx(ET_STORE_B, storeb, ptr, b, none, b)
    xx(ET_STORE_PTR, storeptr, ptr, ptr, none, ptr)

    xx(ET_COPY, copy, ptr, ptr, uintptr, none)
    xx(ET_CLEAR, clear, ptr, uintptr, none, none)

    xx(ET_SEQ, copy2ndchild, none, none, none, none)

    xx(ET_ADDRESS_E, extern_addr, none, none, none, none)
    xx(ET_ADDRESS_G, global_addr, none, none, none, none)
    xx(ET_ADDRESS_P, param_addr, none, none, none, none)
    xx(ET_ADDRESS_L, local_addr, none, none, none, none)
    xx(ET_ADDRESS_R, return_value_addr, none, none, none, none)

    xx(ET_BITWISE_AND_SLLI, bitwise_and, slli, slli, none, slli)
    xx(ET_BITWISE_AND_SLI, bitwise_and, sli, sli, none, sli)
    xx(ET_BITWISE_AND_SI, bitwise_and, si, si, none, si)
    xx(ET_BITWISE_AND_ULLI, bitwise_and, ulli, ulli, none, ulli)
    xx(ET_BITWISE_AND_ULI, bitwise_and, uli, uli, none, uli)
    xx(ET_BITWISE_AND_UI, bitwise_and, ui, ui, none, ui)

    xx(ET_BITWISE_OR_SLLI, bitwise_or, slli, slli, none, slli)
    xx(ET_BITWISE_OR_SLI, bitwise_or, sli, sli, none, sli)
    xx(ET_BITWISE_OR_SI, bitwise_or, si, si, none, si)
    xx(ET_BITWISE_OR_ULLI, bitwise_or, ulli, ulli, none, ulli)
    xx(ET_BITWISE_OR_ULI, bitwise_or, uli, uli, none, uli)
    xx(ET_BITWISE_OR_UI, bitwise_or, ui, ui, none, ui)

    xx(ET_BITWISE_XOR_SLLI, bitwise_xor, slli, slli, none, slli)
    xx(ET_BITWISE_XOR_SLI, bitwise_xor, sli, sli, none, sli)
    xx(ET_BITWISE_XOR_SI, bitwise_xor, si, si, none, si)
    xx(ET_BITWISE_XOR_ULLI, bitwise_xor, ulli, ulli, none, ulli)
    xx(ET_BITWISE_XOR_ULI, bitwise_xor, uli, uli, none, uli)
    xx(ET_BITWISE_XOR_UI, bitwise_xor, ui, ui, none, ui)

    xx(ET_EQU_LDC_LD, equ_dc, ldc, ld, none, si)
    xx(ET_EQU_LDC_LDC, equ_dc, ldc, ldc, none, si)
    xx(ET_EQU_LDC_LDI, equ_dc, ldc, ldi, none, si)
    xx(ET_EQU_DC_D, equ_dc, dc, d, none, si)
    xx(ET_EQU_DC_DC, equ_dc, dc, dc, none, si)
    xx(ET_EQU_DC_DI, equ_dc, dc, di, none, si)
    xx(ET_EQU_FC_F, equ_fc, fc, f, none, si)
    xx(ET_EQU_FC_FC, equ_fc, fc, fc, none, si)
    xx(ET_EQU_FC_FI, equ_fc, fc, fi, none, si)
    xx(ET_EQU_LDI_LD, equ_dc, ldi, ld, none, si)
    xx(ET_EQU_LDI_LDI, equ_dc, ldi, ldi, none, si)
    xx(ET_EQU_DI_D, equ_dc, di, d, none, si)
    xx(ET_EQU_DI_DI, equ_dc, di, di, none, si)
    xx(ET_EQU_FI_F, equ_fc, fi, f, none, si)
    xx(ET_EQU_FI_FI, equ_fc, fi, fi, none, si)
    xx(ET_EQU_LD, equ_dc, ld, ld, none, si)
    xx(ET_EQU_D, equ_dc, d, d, none, si)
    xx(ET_EQU_F, equ_fc, f, f, none, si)
    xx(ET_EQU_SLLI, equ_i, slli, slli, none, si)
    xx(ET_EQU_SLI, equ_i, sli, sli, none, si)
    xx(ET_EQU_SI, equ_i, si, si, none, si)
    xx(ET_EQU_ULLI, equ_u, ulli, ulli, none, si)
    xx(ET_EQU_ULI, equ_u, uli, uli, none, si)
    xx(ET_EQU_UI, equ_u, ui, ui, none, si)
    xx(ET_EQU_PTR, equ_u, ptr, ptr, none, si)

    xx(ET_NEQ_LDC_LD, neq_dc, ldc, ld, none, si)
    xx(ET_NEQ_LDC_LDC, neq_dc, ldc, ldc, none, si)
    xx(ET_NEQ_LDC_LDI, neq_dc, ldc, ldi, none, si)
    xx(ET_NEQ_DC_D, neq_dc, dc, d, none, si)
    xx(ET_NEQ_DC_DC, neq_dc, dc, dc, none, si)
    xx(ET_NEQ_DC_DI, neq_dc, dc, di, none, si)
    xx(ET_NEQ_FC_F, neq_fc, fc, f, none, si)
    xx(ET_NEQ_FC_FC, neq_fc, fc, fc, none, si)
    xx(ET_NEQ_FC_FI, neq_fc, fc, fi, none, si)
    xx(ET_NEQ_LDI_LD, neq_dc, ldi, ld, none, si)
    xx(ET_NEQ_LDI_LDI, neq_dc, ldi, ldi, none, si)
    xx(ET_NEQ_DI_D, neq_dc, di, d, none, si)
    xx(ET_NEQ_DI_DI, neq_dc, di, di, none, si)
    xx(ET_NEQ_FI_F, neq_fc, fi, f, none, si)
    xx(ET_NEQ_FI_FI, neq_fc, fi, fi, none, si)
    xx(ET_NEQ_LD, neq_dc, ld, ld, none, si)
    xx(ET_NEQ_D, neq_dc, d, d, none, si)
    xx(ET_NEQ_F, neq_fc, f, f, none, si)
    xx(ET_NEQ_SLLI, neq_i, slli, slli, none, si)
    xx(ET_NEQ_SLI, neq_i, sli, sli, none, si)
    xx(ET_NEQ_SI, neq_i, si, si, none, si)
    xx(ET_NEQ_ULLI, neq_u, ulli, ulli, none, si)
    xx(ET_NEQ_ULI, neq_u, uli, uli, none, si)
    xx(ET_NEQ_UI, neq_u, ui, ui, none, si)
    xx(ET_NEQ_PTR, neq_u, ptr, ptr, none, si)

    /* NOTE: ET_LOGICAL_AND, ET_LOGICAL_OR are already handled above 
     * due to sequence point and conditional evaluation considerations */

    xx(ET_FUNCTION_CALL, func, ptr, none, none, none)
    xx(ET_FUNCTION_ARG_LIST, nop, none, none, none, none)

#undef xx
    default:
      assert(0 && "Missing case");
  }

  int opd;
  for (opd = 0; opd < 3; ++opd) {
    /* Down-convert enumeration types to their integer representation (compatible integer type) */
    if ((load[opd] == enum_cit) || (load[opd] == enum_type)) {
      struct type_node *tn;
      if (load[opd] == enum_cit) {
        tn = expr_type(cc, x->children_[0]);
      }
      else /* (load == enum_type) */ {
        tn = x->type_arg_;
      }
      assert(tn->kind_ == tk_enumeration && "Must have an enum as child");
      tn = tn->derived_from_ /* get Compatible Integer Type */;

      switch (tn->kind_) {
        case tk_bool:
          load[opd] = b;
          break;
        case tk_char:
          load[opd] = c;
          break;
        case tk_signed_char:
          load[opd] = i8;
          break;
        case tk_unsigned_char:
          load[opd] = u8;
          break;
        case tk_short_int:
          load[opd] = i16;
          break;
        case tk_unsigned_short_int:
          load[opd] = u16;
          break;
        case tk_int:
          load[opd] = si;
          break;
        case tk_unsigned_int:
          load[opd] = ui;
          break;
        default:
          assert(0 && "Unsupported enum compatible integer type");
          break;
      }
    }

    if (load[opd] == ptr) {
      load[opd] = uintptr;
    }

    switch (load[opd]) {
      case i8:
        operands_i[opd] = (int8_t)temps[x->children_[opd]->ord_].v_.i64_;
        operands_u[opd] = (uint64_t)operands_i[opd];
        operands_d[opd] = (double)operands_i[opd];
        operands_f[opd] = (float)operands_i[opd];
        operands_fi[opd] = 0.f;
        operands_di[opd] = 0.;
        break;
      case i16:
        operands_i[opd] = (int16_t)temps[x->children_[opd]->ord_].v_.i64_;
        operands_u[opd] = (uint64_t)operands_i[opd];
        operands_d[opd] = (double)operands_i[opd];
        operands_f[opd] = (float)operands_i[opd];
        operands_fi[opd] = 0.f;
        operands_di[opd] = 0.;
        break;
      case i32:
        operands_i[opd] = (int32_t)temps[x->children_[opd]->ord_].v_.i64_;
        operands_u[opd] = (uint64_t)operands_i[opd];
        operands_d[opd] = (double)operands_i[opd];
        operands_f[opd] = (float)operands_i[opd];
        operands_fi[opd] = 0.f;
        operands_di[opd] = 0.;
        break;
      case i64:
        operands_i[opd] = (int64_t)temps[x->children_[opd]->ord_].v_.i64_;
        operands_u[opd] = (uint64_t)operands_i[opd];
        operands_d[opd] = (double)operands_i[opd];
        operands_f[opd] = (float)operands_i[opd];
        operands_fi[opd] = 0.f;
        operands_di[opd] = 0.;
        break;
      case u8:
        operands_i[opd] = (uint8_t)temps[x->children_[opd]->ord_].v_.u64_;
        operands_u[opd] = (uint64_t)operands_i[opd];
        operands_d[opd] = (double)operands_u[opd];
        operands_f[opd] = (float)operands_u[opd];
        operands_fi[opd] = 0.f;
        operands_di[opd] = 0.;
        break;
      case u16:
        operands_i[opd] = (uint16_t)temps[x->children_[opd]->ord_].v_.u64_;
        operands_u[opd] = (uint64_t)operands_i[opd];
        operands_d[opd] = (double)operands_u[opd];
        operands_f[opd] = (float)operands_u[opd];
        operands_fi[opd] = 0.f;
        operands_di[opd] = 0.;
        break;
      case u32:
        operands_i[opd] = (uint32_t)temps[x->children_[opd]->ord_].v_.u64_;
        operands_u[opd] = (uint64_t)operands_i[opd];
        operands_d[opd] = (double)operands_u[opd];
        operands_f[opd] = (float)operands_u[opd];
        operands_fi[opd] = 0.f;
        operands_di[opd] = 0.;
        break;
      case u64:
        operands_i[opd] = (uint64_t)temps[x->children_[opd]->ord_].v_.u64_;
        operands_u[opd] = (uint64_t)operands_i[opd];
        operands_d[opd] = (double)operands_u[opd];
        operands_f[opd] = (float)operands_u[opd];
        operands_fi[opd] = 0.f;
        operands_di[opd] = 0.;
        break;
      case ld:
        operands_i[opd] = (int64_t)temps[x->children_[opd]->ord_].v_.d_;
        operands_u[opd] = (uint64_t)temps[x->children_[opd]->ord_].v_.d_;
        operands_d[opd] = temps[x->children_[opd]->ord_].v_.d_;
        operands_f[opd] = (float)temps[x->children_[opd]->ord_].v_.d_;
        operands_fi[opd] = 0.f;
        operands_di[opd] = 0.;
        break;
      case d:
        operands_i[opd] = (int64_t)temps[x->children_[opd]->ord_].v_.d_;
        operands_u[opd] = (uint64_t)temps[x->children_[opd]->ord_].v_.d_;
        operands_d[opd] = temps[x->children_[opd]->ord_].v_.d_;
        operands_f[opd] = (float)temps[x->children_[opd]->ord_].v_.d_;
        operands_fi[opd] = 0.f;
        operands_di[opd] = 0.;
        break;
      case f:
        operands_i[opd] = (int64_t)temps[x->children_[opd]->ord_].v_.f_;
        operands_u[opd] = (uint64_t)temps[x->children_[opd]->ord_].v_.f_;
        operands_d[opd] = (double)temps[x->children_[opd]->ord_].v_.f_;
        operands_f[opd] = temps[x->children_[opd]->ord_].v_.f_;
        operands_fi[opd] = 0.f;
        operands_di[opd] = 0.;
        break;
      case fi:
        operands_i[opd] = 0;
        operands_u[opd] = 0;
        operands_d[opd] = 0.;
        operands_f[opd] = 0.f;
        operands_fi[opd] = temps[x->children_[opd]->ord_].v_.fi_;
        operands_di[opd] = (double)temps[x->children_[opd]->ord_].v_.fi_;
        break;
      case di:
        operands_i[opd] = 0;
        operands_u[opd] = 0;
        operands_d[opd] = 0.;
        operands_f[opd] = 0.f;
        operands_fi[opd] = (float)temps[x->children_[opd]->ord_].v_.di_;
        operands_di[opd] = temps[x->children_[opd]->ord_].v_.di_;
        break;
      case ldi:
        operands_i[opd] = 0;
        operands_u[opd] = 0;
        operands_d[opd] = 0.;
        operands_f[opd] = 0.f;
        operands_fi[opd] = (float)temps[x->children_[opd]->ord_].v_.di_;
        operands_di[opd] = temps[x->children_[opd]->ord_].v_.di_;
        break;
      case fc:
        operands_i[opd] = (int64_t)temps[x->children_[opd]->ord_].v_.fc_.f_;
        operands_u[opd] = (uint64_t)temps[x->children_[opd]->ord_].v_.fc_.f_;
        operands_d[opd] = (double)temps[x->children_[opd]->ord_].v_.fc_.f_;
        operands_f[opd] = temps[x->children_[opd]->ord_].v_.fc_.f_;
        operands_fi[opd] = temps[x->children_[opd]->ord_].v_.fc_.fi_;
        operands_di[opd] = (double)temps[x->children_[opd]->ord_].v_.fc_.fi_;
        break;
      case dc:
        operands_i[opd] = (int64_t)temps[x->children_[opd]->ord_].v_.dc_.d_;
        operands_u[opd] = (uint64_t)temps[x->children_[opd]->ord_].v_.dc_.d_;
        operands_d[opd] = temps[x->children_[opd]->ord_].v_.dc_.d_;
        operands_f[opd] = (float)temps[x->children_[opd]->ord_].v_.dc_.d_;
        operands_fi[opd] = (float)temps[x->children_[opd]->ord_].v_.dc_.di_;
        operands_di[opd] = temps[x->children_[opd]->ord_].v_.dc_.di_;
        break;
      case ldc:
        operands_i[opd] = (int64_t)temps[x->children_[opd]->ord_].v_.dc_.d_;
        operands_u[opd] = (uint64_t)temps[x->children_[opd]->ord_].v_.dc_.d_;
        operands_d[opd] = temps[x->children_[opd]->ord_].v_.dc_.d_;
        operands_f[opd] = (float)temps[x->children_[opd]->ord_].v_.dc_.d_;
        operands_fi[opd] = (float)temps[x->children_[opd]->ord_].v_.dc_.di_;
        operands_di[opd] = temps[x->children_[opd]->ord_].v_.dc_.di_;
        break;
      case b:
        /* double logical-not converts evaluates int as boolean to 0 or 1 */
        operands_i[opd] = (uint8_t)!!temps[x->children_[opd]->ord_].v_.i64_;
        operands_u[opd] = (uint64_t)operands_i[opd];
        operands_d[opd] = (double)operands_u[opd];
        operands_f[opd] = (float)operands_u[opd];
        operands_fi[opd] = 0.f;
        operands_di[opd] = 0.;
        break;
      case enum_type:
      case enum_cit:
        assert(0 && "Enumeration types should have pre-translated to Compatible Integer Type (CIT) at this point");
        break;
      case ptr:
        assert(0 && "Pointer types should have pre-translated to their uint equivalent at this point");
        break;
      case none:
        /* Ignore operand */
        break;
    }
  }

  switch (op) {
    case nop:
      /* Already done (eg. conversion or whatnot) */
      break;
    case mul:
      operands_i[0] *= operands_i[1];
      operands_u[0] *= operands_u[1];
      complex_mul_d(operands_d + 0, operands_di + 0, operands_d[0], operands_di[0], operands_d[1], operands_di[1]);
      complex_mul_f(operands_f + 0, operands_fi + 0, operands_f[0], operands_fi[0], operands_f[1], operands_fi[1]);
      break;
    case div:
      operands_i[0] /= operands_i[1];
      operands_u[0] /= operands_u[1];
      complex_div_d(operands_d + 0, operands_di + 0, operands_d[0], operands_di[0], operands_d[1], operands_di[1]);
      complex_div_f(operands_f + 0, operands_fi + 0, operands_f[0], operands_fi[0], operands_f[1], operands_fi[1]);
      break;
    case rem:
      operands_i[0] %= operands_i[1];
      operands_u[0] %= operands_u[1];
      break;
    case add:
      operands_i[0] += operands_i[1];
      operands_u[0] += operands_u[1];
      operands_d[0] += operands_d[1];
      operands_f[0] += operands_f[1];
      operands_fi[0] += operands_fi[1];
      operands_di[0] += operands_di[1];
      break;
    case sub:
      operands_i[0] -= operands_i[1];
      operands_u[0] -= operands_u[1];
      operands_d[0] -= operands_d[1];
      operands_f[0] -= operands_f[1];
      operands_fi[0] -= operands_fi[1];
      operands_di[0] -= operands_di[1];
      break;
    case subptr_ptr: {
      operands_u[0] = (operands_u[0] - operands_u[1]) / operands_u[2];
      break;
    }
    case imm_si:
    case imm_sli:
    case imm_slli:
      /* Rely on the expr node construction having done the appropriate sign-extension / bit-clamping
       * prior to setting x->v_. */
      operands_i[0] = x->v_.i64_;
      operands_u[0] = (uint64_t)operands_i[0];
      operands_d[0] = (double)operands_i[0];
      operands_f[0] = (float)operands_i[0];
      operands_fi[0] = 0.f;
      operands_di[0] = 0.;
      break;
    case imm_ui:
    case imm_uli:
    case imm_ulli:
    case imm_usi:
      operands_u[0] = x->v_.u64_;
      operands_i[0] = (int64_t)operands_u[0];
      operands_d[0] = (double)operands_u[0];
      operands_f[0] = (float)operands_u[0];
      operands_fi[0] = 0.f;
      operands_di[0] = 0.;
      break;
    case imm_d:
      operands_i[0] = (int64_t)x->v_.d_;
      operands_u[0] = (uint64_t)x->v_.d_;
      operands_d[0] = x->v_.d_;
      operands_f[0] = (float)x->v_.d_;
      operands_fi[0] = 0.f;
      operands_di[0] = 0.;
      break;
    case imm_f:
      operands_i[0] = (int64_t)x->v_.f_;
      operands_u[0] = (uint64_t)x->v_.f_;
      operands_d[0] = (double)x->v_.f_;
      operands_f[0] = x->v_.f_;
      operands_fi[0] = 0.f;
      operands_di[0] = 0.;
      break;
    case neg:
      operands_i[0] = -operands_i[0];
      operands_u[0] = (uint64_t)-(int64_t)operands_u[0]; /* Casting to silence C4146 */
      operands_d[0] = -operands_d[0];
      operands_f[0] = -operands_f[0];
      operands_fi[0] = -operands_fi[0];
      operands_di[0] = -operands_di[0];
      break;
    case compl:
      operands_i[0] = ~operands_i[0];
      operands_u[0] = ~operands_u[0];
      operands_d[0] = 0.;
      operands_f[0] = 0.f;
      operands_fi[0] = 0.f;
      operands_di[0] = 0.;
      break;
    case indir: {
      assert(!is_constant_expr && "Cannot execute indirection instructions in constant expression");
      void *p = ((void *)(operands_u[0]));
      operands_i[0] = 0;
      operands_u[0] = 0;
      operands_d[0] = 0.;
      operands_f[0] = 0.f;
      operands_fi[0] = 0.f;
      operands_di[0] = 0.;

      struct type_node *tn = expr_type(cc, x);
      tn = type_node_unqualified(tn);
      enum type_kind tk = tn->kind_;
      if (tk == tk_enumeration) {
        tk = type_node_arith_type_kind_no_enum(&cc->tb_, tn);
      }
      if (tk == tk_pointer) {
        tk = cc->tb_.uintptr_equivalent_;
      }
      switch (tk) {
        case tk_char: indi = c; break;
        case tk_signed_char: indi = i8; break;
        case tk_short_int: indi = i16; break;
        case tk_int: indi = si; break;
        case tk_long_int: indi = sli; break;
        case tk_long_long_int: indi = slli; break;
        case tk_bool: indi = b; break;
        case tk_unsigned_char: indi = u8; break;
        case tk_unsigned_short_int: indi = u16; break;
        case tk_unsigned_int: indi = ui; break;
        case tk_unsigned_long_int: indi = uli; break;
        case tk_unsigned_long_long_int: indi = ulli; break;
        case tk_float: indi = f; break;
        case tk_double: indi = d; break;
        case tk_long_double: indi = ld; break;
        case tk_float_complex: indi = fc; break;
        case tk_double_complex: indi = dc; break;
        case tk_long_double_complex: indi = ldc; break;
        case tk_float_imaginary: indi = fi; break;
        case tk_double_imaginary: indi = di; break;
        case tk_long_double_imaginary: indi = ldi; break;
        case tk_void:
          assert(0 && "Indirection of void type not supported");
          break;
        case tk_enumeration:
          assert(0 && "Enumeration with enumeration as integer type");
          break;
        case tk_array:
          /* XXX: Handle array indirection; should array not have converted to pointer ?? */
          assert(0 && "XXX: Not expecting array");
          break;
        case tk_structure:
        case tk_union:
          /* Silently ignore structures, the thinking is that this indirection is either at
           * the root, or this is an intermediate expression tree. */
          /* XXX: Find some field access operator for structures */
          indi = none;
          break;
        case tk_function:
          assert(0 && "Indirection of function not possible");
          break;
        case tk_pointer:
          assert(0 && "Should not have pointer type");
          break;
        case tk_qualifier:
          assert(0 && "Types should have been fully dequalified");
          break;
      }
      store = indi;
      switch (indi) {
        case i8:
          operands_i[0] = *(int8_t *)p;
          break;
        case i16:
          operands_i[0] = *(int16_t *)p;
          break;
        case i32:
          operands_i[0] = *(int32_t *)p;
          break;
        case i64:
          operands_i[0] = *(int64_t *)p;
          break;
        case u8:
          operands_u[0] = *(uint8_t *)p;
          break;
        case u16:
          operands_u[0] = *(uint16_t *)p;
          break;
        case u32:
          operands_u[0] = *(uint32_t *)p;
          break;
        case u64:
          operands_u[0] = *(uint64_t *)p;
          break;
        case ld:
        case d:
          operands_d[0] = *(double *)p;
          break;
        case f:
          operands_f[0] = *(float *)p;
          break;
        case fi:
          operands_fi[0] = *(float *)p;
          break;
        case di:
        case ldi:
          operands_di[0] = *(double *)p;
          break;
        case fc:
          operands_f[0] = ((float *)p)[0];
          operands_fi[0] = ((float *)p)[1];
          break;
        case dc:
        case ldc:
          operands_d[0] = ((double *)p)[0];
          operands_di[0] = ((double *)p)[1];
          break;
        case b:
          operands_i[0] = !!*(char *)p;
          break;
        case none:
          break;
        default:
          assert(0 && "Unexpected indirection final operand type");
          break;
      }
      break;
    }
    case shl:
      operands_i[0] = operands_i[0] << operands_i[1];
      operands_u[0] = operands_u[0] << operands_i[1];
      break;
    case shr:
      operands_i[0] = operands_i[0] >> operands_i[1];
      operands_u[0] = operands_u[0] >> operands_i[1];
      break;
    case lt:
      operands_i[0] = operands_i[0] < operands_i[1];
      break;
    case ult:
      operands_i[0] = operands_u[0] < operands_u[1];
      break;
    case ltf:
      operands_i[0] = operands_f[0] < operands_f[1];
      break;
    case ltd:
      operands_i[0] = operands_d[0] < operands_d[1];
      break;
    case gt:
      operands_i[0] = operands_i[0] > operands_i[1];
      break;
    case ugt:
      operands_i[0] = operands_u[0] > operands_u[1];
      break;
    case gtf:
      operands_i[0] = operands_f[0] > operands_f[1];
      break;
    case gtd:
      operands_i[0] = operands_d[0] > operands_d[1];
      break;
    case lte:
      operands_i[0] = operands_i[0] <= operands_i[1];
      break;
    case ulte:
      operands_i[0] = operands_u[0] <= operands_u[1];
      break;
    case ltef:
      operands_i[0] = operands_f[0] <= operands_f[1];
      break;
    case lted:
      operands_i[0] = operands_d[0] <= operands_d[1];
      break;
    case gte:
      operands_i[0] = operands_i[0] >= operands_i[1];
      break;
    case ugte:
      operands_i[0] = operands_u[0] >= operands_u[1];
      break;
    case gtef:
      operands_i[0] = operands_f[0] >= operands_f[1];
      break;
    case gted:
      operands_i[0] = operands_d[0] >= operands_d[1];
      break;

    case storeldc:
      assert(!is_constant_expr && "Cannot execute store instructions in constant expression");
      operands_d[0] = operands_d[1];
      operands_di[0] = operands_di[1];
      ((double *)operands_u[0])[0] = operands_d[0];
      ((double *)operands_u[0])[1] = operands_di[0];
      break;
    case storeldi:
      assert(!is_constant_expr && "Cannot execute store instructions in constant expression");
      operands_di[0] = operands_di[1];
      ((double *)operands_u[0])[0] = operands_di[0];
      break;
    case storedc:
      assert(!is_constant_expr && "Cannot execute store instructions in constant expression");
      operands_d[0] = operands_d[1];
      operands_di[0] = operands_di[1];
      ((double *)operands_u[0])[0] = operands_d[0];
      ((double *)operands_u[0])[1] = operands_di[0];
      break;
    case storedi:
      assert(!is_constant_expr && "Cannot execute store instructions in constant expression");
      operands_di[0] = operands_di[1];
      ((double *)operands_u[0])[0] = operands_di[0];
      break;
    case storefc:
      assert(!is_constant_expr && "Cannot execute store instructions in constant expression");
      operands_f[0] = operands_f[1];
      operands_fi[0] = operands_fi[1];
      ((float *)operands_u[0])[0] = operands_f[0];
      ((float *)operands_u[0])[1] = operands_fi[0];
      break;
    case storefi:
      assert(!is_constant_expr && "Cannot execute store instructions in constant expression");
      operands_fi[0] = operands_fi[1];
      ((float *)operands_u[0])[0] = operands_fi[0];
      break;
    case storeld:
      assert(!is_constant_expr && "Cannot execute store instructions in constant expression");
      operands_d[0] = operands_d[1];
      ((double *)operands_u[0])[0] = operands_d[0];
      break;
    case stored:
      assert(!is_constant_expr && "Cannot execute store instructions in constant expression");
      operands_d[0] = operands_d[1];
      ((double *)operands_u[0])[0] = operands_d[0];
      break;
    case storef:
      assert(!is_constant_expr && "Cannot execute store instructions in constant expression");
      operands_f[0] = operands_f[1];
      ((float *)operands_u[0])[0] = operands_f[0];
      break;
    case storeslli:
    case storesli:
    case storesi:
    case storess:
    case storesc: {
      assert(!is_constant_expr && "Cannot execute store instructions in constant expression");
      int bits;
      switch (op) {
        case storeslli: bits = cc->tb_.bits_per_long_long_; break;
        case storesli: bits = cc->tb_.bits_per_long_; break;
        case storesi: bits = cc->tb_.bits_per_int_; break;
        case storess: bits = 16; break;
        case storesc: bits = 8; break;
      }
      switch (bits) {
        case 64:
          ((int64_t *)operands_u[0])[0] = operands_i[1];
          operands_i[0] = operands_i[1];
          break;
        case 32:
          ((int32_t *)operands_u[0])[0] = (int32_t)operands_i[1];
          operands_i[0] = operands_i[1];
          break;
        case 16:
          ((int16_t *)operands_u[0])[0] = (int16_t)operands_i[1];
          operands_i[0] = operands_i[1];
          break;
        case 8:
          ((int8_t *)operands_u[0])[0] = (int8_t)operands_i[1];
          operands_i[0] = operands_i[1];
          break;
      }
      break;
    }

    case storeulli:
    case storeuli:
    case storeui:
    case storeus:
    case storeuc: {
      assert(!is_constant_expr && "Cannot execute store instructions in constant expression");
      int bits;
      switch (op) {
        case storeulli: bits = cc->tb_.bits_per_long_long_; break;
        case storeuli: bits = cc->tb_.bits_per_long_; break;
        case storeui: bits = cc->tb_.bits_per_int_; break;
        case storeus: bits = 16; break;
        case storeuc: bits = 8; break;
      }
      switch (bits) {
        case 64:
          ((uint64_t *)operands_u[0])[0] = operands_u[1];
          operands_u[0] = operands_u[1];
          break;
        case 32:
          ((uint32_t *)operands_u[0])[0] = (uint32_t)operands_u[1];
          operands_u[0] = operands_u[1];
          break;
        case 16:
          ((uint16_t *)operands_u[0])[0] = (uint16_t)operands_u[1];
          operands_u[0] = operands_u[1];
          break;
        case 8:
          ((uint8_t *)operands_u[0])[0] = (uint8_t)operands_u[1];
          operands_u[0] = operands_u[1];
          break;
      }
      break;
    }

    case storec:
      assert(!is_constant_expr && "Cannot execute store instructions in constant expression");
      if (cc->tb_.char_is_signed_) {
        ((int8_t *)operands_u[0])[0] = (int8_t)operands_i[1];
        operands_i[0] = operands_i[1];
      }
      else {
        ((uint8_t *)operands_u[0])[0] = (uint8_t)operands_u[1];
        operands_u[0] = operands_u[1];
      }
      break;
    case storeb:
      assert(!is_constant_expr && "Cannot execute store instructions in constant expression");
      ((uint8_t *)operands_u[0])[0] = (uint8_t)operands_u[1];
      operands_u[0] = operands_u[1];
      break;
    case storeptr: {
      assert(!is_constant_expr && "Cannot execute store instructions in constant expression");
      int bits_per_ptr;
      switch (cc->tb_.uintptr_equivalent_) {
        case tk_unsigned_long_long_int: bits_per_ptr = cc->tb_.bits_per_long_long_; break;
        case tk_unsigned_long_int: bits_per_ptr = cc->tb_.bits_per_long_; break;
        case tk_unsigned_int: bits_per_ptr = cc->tb_.bits_per_int_; break;
      }
      switch (bits_per_ptr) {
        case 64:
          ((uint64_t *)operands_u[0])[0] = operands_u[1];
          operands_u[0] = operands_u[1];
          break;
        case 32:
          ((uint32_t *)operands_u[0])[0] = (uint32_t)operands_u[1];
          operands_u[0] = operands_u[1];
          break;
        case 16:
          ((uint16_t *)operands_u[0])[0] = (uint16_t)operands_u[1];
          operands_u[0] = operands_u[1];
          break;
      }
      operands_u[0] = operands_u[1];
      break;
    }
    case copy: {
      assert(!is_constant_expr && "Cannot execute copy instructions in constant expression");
      memcpy((void *)operands_u[0], (void *)operands_u[1], operands_u[2]);
      break;
    }
    case clear: {
      assert(!is_constant_expr && "Cannot execute copy instructions in constant expression");
      memset((void *)operands_u[0], 0, operands_u[1]);
      break;
    }

    case copy2ndchild:
      temps[x->ord_].v_ = temps[x->children_[1]->ord_].v_;
      break;

    case extern_addr:
      /* Load & link resolution */
      if (x->decl_ && (x->decl_ == nulled_out_decl)) {
        /* Evaluate to zero so relocator / linker can fix it up */
        temps[x->ord_].v_.u64_ = 0;
        break;
      }
      if (!x->decl_->external_) {
        void *p;
        int r;
        r = drl_resolve_sym(cc, x->decl_, &p);

        if (r) {
          cc_fatal_loc(cc, &x->decl_->def_loc_, "Failed to dynamically resolve linkage for \"%s\" (%d)\n", x->decl_->sym_.ident_, r);
          break;
        }
        else {
          x->decl_->external_ = p;
        }
      }

      temps[x->ord_].v_.u64_ = (uint64_t)x->decl_->external_;
      break;

    case global_addr:
      if (x->decl_ && (x->decl_ == nulled_out_decl)) {
        /* Evaluate to zero so relocator / linker can fix it up */
        temps[x->ord_].v_.u64_ = 0;
        break;
      }
      temps[x->ord_].v_.u64_ = (uint64_t)x->dsp_->data_;
      break;
    case local_addr: {
      temps[x->ord_].v_.u64_ = (uint64_t)(((char *)local_base) + x->decl_->local_offset_);
      break;
    }
    case param_addr: {
      temps[x->ord_].v_.u64_ = (uint64_t)(((char *)param_base) + x->decl_->param_offset_);
      break;
    }
    case return_value_addr: {
      temps[x->ord_].v_.u64_ = (uint64_t)return_value_ptr;
    }

    case bitwise_and:
      operands_i[0] &= operands_i[1];
      operands_u[0] &= operands_u[1];
      break;
    case bitwise_or:
      operands_i[0] |= operands_i[1];
      operands_u[0] |= operands_u[1];
      break;
    case bitwise_xor:
      operands_i[0] ^= operands_i[1];
      operands_u[0] ^= operands_u[1];
      break;

    case equ_dc:
      operands_i[0] = (operands_d[0] == operands_d[1]) && (operands_di[0] == operands_di[1]);
      break;
    case equ_fc:
      operands_i[0] = (operands_f[0] == operands_f[1]) && (operands_fi[0] == operands_fi[1]);
      break;
    case equ_i:
      operands_i[0] = operands_i[0] == operands_i[1];
      break;
    case equ_u:
      operands_i[0] = operands_u[0] == operands_u[1];
      break;

    case neq_dc:
      operands_i[0] = (operands_d[0] != operands_d[1]) || (operands_di[0] != operands_di[1]);
      break;
    case neq_fc:
      operands_i[0] = (operands_f[0] != operands_f[1]) || (operands_fi[0] != operands_fi[1]);
      break;
    case neq_i:
      operands_i[0] = operands_i[0] != operands_i[1];
      break;
    case neq_u:
      operands_i[0] = operands_u[0] != operands_u[1];
      break;
    case func: {
      /* Handle *actual* function call here.. */
      if ((!x->children_[0]) || (!temps[x->children_[0]->ord_].v_.u64_)) {
        if (x->children_[0] && x->children_[0]->decl_) {
          cc_fatal_loc(cc, &x->children_[0]->decl_->def_loc_, "Runtime error: function entry for \"%s\" resolves to NULL pointer\n", x->children_[0]->decl_->sym_.ident_);
        }
        else {
          cc_fatal(cc, "Runtime error: function entry resolves to NULL pointer\n");
        }
      }
      else {
        /* Invoke function */
        r = expr_invoke_function_call(cc, x, temps);
        if (r) {
          return r;
        }
      }
      break;
    }
    default:
      assert(0 && "operation not handled");
      break;
  }

    /* Down-convert enumeration types to their integer representation (compatible integer type) */
  if ((store == enum_cit) || (store == enum_type)) {
    struct type_node *tn;
    if (store == enum_cit) {
      tn = expr_type(cc, x->children_[0]);
    }
    else /* (store == enum_type) */ {
      tn = x->type_arg_;
    }
    assert(tn->kind_ == tk_enumeration && "Must have an enum as child");
    tn = tn->derived_from_ /* get Compatible Integer Type */;

    switch (tn->kind_) {
      case tk_bool:
        store = b;
        break;
      case tk_char:
        store = c;
        break;
      case tk_signed_char:
        store = i8;
        break;
      case tk_unsigned_char:
        store = u8;
        break;
      case tk_short_int:
        store = i16;
        break;
      case tk_unsigned_short_int:
        store = u16;
        break;
      case tk_int:
        store = si;
        break;
      case tk_unsigned_int:
        store = ui;
        break;
      default:
        assert(0 && "Unsupported enum compatible integer type");
        break;
    }
  }

  if (store == ptr) {
    store = uintptr;
  }

  switch (store) {
    case i8:
      temps[x->ord_].v_.i64_ = (int8_t)operands_i[0];
      break;
    case i16:
      temps[x->ord_].v_.i64_ = (int16_t)operands_i[0];
      break;
    case i32:
      temps[x->ord_].v_.i64_ = (int32_t)operands_i[0];
      break;
    case i64:
      temps[x->ord_].v_.i64_ = operands_i[0];
      break;
    case u8:
      temps[x->ord_].v_.u64_ = (uint8_t)operands_u[0];
      break;
    case u16:
      temps[x->ord_].v_.u64_ = (uint16_t)operands_u[0];
      break;
    case u32:
      temps[x->ord_].v_.u64_ = (uint32_t)operands_u[0];
      break;
    case u64:
      temps[x->ord_].v_.u64_ = operands_u[0];
      break;
    case ld:
    case d:
      temps[x->ord_].v_.d_ = operands_d[0];
      break;
    case f:
      temps[x->ord_].v_.f_ = operands_f[0];
      break;
    case fi:
      temps[x->ord_].v_.fi_ = operands_fi[0];
      break;
    case ldi:
    case di:
      temps[x->ord_].v_.di_ = operands_di[0];
      break;
    case fc:
      temps[x->ord_].v_.fi_ = operands_fi[0];
      temps[x->ord_].v_.f_ = operands_f[0];
      break;
    case ldc:
    case dc:
      temps[x->ord_].v_.di_ = operands_di[0];
      temps[x->ord_].v_.d_ = operands_d[0];
      break;
    case b:
      temps[x->ord_].v_.i64_ = !!operands_i[0];
      break;
    case enum_type:
    case enum_cit:
      assert(0 && "Enumeration types should have pre-translated to Compatible Integer Type (CIT) at this point");
      break;
    case ptr:
      assert(0 && "Pointer types should have pre-translated to their uint equivalent at this point");
      break;
    case none:
      /* Ignore operand */
      break;
  }

  return 0;
}

int expr_eval(struct c_compiler *cc, struct expr_temp *result, struct expr *x, int is_constant_expr, struct decl *nulled_out_decl, void *local_base, void *param_base, void *return_value_ptr) {
  int r;
  if (!x) return -1;

#ifdef _WIN32
#define EXPR_ALLOCA _alloca
#else
#define EXPR_ALLOCA alloca
#endif
  struct expr_temp *temps = (struct expr_temp *)EXPR_ALLOCA((1 + x->num_ords_ /* ordinals are 1-based */) * sizeof(struct expr_temp));
  memset(temps, 0, (1 + x->num_ords_) * sizeof(struct expr_temp));
  r = expr_eval_impl(cc, x, temps, 0, is_constant_expr, nulled_out_decl, local_base, param_base, return_value_ptr);
  if (r) {
    return r;
  }
  memcpy(result, &temps[1], sizeof(*result));

  return 0;
}

int expr_constant_evaluation(struct c_compiler *cc, struct expr_temp *result, struct expr *x) {
  /* Constant expression evaluation occurs during compilation (parsing) prior to execution,
   * the expression is therefore not yet prepared for evaluation. Evaluate it here, knowing
   * full well that we may have to do it again. */
  expr_prepare(x);
  return expr_eval(cc, result, x, 1, NULL, NULL, NULL, NULL);
}

int expr_usual_arithmetic_conversions(struct c_compiler *cc, struct expr **left_expr, struct expr **right_expr) {
  enum type_kind tk_left, tk_right;
  struct type_node *type_left, *type_right;
  
  type_left = expr_type(cc, *left_expr);
  tk_left = type_left->kind_;
  if (tk_left == tk_enumeration) {
    /* enumeration's Compatible integer type */
    tk_left = type_left->derived_from_->kind_;
  }
  
  type_right = expr_type(cc, *right_expr);
  tk_right = type_right->kind_;
  if (tk_right == tk_enumeration) {
    /* enumeration's Compatible integer type */
    tk_right = type_right->derived_from_->kind_;
  }

  struct type_node *common_real_type = NULL;
  enum type_kind common_real_type_tk = tk_invalid;
  enum type_kind tk_left_real_type = type_kind_get_real_type(tk_left);
  enum type_kind tk_right_real_type = type_kind_get_real_type(tk_right);
  if ((tk_left_real_type == tk_long_double) || (tk_right_real_type == tk_long_double)) {
    common_real_type_tk = tk_long_double;
  }
  else if ((tk_left_real_type == tk_double) || (tk_right_real_type == tk_double)) {
    common_real_type_tk = tk_double;
  }
  else if ((tk_left_real_type == tk_float) || (tk_right_real_type == tk_float)) {
    common_real_type_tk = tk_float;
  }
  else {
    int r;
    r = expr_integer_promotions(cc, left_expr);
    if (r) return r;
    r = expr_integer_promotions(cc, right_expr);
    if (r) return r;

    /* Re-assess types, may still be enum if compatible integer type has rank exceeding int (unlikely but I see
     * no reason why it'd be impossible.) */
    type_left = expr_type(cc, *left_expr);
    tk_left = type_left->kind_;
    if (tk_left == tk_enumeration) {
      tk_left = type_left->derived_from_->kind_;
    }
    type_right = expr_type(cc, *right_expr);
    tk_right = type_right->kind_;
    if (tk_right == tk_enumeration) {
      tk_right = type_right->derived_from_->kind_;
    }

    int left_rank, left_is_signed;
    type_kind_classify_integer_type(tk_left, &left_is_signed, &left_rank);

    int right_rank, right_is_signed;
    type_kind_classify_integer_type(tk_right, &right_is_signed, &right_rank);

    if (tk_left == tk_right) {
      /* Operands have same type, no further action, we do still convert, to shed the enum type, if any */
      common_real_type_tk = tk_left;
    }
    else if (left_is_signed == right_is_signed) {
      /* If both operands have signed integer types, or both have unsigned integer types, the operand
       * with the type of lesser integer conversion rank is converted to the type of the operand
       * with greater rank. */
      if (left_rank > right_rank) {
        common_real_type_tk = tk_left;
      }
      else {
        common_real_type_tk = tk_right;
      }
    }
    else if ((left_rank >= right_rank) && !left_is_signed) {
      /* If the left rank is greater or equal to the right, and the left type is unsigned,
       * then convert the signed type of the right to the unsigned type of the left. */
      common_real_type_tk = tk_left;
    }
    else if ((right_rank >= left_rank) && !right_is_signed) {
      /* If the right rank is greater or equal to the left, and the right type is unsigned,
       * then convert the signed type of the left to the unsigned type of the right. */
      common_real_type_tk = tk_right;
    }
    else if ((left_rank > right_rank) && left_is_signed) {
      /* Operand with the signed integer type can represent all values of the type of the
       * operand with the unsigned integer type. Convert unsigned type to signed type. */
      /* XXX: Should this consider bitfields? With integer promotions, we consider bitfields
       *      for a similar scenario. Presumably the spec rhymes and we should consider
       *      bitfields here as well.. C99 6.3.1.8-1 */
      common_real_type_tk = tk_left;
    }
    else if ((right_rank > left_rank) && right_is_signed) {
      /* Same as above, but now signed left can encompass unsigned right */
      /* XXX: Should this consider bitfields? With integer promotions, we consider bitfields
      *      for a similar scenario. Presumably the spec rhymes and we should consider
      *      bitfields here as well.. C99 6.3.1.8-1 */
      common_real_type_tk = tk_right;
    }
    else {
      /* Otherwise, both operands are converted to the unsigned integer type corresponding to 
       * the type of the operand with signed integer type. */
      if (left_is_signed) common_real_type_tk = type_kind_get_unsigned_kind(tk_left);
      else /* (right_is_signed) */ common_real_type_tk = type_kind_get_unsigned_kind(tk_right);
    }
  }

  /* Perform the final conversions */

  struct expr *x = expr_convert_arithmetic_type(cc, type_base_simple(&cc->tb_, type_kind_get_type_in_domain(tk_left, common_real_type_tk)), *left_expr);
  if (!x) {
    cc_error(cc, "Internal error, failed to perform the usual arithmetic conversions\n");
    return -1;
  }
  *left_expr = x;
  x = expr_convert_arithmetic_type(cc, type_base_simple(&cc->tb_, type_kind_get_type_in_domain(tk_right, common_real_type_tk)), *right_expr);
  if (!x) {
    cc_error(cc, "Internal error, failed to perform the usual arithmetic conversions\n");
    return -1;
  }
  *right_expr = x;

  return 0;
}

enum type_kind expr_arith_type_kind_no_enum(struct c_compiler *cc, struct expr *x) {
  struct type_node *tn = expr_type(cc, x);
  if (!tn) return tk_invalid;
  return type_node_arith_type_kind_no_enum(&cc->tb_, tn);
}

int expr_mul(struct c_compiler *cc, struct expr **dst, struct situs *left_loc, struct expr **left, struct situs *op_loc, struct situs *right_loc, struct expr **right) {
  struct type_node *left_type, *right_type;
  if ((!*left) || (!*right)) {
    /* One of the sides is invalid. Simple pass through of an error */
    return 0;
  }
  left_type = expr_type(cc, *left);
  if (!left_type) {
    cc_error_loc(cc, left_loc, "cannot determine type of left operand");
    *dst = NULL;
    return -1;
  }
  right_type = expr_type(cc, *right);
  if (!right_type) {
    cc_error_loc(cc, right_loc, "cannot determine type of right operand");
    *dst = NULL;
    return -1;
  }
  int is_arith_left = type_node_is_arithmetic_type(left_type);
  int is_arith_right = type_node_is_arithmetic_type(right_type);
  if (!is_arith_left) {
    cc_error_loc(cc, left_loc, "left operand not an arithmetic type");
  }
  if (!is_arith_right) {
    cc_error_loc(cc, right_loc, "right operand not an arithmetic type");
  }

  if (!is_arith_left || !is_arith_right) {
    *dst = NULL;
    return 0;
  }

  if (expr_usual_arithmetic_conversions(cc, left, right)) {
    cc_error_loc(cc, op_loc, "failed to perform the usual arithmetic conversions");
    return 0;
  }

  enum type_kind tk_left, tk_right;
  tk_left = expr_arith_type_kind_no_enum(cc, *left);
  tk_right = expr_arith_type_kind_no_enum(cc, *right);

  enum expr_type opcode = ET_INVALID;
  int swapped = 0;
  switch (tk_left) {
  case tk_long_double_complex:
    if (tk_right == tk_long_double) {
      opcode = ET_MUL_LDC_LD;
    }
    else if (tk_right == tk_long_double_complex) {
      opcode = ET_MUL_LDC_LDC;
    }
    else if (tk_right == tk_long_double_imaginary) {
      opcode = ET_MUL_LDC_LDI;
    }
    break;
  case tk_double_complex:
    if (tk_right == tk_double) {
      opcode = ET_MUL_DC_D;
    }
    else if (tk_right == tk_double_complex) {
      opcode = ET_MUL_DC_DC;
    }
    else if (tk_right == tk_double_imaginary) {
      opcode = ET_MUL_DC_DI;
    }
    break;
  case tk_float_complex:
    if (tk_right == tk_float) {
      opcode = ET_MUL_FC_F;
    }
    else if (tk_right == tk_float_complex) {
      opcode = ET_MUL_FC_FC;
    }
    else if (tk_right == tk_float_imaginary) {
      opcode = ET_MUL_FC_FI;
    }
    break;
  case tk_long_double_imaginary:
    if (tk_right == tk_long_double) {
      opcode = ET_MUL_LDI_LD;
    }
    else if (tk_right == tk_long_double_complex) {
      opcode = ET_MUL_LDC_LDI;
      swapped = 1;
    }
    else if (tk_right == tk_long_double_imaginary) {
      opcode = ET_MUL_LDI_LDI;
    }
    break;
  case tk_double_imaginary:
    if (tk_right == tk_double) {
      opcode = ET_MUL_DI_D;
    }
    else if (tk_right == tk_double_complex) {
      opcode = ET_MUL_DC_DI;
      swapped = 1;
    }
    else if (tk_right == tk_double_imaginary) {
      opcode = ET_MUL_DI_DI;
    }
    break;
  case tk_float_imaginary:
    if (tk_right == tk_float) {
      opcode = ET_MUL_FI_F;
    }
    else if (tk_right == tk_float_complex) {
      opcode = ET_MUL_FC_FI;
      swapped = 1;
    }
    else if (tk_right == tk_float_imaginary) {
      opcode = ET_MUL_FI_FI;
    }
    break;
  case tk_long_double:
    if (tk_right == tk_long_double) {
      opcode = ET_MUL_LD;
    }
    else if (tk_right == tk_long_double_complex) {
      opcode = ET_MUL_LDC_LD;
      swapped = 1;
    }
    else if (tk_right == tk_long_double_imaginary) {
      opcode = ET_MUL_LDI_LD;
      swapped = 1;
    }
    break;
  case tk_double:
    if (tk_right == tk_double) {
      opcode = ET_MUL_D;
    }
    else if (tk_right == tk_double_complex) {
      opcode = ET_MUL_DC_D;
      swapped = 1;
    }
    else if (tk_right == tk_double_imaginary) {
      opcode = ET_MUL_DI_D;
      swapped = 1;
    }
    break;
  case tk_float:
    if (tk_right == tk_float) {
      opcode = ET_MUL_F;
    }
    else if (tk_right == tk_float_complex) {
      opcode = ET_MUL_FC_F;
      swapped = 1;
    }
    else if (tk_right == tk_float_imaginary) {
      opcode = ET_MUL_FI_F;
      swapped = 1;
    }
    break;
  case tk_long_long_int:
    opcode = ET_MUL_SLLI;
    break;
  case tk_long_int:
    opcode = ET_MUL_SLI;
    break;
  case tk_int:
    opcode = ET_MUL_SI;
    break;
  case tk_unsigned_long_long_int:
    opcode = ET_MUL_ULLI;
    break;
  case tk_unsigned_long_int:
    opcode = ET_MUL_ULI;
    break;
  case tk_unsigned_int:
    opcode = ET_MUL_UI;
    break;
  }
  if (opcode == ET_INVALID) {
    cc_error(cc, "Internal error: no matching opcode for operand types\n");
    return -1;
  }
  struct expr *x = expr_alloc(opcode);
  if (!x) {
    cc_no_memory(cc);
    return -1;
  }
  if (!swapped) {
    x->children_[0] = *left;
    x->children_[1] = *right;
  }
  else {
    x->children_[0] = *right;
    x->children_[1] = *left;
  }
  *left = NULL;
  *right = NULL;
  *dst = x;
  
  return 0;
}


int expr_div(struct c_compiler *cc, struct expr **dst, struct situs *left_loc, struct expr **left, struct situs *op_loc, struct situs *right_loc, struct expr **right) {
  struct type_node *left_type, *right_type;
  if ((!*left) || (!*right)) {
    /* One of the sides is invalid. Simple pass through of an error */
    return 0;
  }
  left_type = expr_type(cc, *left);
  if (!left_type) {
    cc_error_loc(cc, left_loc, "cannot determine type of left operand");
    *dst = NULL;
    return -1;
  }
  right_type = expr_type(cc, *right);
  if (!right_type) {
    cc_error_loc(cc, right_loc, "cannot determine type of right operand");
    *dst = NULL;
    return -1;
  }
  int is_arith_left = type_node_is_arithmetic_type(left_type);
  int is_arith_right = type_node_is_arithmetic_type(right_type);
  if (!is_arith_left) {
    cc_error_loc(cc, left_loc, "left operand not an arithmetic type");
  }
  if (!is_arith_right) {
    cc_error_loc(cc, right_loc, "right operand not an arithmetic type");
  }

  if (!is_arith_left || !is_arith_right) {
    *dst = NULL;
    return 0;
  }

  if (expr_usual_arithmetic_conversions(cc, left, right)) {
    cc_error_loc(cc, op_loc, "failed to perform the usual arithmetic conversions");
    return -1;
  }

  enum type_kind tk_left, tk_right;
  tk_left = expr_arith_type_kind_no_enum(cc, *left);
  tk_right = expr_arith_type_kind_no_enum(cc, *right);

  enum expr_type opcode = ET_INVALID;
  switch (tk_left) {
    case tk_long_double_complex:
      switch (tk_right) {
        case tk_long_double_complex:
          opcode = ET_DIV_LDC_LDC;
          break;
        case tk_long_double_imaginary:
          opcode = ET_DIV_LDC_LDI;
          break;
        case tk_long_double:
          opcode = ET_DIV_LDC_LD;
          break;
      }
      break;
    case tk_long_double_imaginary:
      switch (tk_right) {
        case tk_long_double_complex:
          opcode = ET_DIV_LDI_LDC;
          break;
        case tk_long_double_imaginary:
          opcode = ET_DIV_LDI_LDI;
          break;
        case tk_long_double:
          opcode = ET_DIV_LDI_LD;
          break;
      }
      break;
    case tk_double_complex:
      switch (tk_right) {
        case tk_double_complex:
          opcode = ET_DIV_DC_DC;
          break;
        case tk_double_imaginary:
          opcode = ET_DIV_DC_DI;
          break;
        case tk_double:
          opcode = ET_DIV_DC_D;
          break;
      }
      break;
    case tk_double_imaginary:
      switch (tk_right) {
        case tk_double_complex:
          opcode = ET_DIV_DI_DC;
          break;
        case tk_double_imaginary:
          opcode = ET_DIV_DI_DI;
          break;
        case tk_double:
          opcode = ET_DIV_DI_D;
          break;
      }
      break;
    case tk_float_complex:
      switch (tk_right) {
        case tk_float_complex:
          opcode = ET_DIV_FC_FC;
          break;
        case tk_float_imaginary:
          opcode = ET_DIV_FC_FI;
          break;
        case tk_float:
          opcode = ET_DIV_FC_F;
          break;
      }
      break;
    case tk_float_imaginary:
      switch (tk_right) {
        case tk_float_complex:
          opcode = ET_DIV_FI_FC;
          break;
        case tk_float_imaginary:
          opcode = ET_DIV_FI_FI;
          break;
        case tk_float:
          opcode = ET_DIV_FI_F;
          break;
      }
      break;
    case tk_long_double:
      switch (tk_right) {
        case tk_long_double_complex:
          opcode = ET_DIV_LD_LDC;
          break;
        case tk_long_double_imaginary:
          opcode = ET_DIV_LD_LDI;
          break;
        case tk_long_double:
          opcode = ET_DIV_LD;
          break;
      }
      break;
    case tk_double:
      switch (tk_right) {
        case tk_double_complex:
          opcode = ET_DIV_D_DC;
          break;
        case tk_double_imaginary:
          opcode = ET_DIV_D_DI;
          break;
        case tk_double:
          opcode = ET_DIV_D;
          break;
      }
      break;
    case tk_float:
      switch (tk_right) {
        case tk_float_complex:
          opcode = ET_DIV_F_FC;
          break;
        case tk_float_imaginary:
          opcode = ET_DIV_F_FI;
          break;
        case tk_float:
          opcode = ET_DIV_F;
          break;
      }
      break;
    case tk_long_long_int:
      opcode = ET_DIV_SLLI;
      break;
    case tk_long_int:
      opcode = ET_DIV_SLI;
      break;
    case tk_int:
      opcode = ET_DIV_SI;
      break;
    case tk_unsigned_long_long_int:
      opcode = ET_DIV_ULLI;
      break;
    case tk_unsigned_long_int:
      opcode = ET_DIV_ULI;
      break;
    case tk_unsigned_int:
      opcode = ET_DIV_UI;
      break;
  }
  if (opcode == ET_INVALID) {
    cc_fatal_loc(cc, op_loc, "Internal error: no matching div opcode for operand types");
    return -1;
  }
  struct expr *x = expr_alloc(opcode);
  if (!x) {
    cc_no_memory(cc);
    return -1;
  }
  x->children_[0] = *left;
  x->children_[1] = *right;
  *left = NULL;
  *right = NULL;
  *dst = x;

  return 0;
}


int expr_rem(struct c_compiler *cc, struct expr **dst, struct situs *left_loc, struct expr **left, struct situs *op_loc, struct situs *right_loc, struct expr **right) {
  struct type_node *left_type, *right_type;
  if ((!*left) || (!*right)) {
    /* One of the sides is invalid. Simple pass through of an error */
    return 0;
  }
  left_type = expr_type(cc, *left);
  if (!left_type) {
    cc_error_loc(cc, left_loc, "cannot determine type of left operand");
    *dst = NULL;
    return -1;
  }
  right_type = expr_type(cc, *right);
  if (!right_type) {
    cc_error_loc(cc, right_loc, "cannot determine type of right operand");
    *dst = NULL;
    return -1;
  }
  int is_integer_left = type_node_is_integer_type(left_type);
  int is_integer_right = type_node_is_integer_type(right_type);
  if (!is_integer_left) {
    cc_error_loc(cc, left_loc, "left operand not an integer type");
  }
  if (!is_integer_right) {
    cc_error_loc(cc, right_loc, "right operand not an integer type");
  }

  if (!is_integer_left || !is_integer_right) {
    *dst = NULL;
    return 0;
  }

  if (expr_usual_arithmetic_conversions(cc, left, right)) {
    cc_error_loc(cc, op_loc, "failed to perform the usual arithmetic conversions");
    return -1;
  }

  enum type_kind tk_left, tk_right;
  tk_left = expr_arith_type_kind_no_enum(cc, *left);
  tk_right = expr_arith_type_kind_no_enum(cc, *right);

  enum expr_type opcode = ET_INVALID;
  switch (tk_left) {
    case tk_long_long_int:
      opcode = ET_REM_SLLI;
      break;
    case tk_long_int:
      opcode = ET_REM_SLI;
      break;
    case tk_int:
      opcode = ET_REM_SI;
      break;
    case tk_unsigned_long_long_int:
      opcode = ET_REM_ULLI;
      break;
    case tk_unsigned_long_int:
      opcode = ET_REM_ULI;
      break;
    case tk_unsigned_int:
      opcode = ET_REM_UI;
      break;
  }
  if (opcode == ET_INVALID) {
    cc_fatal_loc(cc, op_loc, "Internal error : no matching remainder opcode for operand types");
    return -1;
  }
  struct expr *x = expr_alloc(opcode);
  if (!x) {
    cc_no_memory(cc);
    return -1;
  }

  x->children_[0] = *left;
  x->children_[1] = *right;
  *left = NULL;
  *right = NULL;
  *dst = x;

  return 0;
}

int expr_shift_left(struct c_compiler *cc, struct expr **dst, struct situs *left_loc, struct expr **left, struct situs *op_loc, struct situs *right_loc, struct expr **right) {
  struct type_node *left_type, *right_type;
  if ((!*left) || (!*right)) {
    /* One of the sides is invalid. Simple pass through of an error */
    return 0;
  }
  left_type = expr_type(cc, *left);
  if (!left_type) {
    cc_error_loc(cc, left_loc, "cannot determine type of left operand");
    *dst = NULL;
    return -1;
  }
  right_type = expr_type(cc, *right);
  if (!right_type) {
    cc_error_loc(cc, right_loc, "cannot determine type of right operand");
    *dst = NULL;
    return -1;
  }
  int is_integer_left = type_node_is_integer_type(left_type);
  int is_integer_right = type_node_is_integer_type(right_type);
  if (!is_integer_left) {
    cc_error_loc(cc, left_loc, "left operand not an integer type");
  }
  if (!is_integer_right) {
    cc_error_loc(cc, right_loc, "right operand not an integer type");
  }

  if (!is_integer_left || !is_integer_right) {
    *dst = NULL;
    return -1;
  }

  if (expr_integer_promotions(cc, left)) {
    cc_error_loc(cc, left_loc, "failed to perform the integer promotions");
    return -1;
  }

  enum type_kind tk_left, tk_right;
  tk_left = expr_arith_type_kind_no_enum(cc, *left);
  tk_right = expr_arith_type_kind_no_enum(cc, *right);

  enum expr_type opcode = ET_INVALID;
  /* opcode is defined by integer type of left operand, the right operand can be any integer type */
  switch (tk_left) {
    case tk_long_long_int:
      opcode = ET_SHIFT_LEFT_SLLI;
      break;
    case tk_long_int:
      opcode = ET_SHIFT_LEFT_SLI;
      break;
    case tk_int:
      opcode = ET_SHIFT_LEFT_SI;
      break;
    case tk_unsigned_long_long_int:
      opcode = ET_SHIFT_LEFT_ULLI;
      break;
    case tk_unsigned_long_int:
      opcode = ET_SHIFT_LEFT_ULI;
      break;
    case tk_unsigned_int:
      opcode = ET_SHIFT_LEFT_UI;
      break;
  }
  if (opcode == ET_INVALID) {
    cc_fatal_loc(cc, op_loc, "Internal error: no matching shift-left opcode for operand types");
    return -1;
  }
  struct expr *x = expr_alloc(opcode);
  if (!x) {
    return -1;
  }
  x->children_[0] = *left;
  x->children_[1] = *right;
  *left = NULL;
  *right = NULL;
  *dst = x;

  return 0;
}

int expr_shift_right(struct c_compiler *cc, struct expr **dst, struct situs *left_loc, struct expr **left, struct situs *op_loc, struct situs *right_loc, struct expr **right) {
  struct type_node *left_type, *right_type;
  if ((!*left) || (!*right)) {
    /* One of the sides is invalid. Simple pass through of an error */
    return 0;
  }
  left_type = expr_type(cc, *left);
  if (!left_type) {
    cc_error_loc(cc, left_loc, "cannot determine type of left operand");
    *dst = NULL;
    return -1;
  }
  right_type = expr_type(cc, *right);
  if (!right_type) {
    cc_error_loc(cc, right_loc, "cannot determine type of right operand");
    *dst = NULL;
    return -1;
  }
  int is_integer_left = type_node_is_integer_type(left_type);
  int is_integer_right = type_node_is_integer_type(right_type);
  if (!is_integer_left) {
    cc_error_loc(cc, left_loc, "left operand not an integer type");
  }
  if (!is_integer_right) {
    cc_error_loc(cc, right_loc, "right operand not an integer type");
  }

  if (!is_integer_left || !is_integer_right) {
    *dst = NULL;
    return -1;
  }

  if (expr_integer_promotions(cc, left)) {
    cc_error_loc(cc, left_loc, "failed to perform the integer promotions");
    return -1;
  }

  enum type_kind tk_left, tk_right;
  tk_left = expr_arith_type_kind_no_enum(cc, *left);
  tk_right = expr_arith_type_kind_no_enum(cc, *right);

  enum expr_type opcode = ET_INVALID;
  /* opcode is defined by integer type of left operand, the right operand can be any integer type */
  switch (tk_left) {
    case tk_long_long_int:
      opcode = ET_SHIFT_RIGHT_SLLI;
      break;
    case tk_long_int:
      opcode = ET_SHIFT_RIGHT_SLI;
      break;
    case tk_int:
      opcode = ET_SHIFT_RIGHT_SI;
      break;
    case tk_unsigned_long_long_int:
      opcode = ET_SHIFT_RIGHT_ULLI;
      break;
    case tk_unsigned_long_int:
      opcode = ET_SHIFT_RIGHT_ULI;
      break;
    case tk_unsigned_int:
      opcode = ET_SHIFT_RIGHT_UI;
      break;
  }
  if (opcode == ET_INVALID) {
    cc_error_loc(cc, op_loc, "Internal error: no matching shift-right opcode for operand types");
    return -1;
  }
  struct expr *x = expr_alloc(opcode);
  if (!x) {
    return -1;
  }
  x->children_[0] = *left;
  x->children_[1] = *right;
  *left = NULL;
  *right = NULL;
  *dst = x;

  return 0;
}

static int expr_rel_op(struct c_compiler *cc, struct expr **dst, struct situs *left_loc, struct expr **left, struct situs *op_loc, struct situs *right_loc, struct expr **right,
                       enum expr_type op_ptr, enum expr_type op_ld, enum expr_type op_d, enum expr_type op_f, enum expr_type op_slli, enum expr_type op_sli, enum expr_type op_si, enum expr_type op_ulli, enum expr_type op_uli, enum expr_type op_ui) {
  struct type_node *left_type, *right_type;

  if ((!*left) || (!*right)) {
    /* One of the sides is invalid. Simple pass through of an error */
    return 0;
  }

  int r;
  if ((r = expr_pointer_decay(cc, left)) || (r = expr_pointer_decay(cc, right))) {
    return r;
  }

  left_type = expr_type(cc, *left);
  if (!left_type) {
    cc_error_loc(cc, left_loc, "cannot determine type of left operand");
    *dst = NULL;
    return -1;
  }
  right_type = expr_type(cc, *right);
  if (!right_type) {
    cc_error_loc(cc, right_loc, "cannot determine type of right operand");
    *dst = NULL;
    return -1;
  }
  right_type = type_node_unqualified(right_type);
  left_type = type_node_unqualified(left_type);
  if ((left_type->kind_ == tk_pointer) && (right_type->kind_ == tk_pointer)) {
    if (!type_node_is_compatible(left_type->derived_from_, right_type->derived_from_)) {
      struct situs sit;
      situs_init(&sit);
      if (situs_concat(&sit, left_loc) || situs_concat(&sit, op_loc) || situs_concat(&sit, right_loc)) {
        cc_no_memory(cc);
        situs_cleanup(&sit);
        return -1;
      }
      cc_error_loc(cc, &sit, "left and right operands have incompatible types");
      situs_cleanup(&sit);
      *dst = NULL;
      return -1;
    }
    struct expr *x = expr_alloc(op_ptr);
    if (!x) {
      return -1;
    }
    x->children_[0] = *left;
    x->children_[1] = *right;
    *left = NULL;
    *right = NULL;
    *dst = x;
    return 0;
  }
  else if (type_node_is_real_type(left_type) && type_node_is_real_type(right_type)) {
    if (expr_usual_arithmetic_conversions(cc, left, right)) {
      cc_error_loc(cc, op_loc, "failed to perform the usual arithmetic conversions");
      return -1;
    }

    enum type_kind tk_left, tk_right;
    tk_left = expr_arith_type_kind_no_enum(cc, *left);
    tk_right = expr_arith_type_kind_no_enum(cc, *right);
    assert(tk_left == tk_right);
    enum expr_type opcode = ET_INVALID;
    /* opcode is defined by integer type of left operand, the right operand can be any integer type */
    switch (tk_left) {
      case tk_long_double:
        opcode = op_ld;
        break;
      case tk_double:
        opcode = op_d;
        break;
      case tk_float:
        opcode = op_f;
        break;
      case tk_long_long_int:
        opcode = op_slli;
        break;
      case tk_long_int:
        opcode = op_sli;
        break;
      case tk_int:
        opcode = op_si;
        break;
      case tk_unsigned_long_long_int:
        opcode = op_ulli;
        break;
      case tk_unsigned_long_int:
        opcode = op_uli;
        break;
      case tk_unsigned_int:
        opcode = op_ui;
        break;
    }
    if (opcode == ET_INVALID) {
      cc_fatal_loc(cc, op_loc, "Internal error: no matching relational opcode for operand types");
      return -1;
    }
    struct expr *x = expr_alloc(opcode);
    if (!x) {
      return -1;
    }
    x->children_[0] = *left;
    x->children_[1] = *right;
    *left = NULL;
    *right = NULL;
    *dst = x;
    return 0;
  }
  else {
    struct situs sit;
    situs_init(&sit);
    if (situs_concat(&sit, left_loc) || situs_concat(&sit, op_loc) || situs_concat(&sit, right_loc)) {
      cc_no_memory(cc);
      situs_cleanup(&sit);
      return -1;
    }
    cc_error_loc(cc, &sit, "left and right operands have incompatible types");
    situs_cleanup(&sit);

    return -1;
  }
}

int expr_less_than(struct c_compiler *cc, struct expr **dst, struct situs *left_loc, struct expr **left, struct situs *op_loc, struct situs *right_loc, struct expr **right) {  
  return expr_rel_op(cc, dst, left_loc, left, op_loc, right_loc, right,
                     ET_LESS_THAN_PTR, ET_LESS_THAN_LD, ET_LESS_THAN_D, ET_LESS_THAN_F, ET_LESS_THAN_SLLI, ET_LESS_THAN_SLI, ET_LESS_THAN_SI, ET_LESS_THAN_ULLI, ET_LESS_THAN_ULI, ET_LESS_THAN_UI);
}

int expr_greater_than(struct c_compiler *cc, struct expr **dst, struct situs *left_loc, struct expr **left, struct situs *op_loc, struct situs *right_loc, struct expr **right) {  
  return expr_rel_op(cc, dst, left_loc, left, op_loc, right_loc, right,
                     ET_GREATER_THAN_PTR, ET_GREATER_THAN_LD, ET_GREATER_THAN_D, ET_GREATER_THAN_F, ET_GREATER_THAN_SLLI, ET_GREATER_THAN_SLI, ET_GREATER_THAN_SI, ET_GREATER_THAN_ULLI, ET_GREATER_THAN_ULI, ET_GREATER_THAN_UI);
}

int expr_less_than_or_equal(struct c_compiler *cc, struct expr **dst, struct situs *left_loc, struct expr **left, struct situs *op_loc, struct situs *right_loc, struct expr **right) {  
  return expr_rel_op(cc, dst, left_loc, left, op_loc, right_loc, right,
                     ET_LESS_THAN_OR_EQUAL_PTR, ET_LESS_THAN_OR_EQUAL_LD, ET_LESS_THAN_OR_EQUAL_D, ET_LESS_THAN_OR_EQUAL_F, ET_LESS_THAN_OR_EQUAL_SLLI, ET_LESS_THAN_OR_EQUAL_SLI, ET_LESS_THAN_OR_EQUAL_SI, ET_LESS_THAN_OR_EQUAL_ULLI, ET_LESS_THAN_OR_EQUAL_ULI, ET_LESS_THAN_OR_EQUAL_UI);
}

int expr_greater_than_or_equal(struct c_compiler *cc, struct expr **dst, struct situs *left_loc, struct expr **left, struct situs *op_loc, struct situs *right_loc, struct expr **right) {  
  return expr_rel_op(cc, dst, left_loc, left, op_loc, right_loc, right,
                     ET_GREATER_THAN_OR_EQUAL_PTR, ET_GREATER_THAN_OR_EQUAL_LD, ET_GREATER_THAN_OR_EQUAL_D, ET_GREATER_THAN_OR_EQUAL_F,  ET_GREATER_THAN_OR_EQUAL_SLLI, ET_GREATER_THAN_OR_EQUAL_SLI, ET_GREATER_THAN_OR_EQUAL_SI, ET_GREATER_THAN_OR_EQUAL_ULLI, ET_GREATER_THAN_OR_EQUAL_ULI, ET_GREATER_THAN_OR_EQUAL_UI);
}


static int expr_bitwise_binop(struct c_compiler *cc, struct expr **dst, struct situs *left_loc, struct expr **left, struct situs *op_loc, struct situs *right_loc, struct expr **right,
                              enum expr_type op_slli, enum expr_type op_sli, enum expr_type op_si, enum expr_type op_ulli, enum expr_type op_uli, enum expr_type op_ui) {
  struct type_node *left_type, *right_type;
  /* XXX: DEQUALIFY OR THIS WILL ALL FAIL  */
  if ((!*left) || (!*right)) {
    /* One of the sides is invalid. Simple pass through of an error */
    return 0;
  }
  left_type = expr_type(cc, *left);
  if (!left_type) {
    cc_error_loc(cc, left_loc, "cannot determine type of left operand");
    *dst = NULL;
    return -1;
  }
  right_type = expr_type(cc, *right);
  if (!right_type) {
    cc_error_loc(cc, right_loc, "cannot determine type of right operand");
    *dst = NULL;
    return -1;
  }

  int is_integer_left = type_node_is_integer_type(left_type);
  int is_integer_right = type_node_is_integer_type(right_type);

  if (!is_integer_left || !is_integer_right) {
    if (!is_integer_left) {
      cc_error_loc(cc, left_loc, "bitwise-and requires integer type");
    }
    if (!is_integer_right) {
      cc_error_loc(cc, right_loc, "bitwise-and requires integer type");
    }
    return -1;
  }

  if (!expr_usual_arithmetic_conversions(cc, left, right)) {
    enum type_kind tk_left, tk_right;
    tk_left = expr_arith_type_kind_no_enum(cc, *left);
    tk_right = expr_arith_type_kind_no_enum(cc, *right);
    assert(tk_left == tk_right && "Integer types must always be identical after usual arithmetic conversions");

    enum expr_type opcode = ET_INVALID;
    switch (tk_left) {
      case tk_long_long_int:
        opcode = op_slli;
        break;
      case tk_long_int:
        opcode = op_sli;
        break;
      case tk_int:
        opcode = op_si;
        break;
      case tk_unsigned_long_long_int:
        opcode = op_ulli;
        break;
      case tk_unsigned_long_int:
        opcode = op_uli;
        break;
      case tk_unsigned_int:
        opcode = op_ui;
        break;
    }
    if (opcode == ET_INVALID) {
      cc_fatal_loc(cc, op_loc, "Internal error: no matching opcode for operand types");
      return -1;
    }
    struct expr *x = expr_alloc(opcode);
    if (!x) {
      return -1;
    }
    x->children_[0] = *left;
    x->children_[1] = *right;
    *left = NULL;
    *right = NULL;
    *dst = x;
    return 0;
  }
  else {
    cc_error_loc(cc, op_loc, "failed to perform the usual arithmetic conversions");
    return -1;
  }
}

int expr_bitwise_and(struct c_compiler *cc, struct expr **dst, struct situs *left_loc, struct expr **left, struct situs *op_loc, struct situs *right_loc, struct expr **right) {
  return expr_bitwise_binop(cc, dst, left_loc, left, op_loc, right_loc, right,
                            ET_BITWISE_AND_SLLI,
                            ET_BITWISE_AND_SLI,
                            ET_BITWISE_AND_SI,
                            ET_BITWISE_AND_ULLI,
                            ET_BITWISE_AND_ULI,
                            ET_BITWISE_AND_UI);
}

int expr_bitwise_or(struct c_compiler *cc, struct expr **dst, struct situs *left_loc, struct expr **left, struct situs *op_loc, struct situs *right_loc, struct expr **right) {
  return expr_bitwise_binop(cc, dst, left_loc, left, op_loc, right_loc, right,
                            ET_BITWISE_OR_SLLI,
                            ET_BITWISE_OR_SLI,
                            ET_BITWISE_OR_SI,
                            ET_BITWISE_OR_ULLI,
                            ET_BITWISE_OR_ULI,
                            ET_BITWISE_OR_UI);
}

int expr_bitwise_xor(struct c_compiler *cc, struct expr **dst, struct situs *left_loc, struct expr **left, struct situs *op_loc, struct situs *right_loc, struct expr **right) {
  return expr_bitwise_binop(cc, dst, left_loc, left, op_loc, right_loc, right,
                            ET_BITWISE_XOR_SLLI,
                            ET_BITWISE_XOR_SLI,
                            ET_BITWISE_XOR_SI,
                            ET_BITWISE_XOR_ULLI,
                            ET_BITWISE_XOR_ULI,
                            ET_BITWISE_XOR_UI);
}

int expr_add(struct c_compiler *cc, struct expr **dst, struct situs *left_loc, struct expr **left, struct situs *op_loc, struct situs *right_loc, struct expr **right) {
  struct type_node *left_type, *right_type;
  /* XXX: DEQUALIFY OR THIS WILL ALL FAIL  */
  if ((!*left) || (!*right)) {
    /* One of the sides is invalid. Simple pass through of an error */
    return 0;
  }
  int r;
  if ((r = expr_pointer_decay(cc, left)) || (r = expr_pointer_decay(cc, right))) {
    return r;
  }

  left_type = expr_type(cc, *left);
  if (!left_type) {
    cc_error_loc(cc, left_loc, "cannot determine type of left operand");
    *dst = NULL;
    return -1;
  }
  right_type = expr_type(cc, *right);
  if (!right_type) {
    cc_error_loc(cc, right_loc, "cannot determine type of right operand");
    *dst = NULL;
    return -1;
  }

  int is_arith_left = type_node_is_arithmetic_type(left_type);
  int is_arith_right = type_node_is_arithmetic_type(right_type);
  int is_integer_left = type_node_is_integer_type(left_type);
  int is_integer_right = type_node_is_integer_type(right_type);

  if (is_arith_left && is_arith_right) {
    if (!expr_usual_arithmetic_conversions(cc, left, right)) {
      enum type_kind tk_left, tk_right;
      tk_left = expr_arith_type_kind_no_enum(cc, *left);
      tk_right = expr_arith_type_kind_no_enum(cc, *right);

      enum expr_type opcode = ET_INVALID;
      int swapped = 0;
      switch (tk_left) {
      case tk_long_double_complex:
        if (tk_right == tk_long_double) {
          opcode = ET_ADD_LDC_LD;
        }
        else if (tk_right == tk_long_double_complex) {
          opcode = ET_ADD_LDC_LDC;
        }
        else if (tk_right == tk_long_double_imaginary) {
          opcode = ET_ADD_LDC_LDI;
        }
        break;
      case tk_double_complex:
        if (tk_right == tk_double) {
          opcode = ET_ADD_DC_D;
        }
        else if (tk_right == tk_double_complex) {
          opcode = ET_ADD_DC_DC;
        }
        else if (tk_right == tk_double_imaginary) {
          opcode = ET_ADD_DC_DI;
        }
        break;
      case tk_float_complex:
        if (tk_right == tk_float) {
          opcode = ET_ADD_FC_F;
        }
        else if (tk_right == tk_float_complex) {
          opcode = ET_ADD_FC_FC;
        }
        else if (tk_right == tk_float_imaginary) {
          opcode = ET_ADD_FC_FI;
        }
        break;
      case tk_long_double_imaginary:
        if (tk_right == tk_long_double) {
          opcode = ET_ADD_LDI_LD;
        }
        else if (tk_right == tk_long_double_complex) {
          opcode = ET_ADD_LDC_LDI;
          swapped = 1;
        }
        else if (tk_right == tk_long_double_imaginary) {
          opcode = ET_ADD_LDI_LDI;
        }
        break;
      case tk_double_imaginary:
        if (tk_right == tk_double) {
          opcode = ET_ADD_DI_D;
        }
        else if (tk_right == tk_double_complex) {
          opcode = ET_ADD_DC_DI;
          swapped = 1;
        }
        else if (tk_right == tk_double_imaginary) {
          opcode = ET_ADD_DI_DI;
        }
        break;
      case tk_float_imaginary:
        if (tk_right == tk_float) {
          opcode = ET_ADD_FI_F;
        }
        else if (tk_right == tk_float_complex) {
          opcode = ET_ADD_FC_FI;
          swapped = 1;
        }
        else if (tk_right == tk_float_imaginary) {
          opcode = ET_ADD_FI_FI;
        }
        break;
      case tk_long_double:
        if (tk_right == tk_long_double) {
          opcode = ET_ADD_LD;
        }
        else if (tk_right == tk_long_double_complex) {
          opcode = ET_ADD_LDC_LD;
          swapped = 1;
        }
        else if (tk_right == tk_long_double_imaginary) {
          opcode = ET_ADD_LDI_LD;
          swapped = 1;
        }
        break;
      case tk_double:
        if (tk_right == tk_double) {
          opcode = ET_ADD_D;
        }
        else if (tk_right == tk_double_complex) {
          opcode = ET_ADD_DC_D;
          swapped = 1;
        }
        else if (tk_right == tk_double_imaginary) {
          opcode = ET_ADD_DI_D;
          swapped = 1;
        }
        break;
      case tk_float:
        if (tk_right == tk_float) {
          opcode = ET_ADD_F;
        }
        else if (tk_right == tk_float_complex) {
          opcode = ET_ADD_FC_F;
          swapped = 1;
        }
        else if (tk_right == tk_float_imaginary) {
          opcode = ET_ADD_FI_F;
          swapped = 1;
        }
        break;
      case tk_long_long_int:
        opcode = ET_ADD_SLLI;
        break;
      case tk_long_int:
        opcode = ET_ADD_SLI;
        break;
      case tk_int:
        opcode = ET_ADD_SI;
        break;
      case tk_unsigned_long_long_int:
        opcode = ET_ADD_ULLI;
        break;
      case tk_unsigned_long_int:
        opcode = ET_ADD_ULI;
        break;
      case tk_unsigned_int:
        opcode = ET_ADD_UI;
        break;
      }
      if (opcode == ET_INVALID) {
        cc_fatal_loc(cc, op_loc, "Internal error: no matching add opcode for operand types");
        return -1;
      }
      struct expr *x = expr_alloc(opcode);
      if (!x) {
        return -1;
      }
      if (!swapped) {
        x->children_[0] = *left;
        x->children_[1] = *right;
      }
      else {
        x->children_[0] = *right;
        x->children_[1] = *left;
      }
      *left = NULL;
      *right = NULL;
      *dst = x;
      return 0;
    }
    else {
      cc_error_loc(cc, op_loc, "failed to perform the usual arithmetic conversions");
      return -1;
    }
  }
  else if (is_integer_left || is_integer_right) {
    struct expr *index, *pointer;
    struct situs *index_loc;
    struct type_node *pointer_type;
    if (right_type->kind_ == tk_pointer) {
      index = *left;
      index_loc = left_loc;
      pointer = *right;
      pointer_type = right_type;
    }
    else if (left_type->kind_ == tk_pointer) {
      index = *right;
      index_loc = right_loc;
      pointer = *left;
      pointer_type = left_type;
    }
    else {
      struct situs sit;
      situs_init(&sit);
      if (situs_concat(&sit, left_loc) || situs_concat(&sit, op_loc) || situs_concat(&sit, right_loc)) {
        cc_no_memory(cc);
        situs_cleanup(&sit);
        return -1;
      }
      cc_error_loc(cc, &sit, "types mismatch for addition, must be either both arithmetic, or pointer and an integer");
      situs_cleanup(&sit);
      return -1;
    }

    enum expr_type et_and, et_add, et_sub, et_c, et_mul, et_lt;
    type_node_uintptr_expr(&cc->tb_, &et_and, &et_add, &et_sub, &et_c, &et_mul, &et_lt);

    struct expr *index_expr = expr_convert_arithmetic_type(cc, type_base_simple(&cc->tb_, cc->tb_.uintptr_equivalent_), index);
    if (!index_expr) {
      cc_error_loc(cc, index_loc, "unable to convert index type to pointer sized integer");
      return -1;
    }

    struct expr *x = expr_alloc(ET_ADD_PTR_UINTPTR);
    struct expr *size_expr = type_node_size_expr(cc, pointer_type->derived_from_);
    struct expr *offset_expr = expr_alloc(et_mul);
    if (!x || !offset_expr || !size_expr) {
      expr_free(x);
      expr_free(size_expr);
      expr_free(index_expr);
      expr_free(offset_expr);
      cc_no_memory(cc);
      return -1;
    }
    offset_expr->children_[0] = size_expr;
    offset_expr->children_[1] = index_expr;

    x->children_[0] = pointer;
    x->children_[1] = offset_expr;
    *left = NULL;
    *right = NULL;
    *dst = x;
    return 0;
  }
  else {
    cc_error_loc(cc, op_loc, "types mismatch for addition, must be either both arithmetic, or pointer and an integer");
  }

  return -1;
}


int expr_sub(struct c_compiler *cc, struct expr **dst, struct situs *left_loc, struct expr **left, struct situs *op_loc, struct situs *right_loc, struct expr **right) {
  struct type_node *left_type, *right_type;
  /* XXX: DEQUALIFY OR THIS WILL ALL FAIL  */
  if ((!*left) || (!*right)) {
    /* One of the sides is invalid. Simple pass through of an error */
    return 0;
  }
  int r;
  if ((r = expr_pointer_decay(cc, left)) || (r = expr_pointer_decay(cc, right))) {
    return r;
  }

  left_type = expr_type(cc, *left);
  if (!left_type) {
    cc_error_loc(cc, left_loc, "cannot determine type of left operand\n");
    *dst = NULL;
    return -1;
  }
  right_type = expr_type(cc, *right);
  if (!right_type) {
    cc_error_loc(cc, right_loc, "cannot determine type of right operand\n");
    *dst = NULL;
    return -1;
  }

  int is_arith_left = type_node_is_arithmetic_type(left_type);
  int is_arith_right = type_node_is_arithmetic_type(right_type);
  int is_integer_left = type_node_is_integer_type(left_type);
  int is_integer_right = type_node_is_integer_type(right_type);

  if (is_arith_left && is_arith_right) {
    if (!expr_usual_arithmetic_conversions(cc, left, right)) {
      enum type_kind tk_left, tk_right;
      tk_left = expr_arith_type_kind_no_enum(cc, *left);
      tk_right = expr_arith_type_kind_no_enum(cc, *right);

      enum expr_type opcode = ET_INVALID;
      int swapped = 0;
      switch (tk_left) {
        case tk_long_double_complex:
          switch (tk_right) {
            case tk_long_double_complex:
              opcode = ET_SUB_LDC_LDC;
              break;
            case tk_long_double_imaginary:
              opcode = ET_SUB_LDC_LDI;
              break;
            case tk_long_double:
              opcode = ET_SUB_LDC_LD;
              break;
          }
          break;
        case tk_long_double_imaginary:
          switch (tk_right) {
            case tk_long_double_complex:
              opcode = ET_SUB_LDI_LDC;
              break;
            case tk_long_double_imaginary:
              opcode = ET_SUB_LDI_LDI;
              break;
            case tk_long_double:
              opcode = ET_SUB_LDI_LD;
              break;
          }
          break;
        case tk_double_complex:
          switch (tk_right) {
            case tk_double_complex:
              opcode = ET_SUB_DC_DC;
              break;
            case tk_double_imaginary:
              opcode = ET_SUB_DC_DI;
              break;
            case tk_double:
              opcode = ET_SUB_DC_D;
              break;
          }
          break;
        case tk_double_imaginary:
          switch (tk_right) {
            case tk_double_complex:
              opcode = ET_SUB_DI_DC;
              break;
            case tk_double_imaginary:
              opcode = ET_SUB_DI_DI;
              break;
            case tk_double:
              opcode = ET_SUB_DI_D;
              break;
          }
          break;
        case tk_float_complex:
          switch (tk_right) {
            case tk_float_complex:
              opcode = ET_SUB_FC_FC;
              break;
            case tk_float_imaginary:
              opcode = ET_SUB_FC_FI;
              break;
            case tk_float:
              opcode = ET_SUB_FC_F;
              break;
          }
          break;
        case tk_float_imaginary:
          switch (tk_right) {
            case tk_float_complex:
              opcode = ET_SUB_FI_FC;
              break;
            case tk_float_imaginary:
              opcode = ET_SUB_FI_FI;
              break;
            case tk_float:
              opcode = ET_SUB_FI_F;
              break;
          }
          break;
        case tk_long_double:
          switch (tk_right) {
            case tk_long_double_complex:
              opcode = ET_SUB_LD_LDC;
              break;
            case tk_long_double_imaginary:
              opcode = ET_SUB_LD_LDI;
              break;
            case tk_long_double:
              opcode = ET_SUB_LD;
              break;
          }
          break;
        case tk_double:
          switch (tk_right) {
            case tk_double_complex:
              opcode = ET_SUB_D_DC;
              break;
            case tk_double_imaginary:
              opcode = ET_SUB_D_DI;
              break;
            case tk_double:
              opcode = ET_SUB_D;
              break;
          }
          break;
        case tk_float:
          switch (tk_right) {
            case tk_float_complex:
              opcode = ET_SUB_F_FC;
              break;
            case tk_float_imaginary:
              opcode = ET_SUB_F_FI;
              break;
            case tk_float:
              opcode = ET_SUB_F;
              break;
          }
          break;
        case tk_long_long_int:
          opcode = ET_SUB_SLLI;
          break;
        case tk_long_int:
          opcode = ET_SUB_SLI;
          break;
        case tk_int:
          opcode = ET_SUB_SI;
          break;
        case tk_unsigned_long_long_int:
          opcode = ET_SUB_ULLI;
          break;
        case tk_unsigned_long_int:
          opcode = ET_SUB_ULI;
          break;
        case tk_unsigned_int:
          opcode = ET_SUB_UI;
          break;
      }
      if (opcode == ET_INVALID) {
        cc_fatal_loc(cc, left_loc, "Internal error: no matching sub opcode for operand types");
        return -1;
      }
      struct expr *x = expr_alloc(opcode);
      if (!x) {
        return -1;
      }
      x->children_[0] = *left;
      x->children_[1] = *right;
      *left = NULL;
      *right = NULL;
      *dst = x;
      return 0;
    }
    else {
      cc_error_loc(cc, op_loc, "failed to perform the usual arithmetic conversions");
      return -1;
    }
  }
  else if ((left_type->kind_ == tk_pointer) && (right_type->kind_ == tk_pointer)) {
    /* Check they are compatible object types */
    /* XXX: Compatible object types does not mean *identical* object types WORK THIS */
    struct type_node *void_type = type_base_simple(&cc->tb_, tk_void);
    if ((left_type->derived_from_ == void_type) || (right_type->derived_from_ == void_type)) {
      cc_error_loc(cc, op_loc, "subtraction of void pointers not possible");
      return -1;
    }
    if (left_type->derived_from_ == right_type->derived_from_) {
      struct expr *x = expr_alloc(ET_SUB_PTR);
      if (!x) {
        return -1;
      }
      struct expr *size_expr = type_node_size_expr(cc, left_type->derived_from_);
      if (!size_expr) {
        expr_free(x);
        return -1;
      }
      x->children_[0] = *left;
      x->children_[1] = *right;
      x->children_[2] = size_expr;
      *left = NULL;
      *right = NULL;
      *dst = x;
      return 0;
    }
    else {
      cc_error_loc(cc, op_loc, "subtraction of pointers that are incompatible");
      return -1;
    }
  }
  else if ((left_type->kind_ == tk_pointer) && is_integer_right) {
    struct expr *x = expr_alloc(ET_SUB_PTR_UINTPTR);
    if (!x) {
      cc_no_memory(cc);
      return -1;
    }

    struct expr *index_expr = expr_convert_arithmetic_type(cc, type_base_simple(&cc->tb_, cc->tb_.uintptr_equivalent_), *right);
    if (!index_expr) {
      cc_error_loc(cc, right_loc, "unable to convert index type to pointer sized integer");
      expr_free(x);
      return -1;
    }

    x->children_[0] = *left;
    x->children_[1] = index_expr;
    *left = NULL;
    *right = NULL;
    *dst = x;
    return 0;
  }
  else {
    cc_error_loc(cc, op_loc, "types mismatch for addition, must be either both arithmetic, or pointer and an integer");
    return -1;
  }
  return -1;
}


int expr_unary_plus(struct c_compiler *cc, struct expr **dst, struct situs *op_loc, struct situs *opd_loc, struct expr **operand) {
  if (!*operand) {
    /* One of the sides is invalid. Simple pass through of an error */
    return -1;
  }
  struct type_node *opd_type = expr_type(cc, *operand);

  if (!opd_type) {
    cc_error_loc(cc, opd_loc, "cannot determine type of operand");
    return -1;
  }

  int is_arith = type_node_is_arithmetic_type(opd_type);
  if (!is_arith) {
    cc_error_loc(cc, opd_loc, "unary plus requires arithmetic type");
    return 0;
  }

  int r = expr_integer_promotions(cc, operand);

  if (r) {
    return r;
  }

  enum type_kind tk_opd;
  tk_opd = expr_arith_type_kind_no_enum(cc, *operand);
  enum expr_type opcode = ET_INVALID;

  switch (tk_opd) {
    case tk_long_double_complex:
      opcode = ET_UNARY_PLUS_LDC;
      break;
    case tk_long_double_imaginary:
      opcode = ET_UNARY_PLUS_LDI;
      break;
    case tk_double_complex:
      opcode = ET_UNARY_PLUS_DC;
      break;
    case tk_double_imaginary:
      opcode = ET_UNARY_PLUS_DI;
      break;
    case tk_float_complex:
      opcode = ET_UNARY_PLUS_FC;
      break;
    case tk_float_imaginary:
      opcode = ET_UNARY_PLUS_FI;
      break;
    case tk_long_double:
      opcode = ET_UNARY_PLUS_LD;
      break;
    case tk_double:
      opcode = ET_UNARY_PLUS_D;
      break;
    case tk_float:
      opcode = ET_UNARY_PLUS_F;
      break;
    case tk_long_long_int:
      opcode = ET_UNARY_PLUS_SLLI;
      break;
    case tk_long_int:
      opcode = ET_UNARY_PLUS_SLI;
      break;
    case tk_int:
      opcode = ET_UNARY_PLUS_SI;
      break;
    case tk_unsigned_long_long_int:
      opcode = ET_UNARY_PLUS_ULLI;
      break;
    case tk_unsigned_long_int:
      opcode = ET_UNARY_PLUS_ULI;
      break;
    case tk_unsigned_int:
      opcode = ET_UNARY_PLUS_UI;
      break;
  }
  if (opcode == ET_INVALID) {
    cc_fatal_loc(cc, opd_loc, "Internal error: no matching unary plus opcode for operand type");
    return -1;
  }

  struct expr *x = expr_alloc(opcode);
  if (!x) {
    cc_no_memory(cc);
    return -1;
  }
  x->children_[0] = *operand;
  *operand = NULL;
  *dst = x;
  return 0;
}

int expr_unary_minus(struct c_compiler *cc, struct expr **dst, struct situs *op_loc, struct situs *opd_loc, struct expr **operand) {
  if (!*operand) {
    /* One of the sides is invalid. Simple pass through of an error */
    return -1;
  }
  struct type_node *opd_type = expr_type(cc, *operand);

  if (!opd_type) {
    cc_error_loc(cc, opd_loc, "cannot determine type of operand");
    return -1;
  }

  int is_arith = type_node_is_arithmetic_type(opd_type);
  if (!is_arith) {
    cc_error_loc(cc, opd_loc, "unary minus requires arithmetic type");
    return 0;
  }

  int r = expr_integer_promotions(cc, operand);

  if (r) {
    return r;
  }

  enum type_kind tk_opd;
  tk_opd = expr_arith_type_kind_no_enum(cc, *operand);
  enum expr_type opcode = ET_INVALID;

  switch (tk_opd) {
    case tk_long_double_complex:
      opcode = ET_UNARY_MINUS_LDC;
      break;
    case tk_long_double_imaginary:
      opcode = ET_UNARY_MINUS_LDI;
      break;
    case tk_double_complex:
      opcode = ET_UNARY_MINUS_DC;
      break;
    case tk_double_imaginary:
      opcode = ET_UNARY_MINUS_DI;
      break;
    case tk_float_complex:
      opcode = ET_UNARY_MINUS_FC;
      break;
    case tk_float_imaginary:
      opcode = ET_UNARY_MINUS_FI;
      break;
    case tk_long_double:
      opcode = ET_UNARY_MINUS_LD;
      break;
    case tk_double:
      opcode = ET_UNARY_MINUS_D;
      break;
    case tk_float:
      opcode = ET_UNARY_MINUS_F;
      break;
    case tk_long_long_int:
      opcode = ET_UNARY_MINUS_SLLI;
      break;
    case tk_long_int:
      opcode = ET_UNARY_MINUS_SLI;
      break;
    case tk_int:
      opcode = ET_UNARY_MINUS_SI;
      break;
    case tk_unsigned_long_long_int:
      opcode = ET_UNARY_MINUS_ULLI;
      break;
    case tk_unsigned_long_int:
      opcode = ET_UNARY_MINUS_ULI;
      break;
    case tk_unsigned_int:
      opcode = ET_UNARY_MINUS_UI;
      break;
  }
  if (opcode == ET_INVALID) {
    cc_fatal_loc(cc, opd_loc, "Internal error: no matching unary minus opcode for operand type");
    return -1;
  }

  struct expr *x = expr_alloc(opcode);
  if (!x) {
    cc_no_memory(cc);
    return -1;
  }
  x->children_[0] = *operand;
  *operand = NULL;
  *dst = x;
  return 0;
}

int expr_bitwise_complement(struct c_compiler *cc, struct expr **dst, struct situs *op_loc, struct situs *opd_loc, struct expr **operand) {
  if (!*operand) {
    /* One of the sides is invalid. Simple pass through of an error */
    return -1;
  }
  struct type_node *opd_type = expr_type(cc, *operand);

  if (!opd_type) {
    cc_error_loc(cc, opd_loc, "cannot determine type of operand");
    return -1;
  }

  int is_integer = type_node_is_integer_type(opd_type);
  if (!is_integer) {
    cc_error_loc(cc, opd_loc, "bitwise complement requires integer type");
    return 0;
  }

  int r = expr_integer_promotions(cc, operand);

  if (r) {
    return r;
  }

  enum type_kind tk_opd;
  tk_opd = expr_arith_type_kind_no_enum(cc, *operand);
  enum expr_type opcode = ET_INVALID;
  switch (tk_opd) {
    case tk_long_long_int:
      opcode = ET_BITWISE_COMPLEMENT_SLLI;
      break;
    case tk_long_int:
      opcode = ET_BITWISE_COMPLEMENT_SLI;
      break;
    case tk_int:
      opcode = ET_BITWISE_COMPLEMENT_SI;
      break;
    case tk_unsigned_long_long_int:
      opcode = ET_BITWISE_COMPLEMENT_ULLI;
      break;
    case tk_unsigned_long_int:
      opcode = ET_BITWISE_COMPLEMENT_ULI;
      break;
    case tk_unsigned_int:
      opcode = ET_BITWISE_COMPLEMENT_UI;
      break;
  }
  if (opcode == ET_INVALID) {
    cc_fatal_loc(cc, opd_loc, "Internal error: no matching bitwise complement opcode for operand type");
    return -1;
  }
  struct expr *x = expr_alloc(opcode);
  if (!x) {
    cc_no_memory(cc);
    return -1;
  }
  x->children_[0] = *operand;
  *operand = NULL;
  *dst = x;
  return 0;
}

int expr_indirection(struct c_compiler *cc, struct expr **dst, struct situs *op_loc, struct situs *opd_loc, struct expr **operand) {
  if (!*operand) {
    /* One of the sides is invalid. Simple pass through of an error */
    return -1;
  }

  int r;
  if (r = expr_pointer_decay(cc, operand)) {
    return r;
  }

  struct type_node *opd_type = expr_type(cc, *operand);

  if (!opd_type) {
    cc_error_loc(cc, opd_loc, "cannot determine type of operand");
    return -1;
  }

  opd_type = type_node_unqualified(opd_type);

  if (opd_type->kind_ != tk_pointer) {
    cc_error_loc(cc, opd_loc, "indirection operand is not of pointer type");
    return 0;
  }

  struct expr *x = expr_alloc(ET_INDIRECTION_PTR);

  if (!x) {
    cc_no_memory(cc);
    return -1;
  }

  x->children_[0] = *operand;
  *operand = NULL;
  *dst = x;

  return 0;
}

static int expr_post_op_by_one(struct c_compiler *cc, struct expr **dst, struct situs *op_loc, struct situs *opd_loc, struct expr **operand,
                               int (*expr_op)(struct c_compiler *cc, struct expr **dst, struct situs *left_loc, struct expr **left, struct situs *op_loc, struct situs *right_loc, struct expr **right)) {
  int r;

  struct expr *one;
  one = expr_alloc(ET_C_INT);
  if (one) {
    one->v_.i64_ = 1;
  }
  else {
    cc_no_memory(cc);
    return -1;
  }

  struct expr *seq;
  seq = expr_alloc(ET_SEQ);
  if (!seq) {
    expr_free(one);
    cc_no_memory(cc);
    return -1;
  }

  struct expr *opd_plus_one = NULL;
  struct expr *lvalue = *operand;
  lvalue->refs_++;
  struct expr *original = *operand;
  original->refs_++;

  r = expr_op(cc, &opd_plus_one, opd_loc, operand, op_loc, opd_loc, &one);
  if (r) {
    expr_free(one);
    expr_free(seq);
    expr_free(lvalue);
    expr_free(original);
    return r;
  }

  r = expr_assign(cc, &seq->children_[0], opd_loc, &lvalue, op_loc, opd_loc, &opd_plus_one);
  if (r) {
    expr_free(one);
    expr_free(seq);
    expr_free(lvalue);
    expr_free(opd_plus_one);
    expr_free(original);
    return r;
  }

  // XXX: expr_assign pops the top expr (the indirection) to perform the assignment on the pointer itself, we, however, rely
  //      on the continued existence of the indirection (as a source for the increment as well as the resulting value) -- are
  //      current measures enough? Will the indirection survive with a refcount of 2 total?

  seq->children_[1] = original;
  *dst = seq;

  return 0;
}

int expr_post_inc(struct c_compiler *cc, struct expr **dst, struct situs *op_loc, struct situs *opd_loc, struct expr **operand) {
  return expr_post_op_by_one(cc, dst, op_loc, opd_loc, operand, expr_add);
}

int expr_post_dec(struct c_compiler *cc, struct expr **dst, struct situs *op_loc, struct situs *opd_loc, struct expr **operand) {
  return expr_post_op_by_one(cc, dst, op_loc, opd_loc, operand, expr_sub);
}

static int expr_pre_op_by_one(struct c_compiler *cc, struct expr **dst, struct situs *op_loc, struct situs *opd_loc, struct expr **operand,
                              int (*expr_op)(struct c_compiler *cc, struct expr **dst, struct situs *left_loc, struct expr **left, struct situs *op_loc, struct situs *right_loc, struct expr **right)) {
  int r;

  struct expr *one;
  one = expr_alloc(ET_C_INT);
  if (one) {
    one->v_.i64_ = 1;
  }
  else {
    cc_no_memory(cc);
    return -1;
  }

  struct expr *opd_plus_one = NULL;
  struct expr *lvalue = *operand;
  lvalue->refs_++;

  r = expr_op(cc, &opd_plus_one, opd_loc, operand, op_loc, opd_loc, &one);
  if (r) {
    expr_free(one);
    expr_free(lvalue);
    return r;
  }

  r = expr_assign(cc, dst, opd_loc, &lvalue, op_loc, opd_loc, &opd_plus_one);
  if (r) {
    expr_free(one);
    expr_free(lvalue);
    expr_free(opd_plus_one);
    return r;
  }

  return 0;
}

int expr_pre_inc(struct c_compiler *cc, struct expr **dst, struct situs *op_loc, struct situs *opd_loc, struct expr **operand) {
  return expr_pre_op_by_one(cc, dst, op_loc, opd_loc, operand, expr_add);
}

int expr_pre_dec(struct c_compiler *cc, struct expr **dst, struct situs *op_loc, struct situs *opd_loc, struct expr **operand) {
  return expr_pre_op_by_one(cc, dst, op_loc, opd_loc, operand, expr_sub);
}


int expr_address_of(struct c_compiler *cc, struct expr **dst, struct situs *op_loc, struct situs *opd_loc, struct expr **operand) {
  if (!*operand) {
    /* One of the sides is invalid. Simple pass through of an error */
    return -1;
  }

  /* Possibly somewhat counter-intuitively, taking the address of an operand actually consists of stripping away an ET_INDIRECTION_PTR instruction. 
   * If the top of the stack is a value that is not an ET_INDIRECTION_PTR, (eg. it is computed) then no address-of the value can be taken. */
  if ((*operand)->et_ != ET_INDIRECTION_PTR) {
    cc_error_loc(cc, opd_loc, "address-of operand is not an lvalue");
    return 0;
  }

  struct expr *indirection_address_child = (*operand)->children_[0];
  /* Clear out the child so we may free the indirection itself without giving up the address */
  (*operand)->children_[0] = NULL;
  *dst = indirection_address_child;
  expr_free(*operand);
  *operand = NULL;
  
  return 0;
}

int expr_cast(struct c_compiler *cc, struct expr **dst, struct situs *type_loc, struct type_node *ptype, struct situs *val_loc, struct expr **val) {  
  if ((!ptype) || (!*val)) {
    /* One of the sides is invalid. Simple pass through of an error */
    return -1;
  }

  int r;
  if (r = expr_pointer_decay(cc, val)) {
    return r;
  }

  struct type_node *val_type;
  val_type = expr_type(cc, *val);
  if (!val_type) {
    cc_error_loc(cc, val_loc, "cannot determine type of right operand");
    return -1;
  }

  struct type_node *cast_type = ptype;

  int is_arith_cast = type_node_is_arithmetic_type(cast_type);
  int is_arith_val = type_node_is_arithmetic_type(val_type);
  int is_integer_cast = type_node_is_integer_type(cast_type);
  int is_integer_val = type_node_is_integer_type(val_type);
  int is_pointer_cast = type_node_is_pointer_type(cast_type);
  int is_pointer_val = type_node_is_pointer_type(val_type);
  int is_struct_or_union_cast = type_node_is_struct_or_union(cast_type);
  int is_struct_or_union_val = type_node_is_struct_or_union(val_type);

  if (is_arith_cast && is_arith_val) {
    enum expr_type opcode = ET_INVALID;
    enum type_kind tk_opd;
    tk_opd = type_node_arith_type_kind_no_enum(&cc->tb_, cast_type);
    struct expr *rc = expr_convert_type(cc, cast_type, *val);
    if (!rc) {
      cc_error_loc(cc, val_loc, "failed to convert value to type of cast");
      return 0;
    }
    /* left is parent of ptr_exp; we now drop the reference so parent frees while ptr child remains */
    *val = NULL;
    *dst = rc;
    return 0;
  }
  else if (is_struct_or_union_cast || is_struct_or_union_val) {
    /* Is left compatible with the right ? */
    cc_error_loc(cc, type_loc, "cannot cast to or from structure types\n");
    return 0;
  }
  else if (is_pointer_cast && is_pointer_val) {
    /* Casting is very liberal when it comes to pointer types */
    struct type_node *left_type_pointed_to = type_node_unqualified(cast_type)->derived_from_;
    struct type_node *right_type_pointed_to = type_node_unqualified(val_type)->derived_from_;
    int qualifiers_left = (left_type_pointed_to->kind_ == tk_qualifier) ? left_type_pointed_to->qualifiers_ : 0;
    int qualifiers_right = (right_type_pointed_to->kind_ == tk_qualifier) ? right_type_pointed_to->qualifiers_ : 0;

    if (qualifiers_right & ~qualifiers_left) {
      /* There are qualifiers in the right that do not exist in the left */
      /* For instance:
       * void *p;
       * const int *q;
       * p = (void *)q; // qualifiers in the right (const) that do not exist in the left */
      cc_error_loc(cc, type_loc, "cast would lose qualifiers");
      return 0;
    }

    /* Passed checks, convert pointer */
    struct expr *rc = expr_convert_type(cc, cast_type, *val);
    if (!rc) {
      cc_error_loc(cc, val_loc, "failed to convert value to type of cast");
      return -1;
    }
    *val = NULL;
    *dst = rc;
    return 0;
  }
  else if (is_pointer_cast) {
    struct expr *rc = expr_convert_type(cc, cast_type, *val);
    if (!rc) {
      cc_error_loc(cc, val_loc, "failed to convert value to type of cast");
      return -1;
    }
    *val = NULL;
    *dst = rc;
    return 0;
  }
  else if (is_integer_cast) {
    struct expr *rc = expr_convert_type(cc, cast_type, *val);
    if (!rc) {
      cc_error_loc(cc, val_loc, "failed to convert value to integer type");
      return -1;
    }
    *val = NULL;
    *dst = rc;
    return 0;
  }
  else if ((type_node_unqualified(cast_type)->kind_ == tk_bool) && is_pointer_val) {
    struct expr *rc = expr_convert_type(cc, cast_type, *val);
    if (!rc) {
      cc_error_loc(cc, val_loc, "failed to convert value to type of cast");
      return -1;
    }
    *val = NULL;
    *dst = rc;
    return 0;
  }
  cc_fatal_loc(cc, val_loc, "failed to explicitly handle incompatible types for casting");

  return -1;
}

static int expr_equality_operators(struct c_compiler *cc, struct expr **dst, struct situs *left_loc, struct expr **left, struct situs *op_loc, struct situs *right_loc, struct expr **right, int not_equals) {  
  if ((!*left) || (!*right)) {
    /* One of the sides is invalid. Simple pass through of an error */
    return -1;
  }

  int r;
  if ((r = expr_pointer_decay(cc, left)) || (r = expr_pointer_decay(cc, right))) {
    return r;
  }

  struct type_node *left_type, *right_type;
  left_type = expr_type(cc, *left);
  if (!left_type) {
    cc_error_loc(cc, left_loc, "cannot determine type of left operand");
    *dst = NULL;
    return -1;
  }
  right_type = expr_type(cc, *right);
  if (!right_type) {
    cc_error_loc(cc, right_loc, "cannot determine type of right operand");
    *dst = NULL;
    return -1;
  }

  left_type = type_node_unqualified(left_type);
  right_type = type_node_unqualified(right_type);

  int is_arith_left = type_node_is_arithmetic_type(left_type);
  int is_arith_right = type_node_is_arithmetic_type(right_type);

  int is_ptr_left = type_node_is_pointer_type(left_type);
  int is_ptr_right = type_node_is_pointer_type(right_type);

  if (is_arith_left && is_arith_right) {
    if (!expr_usual_arithmetic_conversions(cc, left, right)) {
      enum type_kind tk_left, tk_right;
      tk_left = expr_arith_type_kind_no_enum(cc, *left);
      tk_right = expr_arith_type_kind_no_enum(cc, *right);

      enum expr_type opcode = ET_INVALID;
      int swapped = 0;
      switch (tk_left) {
        case tk_long_double_complex:
          if (tk_right == tk_long_double) {
            opcode = not_equals ? ET_NEQ_LDC_LD : ET_EQU_LDC_LD;
          }
          else if (tk_right == tk_long_double_complex) {
            opcode = not_equals ? ET_NEQ_LDC_LDC : ET_EQU_LDC_LDC;
          }
          else if (tk_right == tk_long_double_imaginary) {
            opcode = not_equals ? ET_NEQ_LDC_LDI : ET_EQU_LDC_LDI;
          }
          break;
        case tk_double_complex:
          if (tk_right == tk_double) {
            opcode = not_equals ? ET_NEQ_DC_D : ET_EQU_DC_D;
          }
          else if (tk_right == tk_double_complex) {
            opcode = not_equals ? ET_NEQ_DC_DC : ET_EQU_DC_DC;
          }
          else if (tk_right == tk_double_imaginary) {
            opcode = not_equals ? ET_NEQ_DC_DI : ET_EQU_DC_DI;
          }
          break;
        case tk_float_complex:
          if (tk_right == tk_float) {
            opcode = not_equals ? ET_NEQ_FC_F : ET_EQU_FC_F;
          }
          else if (tk_right == tk_float_complex) {
            opcode = not_equals ? ET_NEQ_FC_FC : ET_EQU_FC_FC;
          }
          else if (tk_right == tk_float_imaginary) {
            opcode = not_equals ? ET_NEQ_FC_FI : ET_EQU_FC_FI;
          }
          break;
        case tk_long_double_imaginary:
          if (tk_right == tk_long_double) {
            opcode = not_equals ? ET_NEQ_LDI_LD : ET_EQU_LDI_LD;
          }
          else if (tk_right == tk_long_double_complex) {
            opcode = not_equals ? ET_NEQ_LDC_LDI : ET_EQU_LDC_LDI;
            swapped = 1;
          }
          else if (tk_right == tk_long_double_imaginary) {
            opcode = not_equals ? ET_NEQ_LDI_LDI : ET_EQU_LDI_LDI;
          }
          break;
        case tk_double_imaginary:
          if (tk_right == tk_double) {
            opcode = not_equals ? ET_NEQ_DI_D : ET_EQU_DI_D;
          }
          else if (tk_right == tk_double_complex) {
            opcode = not_equals ? ET_NEQ_DC_DI : ET_EQU_DC_DI;
            swapped = 1;
          }
          else if (tk_right == tk_double_imaginary) {
            opcode = not_equals ? ET_NEQ_DI_DI : ET_EQU_DI_DI;
          }
          break;
        case tk_float_imaginary:
          if (tk_right == tk_float) {
            opcode = not_equals ? ET_NEQ_FI_F : ET_EQU_FI_F;
          }
          else if (tk_right == tk_float_complex) {
            opcode = not_equals ? ET_NEQ_FC_FI : ET_EQU_FC_FI;
            swapped = 1;
          }
          else if (tk_right == tk_float_imaginary) {
            opcode = not_equals ? ET_NEQ_FI_FI : ET_EQU_FI_FI;
          }
          break;
        case tk_long_double:
          if (tk_right == tk_long_double) {
            opcode = not_equals ? ET_NEQ_LD : ET_EQU_LD;
          }
          else if (tk_right == tk_long_double_complex) {
            opcode = not_equals ? ET_NEQ_LDC_LD : ET_EQU_LDC_LD;
            swapped = 1;
          }
          else if (tk_right == tk_long_double_imaginary) {
            opcode = not_equals ? ET_NEQ_LDI_LD : ET_EQU_LDI_LD;
            swapped = 1;
          }
          break;
        case tk_double:
          if (tk_right == tk_double) {
            opcode = not_equals ? ET_NEQ_D : ET_EQU_D;
          }
          else if (tk_right == tk_double_complex) {
            opcode = not_equals ? ET_NEQ_DC_D : ET_EQU_DC_D;
            swapped = 1;
          }
          else if (tk_right == tk_double_imaginary) {
            opcode = not_equals ? ET_NEQ_DI_D : ET_EQU_DI_D;
            swapped = 1;
          }
          break;
        case tk_float:
          if (tk_right == tk_float) {
            opcode = not_equals ? ET_NEQ_F : ET_EQU_F;
          }
          else if (tk_right == tk_float_complex) {
            opcode = not_equals ? ET_NEQ_FC_F : ET_EQU_FC_F;
            swapped = 1;
          }
          else if (tk_right == tk_float_imaginary) {
            opcode = not_equals ? ET_NEQ_FI_F : ET_EQU_FI_F;
            swapped = 1;
          }
          break;
        case tk_long_long_int:
          opcode = not_equals ? ET_NEQ_SLLI : ET_EQU_SLLI;
          break;
        case tk_long_int:
          opcode = not_equals ? ET_NEQ_SLI : ET_EQU_SLI;
          break;
        case tk_int:
          opcode = not_equals ? ET_NEQ_SI : ET_EQU_SI;
          break;
        case tk_unsigned_long_long_int:
          opcode = not_equals ? ET_NEQ_ULLI : ET_EQU_ULLI;
          break;
        case tk_unsigned_long_int:
          opcode = not_equals ? ET_NEQ_ULI : ET_EQU_ULI;
          break;
        case tk_unsigned_int:
          opcode = not_equals ? ET_NEQ_UI : ET_EQU_UI;
          break;
      }
      if (opcode == ET_INVALID) {
        cc_fatal_loc(cc, op_loc, "Internal error: no matching equality opcode for operand types");
        return -1;
      }
      struct expr *x = expr_alloc(opcode);
      if (!x) {
        return -1;
      }
      if (!swapped) {
        x->children_[0] = *left;
        x->children_[1] = *right;
      }
      else {
        x->children_[0] = *right;
        x->children_[1] = *left;
      }
      *left = NULL;
      *right = NULL;
      *dst = x;
      return 0;
    }
    else {
      cc_error_loc(cc, op_loc, "failed to perform the usual arithmetic conversions");
      return -1;
    }
  }
  else if (is_ptr_left && is_ptr_right) {
    /* 6.5.9-2 both operands are pointers to qualified or unqualified versions of compatible types;
     *         one operand is a pointer to an object or incomplete type and the other is a pointer to a
     *         qualified or unqualified version of void. */ 
    if (type_node_is_compatible(type_node_unqualified(left_type->derived_from_), type_node_unqualified(right_type->derived_from_)) ||
        (type_node_unqualified(left_type->derived_from_)->kind_ == tk_void) ||
        (type_node_unqualified(right_type->derived_from_)->kind_ == tk_void)) {
      struct expr *x = expr_alloc(not_equals ? ET_NEQ_PTR : ET_EQU_PTR);
      if (!x) {
        cc_no_memory(cc);
        return -1;
      }
      x->children_[0] = *left;
      x->children_[1] = *right;
      *left = *right = NULL;
      *dst = x;
      return 0;
    }
    else {
      cc_error_loc(cc, op_loc, "cannot compare incompatible pointer types");
      return 0;
    }
  }
  else if (is_ptr_left || is_ptr_right) {
    /* 6.5.9-2 one operand is a pointer and the other is a null pointer constant */
    if (is_ptr_left) {
      int is_null_ptr_right = 0;
      r = expr_is_null_ptr(cc, *right, &is_null_ptr_right);
      if (r) {
        cc_error_loc(cc, right_loc, "failed to evaluate expression as null pointer");
        return r;
      }

      if (is_null_ptr_right) {
        struct expr *x = expr_alloc(not_equals ? ET_NEQ_PTR : ET_EQU_PTR);
        if (!x) {
          cc_no_memory(cc);
          return -1;
        }
        x->children_[1] = expr_convert_type(cc, left_type, *right);
        if (!x->children_[1]) {
          /* Failed to convert types */
          expr_free(x);
          return -1;
        }
        x->children_[0] = *left;
        *left = NULL;
        *right = NULL;
        *dst = x;
        return 0;
      }
    }

    if (is_ptr_right) {
      int is_null_ptr_left = 0;
      r = expr_is_null_ptr(cc, *left, &is_null_ptr_left);
      if (r) {
        cc_error_loc(cc, left_loc, "failed to evaluate expression as null pointer");
        return r;
      }

      if (is_null_ptr_left) {
        struct expr *x = expr_alloc(not_equals ? ET_NEQ_PTR : ET_EQU_PTR);
        if (!x) {
          cc_no_memory(cc);
          return -1;
        }
        x->children_[0] = expr_convert_type(cc, right_type, *left);
        if (!x->children_[0]) {
          /* Failed to convert types */
          expr_free(x);
          return -1;
        }
        x->children_[1] = *right;
        *left = NULL;
        *right = NULL;
        *dst = x;
        return 0;
      }
    }
  }

  cc_error_loc(cc, op_loc, "failed to compare mismatching types");
  return -1;
}

int expr_equality(struct c_compiler *cc, struct expr **dst, struct situs *left_loc, struct expr **left, struct situs *op_loc, struct situs *right_loc, struct expr **right) {
  return expr_equality_operators(cc, dst, left_loc, left, op_loc, right_loc, right, 0);
}

int expr_inequality(struct c_compiler *cc, struct expr **dst, struct situs *left_loc, struct expr **left, struct situs *op_loc, struct situs *right_loc, struct expr **right) {
  return expr_equality_operators(cc, dst, left_loc, left, op_loc, right_loc, right, 1);
}

int expr_cond_is_true(struct c_compiler *cc, struct expr **dst, struct situs *loc, struct expr **val) {
  /* Convert to a value of 1 or 0 int by comparing for inequality to 0. */
  struct expr *x = expr_alloc(ET_C_INT);
  if (!x) {
    cc_no_memory(cc);
    return -1;
  }
  x->v_.i64_ = 0;
  int r;
  r = expr_inequality(cc, dst, loc, val, loc, loc, &x);
  if (r) {
    expr_free(x);
    return r;
  }
  return 0;
}

int expr_cond_is_false(struct c_compiler *cc, struct expr **dst, struct situs *loc, struct expr **val) {
  /* Convert to a value of 1 or 0 int by comparing for equality to 0. */
  struct expr *x = expr_alloc(ET_C_INT);
  if (!x) {
    cc_no_memory(cc);
    return -1;
  }
  x->v_.i64_ = 0;
  int r;
  r = expr_equality(cc, dst, loc, val, loc, loc, &x);
  if (r) {
    expr_free(x);
    return r;
  }
  return 0;
}

int expr_logical_not(struct c_compiler *cc, struct expr **dst, struct situs *op_loc, struct situs *opd_loc, struct expr **operand) {  
  /* Convert to a value of 1 or 0 int by comparing for equality to 0. */
  struct expr *x = expr_alloc(ET_C_INT);
  if (!x) {
    cc_no_memory(cc);
    return -1;
  }
  x->v_.i64_ = 0;
  int r;
  r = expr_equality(cc, dst, opd_loc, operand, op_loc, opd_loc, &x);
  if (r) {
    expr_free(x);
    return r;
  }
  return 0;
}

static int expr_logical_binop(struct c_compiler *cc, struct expr **dst, struct situs *left_loc, struct expr **left, struct situs *op_loc, struct situs *right_loc, struct expr **right, enum expr_type op) {
  int r;
  struct expr *first = NULL;
  r = expr_cond_is_true(cc, &first, left_loc, left);
  if (r) {
    return r;
  }
  struct expr *second = NULL;
  r = expr_cond_is_true(cc, &second, right_loc, right);
  if (r) {
    expr_free(first);
    return r;
  }
  struct expr *x = expr_alloc(op);
  if (!x) {
    cc_no_memory(cc);
    expr_free(first);
    expr_free(second);
    return -1;
  }
  *dst = x;
  x->children_[0] = first;
  x->children_[1] = second;
  return 0;
}


int expr_logical_and(struct c_compiler *cc, struct expr **dst, struct situs *left_loc, struct expr **left, struct situs *op_loc, struct situs *right_loc, struct expr **right) {
  return expr_logical_binop(cc, dst, left_loc, left, op_loc, right_loc, right, ET_LOGICAL_AND);
}


int expr_logical_or(struct c_compiler *cc, struct expr **dst, struct situs *left_loc, struct expr **left, struct situs *op_loc, struct situs *right_loc, struct expr **right) {
  return expr_logical_binop(cc, dst, left_loc, left, op_loc, right_loc, right, ET_LOGICAL_OR);
}


int expr_assign(struct c_compiler *cc, struct expr **dst, struct situs *left_loc, struct expr **left, struct situs *op_loc, struct situs *right_loc, struct expr **right) {
  if ((!*left) || (!*right)) {
    /* One of the sides is invalid. Simple pass through of an error */
    return -1;
  }

  int r;
  if ((r = expr_pointer_decay(cc, left)) || (r = expr_pointer_decay(cc, right))) {
    return r;
  }

  struct type_node *left_type, *right_type;
  left_type = expr_type(cc, *left);
  if (!left_type) {
    cc_error_loc(cc, left_loc, "cannot determine type of left operand");
    *dst = NULL;
    return -1;
  }
  right_type = expr_type(cc, *right);
  if (!right_type) {
    cc_error_loc(cc, right_loc, "cannot determine type of right operand");
    *dst = NULL;
    return -1;
  }

  if ((*left)->et_ != ET_INDIRECTION_PTR) {
    cc_error_loc(cc, left_loc, "left side of assignment is not an lvalue");
    return 0;
  }
  struct expr *ptr_exp = (*left)->children_[0];

  if ((left_type->kind_ == tk_qualifier) && (left_type->qualifiers_ & TYPE_QUALIFIER_CONST)) {
    cc_error_loc(cc, left_loc, "cannot assign to const value");
    return 0;
  }

  /* XXX: Check 6.3.2.1 that left is a modifiable lvalue, this means ensuring that it is not
   *      const qualified (as per above, we've done that,) but also measn ensuring that, if the left
   *      side is a struct or union type, that it has no const qualified member; a check that must
   *      be done recursively.. */

  int is_arith_left = type_node_is_arithmetic_type(left_type);
  int is_arith_right = type_node_is_arithmetic_type(right_type);
  int is_integer_left = type_node_is_integer_type(left_type);
  int is_integer_right = type_node_is_integer_type(right_type);
  int is_pointer_left = type_node_is_pointer_type(left_type);
  int is_pointer_right = type_node_is_pointer_type(right_type);
  int is_struct_or_union_left = type_node_is_struct_or_union(left_type);
  int is_struct_or_union_right = type_node_is_struct_or_union(right_type);
  int is_void_ptr_left = is_pointer_left && (type_node_unqualified(left_type)->derived_from_->kind_ == tk_void);
  int is_void_ptr_right = is_pointer_right && (type_node_unqualified(right_type)->derived_from_->kind_ == tk_void);


  if (is_arith_left && is_arith_right) {
    enum expr_type opcode = ET_INVALID;
    enum type_kind tk_opd;
    tk_opd = expr_arith_type_kind_no_enum(cc, *left);
    switch (tk_opd) {
      case tk_long_double_complex:
        opcode = ET_STORE_LDC;
        break;
      case tk_long_double_imaginary:
        opcode = ET_STORE_LDI;
        break;
      case tk_double_complex:
        opcode = ET_STORE_DC;
        break;
      case tk_double_imaginary:
        opcode = ET_STORE_DI;
        break;
      case tk_float_complex:
        opcode = ET_STORE_FC;
        break;
      case tk_float_imaginary:
        opcode = ET_STORE_FI;
        break;
      case tk_long_double:
        opcode = ET_STORE_LD;
        break;
      case tk_double:
        opcode = ET_STORE_D;
        break;
      case tk_float:
        opcode = ET_STORE_F;
        break;
      case tk_long_long_int:
        opcode = ET_STORE_SLLI;
        break;
      case tk_long_int:
        opcode = ET_STORE_SLI;
        break;
      case tk_int:
        opcode = ET_STORE_SI;
        break;
      case tk_short_int:
        opcode = ET_STORE_SS;
        break;
      case tk_signed_char:
        opcode = ET_STORE_SC;
        break;
      case tk_unsigned_long_long_int:
        opcode = ET_STORE_ULLI;
        break;
      case tk_unsigned_long_int:
        opcode = ET_STORE_ULI;
        break;
      case tk_unsigned_int:
        opcode = ET_STORE_UI;
        break;
      case tk_unsigned_short_int:
        opcode = ET_STORE_US;
        break;
      case tk_unsigned_char:
        opcode = ET_STORE_UC;
        break;
      case tk_char:
        opcode = ET_STORE_C;
        break;
    }
    if (opcode == ET_INVALID) {
      cc_fatal_loc(cc, left_loc, "Internal error: no matching opcode for left type");
      return -1;
    }
    struct expr *x = expr_alloc(opcode);
    if (!x) {
      cc_no_memory(cc);
      return -1;
    }
    struct expr *rc = expr_convert_type(cc, left_type, *right);
    if (!rc) {
      cc_error_loc(cc, right_loc, "failed to convert right side to type of left side");
      expr_free(x);
      return 0;
    }
    /* left is parent of ptr_exp; we now drop the reference so parent frees while ptr child remains */
    x->children_[0] = ptr_exp; 
    ptr_exp->refs_++; /* expr_free() of *left parent decrements this back down by one; the ++ therefore ensures its survival */
    expr_free(*left); 
    x->children_[1] = rc;
    *left = NULL;
    *right = NULL;
    *dst = x;
    return 0;
  }
  else if  (is_struct_or_union_left && is_struct_or_union_right) {
    /* Is left compatible with the right ? */
    if (type_node_is_compatible(type_node_unqualified(left_type), type_node_unqualified(right_type))) {
      /* XXX: Not yet implemented */
      cc_fatal_loc(cc, op_loc, "struct assignment is not yet implemented");
      return -1;
    }
    else {
      cc_error_loc(cc, op_loc, "operand types are incompatible for assignment");
      return 0;
    }
  }
  else if (is_pointer_left && is_pointer_right) {
    struct type_node *left_type_pointed_to = type_node_unqualified(left_type)->derived_from_;
    struct type_node *right_type_pointed_to = type_node_unqualified(right_type)->derived_from_;
    int qualifiers_left = (left_type_pointed_to->kind_ == tk_qualifier) ? left_type_pointed_to->qualifiers_ : 0;
    int qualifiers_right = (right_type_pointed_to->kind_ == tk_qualifier) ? right_type_pointed_to->qualifiers_ : 0;

    /* Is one a void pointer and the other not ? */
    if (is_void_ptr_left ^ is_void_ptr_right) {
      if (qualifiers_right & ~qualifiers_left) {
        /* There are qualifiers in the right that do not exist in the left */
        /* For instance:
         * void *p;
         * const int *q;
         * p = q; // qualifiers in the right (const) that do not exist in the left */
        cc_error_loc(cc, op_loc, "assignment would lose qualifiers");
        return 0;
      }
    }
    /* Are both pointers to compatible types ? */
    else if (type_node_is_compatible(type_node_unqualified(left_type_pointed_to), type_node_unqualified(right_type_pointed_to))) {
      /* Does the left have all the qualifiers of the type pointed to by the right ? */
      if (qualifiers_right & ~qualifiers_left) {
        /* There are qualifiers in the right that do not exist in the left */
        /* For instance:
         * int *p;
         * const int *q;
         * p = q; // qualifiers in the right (const) that do not exist in the left */
        cc_error_loc(cc, op_loc, "assignment would lose qualifiers");
        return 0;
      }
    }
    else {
      /* Assignment of incompatible pointers */
      cc_error_loc(cc, op_loc, "incompatible pointer assignment");
      return 0;
    }

    /* Passed checks, assign pointer */
    struct expr *x = expr_alloc(ET_STORE_PTR);
    if (!x) {
      cc_no_memory(cc);
      return -1;
    }
    struct expr *rc = expr_convert_type(cc, left_type, *right);
    if (!rc) {
      cc_error_loc(cc, right_loc, "failed to convert right side to type of left side");
      expr_free(x);
      return 0;
    }
    /* left is parent of ptr_exp; we now drop the reference so parent frees while ptr child remains */
    x->children_[0] = ptr_exp;
    ptr_exp->refs_++; /* expr_free() of *left parent decrements this back down by one; the ++ therefore ensures its survival */
    expr_free(*left);
    x->children_[1] = rc;
    *left = NULL;
    *right = NULL;
    *dst = x;
    return 0;
  }
  else if (is_pointer_left) {
    int is_null_ptr_right = 0;
    int r;
    r = expr_is_null_ptr(cc, *right, &is_null_ptr_right);
    if (r) {
      return -1;
    }
    if (!is_null_ptr_right) {
      cc_error_loc(cc, op_loc, "incompatible pointer assignment");
      return 0;
    }
    /* Convert right to pointer of type of left (requires conversions to/from pointers) */
    struct expr *x = expr_alloc(ET_STORE_PTR);
    if (!x) {
      cc_no_memory(cc);
      return -1;
    }
    struct expr *rc = expr_convert_type(cc, left_type, *right);
    if (!rc) {
      cc_error_loc(cc, right_loc, "failed to convert right side to type of left side");
      expr_free(x);
      return 0;
    }
    /* left is parent of ptr_exp; we now drop the reference so parent frees while ptr child remains */
    x->children_[0] = ptr_exp;
    ptr_exp->refs_++; /* expr_free() of *left parent decrements this back down by one; the ++ therefore ensures its survival */
    expr_free(*left);
    x->children_[1] = rc;
    *left = NULL;
    *right = NULL;
    *dst = x;
    return 0;
  }
  else if ((type_node_unqualified(left_type)->kind_ == tk_bool) && is_pointer_right) {
    /* Convert right pointer type to boolean */
    struct expr *x = expr_alloc(ET_STORE_B);
    if (!x) {
      cc_no_memory(cc);
      return -1;
    }
    struct expr *rc = expr_convert_type(cc, left_type, *right);
    if (!rc) {
      cc_error_loc(cc, right_loc, "failed to convert right side to type of left side");
      expr_free(x);
      return 0;
    }
    /* left is parent of ptr_exp; we now drop the reference so parent frees while ptr child remains */
    x->children_[0] = ptr_exp;
    ptr_exp->refs_++; /* expr_free() of *left parent decrements this back down by one; the ++ therefore ensures its survival */
    expr_free(*left);
    x->children_[1] = rc;
    *left = NULL;
    *right = NULL;
    *dst = x;
    return 0;
  }
  cc_error_loc(cc, op_loc, "incompatible types for assignment");

  return 0;
}


static int expr_assign_op_by_x(struct c_compiler *cc, struct expr **dst, struct situs *left_loc, struct expr **left, struct situs *op_loc, struct situs *right_loc, struct expr **right,
                               int (*expr_op)(struct c_compiler *cc, struct expr **dst, struct situs *left_loc, struct expr **left, struct situs *op_loc, struct situs *right_loc, struct expr **right)) {
  int r;

  struct expr *x = NULL;
  struct expr *lvalue = *left;
  lvalue->refs_++;

  r = expr_op(cc, &x, left_loc, left, op_loc, right_loc, right);
  if (r) {
    expr_free(lvalue);
    return r;
  }

  r = expr_assign(cc, dst, left_loc, &lvalue, op_loc, right_loc, &x);
  if (r) {
    expr_free(x);
    expr_free(lvalue);
    return r;
  }

  return 0;
}

int expr_assign_mul(struct c_compiler *cc, struct expr **dst, struct situs *left_loc, struct expr **left, struct situs *op_loc, struct situs *right_loc, struct expr **right) {
  return expr_assign_op_by_x(cc, dst, left_loc, left, op_loc, right_loc, right, expr_mul);
}

int expr_assign_div(struct c_compiler *cc, struct expr **dst, struct situs *left_loc, struct expr **left, struct situs *op_loc, struct situs *right_loc, struct expr **right) {
  return expr_assign_op_by_x(cc, dst, left_loc, left, op_loc, right_loc, right, expr_div);
}

int expr_assign_rem(struct c_compiler *cc, struct expr **dst, struct situs *left_loc, struct expr **left, struct situs *op_loc, struct situs *right_loc, struct expr **right) {
  return expr_assign_op_by_x(cc, dst, left_loc, left, op_loc, right_loc, right, expr_rem);
}

int expr_assign_add(struct c_compiler *cc, struct expr **dst, struct situs *left_loc, struct expr **left, struct situs *op_loc, struct situs *right_loc, struct expr **right) {
  return expr_assign_op_by_x(cc, dst, left_loc, left, op_loc, right_loc, right, expr_add);
}

int expr_assign_sub(struct c_compiler *cc, struct expr **dst, struct situs *left_loc, struct expr **left, struct situs *op_loc, struct situs *right_loc, struct expr **right) {
  return expr_assign_op_by_x(cc, dst, left_loc, left, op_loc, right_loc, right, expr_sub);
}

int expr_assign_shift_left(struct c_compiler *cc, struct expr **dst, struct situs *left_loc, struct expr **left, struct situs *op_loc, struct situs *right_loc, struct expr **right) {
  return expr_assign_op_by_x(cc, dst, left_loc, left, op_loc, right_loc, right, expr_shift_left);
}

int expr_assign_shift_right(struct c_compiler *cc, struct expr **dst, struct situs *left_loc, struct expr **left, struct situs *op_loc, struct situs *right_loc, struct expr **right) {
  return expr_assign_op_by_x(cc, dst, left_loc, left, op_loc, right_loc, right, expr_shift_right);
}

int expr_assign_bitwise_and(struct c_compiler *cc, struct expr **dst, struct situs *left_loc, struct expr **left, struct situs *op_loc, struct situs *right_loc, struct expr **right) {
  return expr_assign_op_by_x(cc, dst, left_loc, left, op_loc, right_loc, right, expr_bitwise_and);
}

int expr_assign_bitwise_xor(struct c_compiler *cc, struct expr **dst, struct situs *left_loc, struct expr **left, struct situs *op_loc, struct situs *right_loc, struct expr **right) {
  return expr_assign_op_by_x(cc, dst, left_loc, left, op_loc, right_loc, right, expr_bitwise_xor);
}

int expr_assign_bitwise_or(struct c_compiler *cc, struct expr **dst, struct situs *left_loc, struct expr **left, struct situs *op_loc, struct situs *right_loc, struct expr **right) {
  return expr_assign_op_by_x(cc, dst, left_loc, left, op_loc, right_loc, right, expr_bitwise_or);
}

int expr_sequence(struct c_compiler *cc, struct expr **dst, struct situs *left_loc, struct expr **left, struct situs *op_loc, struct situs *right_loc, struct expr **right) {
  struct expr *x = expr_alloc(ET_SEQ);
  if (!x) {
    cc_no_memory(cc);
    return -1;
  }
  x->children_[0] = *left;
  x->children_[1] = *right;
  *left = NULL;
  *right = NULL;
  *dst = x;
  return 0;
}

int expr_condition(struct c_compiler *cc, struct expr **dst, struct situs *cond_loc, struct expr **cond, struct situs *true_loc, struct expr **true_expr, struct situs *false_loc, struct expr **false_expr) { 
  int r;
  struct expr *condition = NULL;

  if (!*cond || !*true_expr || !*false_expr) {
    /* Propagate error upwards */
    return 0;
  }

  if ((r = expr_pointer_decay(cc, true_expr)) || (r = expr_pointer_decay(cc, false_expr))) {
    return r;
  }

  struct type_node *true_type = expr_type(cc, *true_expr);
  if (!true_type) {
    cc_error_loc(cc, true_loc, "cannot determine type of true operand");
    *dst = NULL;
    return -1;
  }
  struct type_node *false_type = expr_type(cc, *false_expr);
  if (!false_type) {
    cc_error_loc(cc, false_loc, "cannot determine type of false operand");
    *dst = NULL;
    return -1;
  }

  int is_arith_true = type_node_is_arithmetic_type(true_type);
  int is_arith_false = type_node_is_arithmetic_type(false_type);
  int is_pointer_true = type_node_is_pointer_type(true_type);
  int is_pointer_false = type_node_is_pointer_type(false_type);

  struct type_node *result_type = NULL;

  if (is_arith_true && is_arith_false) {
    r = expr_usual_arithmetic_conversions(cc, true_expr, false_expr);
    if (r) {
      cc_error_loc(cc, cond_loc, "failed to perform the usual arithmetic conversions");
      return 0;
    }
    result_type = expr_type(cc, *true_expr);
    if (!result_type) {
      cc_fatal_loc(cc, true_loc, "failed to determine type after the usual arithmetic conversions");
      return -1;
    }
  }
  else if (type_node_is_compatible(true_type, false_type)) {
    /* Compatible structure or union types, or tk_void */
    result_type = type_node_unqualified(true_type);
  }
  else if (is_pointer_true) {
    int is_null_ptr_false = 0;
    r = expr_is_null_ptr(cc, *false_expr, &is_null_ptr_false);
    if (r) {
      cc_error_loc(cc, false_loc, "failed to determine if expression is null pointer");
      return r;
    }
    if (is_null_ptr_false) {
      result_type = type_node_unqualified(true_type);
    }
  }
  if (!result_type && is_pointer_false) {
    int is_null_ptr_true = 0;
    r = expr_is_null_ptr(cc, *true_expr, &is_null_ptr_true);
    if (r) {
      cc_error_loc(cc, true_loc, "failed to determine if expression is null pointer");
      return r;
    }
    if (is_null_ptr_true) {
      result_type = type_node_unqualified(false_type);
    }
  }
  if (!result_type && is_pointer_true && is_pointer_false) {
    int pointed_to_qualifiers = 0;
    struct type_node *true_pointed_to = type_node_unqualified(true_type)->derived_from_;
    struct type_node *false_pointed_to = type_node_unqualified(false_type)->derived_from_;
    pointed_to_qualifiers = (true_pointed_to->kind_ == tk_qualifier) ? true_pointed_to->qualifiers_ : 0;
    pointed_to_qualifiers |= (false_pointed_to->kind_ == tk_qualifier) ? false_pointed_to->qualifiers_ : 0;

    /* is true_type or false_type a (potentially qualified) void ptr? */
    if (type_node_unqualified(true_pointed_to)->kind_ == tk_void) {
      result_type = type_base_qualifier(&cc->tb_, type_base_simple(&cc->tb_, tk_void), pointed_to_qualifiers);
      result_type = type_base_pointer(&cc->tb_, result_type);
    }
    else if (type_node_is_compatible(type_node_unqualified(true_pointed_to), type_node_unqualified(false_pointed_to))) {
      result_type = type_node_composite(cc, type_node_unqualified(true_pointed_to), type_node_unqualified(false_pointed_to));
      result_type = type_base_qualifier(&cc->tb_, result_type, pointed_to_qualifiers);
      result_type = type_base_pointer(&cc->tb_, result_type);
    }
    else {
      cc_error_loc(cc, cond_loc, "conditional operator with incompatible pointer types");
      return 0;
    }
  }

  if (!result_type) {
    cc_error_loc(cc, cond_loc, "conditional operator with incompatible types");
    return 0;
  }

  r = expr_cond_is_true(cc, &condition, cond_loc, cond);
  if (r) {
    return r;
  }

  struct expr *x = expr_alloc(ET_CONDITION);
  if (!x) {
    cc_no_memory(cc);
    expr_free(condition);
    return -1;
  }
  r = expr_cast(cc, x->children_ + 1, cond_loc, result_type, true_loc, true_expr);
  if (!r) {
    r = expr_cast(cc, x->children_ + 2, cond_loc, result_type, false_loc, false_expr);
  }
  if (r) {
    expr_free(condition);
    expr_free(x);
    return r;
  }
  x->children_[0] = condition;
  x->type_arg_ = result_type;
  *cond = *true_expr = *false_expr = NULL;
  *dst = x;
  return 0;
}

int expr_array_index(struct c_compiler *cc, struct expr **dst, struct situs *arr_loc, struct expr **arr, struct situs *op_loc, struct situs *index_loc, struct expr **index) {  
  if ((!*arr) || (!*index)) {
    /* One of the operands is invalid. Simple pass through of an error */
    return -1;
  }
  int r;
  /* both arr and index may be a pointer, eg. the expression "index[pointer]" is valid in C; 
   * consequently try to decay both. */
  if ((r = expr_pointer_decay(cc, arr)) || (r = expr_pointer_decay(cc, index))) {
    return r;
  }
  struct type_node *arr_type, *index_type;
  arr_type = expr_type(cc, *arr);
  if (!arr_type) {
    cc_error_loc(cc, arr_loc, "cannot determine type of array operand\n");
    *dst = NULL;
    return -1;
  }
  index_type = expr_type(cc, *index);
  if (!index_type) {
    cc_error_loc(cc, index_loc, "cannot determine type of index operand\n");
    *dst = NULL;
    return -1;
  }

  int is_arith_left = type_node_is_arithmetic_type(arr_type);
  int is_arith_right = type_node_is_arithmetic_type(index_type);
  int is_integer_arr = type_node_is_integer_type(arr_type);
  int is_integer_index = type_node_is_integer_type(index_type);

  if (is_integer_arr || is_integer_index) {
    struct expr *index_x, *pointer;
    struct situs *index_x_loc;
    struct type_node *pointer_type;
    if (arr_type->kind_ == tk_pointer) {
      index_x = *index;
      index_x_loc = index_loc;
      pointer = *arr;
      pointer_type = arr_type;
    }
    else if (index_type->kind_ == tk_pointer) {
      index_x = *arr;
      index_x_loc = arr_loc;
      pointer = *index;
      pointer_type = index_type;
    }
    else {
      cc_error_loc(cc, op_loc, "types mismatch for array indexing, must be a pointer and an integer");
      return 0;
    }

    enum expr_type et_and, et_add, et_sub, et_c, et_mul, et_lt;
    type_node_uintptr_expr(&cc->tb_, &et_and, &et_add, &et_sub, &et_c, &et_mul, &et_lt);

    struct expr *index_expr = expr_convert_arithmetic_type(cc, type_base_simple(&cc->tb_, cc->tb_.uintptr_equivalent_), index_x);
    if (!index_expr) {
      cc_error_loc(cc, index_x_loc, "unable to convert index type to pointer sized integer");
      return -1;
    }

    struct expr *x = expr_alloc(ET_ADD_PTR_UINTPTR);
    struct expr *size_expr = type_node_size_expr(cc, pointer_type->derived_from_);
    struct expr *offset_expr = expr_alloc(et_mul);
    struct expr *indirection = expr_alloc(ET_INDIRECTION_PTR);
    if (!x || !offset_expr || !size_expr || !indirection) {
      expr_free(x);
      expr_free(size_expr);
      expr_free(index_expr);
      expr_free(offset_expr);
      expr_free(indirection);
      cc_no_memory(cc);
      return -1;
    }
    offset_expr->children_[0] = size_expr;
    offset_expr->children_[1] = index_expr;

    x->children_[0] = pointer;
    x->children_[1] = offset_expr;

    *arr = NULL;
    *index = NULL;
    indirection->children_[0] = x;
    *dst = indirection;
    return 0;
  }
  else {
    cc_error_loc(cc, op_loc, "types mismatch for array indexing, must be a pointer and an integer");
    return 0;
  }
}

static struct expr *expr_args_final_conversions(struct c_compiler *cc, struct type_node *func, struct situs *func_loc, struct type_param **pparam, struct expr *argslist, size_t *pcurrent_index, int *error_reported) {
  if (!argslist) return NULL;
  if (argslist->et_ == ET_FUNCTION_ARG_LIST) {
    argslist->children_[0] = expr_args_final_conversions(cc, func, func_loc, pparam, argslist->children_[0], pcurrent_index, error_reported);
    argslist->children_[1] = expr_args_final_conversions(cc, func, func_loc, pparam, argslist->children_[1], pcurrent_index, error_reported);
    return argslist;
  }
  /* Currently at an argument at index *pcurrent_index . */
  if (*pparam && (*pparam)->type_) {
    /* Assign argslist to dequalified version of (*pparam)->type_. */
    /* XXX: This is not what we want, what we want is to check the semantics; for instance,
     *      for an "int" parameter, expr_convert_type() will happily take a "char*" argument
     *      and convert the pointer value to an int. This is not what you want to have happen
     *      implicitly, we should be issuing errors on these things. Errors such as checked
     *      by expr_assign(). */
    int r = expr_pointer_decay(cc, &argslist);
    if (r) {
      *error_reported = 1;
    }
    else {
      struct expr *x = expr_convert_type(cc, type_node_unqualified((*pparam)->type_), argslist);
      if (!x) {
        *error_reported = 1;
        cc_error_loc(cc, func_loc, "failed convert argument #%zu to parameter type", *pcurrent_index);
      }
      else {
        argslist = x;
      }
    }
  }
  else /* !*pparam || !(*pparam)->type_) */ {
    if (!func->arguments_unknown_ && !*pparam && func->params_ && !func->params_->is_varadic_) {
      /* The number of arguments does not equal the number of parameters and we are not in the
       * variadic part of the arguments, nor is this a function without known arguments. */
      if (!*error_reported) {
        cc_error_loc(cc, func_loc, "the number of arguments mismatches the number of parameters");
        *error_reported = 1;
      }
    }
    int r = expr_pointer_decay(cc, &argslist);
    if (r) {
      *error_reported = 1;
    }
    else {
      /* Default argument promotions (integer promotions, and promote float to double.) */
      int r = expr_integer_promotions(cc, &argslist);
      if (r) {
        *error_reported = 1;
        cc_error_loc(cc, func_loc, "failed integer promotions on argument #%zu", *pcurrent_index);
      }
      else {
        struct type_node *src_type = expr_type(cc, argslist);
        if (!src_type) {
          *error_reported = 1;
          cc_error_loc(cc, func_loc, "failed to determine type of argument #%zu", *pcurrent_index);
        }
        else {
          if (src_type && type_node_unqualified(src_type)->kind_ == tk_float) {
            struct expr *x = expr_convert_type(cc, type_base_simple(&cc->tb_, tk_double), argslist);
            if (!x) {
              *error_reported = 1;
              cc_error_loc(cc, func_loc, "failed float to double promotions on argument #%zu", *pcurrent_index);
            }
            argslist = x;
          }
        }
      }
    }
  }

  /* Step to next param, if any */
  (*pcurrent_index)++;
  if (*pparam) {
    if ((*pparam) != func->params_) {
      (*pparam) = (*pparam)->chain_;
    }
    else {
      (*pparam) = NULL;
    }
  }

  return argslist;
}

int expr_function_call(struct c_compiler *cc, struct expr **dst, struct situs *func_loc, struct expr **func, struct situs *op_loc, struct situs *args_loc, struct expr **args) {  
  if (!*func || (args && !*args)) {
    /* One of the operands is invalid. Simple pass through of an error */
    /* Note that "args" is allowed to be NULL (meaning: no arguments), but "*args" being NULL would signify an error. */
    return 0;
  }
  int r;

  /* Try and decay the possible function designator into a function pointer */
  if (r = expr_pointer_decay(cc, func)) {
    return r;
  }

  struct type_node *func_type;
  func_type = type_node_unqualified(expr_type(cc, *func));
  /* Function after pointer decay should always be a pointer.. */
  if (!func_type || (func_type->kind_ != tk_pointer)) {
    cc_error_loc(cc, func_loc, "expected function");
    return 0;
  }
  func_type = type_node_unqualified(func_type->derived_from_);
  /* func_type should be a pointer ... to a function. */
  if (func_type->kind_ != tk_function) {
    cc_error_loc(cc, func_loc, "expected function");
    return -1;
  }
  
  r = 0;
  if (args) {
    int error_reported = 0;
    size_t current_index = 0;
    struct type_param *param = func_type->params_ ? func_type->params_->chain_ : NULL;
    struct expr *x = expr_args_final_conversions(cc, func_type, func_loc, &param, *args, &current_index, &error_reported);
    if (x) *args = x;
    if (error_reported) {
      /* Bubble up to caller that we failed in the end.. */
      r = -1;
    }
  }

  struct expr *x = expr_alloc(ET_FUNCTION_CALL);
  if (!x) {
    cc_no_memory(cc);
    return -1;
  }
  x->children_[0] = *func;
  x->children_[1] = args ? *args : NULL;
  *func = NULL;
  if (args) *args = NULL;
  *dst = x;
  return r;
}

int expr_ptr_member_access(struct c_compiler *cc, struct expr **dst, struct situs *exp_loc, struct expr **exp, struct situs *op_loc, struct situs *field_loc, const char *field_ident, const char *operator_str) {
  int r;
  if (!*exp) return 0;
  if (!field_ident) return 0;

  if (r = expr_pointer_decay(cc, exp)) {
    return r;
  }

  struct expr *ptr = *exp;
  struct type_node *ptr_type = type_node_unqualified(expr_type(cc, ptr));
  if (!ptr_type) {
    cc_error_loc(cc, exp_loc, "failed to determine type of pointer");
    return -1;
  }

  if (ptr_type->kind_ != tk_pointer) {
    cc_error_loc(cc, exp_loc, "\"%s\" operator needs a pointer to a struct or union", operator_str);
    return 0;
  }

  struct type_node *qualified_struct_type = ptr_type->derived_from_;
  struct type_node *struct_type = type_node_unqualified(qualified_struct_type);

  if ((struct_type->kind_ != tk_union) && (struct_type->kind_ != tk_structure)) {
    cc_error_loc(cc, exp_loc, "\"%s\" operator needs a pointer to a struct or union", operator_str);
    return 0;
  }

  /* Find the field */
  struct type_field *tf = struct_type->fields_;
  struct type_field *field = NULL;
  if (tf) {
    do {
      tf = tf->chain_;

      if (!strcmp(tf->ident_, field_ident)) {
        field = tf;
        break;
      }
    } while (tf != struct_type->fields_);
  }

  if (!field) {
    cc_error_loc(cc, field_loc, "field \"%s\" not found in struct or union type", field_ident);
    return 0;
  }

  /* Qualifiers combine */
  int qualifiers = (qualified_struct_type->kind_ == tk_qualifier) ? qualified_struct_type->qualifiers_ : 0;
  struct type_node *core_field_type = type_node_unqualified(field->type_);
  if (field->type_->kind_ == tk_qualifier) {
    qualifiers |= field->type_->qualifiers_;
  }
  struct type_node *result_type = type_base_qualifier(&cc->tb_, core_field_type, qualifiers);
  struct type_node *ptr_to_result_type = type_base_pointer(&cc->tb_, result_type);
  if (!ptr_to_result_type) {
    cc_no_memory(cc);
    return -1;
  }

  if (struct_type->kind_ == tk_union) {
    /* Union, not a struct. Find the field and cast the pointer to it. Then apply an indirection so it is an lvalue. */
    struct expr *cast = expr_alloc(ET_CVPTR2PTR);
    struct expr *indir = expr_alloc(ET_INDIRECTION_PTR);
    if (!cast || !indir) {
      expr_free(cast);
      expr_free(indir);
      cc_no_memory(cc);
      return -1;
    }
    cast->type_arg_ = ptr_to_result_type;
    cast->children_[0] = *exp;
    *exp = NULL;
    indir->children_[0] = cast;
    *dst = indir;
    return 0;
  }

  /* struct_type->kind_ == tk_structure */

  struct expr *cast = expr_alloc(ET_CVPTR2PTR);
  struct expr *indir = expr_alloc(ET_INDIRECTION_PTR);
  struct expr *offset_expr = type_node_struct_offset_expr(cc, struct_type, field);
  struct expr *add = expr_alloc(ET_ADD_PTR_UINTPTR);
  if (!cast || !indir || !offset_expr || !add) {
    expr_free(cast);
    expr_free(indir);
    expr_free(offset_expr);
    expr_free(add);
    cc_no_memory(cc);
    return -1;
  }
  add->children_[0] = *exp;
  *exp = NULL;
  add->children_[1] = offset_expr;
  cast->type_arg_ = ptr_to_result_type;
  cast->children_[0] = add;
  indir->children_[0] = cast;
  *dst = indir;
  return 0;
}

int expr_lvalue_member_access(struct c_compiler *cc, struct expr **dst, struct situs *exp_loc, struct expr **exp, struct situs *op_loc, struct situs *field_loc, const char *field_ident) {
  if (!*exp) return -1;
  /* Top node should be indirection, access its pointer then use the -> logic. */
  struct expr *x = *exp;
  if (x->et_ != ET_INDIRECTION_PTR) {
    cc_error_loc(cc, exp_loc, "expected an addressable struct or union");
    return 0;
  }
  x = x->children_[0];
  /* Increment reference so we can free the indirection without touching the ptr */
  x->refs_++;
  expr_free(*exp);
  *exp = x;
  return expr_ptr_member_access(cc, dst, exp_loc, exp, op_loc, field_loc, field_ident, ".");
}

int expr_type_is_constant(enum expr_type et) {
  switch (et) {
    case ET_FUNCTION_CALL:
  
    case ET_INDIRECTION_PTR:

    case ET_STORE_LDC:
    case ET_STORE_LDI:
    case ET_STORE_DC:
    case ET_STORE_DI:
    case ET_STORE_FC:
    case ET_STORE_FI:
    case ET_STORE_LD:
    case ET_STORE_D:
    case ET_STORE_F:
    case ET_STORE_SLLI:
    case ET_STORE_SLI:
    case ET_STORE_SI:
    case ET_STORE_SS:
    case ET_STORE_SC:
    case ET_STORE_ULLI:
    case ET_STORE_ULI:
    case ET_STORE_UI:
    case ET_STORE_US:
    case ET_STORE_C:
    case ET_STORE_B:
    case ET_STORE_PTR:
    case ET_COPY:
    case ET_CLEAR:

    /* All variable access, including the access to the pointer of a variable, is non-const */
    case ET_ADDRESS_G:
    case ET_ADDRESS_P:
    case ET_ADDRESS_L:
      return 0;
  }

  return 1;
}


int expr_is_constant(struct expr *x) {
  size_t n;
  int num_operands = expr_num_operands(x);
  int r;
  /* Expression is constant only if all its children are constant..... */
  for (n = 0; n < num_operands; ++n) {
    r = expr_is_constant(x->children_[n]);
    if (!r) {
      x->is_constant_ = 0;
      return 0;
    }
  }

  /* .... and its operator is a constant type operator (no memory, no variables) */
  if (expr_type_is_constant(x->et_)) {
    x->is_constant_ = 1;
    return 1;
  }
  else {
    x->is_constant_ = 0;
    return 0;
  }
}

int expr_find_decl_addressed(struct c_compiler *cc, struct expr *x, struct decl **addressed) {
  int num_operands = expr_num_operands(x);
  int decl_opd = -1;

  if ((x->et_ == ET_ADDRESS_G) || (x->et_ == ET_ADDRESS_E)) {
    *addressed = x->decl_;
    return EXPR_FIND_DECL_ONE_FOUND;
  }

  for (int n = 0; n < num_operands; ++n) {
    struct decl *d = NULL;
    int r;
    r = expr_find_decl_addressed(cc, x->children_[n], &d);
    if (r == EXPR_FIND_DECL_ONE_FOUND) {
      /* Found a decl in this branch.. */
      if (*addressed) {
        /* .. but we already found one in another branch. */
        return EXPR_FIND_DECL_MULTIPLE_FOUND;
      }
      *addressed = d;
      decl_opd = n;
    } 
    else if ((r == EXPR_FIND_DECL_MULTIPLE_FOUND) || (r == EXPR_FIND_DECL_ERROR)) {
      return r;
    }
  }
  if (!*addressed) {
    /* No decl operand, check if this is a constant operation */
    if (!expr_type_is_constant(x->et_)) {
      /* Not a constant operation, invalid for an initializer */
      return EXPR_FIND_DECL_ERROR;
    }

    return EXPR_FIND_DECL_NONE_FOUND;
  }

  /* We found a decl operand in one of the subtrees at operand index decl_opd,
   * Check if this operation is valid on the decl -- we know that all operands other
   * than the subtree are valid constant operations, and we know that the decl operand
   * subtree is a valid constant initializer address expression. Putting these together,
   * is this node also valid?
   * Casts to other pointer types are valid; casts to integer types sufficiently large
   * to hold a pointer is valid. Addition and subtraction to and from a pointer is valid,
   * and finally, addition and subtraction to and from an integer large enough to
   * hold a pointer is valid. */
  enum expr_type et_andu, et_addu, et_subu, et_cu, et_mulu, et_ltu;
  enum expr_type et_ands, et_adds, et_subs, et_cs, et_muls, et_lts;
  type_node_uintptr_expr(&cc->tb_, &et_andu, &et_addu, &et_subu, &et_cu, &et_mulu, &et_ltu);
  type_node_ptrdiff_expr(&cc->tb_, &et_ands, &et_adds, &et_subs, &et_cs, &et_muls, &et_lts);
  int is_valid = 0;
  if ((x->et_ == et_addu) || (x->et_ == et_subu) || (x->et_ == et_adds) || (x->et_ == et_subs)) {
    /* Integer add or sub at the size of a pointer */
    is_valid = 1;
  }
  else {
    switch (x->et_) {
      case ET_ADD_PTR_UINTPTR:
      case ET_SUB_PTR_UINTPTR:
      case ET_SUB_PTR:
        is_valid = 1;
        break;
      case ET_CVPTR2UINTPTR:
      case ET_CVUINTPTR2PTR:
      case ET_CVPTR2PTR:
        is_valid = 1;
        break;
        
      /* Also want to allow casts between integer types that are the size of pointers, or greater in size still */
      case ET_ADD_ULLI:
      case ET_SUB_ULLI:
      case ET_ADD_SLLI:
      case ET_SUB_SLLI:
        is_valid = 1;
        break;

      case ET_ADD_ULI:
      case ET_SUB_ULI:
      case ET_ADD_SLI:
      case ET_SUB_SLI:
        if (cc->tb_.uintptr_equivalent_ == tk_unsigned_int) {
          /* Must be less than "unsigned long int" - if it *is* unsigned long int, we'll already
           * have captured it at type_node_uintptr_expr() above. */
          is_valid = 1;
        }
        break;
    }
  }

  if (!is_valid) {
    return EXPR_FIND_DECL_ERROR;
  }
  if (*addressed) return EXPR_FIND_DECL_ONE_FOUND;
  return EXPR_FIND_DECL_NONE_FOUND;
  
}

void expr_debug_print(struct c_compiler *cc, struct expr *x) {
  if (!x) {
    cc_printf(cc, "null");
    return;
  }
  const char *name = expr_node_type_to_str(x);
  size_t n;
  int have_children = 0;
  cc_printf(cc, "%s@%d", name + 3 /* skip "ET_" for conciseness */, x->ord_);
  switch (x->et_) {
  case ET_C_INT:
  case ET_C_LONG_INT:
  case ET_C_LONG_LONG_INT:
    cc_printf(cc, " %" PRIx64, x->v_.i64_);
    break;
  case ET_C_UNSIGNED_INT:
  case ET_C_UNSIGNED_LONG_INT:
  case ET_C_UNSIGNED_LONG_LONG_INT:
    cc_printf(cc, " %" PRIx64, x->v_.u64_);
    break;
  case ET_C_FLOAT:
    cc_printf(cc, " %f", x->v_.f_);
    break;
  case ET_C_DOUBLE:
  case ET_C_LONG_DOUBLE:
    cc_printf(cc, " %f", x->v_.d_);
    break;
  }
  for (n = 0; n < (sizeof(x->children_))/sizeof(*x->children_); ++n) {
    if (x->children_[n]) {
      if (!n) {
        cc_printf(cc, "(");
        have_children = 1;
      }
      else {
        cc_printf(cc, ", ");
      }
      expr_debug_print(cc, x->children_[n]);
    }
  }
  if (have_children) cc_printf(cc, ")");
}

int expr_print_temp(struct c_compiler *cc, struct expr *x, struct expr_temp *temp) {
  struct type_node *tn = expr_type(cc, x);
  if (!tn) {
    cc_printf(cc, "(Failed to determine expression type)");
    return -1;
  }
  if (type_node_is_arithmetic_type(tn)) {
    switch (type_node_unqualified(tn)->kind_) {
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
      case tk_enumeration:
        cc_printf(cc, "%" PRIi64, temp->v_.i64_);
        break;
      case tk_unsigned_long_long_int:
        cc_printf(cc, "%" PRIu64, temp->v_.u64_);
        break;
      case tk_float:
        cc_printf(cc, "%f", temp->v_.f_);
        break;
      case tk_double:
        cc_printf(cc, "%f", temp->v_.d_);
        break;
      case tk_long_double:
        cc_printf(cc, "%f", temp->v_.d_);
        break;
      case tk_float_complex:
        cc_printf(cc, "%f + %fi", temp->v_.fc_.f_, temp->v_.fc_.fi_);
        break;
      case tk_double_complex:
        cc_printf(cc, "%f + %fi", temp->v_.dc_.d_, temp->v_.dc_.di_);
        break;
      case tk_long_double_complex:
        cc_printf(cc, "%f + %fi", temp->v_.dc_.d_, temp->v_.dc_.di_);
        break;
      case tk_float_imaginary:
        cc_printf(cc, "%fi", temp->v_.fi_);
        break;
      case tk_double_imaginary:
        cc_printf(cc, "%fi", temp->v_.di_);
        break;
      case tk_long_double_imaginary:
        cc_printf(cc, "%fi", temp->v_.di_);
        break;
      default:
        assert(0 && "Unknown arithmetic type");
        return -1;
    }
    return 0;
  }
  cc_printf(cc, "(non-arithmetic type)");
  return -1;
}

int expr_print_value(struct c_compiler *cc, struct expr *x, struct expr_temp *temps) {
  return expr_print_temp(cc, x, temps + x->ord_);
}
