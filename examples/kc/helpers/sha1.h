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

#ifndef KLT_SHA1_H
#define KLT_SHA1_H

#ifndef INCLUDED_STDINT_H
#define INCLUDED_STDINT_H
#include <stdint.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct sha1_inner_state {
  uint32_t w[80];
  uint32_t h0, h1, h2, h3, h4;
  uint64_t message_bit_length;
  int      current_w_index;
  uint32_t current_w;
};

/* If KLT_SHA1_INVISIBLE_LINKAGE is defined, including "sha1.h" will not only
 * declare but also define (as static) the implementation */
#ifdef KLT_SHA1_INVISIBLE_LINKAGE
#define KLT_SHA1_OPTIONAL_STATIC static
#else
#define KLT_SHA1_OPTIONAL_STATIC
#endif

/* Initializes the state for computing a SHA1 value */
KLT_SHA1_OPTIONAL_STATIC void sha1_init(struct sha1_inner_state *state);

/* Processes message data for the SHA1 being computed in state. This function
 * can be called multiple times in succession for consecutive parts of the
 * same message.
 * Returns 0 upon success, or -1 upon failure (which only happens when a 64
 * bit message length field expressed in bits overflows.) */
KLT_SHA1_OPTIONAL_STATIC int sha1_process(struct sha1_inner_state *state, const void *message_part, size_t message_part_size);

/* Completes the computation of the SHA1 hash and writes it to the 20 bytes
 * at digest. Returns 0 upon success, or -1 upon failure (which only happens
 * when a 64 bit message length field expressed in bits overflows.) */
KLT_SHA1_OPTIONAL_STATIC int sha1_finish(struct sha1_inner_state *state, uint8_t *digest);

#ifdef KLT_SHA1_INVISIBLE_LINKAGE
#include "sha1.c"
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* KLT_SHA1_H */
