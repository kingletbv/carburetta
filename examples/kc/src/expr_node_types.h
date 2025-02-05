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

#ifndef EXPR_NODE_TYPES_H
#define EXPR_NODE_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

#define ENUM_EXPR_TYPES \
  /* Invalid type, used to signal errors internally, should not appear in actual expression trees */ \
  xx(ET_INVALID, 0, tk_invalid) \
\
  /* No-operation; used internally but should not appear in any actual expression trees */ \
  xx(ET_NOP, 0, tk_invalid) \
\
  /* Conversions, for some of these there are no-ops for the code generated, however,
   * having them helps analyse code and determine the type of expressions by looking at
   * just the top node. */ \
\
  /* Convert Char to Signed Char */ \
  xx(ET_CVC2SC, 1, tk_signed_char) \
\
  /* Convert Signed Char to Char */ \
  xx(ET_CVSC2C, 1, tk_char) \
\
  /* Convert Char to Unsigned Char */ \
  xx(ET_CVC2UC, 1, tk_unsigned_char) \
\
  /* Convert Unsigned Char to Char */ \
  xx(ET_CVUC2C, 1, tk_char) \
\
  /* Convert Signed Char to Unsigned Char */ \
  xx(ET_CVSC2UC, 1, tk_unsigned_char) \
\
  /* Convert Unsigned Char to Signed Char */ \
  xx(ET_CVUC2SC, 1, tk_signed_char) \
\
  /* Convert Signed Char to Signed Short */ \
  xx(ET_CVSC2SS, 1, tk_short_int) \
\
  /* Convert Signed Short to Signed Char */ \
  xx(ET_CVSS2SC, 1, tk_signed_char) \
\
  /* Convert Unsigned Char to Unsigned Short */ \
  xx(ET_CVUC2US, 1, tk_unsigned_short_int) \
\
  /* Convert Unsigned Short to Unsigned Char */ \
  xx(ET_CVUS2UC, 1, tk_unsigned_char) \
\
  /* Convert Signed Short to Unsigned Short */ \
  xx(ET_CVSS2US, 1, tk_unsigned_short_int) \
\
  /* Convert Unsigned Short to Signed Short */ \
  xx(ET_CVUS2SS, 1, tk_short_int) \
\
  /* Convert Signed Short to Signed Int */ \
  xx(ET_CVSS2SI, 1, tk_int) \
\
  /* Convert Signed Int to Signed Short */ \
  xx(ET_CVSI2SS, 1, tk_short_int) \
\
  /* Convert Unsigned Short to Unsigned Int */ \
  xx(ET_CVUS2UI, 1, tk_unsigned_int) \
\
  /* Convert Unsigned Int to Unsigned Short */ \
  xx(ET_CVUI2US, 1, tk_unsigned_short_int) \
\
  /* Convert Signed Int to Unsigned Int */ \
  xx(ET_CVSI2UI, 1, tk_unsigned_int) \
\
  /* Convert Unsigned Int to Signed Int */ \
  xx(ET_CVUI2SI, 1, tk_int) \
\
  /* Convert Signed Int to Signed Long Int */ \
  xx(ET_CVSI2SLI, 1, tk_long_int) \
\
  /* Convert Signed Long Int to Signed Int */ \
  xx(ET_CVSLI2SI, 1, tk_int) \
\
  /* Convert Unsigned Int to Unsigned Long Int */ \
  xx(ET_CVUI2ULI, 1, tk_unsigned_long_int) \
\
  /* Convert Unsigned Long Int to Unsigned Int */ \
  xx(ET_CVULI2UI, 1, tk_unsigned_int) \
\
  /* Convert Signed Long Int to Unsigned Long Int */ \
  xx(ET_CVSLI2ULI, 1, tk_unsigned_long_int) \
\
  /* Convert Unsigned Long Int to Signed Long Int */ \
  xx(ET_CVULI2SLI, 1, tk_long_int) \
\
  /* Convert Signed Long Int to Signed Long Long Int */ \
  xx(ET_CVSLI2SLLI, 1, tk_long_long_int) \
\
  /* Convert Singed Long Long Int to Signed Long Int */ \
  xx(ET_CVSLLI2SLI, 1, tk_long_int) \
\
  /* Convert Unsigned Long Int to Unsigned Long Long Int */ \
  xx(ET_CVULI2ULLI, 1, tk_unsigned_long_long_int) \
\
  /* Convert Unsigned Long Long Int to Unsigned Long Int */ \
  xx(ET_CVULLI2ULI, 1, tk_unsigned_long_int) \
\
  /* Convert Signed Long Long Int to Unsigned Long Long Int */ \
  xx(ET_CVSLLI2ULLI, 1, tk_unsigned_long_long_int) \
\
  /* Convert Unsigned Long Long Int to Signed Long Long Int */ \
  xx(ET_CVULLI2SLLI, 1, tk_long_long_int) \
\
  /* Convert Unsigned Long Int to Float */ \
  xx(ET_CVULI2F, 1, tk_float) \
\
  /* Convert Unsigned Long Long Int to Float */ \
  xx(ET_CVULLI2F, 1, tk_float) \
\
  /* Convert Unsigned Long Int to Double */ \
  xx(ET_CVULI2D, 1, tk_double) \
\
  /* Convert Unsigned Long Long Int to Double */ \
  xx(ET_CVULLI2D, 1, tk_double) \
\
  /* Convert Signed Long Int to Float */ \
  xx(ET_CVSLI2F, 1, tk_float) \
\
  /* Convert Signed Long Long Int to Float */ \
  xx(ET_CVSLLI2F, 1, tk_float) \
\
  /* Convert Signed Long Int to Double */ \
  xx(ET_CVSLI2D, 1, tk_double) \
\
  /* Convert Signed Long Long Int to Double */ \
  xx(ET_CVSLLI2D, 1, tk_double) \
\
  /* Convert Float to Unsigned Long Int */ \
  xx(ET_CVF2ULI, 1, tk_unsigned_long_int) \
\
  /* Convert Float to Unsigned Long Long Int */ \
  xx(ET_CVF2ULLI, 1, tk_unsigned_long_long_int) \
\
  /* Convert Double to Unsigned Long Int */ \
  xx(ET_CVD2ULI, 1, tk_unsigned_long_int) \
\
  /* Convert Double to Unsigned Long Long Int */ \
  xx(ET_CVD2ULLI, 1, tk_unsigned_long_long_int) \
\
  /* Convert Float to Signed Long Int */ \
  xx(ET_CVF2SLI, 1, tk_long_int) \
\
  /* Convert Float to Signed Long Long Int */ \
  xx(ET_CVF2SLLI, 1, tk_long_long_int) \
\
  /* Convert Double to Signed Long Int */ \
  xx(ET_CVD2SLI, 1, tk_long_int) \
\
  /* Convert Double to Signed Long Long Int */ \
  xx(ET_CVD2SLLI, 1, tk_long_long_int) \
\
  /* Convert Float to Double */ \
  xx(ET_CVF2D, 1, tk_double) \
\
  /* Convert Double to Float */ \
  xx(ET_CVD2F, 1, tk_double) \
\
  /* Convert Double to Long Double */ \
  xx(ET_CVD2LD, 1, tk_long_double) \
\
  /* Convert Long Double to Double */ \
  xx(ET_CVLD2D, 1, tk_double) \
\
  /* Convert Float Imaginary to Double Imaginary */ \
  xx(ET_CVFI2DI, 1, tk_double_imaginary) \
