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

#ifndef KLT_MD5_H
#define KLT_MD5_H

#ifndef INCLUDED_STDINT_H
#define INCLUDED_STDINT_H
#include <stdint.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct md5_inner_state {
  uint32_t a_, b_, c_, d_;
  uint32_t chunk_[16];
  uint64_t message_bit_length_;
  int      current_byte_index_;
  uint32_t current_word_;
};

/* If KLT_MD5_INVISIBLE_LINKAGE is defined, including "md5.h" will not only
 * declare but also define (as static) the implementation */
#ifdef KLT_MD5_INVISIBLE_LINKAGE
#define KLT_MD5_OPTIONAL_STATIC static
#else
#define KLT_MD5_OPTIONAL_STATIC
#endif

/* Initializes the state for computing a MD5 value */
KLT_MD5_OPTIONAL_STATIC void md5_init(struct md5_inner_state *state);

/* Processes message data for the MD5 being computed in state. This function
 * can be called multiple times in succession for consecutive parts of the same
 * message.
 * Returns 0 upon success, or -1 upon failure. */
KLT_MD5_OPTIONAL_STATIC int md5_process(struct md5_inner_state *state, const void *message_part, size_t message_part_size);

/* Completes computation of the MD5 hash and writes it to the 16 bytes at
 * digest. Returns 0 upon success, or -1 upon failure. */
KLT_MD5_OPTIONAL_STATIC int md5_finish(struct md5_inner_state *state, uint8_t *digest);

#ifdef KLT_MD5_INVISIBLE_LINKAGE
#include "md5.c"
#endif


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* KLT_MD5_H */
