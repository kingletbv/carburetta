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

#ifndef SCAN_HELPERS_H_INCLUDED
#define SCAN_HELPERS_H_INCLUDED
#include "scan_helpers.h"
#endif

#ifndef STMT_H_INCLUDED
#define STMT_H_INCLUDED
#include "stmt.h"
#endif

#ifndef EXPR_H_INCLUDED
#define EXPR_H_INCLUDED
#include "expr.h"
#endif

#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED
#include "types.h"
#endif

#ifndef TEMPL_PARSER_H_INCLUDED
#define TEMPL_PARSER_H_INCLUDED
#include "templ_parser.h"
#endif

#ifndef PP_TOKENS_H_INCLUDED
#define PP_TOKENS_H_INCLUDED
#include "pp_tokens.h"
#endif

int sch_parse_decimal(struct c_compiler *cc, uint64_t *pval, const char *text, size_t len, struct situs *loc) {
  uint64_t val = 0;
  size_t n;
  for (n = 0; n < len; ++n) {
    uint64_t next_digit = text[n] - '0';
    if (val >(UINT64_MAX / 10)) {
      cc_error_loc(cc, loc, "Overflow on integer");
      val = 0;
      return -1;
    }
    val = val * 10;
    val += next_digit;
    if (val < next_digit) {
      cc_error_loc(cc, loc, "Overflow on integer");
      val = 0;
      return -1;
    }
  }
  *pval = val;
  return 0;
}


int sch_parse_octal(struct c_compiler *cc, uint64_t *pval, const char *text, size_t len, struct situs *loc) {
  uint64_t val = 0;
  size_t n;
  for (n = 1 /* leading 0 */; n < len; ++n) {
    uint64_t next_digit = text[n] - '0';
    if (val >(UINT64_MAX / 8)) {
      cc_error_loc(cc, loc, "Overflow on integer");
      val = 0;
      return -1;
    }
    val = val * 8;
    val += next_digit;
    if (val < next_digit) {
      cc_error_loc(cc, loc, "Overflow on integer");
      val = 0;
      return -1;
    }
  }
  *pval = val;
  return 0;
}

int sch_parse_hexadecimal(struct c_compiler *cc, uint64_t *pval, const char *text, size_t len, struct situs *loc) {
  uint64_t val = 0;
  size_t n;
  for (n = 2 /* "0x"|"0X" */; n < len; ++n) {
    uint64_t next_digit;
    if ((text[n] >= '0') && (text[n] <= '9')) {
      next_digit = text[n] - '0';
    }
    else if ((text[n] >= 'a') && (text[n] <= 'f')) {
      next_digit = text[n] - 'a' + 0xA;
    }
    else /* ((text[n] >= 'A') && (text[n] <= 'F')) */ {
      next_digit = text[n] - 'A' + 0xA;
    }

    if (val > (UINT64_MAX / 16)) {
      cc_error_loc(cc, loc, "Overflow on integer");
      val = 0;
      return -1;
    }
    val = val * 16;
    val += next_digit;
    if (val < next_digit) {
      cc_error_loc(cc, loc, "Overflow on integer");
      val = 0;
      return -1;
    }
  }
  *pval = val;
  return 0;
}

int sch_parse_binary(struct c_compiler *cc, uint64_t *pval, const char *text, size_t len, struct situs *loc) {
  uint64_t val = 0;
  size_t n;
  for (n = 2 /* "0b" */; n < len; ++n) {
    uint64_t next_digit;
    next_digit = text[n] - '0';

    if (val >(UINT64_MAX / 2)) {
      cc_error_loc(cc, loc, "Overflow on integer");
      val = 0;
      return -1;
    }
    val = val * 2;
    val += next_digit;
    if (val < next_digit) {
      cc_error_loc(cc, loc, "Overflow on integer");
      val = 0;
      return -1;
    }
  }
  *pval = val;
  return 0;
}

/* ohb: octal, hex, binary, looking at column "Octal or Hexadecimal Constant" in 6.4.4.1 Integer Constants */
void sch_process_ohb_none(struct c_compiler *cc, uint64_t val, struct expr **expr, struct situs *loc) {
  enum expr_type et = ET_INVALID;
  if (val <= type_base_int_type_max(&cc->tb_, tk_int)) {
    et = ET_C_INT;
  }
  else if (val <= type_base_int_type_max(&cc->tb_, tk_unsigned_int)) {
    et = ET_C_UNSIGNED_INT;
  }
  else if (val <= type_base_int_type_max(&cc->tb_, tk_long_int)) {
    et = ET_C_LONG_INT;
  }
  else if (val <= type_base_int_type_max(&cc->tb_, tk_unsigned_long_int)) {
    et = ET_C_UNSIGNED_LONG_INT;
  }
  else if (val <= type_base_int_type_max(&cc->tb_, tk_long_long_int)) {
    et = ET_C_LONG_LONG_INT;
  }
  else {
    if (val > type_base_int_type_max(&cc->tb_, tk_unsigned_long_long_int)) {
      cc_error_loc(cc, loc, "Overflow on integer, value truncated");
    }
    et = ET_C_UNSIGNED_LONG_LONG_INT;
  }
  *expr = expr_alloc(et);
  if (*expr) {
    (*expr)->v_.u64_ = val;
  }
  else {
    cc_no_memory(cc);
  }
}

