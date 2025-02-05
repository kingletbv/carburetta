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

#ifndef INCLUDED_STDINT_H
#define INCLUDED_STDINT_H
#include <stdint.h>
#endif

#ifndef INCLUDED_STRING_H
#define INCLUDED_STRING_H
#include <string.h>
#endif

#include "md5.h"

#define KLT_MD5_ROL32(v, s) \
 (((v) << s) | ((v) >> (32 - s)))

KLT_MD5_OPTIONAL_STATIC void md5_init(struct md5_inner_state *state) {
  state->a_ = 0x67452301;
  state->b_ = 0xefcdab89;
  state->c_ = 0x98badcfe;
  state->d_ = 0x10325476;

  memset(state->chunk_, 0, sizeof(state->chunk_));

  state->message_bit_length_ = 0;
  state->current_byte_index_ = 0;
  state->current_word_ = 0;
}

static void md5_inner_process_chunk(struct md5_inner_state *state) {
  uint32_t a, b, c, d;
  a = state->a_;
  b = state->b_;
  c = state->c_;
  d = state->d_;

  static const uint32_t K[] = {
    0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, 0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
    0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be, 0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
    0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa, 0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
    0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed, 0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
    0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c, 0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
    0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05, 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
    0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039, 0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
    0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1, 0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
  };

  static const int S[] = {
    7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,
    5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,
    4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,
    6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21
  };

  int i;
  for (i = 0; i < 16; ++i) {
    uint32_t f = (b & c) | ((~b) & d);
    f = f + a + K[i] + state->chunk_[i];
    a = d;
    d = c;
    c = b;
    b = b + KLT_MD5_ROL32(f, S[i]);
  }
  for (i = 16; i < 32; ++i) {
    uint32_t f = (d & b) | ((~d) & c);
    f = f + a + K[i] + state->chunk_[(5 * i + 1) & 15];
    a = d;
    d = c;
    c = b;
    b = b + KLT_MD5_ROL32(f, S[i]);
  }
  for (i = 32; i < 48; ++i) {
    uint32_t f = b ^ c ^ d;
    f = f + a + K[i] + state->chunk_[(3 * i + 5) & 15];
    a = d;
    d = c;
    c = b;
    b = b + KLT_MD5_ROL32(f, S[i]);
  }
  for (i = 48; i < 64; ++i) {
    uint32_t f = c ^ (b | (~d));
    f = f + a + K[i] + state->chunk_[(7 * i) & 15];
    a = d;
    d = c;
    c = b;
    b = b + KLT_MD5_ROL32(f, S[i]);
  }

  state->a_ += a;
  state->b_ += b;
  state->c_ += c;
  state->d_ += d;
}

KLT_MD5_OPTIONAL_STATIC int md5_process(struct md5_inner_state *state, const void *message_part, size_t message_part_size) {
  const uint8_t *msg = (const uint8_t *)(message_part);
  size_t n;

  uint64_t message_part_bit_length = ((uint64_t)message_part_size) * 8;
  state->message_bit_length_ += message_part_bit_length;
  /* Note that, for MD5, overflow of length in bits is perfectly acceptable, mod 64 bits */

  for (n = 0; n < message_part_size; ++n) {
    state->current_word_ = (state->current_word_ >> 8) | (((uint32_t)msg[n]) << 24);
    state->current_byte_index_++;
    if (!(state->current_byte_index_ & 3)) {
      state->chunk_[(state->current_byte_index_ >> 2) - 1] = state->current_word_;
      state->current_word_ = 0;
      if (!(state->current_byte_index_ &= 63)) {
        md5_inner_process_chunk(state);
      }
    }
  }

  return 0;
}


KLT_MD5_OPTIONAL_STATIC int md5_finish(struct md5_inner_state *state, uint8_t *digest) {
  /* note the leading 0x80 bit, this is intentional (corresponds to the '1' bit used to pad the
   * content.) */
  static const uint8_t padding[64] = { 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  };

  uint8_t message_length_big_endian[8];
  message_length_big_endian[7] = (uint8_t)(state->message_bit_length_ >> 56);
  message_length_big_endian[6] = (uint8_t)(state->message_bit_length_ >> 48);
  message_length_big_endian[5] = (uint8_t)(state->message_bit_length_ >> 40);
  message_length_big_endian[4] = (uint8_t)(state->message_bit_length_ >> 32);
  message_length_big_endian[3] = (uint8_t)(state->message_bit_length_ >> 24);
  message_length_big_endian[2] = (uint8_t)(state->message_bit_length_ >> 16);
  message_length_big_endian[1] = (uint8_t)(state->message_bit_length_ >> 8);
  message_length_big_endian[0] = (uint8_t)state->message_bit_length_;

  uint64_t message_fully_padded_length = (state->message_bit_length_ + 1 + 64 + 511) & ~(uint64_t)511;
  if (message_fully_padded_length < state->message_bit_length_) {
    return -1;
  }
  uint64_t message_byte_length = state->message_bit_length_ >> 3;
  uint64_t message_padded_byte_length = message_fully_padded_length >> 3;
  md5_process(state, padding, (size_t)(message_padded_byte_length - message_byte_length - 8));
  md5_process(state, message_length_big_endian, 8);

  /* Output is a0:b0:c0:d0 in little endian format.*/
  digest[0] = (uint8_t)(state->a_);
  digest[1] = (uint8_t)(state->a_ >> 8);
  digest[2] = (uint8_t)(state->a_ >> 16);
  digest[3] = (uint8_t)(state->a_ >> 24);
  digest[4] = (uint8_t)(state->b_);
  digest[5] = (uint8_t)(state->b_ >> 8);
  digest[6] = (uint8_t)(state->b_ >> 16);
  digest[7] = (uint8_t)(state->b_ >> 24);
  digest[8] = (uint8_t)(state->c_);
  digest[9] = (uint8_t)(state->c_ >> 8);
  digest[10] = (uint8_t)(state->c_ >> 16);
  digest[11] = (uint8_t)(state->c_ >> 24);
  digest[12] = (uint8_t)(state->d_);
  digest[13] = (uint8_t)(state->d_ >> 8);
  digest[14] = (uint8_t)(state->d_ >> 16);
  digest[15] = (uint8_t)(state->d_ >> 24);

  return 0;
}

