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

#ifndef PARTIAL_TYPE_SPECIFIERS_H
#define PARTIAL_TYPE_SPECIFIERS_H

#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED
#include "types.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define ENUM_PARTIAL_TYPE_SPECIFIERS \
/* Singular type specifiers */ \
xx(0, PTS_INVALID, tk_invalid, "<invalid>") \
xx(0, PTS_NONE, tk_int, "<none>") \
xx(1, PTS_VOID, tk_void, "void") \
xx(1, PTS_CHAR, tk_char, "char") \
xx(1, PTS_SHORT, tk_short_int, "short") \
xx(1, PTS_INT, tk_int, "int") \
xx(1, PTS_SIGNED, tk_int, "signed") \
xx(1, PTS_UNSIGNED, tk_unsigned_int, "unsigned") \
xx(1, PTS_LONG, tk_long_int, "long") \
xx(1, PTS_FLOAT, tk_float, "float") \
xx(1, PTS_DOUBLE, tk_double, "double") \
xx(1, PTS_BOOL, tk_bool, "_Bool") \
xx(0, PTS_COMPLEX, tk_invalid, "_Complex") \
xx(0, PTS_IMAGINARY, tk_invalid, "_Imaginary") \
xx(1, PTS_STRUCT, tk_structure, "struct") \
xx(1, PTS_UNION, tk_union, "union") \
xx(1, PTS_ENUM, tk_enumeration, "enum") \
xx(1, PTS_TYPEDEF_NAME, tk_invalid, "typedef-name") \
\
/* Compound type specifiers */ \
xx(1, PTS_SIGNED_CHAR, tk_signed_char, "signed char") \
xx(1, PTS_UNSIGNED_CHAR, tk_unsigned_char, "unsigned char") \
xx(1, PTS_SIGNED_SHORT, tk_short_int, "signed short") \
xx(1, PTS_SHORT_INT, tk_short_int, "short int") \
xx(1, PTS_SIGNED_SHORT_INT, tk_short_int, "signed short int") \
xx(1, PTS_UNSIGNED_SHORT, tk_unsigned_short_int, "unsigned short") \
xx(1, PTS_UNSIGNED_SHORT_INT, tk_unsigned_short_int, "unsigned short int") \
xx(1, PTS_SIGNED_INT, tk_int, "signed int") \
xx(1, PTS_UNSIGNED_INT, tk_unsigned_int, "unsigned int") \
xx(1, PTS_SIGNED_LONG, tk_long_int, "signed long") \
xx(1, PTS_LONG_INT, tk_long_int, "long int") \
xx(1, PTS_SIGNED_LONG_INT, tk_long_int, "signed long int") \
xx(1, PTS_UNSIGNED_LONG, tk_unsigned_long_int, "unsigned long") \
xx(1, PTS_UNSIGNED_LONG_INT, tk_unsigned_long_int, "unsigned long int") \
xx(1, PTS_LONG_LONG, tk_long_long_int, "long long") \
xx(1, PTS_SIGNED_LONG_LONG, tk_long_long_int, "signed long long") \
xx(1, PTS_LONG_LONG_INT, tk_long_long_int, "long long int") \
xx(1, PTS_SIGNED_LONG_LONG_INT, tk_long_long_int, "signed long long int") \
xx(1, PTS_UNSIGNED_LONG_LONG, tk_unsigned_long_long_int, "unsigned long long") \
xx(1, PTS_UNSIGNED_LONG_LONG_INT, tk_unsigned_long_long_int, "unsigned long long int") \
xx(1, PTS_LONG_DOUBLE, tk_long_double, "long double") \
xx(1, PTS_FLOAT_COMPLEX, tk_float_complex, "float _Complex") \
xx(1, PTS_DOUBLE_COMPLEX, tk_double_complex, "double _Complex") \
xx(1, PTS_LONG_DOUBLE_COMPLEX, tk_long_double_complex, "long double _Complex") \
xx(1, PTS_FLOAT_IMAGINARY, tk_float_imaginary, "float _Imaginary") \
xx(1, PTS_DOUBLE_IMAGINARY, tk_double_imaginary, "double _Imaginary") \
xx(1, PTS_LONG_DOUBLE_IMAGINARY, tk_long_double_imaginary, "long double _Imaginary") \
xx(0, PTS_LONG_COMPLEX, tk_invalid, "long _Complex") \
xx(0, PTS_LONG_IMAGINARY, tk_invalid, "long _Imaginary")

typedef enum pts_type_specifier_enum {
#define xx(is_valid, id, dt_type, str) id,
  ENUM_PARTIAL_TYPE_SPECIFIERS
#undef xx
#define PTS_NUM_SINGULAR PTS_TYPEDEF_NAME
  PTS_LAST_TYPE_SPECIFIER
} pts_type_specifier_t;

/* Initializes a pre-calculated combiner table */
void pts_init();

/* Returns a user legible string for the partial type specifier */
const char *pts_str(pts_type_specifier_t ts);

/* Returns the C type kind for the partial type specifier, or tk_invalid if the
 * partial type does not correspond / resolve to a C language type.*/
enum type_kind pts_type_kind(pts_type_specifier_t ts);

/* Given a compound type specifier (can be any TS_xxx from TS_NONE up to TS_LAST_TYPE_SPECIFIER) combine
 * it with an additional singular type specifier part (can be any TS_xxx from TS_VOID to TS_NUM_SINGULAR).
 * Returns TS_NONE if no such combination is valid (eg. "signed float" would not be valid), or the ordinal
 * constant for the combined type specifier otherwise. */
int pts_combine_type(pts_type_specifier_t compound, pts_type_specifier_t singular);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* TYPE_SPECIFIERS_H */