\
  /* Convert Double Imaginary to Float Imaginary */ \
  xx(ET_CVDI2FI, 1, tk_float_imaginary) \
\
  /* Convert Double Imaginary to Long Double Imaginary */ \
  xx(ET_CVDI2LDI, 1, tk_long_double_imaginary) \
\
  /* Convert Long Double Imaginary to Double Imaginary */ \
  xx(ET_CVLDI2DI, 1, tk_double_imaginary) \
\
  /* Convert Float Imaginary to Float Complex */ \
  xx(ET_CVFI2FC, 1, tk_float_complex) \
\
  /* Convert Float Complex to Float Imaginary */ \
  xx(ET_CVFC2FI, 1, tk_float_imaginary) \
\
  /* Convert Double Imaginary to Double Complex */ \
  xx(ET_CVDI2DC, 1, tk_double_complex) \
\
  /* Convert Double Complex to Double Imaginary */ \
  xx(ET_CVDC2DI, 1, tk_double_imaginary) \
\
  /* Convert Long Double Imaginary to Long Double Complex */ \
  xx(ET_CVLDI2LDC, 1, tk_long_double_complex) \
\
  /* Convert Long Double Complex to Long Double Imaginary */ \
  xx(ET_CVLDC2LDI, 1, tk_long_double_imaginary) \
\
  /* Convert Float Imaginary to Float */ \
  xx(ET_CVFI2F, 1, tk_float) \
\
  /* Convert Float to Float Imaginary */ \
  xx(ET_CVF2FI, 1, tk_float_imaginary) \
\
  /* Convert Double Imaginary to Double */ \
  xx(ET_CVDI2D, 1, tk_double) \
\
  /* Convert Double to Double Imaginary */ \
  xx(ET_CVD2DI, 1, tk_double_imaginary) \
\
  /* Convert Long Double Imaginary to Long Double */ \
  xx(ET_CVLDI2LD, 1, tk_long_double) \
\
  /* Convert Long Double to Long Double Imaginary */ \
  xx(ET_CVLD2LDI, 1, tk_long_double_imaginary) \
\
  /* Convert Float Complex to Double Complex */ \
  xx(ET_CVFC2DC, 1, tk_double_complex) \
\
  /* Convert Double Complex to Float Complex */ \
  xx(ET_CVDC2FC, 1, tk_float_complex) \
\
  /* Convert Double Complex to Long Double Complex */ \
  xx(ET_CVDC2LDC, 1, tk_long_double_complex) \
\
  /* Convert Long Double Complex to Double Complex */ \
  xx(ET_CVLDC2DC, 1, tk_double_complex) \
\
  /* Convert Float Complex to Float */ \
  xx(ET_CVFC2F, 1, tk_float) \
\
  /* Convert Float to Float Complex */ \
  xx(ET_CVF2FC, 1, tk_float_complex) \
\
  /* Convert Double Complex to Double */ \
  xx(ET_CVDC2D, 1, tk_double) \
\
  /* Convert Double to Double Complex */ \
  xx(ET_CVD2DC, 1, tk_double_complex) \
\
  /* Convert Long Double Complex to Long Double */ \
  xx(ET_CVLDC2LD, 1, tk_long_double) \
\
  /* Convert Long Double to Long Double Complex */ \
  xx(ET_CVLD2LDC, 1, tk_long_double_complex) \
\
  /* Convert Bool to Unsigned Char */ \
  xx(ET_CVB2UC, 1, tk_unsigned_char) \
\
  /* Convert Unsigned Char to Bool */ \
  xx(ET_CVUC2B, 1, tk_bool) \
\
  /* Convert Signed Char to Bool */ \
  xx(ET_CVSC2B, 1, tk_bool) \
\
  /* Convert Unsigned Short to Bool */ \
  xx(ET_CVUS2B, 1, tk_bool) \
\
  /* Convert Signed Short to Bool */ \
  xx(ET_CVSS2B, 1, tk_bool) \
\
  /* Convert Unsigned Int to Bool */ \
  xx(ET_CVUI2B, 1, tk_bool) \
\
  /* Convert Signed Int to Bool */ \
  xx(ET_CVSI2B, 1, tk_bool) \
\
  /* Convert Unsigned Long Int to Bool */ \
  xx(ET_CVULI2B, 1, tk_bool) \
\
  /* Convert Signed Long Int to Bool */ \
  xx(ET_CVSLI2B, 1, tk_bool) \
\
  /* Convert Unsigned Long Long Int to Bool */ \
  xx(ET_CVULLI2B, 1, tk_bool) \
\
  /* Convert Signed Long Long Int to Bool */ \
  xx(ET_CVSLLI2B, 1, tk_bool) \
\
  /* Convert Float to Bool */ \
  xx(ET_CVF2B, 1, tk_bool) \
\
  /* Convert Double to Bool */ \
  xx(ET_CVD2B, 1, tk_bool) \
\
  /* Convert Long Double to Bool */ \
  xx(ET_CVLD2B, 1, tk_bool) \
\
  /* Convert Function to Pointer */ \
  xx(ET_CVFUN2PTR, 1, tk_invalid) \
\
  /* Convert Enumeration to its Compatible Integer Type (stored in type_node::derived_from_) */ \
  xx(ET_CVE2CIT, 1, tk_invalid) \
\
  /* Convert an integer type to an enumeration type. The type converted from must be identical
   * to the enumeration's Compatible Integer Type (stored in enumeration's type_node::derived_from_
   * field.) The enumeration type is stored in the expr::type_arg_ field. */ \
  xx(ET_CVCIT2E, 1, tk_invalid) \
\
  /* Convert Pointer to Uintptr (actual type stored in type_base::uintptr_equivalent_) */ \
  xx(ET_CVPTR2UINTPTR, 1, tk_invalid) \
\
  /* Convert Uintptr (actual type stored in type_base::uintptr_equivalent_) to Pointer (type stored in expr::type_arg_) */ \
  xx(ET_CVUINTPTR2PTR, 1, tk_invalid) \
\
  /* Convert Pointer to Pointer (type stored in expr::type_arg_) */ \
  xx(ET_CVPTR2PTR, 1, tk_invalid) \
\
\
  /* Multiply Long Double Complex to Long Double */ \
  xx(ET_MUL_LDC_LD, 2, tk_long_double_complex) \
\
  /* Multiply Long Double Complex to Long Double Complex */ \
  xx(ET_MUL_LDC_LDC, 2, tk_long_double_complex) \
\
  /* Multiply Long Double Complex to Long Double Imaginary */ \
  xx(ET_MUL_LDC_LDI, 2, tk_long_double_complex) \
\
  /* Multiply Double Complex to Double */ \
  xx(ET_MUL_DC_D, 2, tk_double_complex) \
\
  /* Multiply Double Complex to Double Complex */ \
  xx(ET_MUL_DC_DC, 2, tk_double_complex) \
\
  /* Multiply Double Complex to Double Imaginary */ \
  xx(ET_MUL_DC_DI, 2, tk_double_complex) \
\
  /* Multiply Float Complex to Float */ \
  xx(ET_MUL_FC_F, 2, tk_float_complex) \
\
  /* Multiply Float Complex to Float Complex */ \
  xx(ET_MUL_FC_FC, 2, tk_float_complex) \
\
  /* Multiply Float Complex to Float Imaginary */ \
  xx(ET_MUL_FC_FI, 2, tk_float_complex) \
\
  /* Multiply Long Double Imaginary to Long Double */ \
  xx(ET_MUL_LDI_LD, 2, tk_long_double_imaginary) \
