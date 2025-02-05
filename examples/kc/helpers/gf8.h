/* Copyright 2020-2025 Kinglet B.V.
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

#ifndef KLT_GF8_H
#define KLT_GF8_H

#ifndef INCLUDED_STDINT_H
#define INCLUDED_STDINT_H
#include <stdint.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* If KLT_GF8_INVISIBLE_LINKAGE is defined, including "sha1.h" will not only
 * declare but also define (as static) the implementation */
#ifdef KLT_GF8_INVISIBLE_LINKAGE
#define KLT_GF8_OPTIONAL_STATIC static
#else
#define KLT_GF8_OPTIONAL_STATIC
#endif

KLT_GF8_OPTIONAL_STATIC uint8_t gf8_xtime(uint8_t val, uint8_t irreducible);

KLT_GF8_OPTIONAL_STATIC uint8_t gf8_mul(uint8_t lvalue, uint8_t rvalue, uint8_t irreducible);
KLT_GF8_OPTIONAL_STATIC void gf8_init_tables(uint8_t irreducible);
KLT_GF8_OPTIONAL_STATIC uint8_t gf8_tbl_inverse(uint8_t x);
KLT_GF8_OPTIONAL_STATIC uint8_t gf8_tbl_divide(uint8_t lvalue, uint8_t rvalue);
KLT_GF8_OPTIONAL_STATIC uint8_t gf8_tbl_multiply(uint8_t lvalue, uint8_t rvalue);

KLT_GF8_OPTIONAL_STATIC void gf8_gen_c(void);

#ifdef KLT_GF8_INVISIBLE_LINKAGE
#include "gf8.c"
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* KLT_GF8_H */