void sch_process_pptk_ohb_none(struct c_compiler *cc, struct pptk **pp_chain, const char *text, uint64_t val, struct pptk **pptk, struct situs *loc) {
  *pptk = pptk_alloc(cc, pp_chain, text, PPTK_INTEGER_LIT, loc);
  if (!*pptk) {
    cc_no_memory(cc);
    return;
  }
  sch_process_ohb_none(cc, val, &(*pptk)->v_.expr_, loc);
}

void sch_process_ohb_U(struct c_compiler *cc, uint64_t val, struct expr **expr, struct situs *loc) {
  enum expr_type et = ET_INVALID;
  if (val <= type_base_int_type_max(&cc->tb_, tk_unsigned_int)) {
    et = ET_C_UNSIGNED_INT;
  }
  else if (val <= type_base_int_type_max(&cc->tb_, tk_unsigned_long_int)) {
    et = ET_C_UNSIGNED_LONG_INT;
  }
  else {
    if (val > type_base_int_type_max(&cc->tb_, tk_unsigned_long_long_int)) {
      cc_error_loc(cc, loc, "Overflow on integer, value truncated");
    }
    et = ET_C_UNSIGNED_LONG_LONG_INT;
  }
  *expr = expr_alloc(et);
  if (*expr) {
    (*expr)->v_.u64_ = val;
  }
  else {
    cc_no_memory(cc);
  }
}

void sch_process_pptk_ohb_U(struct c_compiler *cc, struct pptk **pp_chain, const char *text, uint64_t val, struct pptk **pptk, struct situs *loc) {
  *pptk = pptk_alloc(cc, pp_chain, text, PPTK_INTEGER_LIT, loc);
  if (!*pptk) {
    cc_no_memory(cc);
    return;
  }
  sch_process_ohb_U(cc, val, &(*pptk)->v_.expr_, loc);
}

void sch_process_ohb_L(struct c_compiler *cc, uint64_t val, struct expr **expr, struct situs *loc) {
  enum expr_type et = ET_INVALID;
  if (val <= type_base_int_type_max(&cc->tb_, tk_long_int)) {
    et = ET_C_LONG_INT;
  }
  else if (val <= type_base_int_type_max(&cc->tb_, tk_unsigned_long_int)) {
    et = ET_C_UNSIGNED_LONG_INT;
  }
  else if (val <= type_base_int_type_max(&cc->tb_, tk_long_long_int)) {
    et = ET_C_LONG_LONG_INT;
  }
  else {
    if (val > type_base_int_type_max(&cc->tb_, tk_unsigned_long_long_int)) {
      cc_error_loc(cc, loc, "Overflow on integer, value truncated");
    }
    et = ET_C_UNSIGNED_LONG_LONG_INT;
  }
  *expr = expr_alloc(et);
  if (*expr) {
    (*expr)->v_.u64_ = val;
  }
  else {
    cc_no_memory(cc);
  }
}

void sch_process_pptk_ohb_L(struct c_compiler *cc, struct pptk **pp_chain, const char *text, uint64_t val, struct pptk **pptk, struct situs *loc) {
  *pptk = pptk_alloc(cc, pp_chain, text, PPTK_INTEGER_LIT, loc);
  if (!*pptk) {
    cc_no_memory(cc);
    return;
  }
  sch_process_ohb_L(cc, val, &(*pptk)->v_.expr_, loc);
}

void sch_process_ohb_UL(struct c_compiler *cc, uint64_t val, struct expr **expr, struct situs *loc) {
  enum expr_type et = ET_INVALID;
  if (val <= type_base_int_type_max(&cc->tb_, tk_unsigned_long_int)) {
    et = ET_C_UNSIGNED_LONG_INT;
  }
  else {
    if (val > type_base_int_type_max(&cc->tb_, tk_unsigned_long_long_int)) {
      cc_error_loc(cc, loc, "Overflow on integer, value truncated");
    }
    et = ET_C_UNSIGNED_LONG_LONG_INT;
  }
  *expr = expr_alloc(et);
  if (*expr) {
    (*expr)->v_.u64_ = val;
  }
  else {
    cc_no_memory(cc);
  }
}

void sch_process_pptk_ohb_UL(struct c_compiler *cc, struct pptk **pp_chain, const char *text, uint64_t val, struct pptk **pptk, struct situs *loc) {
  *pptk = pptk_alloc(cc, pp_chain, text, PPTK_INTEGER_LIT, loc);
  if (!*pptk) {
    cc_no_memory(cc);
    return;
  }
  sch_process_ohb_UL(cc, val, &(*pptk)->v_.expr_, loc);
}