\
  /* Multiply Long Double Imaginary to Long Double Imaginary */ \
  xx(ET_MUL_LDI_LDI, 2, tk_long_double) \
\
  /* Multiply Double Imaginary to Double */ \
  xx(ET_MUL_DI_D, 2, tk_double_imaginary) \
\
  /* Multiply Double Imaginary to Double Imaginary */ \
  xx(ET_MUL_DI_DI, 2, tk_double) \
\
  /* Multiply Float Imaginary to Float */ \
  xx(ET_MUL_FI_F, 2, tk_float_imaginary) \
\
  /* Multiply Float Imaginary to Float Imaginary */ \
  xx(ET_MUL_FI_FI, 2, tk_float) \
\
  /* Multiply Long Double to a Long Double */ \
  xx(ET_MUL_LD, 2, tk_long_double) \
\
  /* Multiply Double to a Double */ \
  xx(ET_MUL_D, 2, tk_double) \
\
  /* Multiply Float to a Float */ \
  xx(ET_MUL_F, 2, tk_float) \
\
  /* Multiply Signed Long Long Int to a Signed Long Long Int */ \
  xx(ET_MUL_SLLI, 2, tk_long_long_int) \
\
  /* Multiply Signed Long Int to a Signed Long Int */ \
  xx(ET_MUL_SLI, 2, tk_long_int) \
\
  /* Multiply Signed Int to a Signed Int */ \
  xx(ET_MUL_SI, 2, tk_int) \
\
  /* Multiply Unsigned Long Long Int to an Unsigned Long Long Int */ \
  xx(ET_MUL_ULLI, 2, tk_unsigned_long_long_int) \
\
  /* Multiply Unsigned Long Int to an Unsigned Long Int */ \
  xx(ET_MUL_ULI, 2, tk_unsigned_long_int) \
\
  /* Multiply Unsigned Int to an Unsigned Int */ \
  xx(ET_MUL_UI, 2, tk_unsigned_int) \
\
\
  /* Long Double Complex Divided by Long Double */ \
  xx(ET_DIV_LDC_LD, 2, tk_long_double_complex) \
\
  /* Long Double Divided by Long Double Complex */ \
  xx(ET_DIV_LD_LDC, 2, tk_long_double_complex) \
\
  /* Long Double Complex Divided by Long Double Complex */ \
  xx(ET_DIV_LDC_LDC, 2, tk_long_double_complex) \
\
  /* Long Double Complex Divided by Long Double Imaginary */ \
  xx(ET_DIV_LDC_LDI, 2, tk_long_double_complex) \
\
  /* Long Double Imaginary Divided by Long Double Complex */ \
  xx(ET_DIV_LDI_LDC, 2, tk_long_double_complex) \
\
  /* Double Complex Divided by Double */ \
  xx(ET_DIV_DC_D, 2, tk_double_complex) \
\
  /* Double Minus Divided by Complex */ \
  xx(ET_DIV_D_DC, 2, tk_double_complex) \
\
  /* Double Complex Divided by Double Complex */ \
  xx(ET_DIV_DC_DC, 2, tk_double_complex) \
\
  /* Double Complex Divided by Double Imaginary */ \
  xx(ET_DIV_DC_DI, 2, tk_double_complex) \
\
  /* Double Imaginary Divided by Double Complex */ \
  xx(ET_DIV_DI_DC, 2, tk_double_complex) \
\
  /* Float Complex Divided by Float */ \
  xx(ET_DIV_FC_F, 2, tk_float_complex) \
\
  /* Float Divided by Float Complex */ \
  xx(ET_DIV_F_FC, 2, tk_float_complex) \
\
  /* Float Complex Divided by Float Complex */ \
  xx(ET_DIV_FC_FC, 2, tk_float_complex) \
\
  /* Float Complex Divided by Float Imaginary */ \
  xx(ET_DIV_FC_FI, 2, tk_float_complex) \
\
  /* Float Imaginary Divided by Float Complex */ \
  xx(ET_DIV_FI_FC, 2, tk_float_complex) \
\
  /* Long Double Imaginary Divided by Long Double */ \
  xx(ET_DIV_LDI_LD, 2, tk_long_double_imaginary) \
\
  /* Long Double Divided by Long Double Imaginary */ \
  xx(ET_DIV_LD_LDI, 2, tk_long_double_imaginary) \
\
  /* Long Double Imaginary Divided by Long Double Imaginary */ \
  xx(ET_DIV_LDI_LDI, 2, tk_long_double) \
\
  /* Double Imaginary Divided by Double */ \
  xx(ET_DIV_DI_D, 2, tk_double_imaginary) \
\
  /* Double Divided by Double Imaginary */ \
  xx(ET_DIV_D_DI, 2, tk_double_imaginary) \
\
  /* Double Imaginary Divided by Double Imaginary */ \
  xx(ET_DIV_DI_DI, 2, tk_double) \
\
  /* Float Imaginary Divided by Float */ \
  xx(ET_DIV_FI_F, 2, tk_float_imaginary) \
\
  /* Float Minus Divided by Imaginary */ \
  xx(ET_DIV_F_FI, 2, tk_float_imaginary) \
\
  /* Float Imaginary Divided by Float Imaginary */ \
  xx(ET_DIV_FI_FI, 2, tk_float) \
\
  /* Long Double Divided by Long Double */ \
  xx(ET_DIV_LD, 2, tk_long_double) \
\
  /* Double Divided by Double */ \
  xx(ET_DIV_D, 2, tk_double) \
\
  /* Float Divided by Float */ \
  xx(ET_DIV_F, 2, tk_float) \
\
  /* Signed Long Long Int Divided by Signed Long Long Int */ \
  xx(ET_DIV_SLLI, 2, tk_long_long_int) \
\
  /* Signed Long Int Divided by Signed Long Int */ \
  xx(ET_DIV_SLI, 2, tk_long_int) \
\
  /* Signed Int Divided by Signed Int */ \
  xx(ET_DIV_SI, 2, tk_int) \
\
  /* Unsigned Long Long Int Divided by Unsigned Long Long Int */ \
  xx(ET_DIV_ULLI, 2, tk_unsigned_long_long_int) \
\
  /* Unsigned Long Int Divided by Unsigned Long Int */ \
  xx(ET_DIV_ULI, 2, tk_unsigned_long_int) \
\
  /* Unsigned Int Divided by Unsigned Int */ \
  xx(ET_DIV_UI, 2, tk_unsigned_int) \
\
\
  /* Signed Long Long Int Remainder */ \
  xx(ET_REM_SLLI, 2, tk_long_long_int) \
\
  /* Signed Long Int Remainder */ \
  xx(ET_REM_SLI, 2, tk_long_int) \
\
  /* Signed Int Remainder */ \
  xx(ET_REM_SI, 2, tk_int) \
\
  /* Unsigned Long Long Int Remainder */ \
  xx(ET_REM_ULLI, 2, tk_unsigned_long_long_int) \
\
  /* Unsigned Long Int Remainder */ \
  xx(ET_REM_ULI, 2, tk_unsigned_long_int) \
\
  /* Unsigned Int Remainder */ \
  xx(ET_REM_UI, 2, tk_unsigned_int) \
\
\
  /* Add Long Double Complex to Long Double */ \
  xx(ET_ADD_LDC_LD, 2, tk_long_double_complex) \
\
  /* Add Long Double Complex to Long Double Complex */ \
  xx(ET_ADD_LDC_LDC, 2, tk_long_double_complex) \
\
  /* Add Long Double Complex to Long Double Imaginary */ \
  xx(ET_ADD_LDC_LDI, 2, tk_long_double_complex) \
