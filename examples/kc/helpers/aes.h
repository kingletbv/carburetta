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

#ifndef KLT_AES_H
#define KLT_AES_H

#ifndef INCLUDED_STDINT_H
#define INCLUDED_STDINT_H
#include <stdint.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* If KLT_AES_INVISIBLE_LINKAGE is defined, including "aes.h" will not only
 * declare but also define (as static) the implementation */
#ifdef KLT_AES_INVISIBLE_LINKAGE
#define KLT_AES_OPTIONAL_STATIC static
#else
#define KLT_AES_OPTIONAL_STATIC
#endif

KLT_AES_OPTIONAL_STATIC void aes128_keyexpansion(uint8_t *key/*[16]*/, uint8_t *w/*[176]*/);
KLT_AES_OPTIONAL_STATIC void aes128_cipher(uint8_t *in/*[16]*/, uint8_t *out/*[16]*/, uint8_t *w/*[176]*/);
KLT_AES_OPTIONAL_STATIC void aes128_inv_cipher(uint8_t *in/*[16]*/, uint8_t *out/*[16]*/, uint8_t *w/*[176]*/);

KLT_AES_OPTIONAL_STATIC void aes192_keyexpansion(uint8_t *key/*[24]*/, uint8_t *w/*[208]*/);
KLT_AES_OPTIONAL_STATIC void aes192_cipher(uint8_t *in/*[16]*/, uint8_t *out/*[16]*/, uint8_t *w/*[208]*/);
KLT_AES_OPTIONAL_STATIC void aes192_inv_cipher(uint8_t *in/*[16]*/, uint8_t *out/*[16]*/, uint8_t *w/*[208]*/);

KLT_AES_OPTIONAL_STATIC void aes256_keyexpansion(uint8_t *key/*[32]*/, uint8_t *w/*[240]*/);
KLT_AES_OPTIONAL_STATIC void aes256_cipher(uint8_t *in/*[16]*/, uint8_t *out/*[16]*/, uint8_t *w/*[240]*/);
KLT_AES_OPTIONAL_STATIC void aes256_inv_cipher(uint8_t *in/*[16]*/, uint8_t *out/*[16]*/, uint8_t *w/*[240]*/);

KLT_AES_OPTIONAL_STATIC void aes_gen_sbox(void);
KLT_AES_OPTIONAL_STATIC void aes_gen_c(void);

#ifdef KLT_AES_INVISIBLE_LINKAGE
#include "aes.c"
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* KLT_AES_H */
