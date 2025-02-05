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

#ifndef KLT_SHA256_H
#define KLT_SHA256_H

#ifndef INCLUDED_STDINT_H
#define INCLUDED_STDINT_H
#include <stdint.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct sha256_inner_state {
  uint32_t w[64];
  uint32_t h0, h1, h2, h3, h4, h5, h6, h7;
  uint64_t message_bit_length;
  int      current_w_index;
  uint32_t current_w;
};

/* If KLT_SHA256_INVISIBLE_LINKAGE is defined, including "sha256.h" will not only
 * declare but also define (as static) the implementation */
#ifdef KLT_SHA256_INVISIBLE_LINKAGE
#define KLT_SHA256_OPTIONAL_STATIC static
#else
#define KLT_SHA256_OPTIONAL_STATIC
#endif

/* Initializes the state for computing a SHA256 value */
KLT_SHA256_OPTIONAL_STATIC void sha256_init(struct sha256_inner_state *state);

/* Processes message data for the SHA256 being computed in state. This function
 * can be called multiple times in succession for consecutive parts of the same
 * message.
 * Returns 0 upon success, or -1 upon failure (only happens when a 64 bit
 * message length field expressed in bits overflows.) */
KLT_SHA256_OPTIONAL_STATIC int sha256_process(struct sha256_inner_state *state, const void *message_part, size_t message_part_size);

/* Completes computation of the SHA256 hash and writes it to the 32 bytes at
 * digest. Returns 0 upon success, or -1 upon failure (only happens when a 64
 * bit message length field expressed in bits overflows.) */
KLT_SHA256_OPTIONAL_STATIC int sha256_finish(struct sha256_inner_state *state, uint8_t *digest);

/* Computes the HMAC using SHA256 of key and message and writes it to the 32
 * bytes at hmac. */
KLT_SHA256_OPTIONAL_STATIC int sha256_hmac(const void *key, size_t key_size, const void *message, size_t message_size, uint8_t *hmac);

#ifdef KLT_SHA256_INVISIBLE_LINKAGE
#include "sha256.c"
#endif


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* KLT_SHA256_H */