\
  /* Add Double Complex to Double */ \
  xx(ET_ADD_DC_D, 2, tk_double_complex) \
\
  /* Add Double Complex to Double Complex */ \
  xx(ET_ADD_DC_DC, 2, tk_double_complex) \
\
  /* Add Double Complex to Double Imaginary */ \
  xx(ET_ADD_DC_DI, 2, tk_double_complex) \
\
  /* Add Float Complex to Float */ \
  xx(ET_ADD_FC_F, 2, tk_float_complex) \
\
  /* Add Float Complex to Float Complex */ \
  xx(ET_ADD_FC_FC, 2, tk_float_complex) \
\
  /* Add Float Complex to Float Imaginary */ \
  xx(ET_ADD_FC_FI, 2, tk_float_complex) \
\
  /* Add Long Double Imaginary to Long Double */ \
  xx(ET_ADD_LDI_LD, 2, tk_long_double_complex) \
\
  /* Add Long Double Imaginary to Long Double Imaginary */ \
  xx(ET_ADD_LDI_LDI, 2, tk_long_double_imaginary) \
\
  /* Add Double Imaginary to Double */ \
  xx(ET_ADD_DI_D, 2, tk_double_complex) \
\
  /* Add Double Imaginary to Double Imaginary */ \
  xx(ET_ADD_DI_DI, 2, tk_double_imaginary) \
\
  /* Add Float Imaginary to Float */ \
  xx(ET_ADD_FI_F, 2, tk_float_complex) \
\
  /* Add Float Imaginary to Float Imaginary */ \
  xx(ET_ADD_FI_FI, 2, tk_float_imaginary) \
\
  /* Add Long Double to a Long Double */ \
  xx(ET_ADD_LD, 2, tk_long_double) \
\
  /* Add Double to a Double */ \
  xx(ET_ADD_D, 2, tk_double) \
\
  /* Add Float to a Float */ \
  xx(ET_ADD_F, 2, tk_float) \
\
  /* Add Signed Long Long Int to a Signed Long Long Int */ \
  xx(ET_ADD_SLLI, 2, tk_long_long_int) \
\
  /* Add Signed Long Int to a Signed Long Int */ \
  xx(ET_ADD_SLI, 2, tk_long_int) \
\
  /* Add Signed Int to a Signed Int */ \
  xx(ET_ADD_SI, 2, tk_int) \
\
  /* Add Unsigned Long Long Int to an Unsigned Long Long Int */ \
  xx(ET_ADD_ULLI, 2, tk_unsigned_long_long_int) \
\
  /* Add Unsigned Long Int to an Unsigned Long Int */ \
  xx(ET_ADD_ULI, 2, tk_unsigned_long_int) \
\
  /* Add Unsigned Int to an Unsigned Int */ \
  xx(ET_ADD_UI, 2, tk_unsigned_int) \
\
\
  /* Long Double Complex Minus Long Double */ \
  xx(ET_SUB_LDC_LD, 2, tk_long_double_complex) \
\
  /* Long Double Minus Long Double Complex */ \
  xx(ET_SUB_LD_LDC, 2, tk_long_double_complex) \
\
  /* Long Double Complex Minus Long Double Complex */ \
  xx(ET_SUB_LDC_LDC, 2, tk_long_double_complex) \
\
  /* Long Double Complex Minus Long Double Imaginary */ \
  xx(ET_SUB_LDC_LDI, 2, tk_long_double_complex) \
\
  /* Long Double Imaginary Minus Long Double Complex */ \
  xx(ET_SUB_LDI_LDC, 2, tk_long_double_complex) \
\
  /* Double Complex Minus Double */ \
  xx(ET_SUB_DC_D, 2, tk_double_complex) \
\
  /* Double Minus Double Complex */ \
  xx(ET_SUB_D_DC, 2, tk_double_complex) \
\
  /* Double Complex Minus Double Complex */ \
  xx(ET_SUB_DC_DC, 2, tk_double_complex) \
\
  /* Double Complex Minus Double Imaginary */ \
  xx(ET_SUB_DC_DI, 2, tk_double_complex) \
\
  /* Double Imaginary Minus Double Complex */ \
  xx(ET_SUB_DI_DC, 2, tk_double_complex) \
\
  /* Float Complex Minus Float */ \
  xx(ET_SUB_FC_F, 2, tk_float_complex) \
\
  /* Float Minus Float Complex */ \
  xx(ET_SUB_F_FC, 2, tk_float_complex) \
\
  /* Float Complex Minus Float Complex */ \
  xx(ET_SUB_FC_FC, 2, tk_float_complex) \
\
  /* Float Complex Minus Float Imaginary */ \
  xx(ET_SUB_FC_FI, 2, tk_float_complex) \
\
  /* Float Imaginary Minus Float Complex */ \
  xx(ET_SUB_FI_FC, 2, tk_float_complex) \
\
  /* Long Double Imaginary Minus Long Double */ \
  xx(ET_SUB_LDI_LD, 2, tk_long_double_complex) \
\
  /* Long Double Minus Long Double Imaginary */ \
  xx(ET_SUB_LD_LDI, 2, tk_long_double_complex) \
\
  /* Long Double Imaginary Minus Long Double Imaginary */ \
  xx(ET_SUB_LDI_LDI, 2, tk_long_double_imaginary) \
\
  /* Double Imaginary Minus Double */ \
  xx(ET_SUB_DI_D, 2, tk_double_complex) \
\
  /* Double Minus Double Imaginary */ \
  xx(ET_SUB_D_DI, 2, tk_double_complex) \
\
  /* Double Imaginary Minus Double Imaginary */ \
  xx(ET_SUB_DI_DI, 2, tk_double_imaginary) \
\
  /* Float Imaginary Minus Float */ \
  xx(ET_SUB_FI_F, 2, tk_float_complex) \
\
  /* Float Minus Float Imaginary */ \
  xx(ET_SUB_F_FI, 2, tk_float_complex) \
\
  /* Float Imaginary Minus Float Imaginary */ \
  xx(ET_SUB_FI_FI, 2, tk_float_imaginary) \
\
  /* Long Double Minus Long Double */ \
  xx(ET_SUB_LD, 2, tk_long_double) \
\
  /* Double Minus Double */ \
  xx(ET_SUB_D, 2, tk_double) \
\
  /* Float Minus Float */ \
  xx(ET_SUB_F, 2, tk_float) \
\
  /* Signed Long Long Int Minus Signed Long Long Int */ \
  xx(ET_SUB_SLLI, 2, tk_long_long_int) \
\
  /* Signed Long Int Minus Signed Long Int */ \
  xx(ET_SUB_SLI, 2, tk_long_int) \
\
  /* Signed Int Minus Signed Int */ \
  xx(ET_SUB_SI, 2, tk_int) \
\
  /* Unsigned Long Long Int Minus Unsigned Long Long Int */ \
  xx(ET_SUB_ULLI, 2, tk_unsigned_long_long_int) \
\
  /* Unsigned Long Int Minus Unsigned Long Int */ \
  xx(ET_SUB_ULI, 2, tk_unsigned_long_int) \
\
  /* Unsigned Int Minus Unsigned Int */ \
  xx(ET_SUB_UI, 2, tk_unsigned_int) \
\
\
  /* Int Constant */ \
  xx(ET_C_INT, 0, tk_int) \
\
  /* Long Int Constant */ \
  xx(ET_C_LONG_INT, 0, tk_long_int) \