void sch_process_ohb_LL(struct c_compiler *cc, uint64_t val, struct expr **expr, struct situs *loc) {
  enum expr_type et = ET_INVALID;
  if (val <= type_base_int_type_max(&cc->tb_, tk_long_long_int)) {
    et = ET_C_LONG_LONG_INT;
  }
  else {
    if (val > type_base_int_type_max(&cc->tb_, tk_unsigned_long_long_int)) {
      cc_error_loc(cc, loc, "Overflow on integer, value truncated");
    }
    et = ET_C_UNSIGNED_LONG_LONG_INT;
  }
  *expr = expr_alloc(et);
  if (*expr) {
    (*expr)->v_.u64_ = val;
  }
  else {
    cc_no_memory(cc);
  }
}

void sch_process_pptk_ohb_LL(struct c_compiler *cc, struct pptk **pp_chain, const char *text, uint64_t val, struct pptk **pptk, struct situs *loc) {
  *pptk = pptk_alloc(cc, pp_chain, text, PPTK_INTEGER_LIT, loc);
  if (!*pptk) {
    cc_no_memory(cc);
    return;
  }
  sch_process_ohb_LL(cc, val, &(*pptk)->v_.expr_, loc);
}

void sch_process_ohb_ULL(struct c_compiler *cc, uint64_t val, struct expr **expr, struct situs *loc) {
  enum expr_type et = ET_INVALID;
  if (val > type_base_int_type_max(&cc->tb_, tk_unsigned_long_long_int)) {
    cc_error_loc(cc, loc, "Overflow on integer, value truncated");
  }
  et = ET_C_UNSIGNED_LONG_LONG_INT;
  *expr = expr_alloc(et);
  if (*expr) {
    (*expr)->v_.u64_ = val;
  }
  else {
    cc_no_memory(cc);
  }
}

void sch_process_pptk_ohb_ULL(struct c_compiler *cc, struct pptk **pp_chain, const char *text, uint64_t val, struct pptk **pptk, struct situs *loc) {
  *pptk = pptk_alloc(cc, pp_chain, text, PPTK_INTEGER_LIT, loc);
  if (!*pptk) {
    cc_no_memory(cc);
    return;
  }
  sch_process_ohb_ULL(cc, val, &(*pptk)->v_.expr_, loc);
}

int sch_read_oct_esc_value(const char *octal_num, int *val) {
  int n = 0;
  int v = 0;
  /* Octal escapes can be at most 3 digits long (eg. see C99 6.4.4.4 paragraph 14) */
  for (n = 0; n < 3; ++n) {
    if ((octal_num[n] >= '0') && (octal_num[n] <= '7')) {
      v = v << 3;
      v += octal_num[n] - '0';
    }
    else {
      break;
    }
  }
  *val = v;
  return n;
}

int sch_read_hex_esc_value(const char *hex_xx_num, int *val) {
  int n = 0;
  int v = 0;

  /* Hexadecimal escapes can be arbitrarily long (eg. see C99 6.4.4.4 paragraph 14) */
  for (;;) {
    if ((hex_xx_num[n] >= '0') && (hex_xx_num[n] <= '9')) {
      v = v << 4;
      v += hex_xx_num[n] - '0';
    }
    else if ((hex_xx_num[n] >= 'a') && (hex_xx_num[n] <= 'f')) {
      v = v << 4;
      v += hex_xx_num[n] - 'a' + 0xA;
    }
    else if ((hex_xx_num[n] >= 'A') && (hex_xx_num[n] <= 'F')) {
      v = v << 4;
      v += hex_xx_num[n] - 'A' + 0xA;
    }
    else {
      *val = v;
      return n;
    }

    ++n;
  }
}

int sch_read_char_value(struct c_compiler *cc, const char *lit_pos, int *val, int max_char, struct situs *loc, const char *kind) {
  const unsigned char *c = (const unsigned char *)lit_pos;
  if (*c != '\\') {
    if (*c == '\n') {
      cc_error_loc(cc, loc, "Newline in %s constant", kind);
    }
    *val = *c;
    return 1;
  }
  int num_digits, num;
  switch (c[1]) {
    case 'a': *val = '\a'; return 2;
    case 'b': *val = '\b'; return 2;
    case 'f': *val = '\f'; return 2;
    case 'n': *val = '\n'; return 2;
    case 'r': *val = '\r'; return 2;
    case 't': *val = '\t'; return 2;
    case 'v': *val = '\v'; return 2;
    case '\'': *val = '\''; return 2;
    case '\"': *val = '\"'; return 2;
    case '\\': *val = '\\'; return 2;
    case '?': *val = '\?'; return 2;
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
      num_digits = sch_read_oct_esc_value(c + 1, &num);
      if ((!num_digits) || (num > max_char)) {
        cc_error_loc(cc, loc, "Invalid octal escape sequence");
        *val = *c;
        return 1;
      }
      else {
        *val = num;
        return 1 + num_digits;
      }
      break;
    case 'x':
      num_digits = sch_read_hex_esc_value(c + 1, &num);
      if ((!num_digits) || (num > max_char)) {
        cc_error_loc(cc, loc, "Invalid hex escape sequence");
        *val = *c;
        return 1;
      }
      else {
        *val = num;
        return 2 + num_digits;
      }
      break;
    default:
      cc_error_loc(cc, loc, "Invalid escape sequence \"\\%c\"\n", c[1]);
      *val = *c;
      return 1;

  }
}