\
  /* Long Long Int Constant */ \
  xx(ET_C_LONG_LONG_INT, 0, tk_long_long_int) \
\
  /* Unsigned Int Constant */ \
  xx(ET_C_UNSIGNED_INT, 0, tk_unsigned_int) \
\
  /* Unsigned Long Int Constant */ \
  xx(ET_C_UNSIGNED_LONG_INT, 0, tk_unsigned_long_int) \
\
  /* Unsigned Long Long Int Constant */ \
  xx(ET_C_UNSIGNED_LONG_LONG_INT, 0, tk_unsigned_long_long_int) \
\
  /* Double Constant */ \
  xx(ET_C_DOUBLE, 0, tk_double) \
\
  /* Long Double Constant */ \
  xx(ET_C_LONG_DOUBLE, 0, tk_long_double) \
\
  /* Float Constant */ \
  xx(ET_C_FLOAT, 0, tk_float) \
\
  /* String Literal (type is "const char[len]") */ \
  xx(ET_C_STRING_LIT, 0, tk_invalid) \
\
  /* Wide String Literal (type is "const unsigned short[len]") */ \
  xx(ET_C_WIDE_STRING_LIT, 0, tk_invalid) \
\
  /* Unsigned Short Int Constant (used for wchar_t wide character constants) */ \
  xx(ET_C_UNSIGNED_SHORT_INT, 0, tk_unsigned_short_int) \
\
\
  /* Add Pointer to Pointer Sized Integer (the type of integer is type_base::uintptr_equivalent_.)
   * The pointer is in children_[0], the integer index value in children_[1].
   * This can be seen as a uintptr sized shorthand for a ET_CVPTR2UINTPTR, ET_ADD_xxx, ET_CVUINTPTR2PTR. 
   * (The size of the type pointed to is irrelevant for this operation.) */ \
  xx(ET_ADD_PTR_UINTPTR, 2, tk_invalid) \
\
  /* Pointer Minus Pointer Sized Integer (the type of integer is type_base::uintptr_equivalent_.)
   * The pointer is in children_[0], the integer index value in children_[1].
   * This can be seen as a uintptr sized shorthand for a ET_CVPTR2UINTPTR, ET_SUB_xxx, ET_CVUINTPTR2PTR. 
   * (The size of the type pointed to is irrelevant for this operation.) */ \
  xx(ET_SUB_PTR_UINTPTR, 2, tk_invalid) \
\
  /* Subtract Pointer from Pointer, result type is type_base::ptrdiff_equivalent_. */ \
  xx(ET_SUB_PTR, 2, tk_invalid) \
\
\
  /* Unary Plus, this is a no-op, but reflects the presence of the + operator in the expression. 
   * The effect of the + operator is that it performs the integer promotions on its operand. */ \
\
  /* Unary Plus of Long Double Complex */ \
  xx(ET_UNARY_PLUS_LDC, 1, tk_long_double_complex) \
\
  /* Unary Plus of Long Double Imaginary */ \
  xx(ET_UNARY_PLUS_LDI, 1, tk_long_double_imaginary) \
\
  /* Unary Plus of Double Complex */ \
  xx(ET_UNARY_PLUS_DC, 1, tk_double_complex) \
\
  /* Unary Plus of Double Imaginary */ \
  xx(ET_UNARY_PLUS_DI, 1, tk_double_imaginary) \
\
  /* Unary Plus of Float Complex */ \
  xx(ET_UNARY_PLUS_FC, 1, tk_float_complex) \
\
  /* Unary Plus of Float Imaginary */ \
  xx(ET_UNARY_PLUS_FI, 1, tk_float_imaginary) \
\
  /* Unary Plus of Long Double */ \
  xx(ET_UNARY_PLUS_LD, 1, tk_long_double) \
\
  /* Unary Plus of Double */ \
  xx(ET_UNARY_PLUS_D, 1, tk_double) \
\
  /* Unary Plus of Float */ \
  xx(ET_UNARY_PLUS_F, 1, tk_float) \
\
  /* Unary Plus of Signed Long Long Integer */ \
  xx(ET_UNARY_PLUS_SLLI, 1, tk_long_long_int) \
\
  /* Unary Plus of Signed Long Integer */ \
  xx(ET_UNARY_PLUS_SLI, 1, tk_int) \
\
  /* Unary Plus of Signed Integer */ \
  xx(ET_UNARY_PLUS_SI, 1, tk_int) \
\
  /* Unary Plus of Unsigned Long Long Integer */ \
  xx(ET_UNARY_PLUS_ULLI, 1, tk_unsigned_long_long_int) \
\
  /* Unary Plus of Unsigned Long Integer */ \
  xx(ET_UNARY_PLUS_ULI, 1, tk_unsigned_long_int) \
\
  /* Unary Plus of Unsigned Integer */ \
  xx(ET_UNARY_PLUS_UI, 1, tk_unsigned_int) \
\
\
  /* Unary Minus of Long Double Complex */ \
  xx(ET_UNARY_MINUS_LDC, 1, tk_long_double_complex) \
\
  /* Unary Minus of Long Double Imaginary */ \
  xx(ET_UNARY_MINUS_LDI, 1, tk_long_double_imaginary) \
\
  /* Unary Minus of Double Complex */ \
  xx(ET_UNARY_MINUS_DC, 1, tk_double_complex) \
\
  /* Unary Minus of Double Imaginary */ \
  xx(ET_UNARY_MINUS_DI, 1, tk_double_imaginary) \
\
  /* Unary Minus of Float Complex */ \
  xx(ET_UNARY_MINUS_FC, 1, tk_float_complex) \
\
  /* Unary Minus of Float Imaginary */ \
  xx(ET_UNARY_MINUS_FI, 1, tk_float_imaginary) \
\
  /* Unary Minus of Long Double */ \
  xx(ET_UNARY_MINUS_LD, 1, tk_long_double) \
\
  /* Unary Minus of Double */ \
  xx(ET_UNARY_MINUS_D, 1, tk_double) \
\
  /* Unary Minus of Float */ \
  xx(ET_UNARY_MINUS_F, 1, tk_float) \
\
  /* Unary Minus of Signed Long Long Integer */ \
  xx(ET_UNARY_MINUS_SLLI, 1, tk_long_long_int) \
\
  /* Unary Minus of Signed Long Integer */ \
  xx(ET_UNARY_MINUS_SLI, 1, tk_int) \
\
  /* Unary Minus of Signed Integer */ \
  xx(ET_UNARY_MINUS_SI, 1, tk_int) \
\
  /* Unary Minus of Unsigned Long Long Integer */ \
  xx(ET_UNARY_MINUS_ULLI, 1, tk_unsigned_long_long_int) \
\
  /* Unary Minus of Unsigned Long Integer */ \
  xx(ET_UNARY_MINUS_ULI, 1, tk_unsigned_long_int) \
\
  /* Unary Minus of Unsigned Integer */ \
  xx(ET_UNARY_MINUS_UI, 1, tk_unsigned_int) \
\
\
  /* Bitwise Complement of Signed Long Long Int */ \
  xx(ET_BITWISE_COMPLEMENT_SLLI, 1, tk_long_long_int) \
\
  /* Bitwise Complement of Signed Long Int */ \
  xx(ET_BITWISE_COMPLEMENT_SLI, 1, tk_long_int) \
\
  /* Bitwise Complement of Signed Int */ \
  xx(ET_BITWISE_COMPLEMENT_SI, 1, tk_int) \
\
  /* Bitwise Complement of Unsigned Long Long Int */ \
  xx(ET_BITWISE_COMPLEMENT_ULLI, 1, tk_unsigned_long_long_int) \
\
  /* Bitwise Complement of Unsigned Long Int */ \
  xx(ET_BITWISE_COMPLEMENT_ULI, 1, tk_unsigned_long_int) \
\
  /* Bitwise Complement of Unsigned Int */ \
  xx(ET_BITWISE_COMPLEMENT_UI, 1, tk_unsigned_int) \
\
\
  /* Indirection of a Pointer. Because all pointer types are accepted and the type
   * of the result should maintain its qualifiers, it is modeled as a single node type. */ \
  xx(ET_INDIRECTION_PTR, 1, tk_invalid) \
\
\
  /* Left Shift Signed Long Long Int */ \
  xx(ET_SHIFT_LEFT_SLLI, 2, tk_long_long_int) \
\
  /* Left Shift Signed Long Int */ \
  xx(ET_SHIFT_LEFT_SLI, 2, tk_long_int) \
\
  /* Left Shift Signed Int */ \
  xx(ET_SHIFT_LEFT_SI, 2, tk_int) \
\
  /* Left Shift Unsigned Long Long Int */ \
  xx(ET_SHIFT_LEFT_ULLI, 2, tk_unsigned_long_long_int) \
\
  /* Left Shift Unsigned Long Int */ \
  xx(ET_SHIFT_LEFT_ULI, 2, tk_unsigned_long_int) \
\
  /* Left Shift Unsigned Int */ \
  xx(ET_SHIFT_LEFT_UI, 2, tk_unsigned_int) \
\
\
  /* Right Shift Signed Long Long Int */ \
  xx(ET_SHIFT_RIGHT_SLLI, 2, tk_long_long_int) \
\
  /* Right Shift Signed Long Int */ \
  xx(ET_SHIFT_RIGHT_SLI, 2, tk_long_int) \
\
  /* Right Shift Signed Int */ \
  xx(ET_SHIFT_RIGHT_SI, 2, tk_int) \
\
  /* Right Shift Unsigned Long Long Int */ \
  xx(ET_SHIFT_RIGHT_ULLI, 2, tk_unsigned_long_long_int) \
\
  /* Right Shift Unsigned Long Int */ \
  xx(ET_SHIFT_RIGHT_ULI, 2, tk_unsigned_long_int) \
\
  /* Right Shift Unsigned Int */ \
  xx(ET_SHIFT_RIGHT_UI, 2, tk_unsigned_int) \
\
\
  /* Pointer compare Less than */ \
  xx(ET_LESS_THAN_PTR, 2, tk_int) \
\
  /* Long Double Less than */ \
  xx(ET_LESS_THAN_LD, 2, tk_int) \
\
  /* Double less than */ \
  xx(ET_LESS_THAN_D, 2, tk_int) \
\
  /* Float less than */ \
  xx(ET_LESS_THAN_F, 2, tk_int) \
\
  /* Signed Long Long Int compare Less than */ \
  xx(ET_LESS_THAN_SLLI, 2, tk_int) \
\
  /* Signed Long Int compare Less than */ \
  xx(ET_LESS_THAN_SLI, 2, tk_int) \
\
  /* Signed Int compare Less than */ \
  xx(ET_LESS_THAN_SI, 2, tk_int) \
\
  /* Unsigned Long Long Int compare Less than */ \
  xx(ET_LESS_THAN_ULLI, 2, tk_int) \
\
  /* Unsigned Long Int compare Less than */ \
  xx(ET_LESS_THAN_ULI, 2, tk_int) \
\
  /* Unsigned Int compare Less than */ \
  xx(ET_LESS_THAN_UI, 2, tk_int) \
\
\
  /* Pointer compare Greater than */ \
  xx(ET_GREATER_THAN_PTR, 2, tk_int) \
\
  /* Long Double Greater than */ \
  xx(ET_GREATER_THAN_LD, 2, tk_int) \
\
  /* Double greater than */ \
  xx(ET_GREATER_THAN_D, 2, tk_int) \
\
  /* Float greater than */ \
  xx(ET_GREATER_THAN_F, 2, tk_int) \
\
  /* Signed Long Long Int compare Greater than */ \
  xx(ET_GREATER_THAN_SLLI, 2, tk_int) \
\
  /* Signed Long Int compare Greater than */ \
  xx(ET_GREATER_THAN_SLI, 2, tk_int) \
\
  /* Signed Int compare Greater than */ \
  xx(ET_GREATER_THAN_SI, 2, tk_int) \
\
  /* Unsigned Long Long Int compare Greater than */ \
  xx(ET_GREATER_THAN_ULLI, 2, tk_int) \
\
  /* Unsigned Long Int compare Greater than */ \
  xx(ET_GREATER_THAN_ULI, 2, tk_int) \
\
  /* Unsigned Int compare Greater than */ \
  xx(ET_GREATER_THAN_UI, 2, tk_int) \
\
\
  /* Pointer compare Less than or equal */ \
  xx(ET_LESS_THAN_OR_EQUAL_PTR, 2, tk_int) \
\
  /* Long Double Less than or equal */ \
  xx(ET_LESS_THAN_OR_EQUAL_LD, 2, tk_int) \
\
  /* Double less than or equal */ \
  xx(ET_LESS_THAN_OR_EQUAL_D, 2, tk_int) \
\
  /* Float less than or equal */ \
  xx(ET_LESS_THAN_OR_EQUAL_F, 2, tk_int) \
\
  /* Signed Long Long Int compare Less than or equal */ \
  xx(ET_LESS_THAN_OR_EQUAL_SLLI, 2, tk_int) \
\
  /* Signed Long Int compare Less than or equal */ \
  xx(ET_LESS_THAN_OR_EQUAL_SLI, 2, tk_int) \
\
  /* Signed Int compare Less than or equal */ \
  xx(ET_LESS_THAN_OR_EQUAL_SI, 2, tk_int) \
\
  /* Unsigned Long Long Int compare Less than or equal */ \
  xx(ET_LESS_THAN_OR_EQUAL_ULLI, 2, tk_int) \
\
  /* Unsigned Long Int compare Less than or equal */ \
  xx(ET_LESS_THAN_OR_EQUAL_ULI, 2, tk_int) \
\
  /* Unsigned Int compare Less than or equal */ \
  xx(ET_LESS_THAN_OR_EQUAL_UI, 2, tk_int) \
\
\
  /* Pointer compare Greater than */ \
  xx(ET_GREATER_THAN_OR_EQUAL_PTR, 2, tk_int) \
\
  /* Long Double Greater than or equal */ \
  xx(ET_GREATER_THAN_OR_EQUAL_LD, 2, tk_int) \
\
  /* Double Greater than or equal */ \
  xx(ET_GREATER_THAN_OR_EQUAL_D, 2, tk_int) \
\
  /* Float Greater than or equal */ \
  xx(ET_GREATER_THAN_OR_EQUAL_F, 2, tk_int) \
\
  /* Signed Long Long Int compare Greater than */ \
  xx(ET_GREATER_THAN_OR_EQUAL_SLLI, 2, tk_int) \
\
  /* Signed Long Int compare Greater than */ \
  xx(ET_GREATER_THAN_OR_EQUAL_SLI, 2, tk_int) \
\
  /* Signed Int compare Greater than */ \
  xx(ET_GREATER_THAN_OR_EQUAL_SI, 2, tk_int) \
\
  /* Unsigned Long Long Int compare Greater than */ \
  xx(ET_GREATER_THAN_OR_EQUAL_ULLI, 2, tk_int) \
\
  /* Unsigned Long Int compare Greater than */ \
  xx(ET_GREATER_THAN_OR_EQUAL_ULI, 2, tk_int) \
\
  /* Unsigned Int compare Greater than */ \
  xx(ET_GREATER_THAN_OR_EQUAL_UI, 2, tk_int) \
\
\
  /* Store nodes evaluate to their right-side value */ \
  /* Store Long Double Complex */ \
  xx(ET_STORE_LDC, 2, tk_long_double_complex) \
\
  /* Store Long Double Imaginary */ \
  xx(ET_STORE_LDI, 2, tk_long_double_imaginary) \
\
  /* Store Double Complex */ \
  xx(ET_STORE_DC, 2, tk_double_complex) \
\
  /* Store Double Imaginary */ \
  xx(ET_STORE_DI, 2, tk_double_imaginary) \
\
  /* Store Float Complex */ \
  xx(ET_STORE_FC, 2, tk_float_complex) \
\
  /* Store Float Imaginary */ \
  xx(ET_STORE_FI, 2, tk_float_imaginary) \
\
  /* Store Long Double */ \
  xx(ET_STORE_LD, 2, tk_long_double) \
\
  /* Store Double */ \
  xx(ET_STORE_D, 2, tk_double) \
\
  /* Store Float */ \
  xx(ET_STORE_F, 2, tk_float) \
\
  /* Store Signed Long Long Int */ \
  xx(ET_STORE_SLLI, 2, tk_long_long_int) \
\
  /* Store Signed Long Int */ \
  xx(ET_STORE_SLI, 2, tk_long_int) \
\
  /* Store Signed Int */ \
  xx(ET_STORE_SI, 2, tk_int) \
\
  /* Store Signed Short */ \
  xx(ET_STORE_SS, 2, tk_short_int) \
\
  /* Store Signed Char */ \
  xx(ET_STORE_SC, 2, tk_signed_char) \
\
  /* Store Unsigned Long Long Int */ \
  xx(ET_STORE_ULLI, 2, tk_unsigned_long_long_int) \
\
  /* Store Unsigned Long Int */ \
  xx(ET_STORE_ULI, 2, tk_unsigned_long_int) \
\
  /* Store Unsigned Int */ \
  xx(ET_STORE_UI, 2, tk_unsigned_int) \
\
  /* Store Unsigned Short */ \
  xx(ET_STORE_US, 2, tk_unsigned_short_int) \
\
  /* Store Unsigned Char */ \
  xx(ET_STORE_UC, 2, tk_unsigned_char) \
\
  /* Store Char */ \
  xx(ET_STORE_C, 2, tk_char) \
\
  /* Store Boolean */ \
  xx(ET_STORE_B, 2, tk_bool) \
\
  /* Store Pointer */ \
  xx(ET_STORE_PTR, 2, tk_invalid) \
\
  /* Copies bytes from the pointer in argument 1 to the pointer in argument 0; the number
   * of bytes to copy is specified in argument 3 (and should be constant.) */ \
  xx(ET_COPY, 3, tk_invalid) \
\
  /* Clears bytes to 0 at the pointer in argument 0; the number of bytes to clear is
   * specified in argument 1 (and should be constant.) */ \
  xx(ET_CLEAR, 2, tk_invalid) \
\
\
  /* Sequence, evaluates the first child, ignores its results, and
   * then evaluates the second child, returning its results as its own.
   * This is used for the "comma" operator, but also for postfix increment
   * and decrement (where the result returned is different from the increment
   * and store operation.)
   */ \
  xx(ET_SEQ, 2, tk_invalid) \
\
\
  /* External address (variable or function) */ \
  xx(ET_ADDRESS_E, 0, tk_invalid) \
\
  /* Global variable address */ \
  xx(ET_ADDRESS_G, 0, tk_invalid) \
\
  /* Local variable address */ \
  xx(ET_ADDRESS_L, 0, tk_invalid) \
\
  /* Parameter variable address */ \
  xx(ET_ADDRESS_P, 0, tk_invalid) \
\
  /* Return value address */ \
  xx(ET_ADDRESS_R, 0, tk_invalid) \
\
\
  /* Bitwise And Signed Long Long Int */ \
  xx(ET_BITWISE_AND_SLLI, 2, tk_long_long_int) \
\
  /* Bitwise And Signed Long Int */ \
  xx(ET_BITWISE_AND_SLI, 2, tk_long_int) \
\
  /* Bitwise And Signed Int */ \
  xx(ET_BITWISE_AND_SI, 2, tk_int) \
\
  /* Bitwise And Unsigned Long Long Int */ \
  xx(ET_BITWISE_AND_ULLI, 2, tk_unsigned_long_long_int) \
\
  /* Bitwise And Unsigned Long Int */ \
  xx(ET_BITWISE_AND_ULI, 2, tk_unsigned_long_int) \
\
  /* Bitwise And Unsigned Int */ \
  xx(ET_BITWISE_AND_UI, 2, tk_unsigned_int) \
\
\
  /* Bitwise Or Signed Long Long Int */ \
  xx(ET_BITWISE_OR_SLLI, 2, tk_long_long_int) \
\
  /* Bitwise Or Signed Long Int */ \
  xx(ET_BITWISE_OR_SLI, 2, tk_long_int) \
\
  /* Bitwise Or Signed Int */ \
  xx(ET_BITWISE_OR_SI, 2, tk_int) \
\
  /* Bitwise Or Unsigned Long Long Int */ \
  xx(ET_BITWISE_OR_ULLI, 2, tk_unsigned_long_long_int) \
\
  /* Bitwise Or Unsigned Long Int */ \
  xx(ET_BITWISE_OR_ULI, 2, tk_unsigned_long_int) \
\
  /* Bitwise Or Unsigned Int */ \
  xx(ET_BITWISE_OR_UI, 2, tk_unsigned_int) \
\
\
  /* Bitwise Exclusive-Oor Signed Long Long Int */ \
  xx(ET_BITWISE_XOR_SLLI, 2, tk_long_long_int) \
\
  /* Bitwise Exclusive-Or Signed Long Int */ \
  xx(ET_BITWISE_XOR_SLI, 2, tk_long_int) \
\
  /* Bitwise Exclusive-Or Signed Int */ \
  xx(ET_BITWISE_XOR_SI, 2, tk_int) \
\
  /* Bitwise Exclusive-Or Unsigned Long Long Int */ \
  xx(ET_BITWISE_XOR_ULLI, 2, tk_unsigned_long_long_int) \
\
  /* Bitwise Exclusive-Or Unsigned Long Int */ \
  xx(ET_BITWISE_XOR_ULI, 2, tk_unsigned_long_int) \
\
  /* Bitwise Exclusive-Or Unsigned Int */ \
  xx(ET_BITWISE_XOR_UI, 2, tk_unsigned_int) \
\
\
  /* Compare for Equality Long Double Complex to Long Double */ \
  xx(ET_EQU_LDC_LD, 2, tk_int) \
\
  /* Compare for Equality Long Double Complex to Long Double Complex */ \
  xx(ET_EQU_LDC_LDC, 2, tk_int) \
\
  /* Compare for Equality Long Double Complex to Long Double Imaginary */ \
  xx(ET_EQU_LDC_LDI, 2, tk_int) \
\
  /* Compare for Equality Double Complex to Double */ \
  xx(ET_EQU_DC_D, 2, tk_int) \
\
  /* Compare for Equality Double Complex to Double Complex */ \
  xx(ET_EQU_DC_DC, 2, tk_int) \
\
  /* Compare for Equality Double Complex to Double Imaginary */ \
  xx(ET_EQU_DC_DI, 2, tk_int) \
\
  /* Compare for Equality Float Complex to Float */ \
  xx(ET_EQU_FC_F, 2, tk_int) \
\
  /* Compare for Equality Float Complex to Float Complex */ \
  xx(ET_EQU_FC_FC, 2, tk_int) \
\
  /* Compare for Equality Float Complex to Float Imaginary */ \
  xx(ET_EQU_FC_FI, 2, tk_int) \
\
  /* Compare for Equality Long Double Imaginary to Long Double */ \
  xx(ET_EQU_LDI_LD, 2, tk_int) \
\
  /* Compare for Equality Long Double Imaginary to Long Double Imaginary */ \
  xx(ET_EQU_LDI_LDI, 2, tk_int) \
\
  /* Compare for Equality Double Imaginary to Double */ \
  xx(ET_EQU_DI_D, 2, tk_int) \
\
  /* Compare for Equality Double Imaginary to Double Imaginary */ \
  xx(ET_EQU_DI_DI, 2, tk_int) \
\
  /* Compare for Equality Float Imaginary to Float */ \
  xx(ET_EQU_FI_F, 2, tk_int) \
\
  /* Compare for Equality Float Imaginary to Float Imaginary */ \
  xx(ET_EQU_FI_FI, 2, tk_int) \
\
  /* Compare for Equality Long Double to a Long Double */ \
  xx(ET_EQU_LD, 2, tk_int) \
\
  /* Compare for Equality Double to a Double */ \
  xx(ET_EQU_D, 2, tk_int) \
\
  /* Compare for Equality Float to a Float */ \
  xx(ET_EQU_F, 2, tk_int) \
\
  /* Compare for Equality Signed Long Long Int to a Signed Long Long Int */ \
  xx(ET_EQU_SLLI, 2, tk_int) \
\
  /* Compare for Equality Signed Long Int to a Signed Long Int */ \
  xx(ET_EQU_SLI, 2, tk_int) \
\
  /* Compare for Equality Signed Int to a Signed Int */ \
  xx(ET_EQU_SI, 2, tk_int) \
\
  /* Compare for Equality Unsigned Long Long Int to an Unsigned Long Long Int */ \
  xx(ET_EQU_ULLI, 2, tk_int) \
\
  /* Compare for Equality Unsigned Long Int to an Unsigned Long Int */ \
  xx(ET_EQU_ULI, 2, tk_int) \
\
  /* Compare for Equality Unsigned Int to an Unsigned Int */ \
  xx(ET_EQU_UI, 2, tk_int) \
\
  /* Compare for Equality Pointer to a Pointer */ \
  xx(ET_EQU_PTR, 2, tk_int) \
\
\
  /* Compare for Inequality Long Double Complex to Long Double */ \
  xx(ET_NEQ_LDC_LD, 2, tk_int) \
\
  /* Compare for Inequality Long Double Complex to Long Double Complex */ \
  xx(ET_NEQ_LDC_LDC, 2, tk_int) \
\
  /* Compare for Inequality Long Double Complex to Long Double Imaginary */ \
  xx(ET_NEQ_LDC_LDI, 2, tk_int) \
\
  /* Compare for Inequality Double Complex to Double */ \
  xx(ET_NEQ_DC_D, 2, tk_int) \
\
  /* Compare for Inequality Double Complex to Double Complex */ \
  xx(ET_NEQ_DC_DC, 2, tk_int) \
\
  /* Compare for Inequality Double Complex to Double Imaginary */ \
  xx(ET_NEQ_DC_DI, 2, tk_int) \
\
  /* Compare for Inequality Float Complex to Float */ \
  xx(ET_NEQ_FC_F, 2, tk_int) \
\
  /* Compare for Inequality Float Complex to Float Complex */ \
  xx(ET_NEQ_FC_FC, 2, tk_int) \
\
  /* Compare for Inequality Float Complex to Float Imaginary */ \
  xx(ET_NEQ_FC_FI, 2, tk_int) \
\
  /* Compare for Inequality Long Double Imaginary to Long Double */ \
  xx(ET_NEQ_LDI_LD, 2, tk_int) \
\
  /* Compare for Inequality Long Double Imaginary to Long Double Imaginary */ \
  xx(ET_NEQ_LDI_LDI, 2, tk_int) \
\
  /* Compare for Inequality Double Imaginary to Double */ \
  xx(ET_NEQ_DI_D, 2, tk_int) \
\
  /* Compare for Inequality Double Imaginary to Double Imaginary */ \
  xx(ET_NEQ_DI_DI, 2, tk_int) \
\
  /* Compare for Inequality Float Imaginary to Float */ \
  xx(ET_NEQ_FI_F, 2, tk_int) \
\
  /* Compare for Inequality Float Imaginary to Float Imaginary */ \
  xx(ET_NEQ_FI_FI, 2, tk_int) \
\
  /* Compare for Inequality Long Double to a Long Double */ \
  xx(ET_NEQ_LD, 2, tk_int) \
\
  /* Compare for Inequality Double to a Double */ \
  xx(ET_NEQ_D, 2, tk_int) \
\
  /* Compare for Inequality Float to a Float */ \
  xx(ET_NEQ_F, 2, tk_int) \
\
  /* Compare for Inequality Signed Long Long Int to a Signed Long Long Int */ \
  xx(ET_NEQ_SLLI, 2, tk_int) \
\
  /* Compare for Inequality Signed Long Int to a Signed Long Int */ \
  xx(ET_NEQ_SLI, 2, tk_int) \
\
  /* Compare for Inequality Signed Int to a Signed Int */ \
  xx(ET_NEQ_SI, 2, tk_int) \
\
  /* Compare for Inequality Unsigned Long Long Int to an Unsigned Long Long Int */ \
  xx(ET_NEQ_ULLI, 2, tk_int) \
\
  /* Compare for Inequality Unsigned Long Int to an Unsigned Long Int */ \
  xx(ET_NEQ_ULI, 2, tk_int) \
\
  /* Compare for Inequality Unsigned Int to an Unsigned Int */ \
  xx(ET_NEQ_UI, 2, tk_int) \
\
  /* Compare for Inequality Pointer to a Pointer */ \
  xx(ET_NEQ_PTR, 2, tk_int) \
\
\
  /* Logical And of two Signed Ints */ \
  xx(ET_LOGICAL_AND, 2, tk_int) \
\
  /* Logical Or of two Signed Ints */ \
  xx(ET_LOGICAL_OR, 2, tk_int) \
\
\
  /* Condition Select one of Two Expressions, depending on the Condition being True or False respetively 
   * (i.e. the ?: conditional operator) */ \
  xx(ET_CONDITION, 3, tk_invalid) \
\
\
  /* Function call, first child is a pointer to the function, second child is the tree of ET_FUNCTION_ARG_LIST
   * nodes that contain the arguments to the function. */ \
  xx(ET_FUNCTION_CALL, 2, tk_invalid) \
\
  /* Argument list nodes that make the non-leaf nodes of the arguments to a function call; rather than represent
   * any one value, this is the glue that holds an arbitrary number of arguments to a function together as one
   * single "expr" child at the ET_FUNCTION_CALL. */ \
  xx(ET_FUNCTION_ARG_LIST, 2, tk_invalid) \
\
\

enum expr_type {
#define xx(id, num_opds, type_of) \
  id,
ENUM_EXPR_TYPES
#undef xx
};

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* EXPR_NODE_TYPES_H */
