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

#include "sha1.h"

#define KLT_SHA1_ROR32(v, s)\
 (((v) >> s) | ((v) << (32 - s)))

#define KLT_SHA1_ROL32(v, s)\
 (((v) << s) | ((v) >> (32 - s)))

KLT_SHA1_OPTIONAL_STATIC void sha1_init(struct sha1_inner_state *state) {
  state->h0 = 0x67452301;
  state->h1 = 0xEFCDAB89;
  state->h2 = 0x98BADCFE;
  state->h3 = 0x10325476;
  state->h4 = 0xC3D2E1F0;
  state->message_bit_length = 0;
  state->current_w_index = 0;
  state->current_w = 0;
}

static void sha1_inner_process_chunk(struct sha1_inner_state *state) {
  size_t i;
  for (i = 16; i < 80; ++i) {
    uint32_t w_i = state->w[i - 3] ^ state->w[i - 8] ^ state->w[i - 14] ^ state->w[i - 16];
    state->w[i] = KLT_SHA1_ROL32(w_i, 1);
  }

  uint32_t a = state->h0;
  uint32_t b = state->h1;
  uint32_t c = state->h2;
  uint32_t d = state->h3;
  uint32_t e = state->h4;

  for (i = 0; i < 80; ++i) {
    uint32_t f, k;
    /* Can we trust compiler to un-roll this....? */
    if ((i >= 0) && (i <= 19)) {
      f = (b & c) | ((~b) & d);
      k = 0x5A827999;
    }
    else if ((i >= 20) && (i <= 39)) {
      f = b ^ c ^ d;
      k = 0x6ED9EBA1;
    }
    else if ((i >= 40) && (i <= 59)) {
      f = (b & c) | (b & d) | (c & d);
      k = 0x8F1BBCDC;
    }
    else /* ((i >= 60) && (i <= 79)) */ {
      f = b ^ c ^ d;
      k = 0xCA62C1D6;
    }

    uint32_t temp = KLT_SHA1_ROL32(a, 5) + f + e + k + state->w[i];
    e = d;
    d = c;
    c = KLT_SHA1_ROL32(b, 30);
    b = a;
    a = temp;
  }

  state->h0 += a;
  state->h1 += b;
  state->h2 += c;
  state->h3 += d;
  state->h4 += e;
}

KLT_SHA1_OPTIONAL_STATIC int sha1_process(struct sha1_inner_state *state, const void *message_part, size_t message_part_size) {
  const uint8_t *msg = (const uint8_t *)message_part;
  size_t n;

  if ( ((uint64_t)message_part_size) > ((~(uint64_t)0) >> 3) ) {
    /* Multiplying by 8 (converting byte-size to bit-size) would overflow */
    return -1;
  }
  uint64_t message_part_bit_length = ((uint64_t)message_part_size) * 8;
  state->message_bit_length += message_part_bit_length;
  if (state->message_bit_length < message_part_bit_length) {
    /* Adding to the existing bit length caused an overflow */
    return -1;
  }

  for (n = 0; n < message_part_size; ++n) {
    state->current_w = (state->current_w << 8) | msg[n];
    state->current_w_index++;
    if (!(state->current_w_index & 3)) {
      state->w[(state->current_w_index >> 2) - 1] = state->current_w;
      state->current_w = 0;
      if (!(state->current_w_index &= 63)) {
        sha1_inner_process_chunk(state);
      }
    }
  }

  return 0;
}

KLT_SHA1_OPTIONAL_STATIC int sha1_finish(struct sha1_inner_state *state, uint8_t *digest) {
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
  message_length_big_endian[0] = (uint8_t)(state->message_bit_length >> 56);
  message_length_big_endian[1] = (uint8_t)(state->message_bit_length >> 48);
  message_length_big_endian[2] = (uint8_t)(state->message_bit_length >> 40);
  message_length_big_endian[3] = (uint8_t)(state->message_bit_length >> 32);
  message_length_big_endian[4] = (uint8_t)(state->message_bit_length >> 24);
  message_length_big_endian[5] = (uint8_t)(state->message_bit_length >> 16);
  message_length_big_endian[6] = (uint8_t)(state->message_bit_length >> 8);
  message_length_big_endian[7] = (uint8_t)state->message_bit_length;

  uint64_t message_fully_padded_length = (state->message_bit_length + 1 + 64 + 511) & ~(uint64_t)511;
  if (message_fully_padded_length < state->message_bit_length) {
    return -1;
  }
  uint64_t message_byte_length = state->message_bit_length >> 3;
  uint64_t message_padded_byte_length = message_fully_padded_length >> 3;
  sha1_process(state, padding, (size_t)(message_padded_byte_length - message_byte_length - 8));
  sha1_process(state, message_length_big_endian, 8);

  digest[0] = (uint8_t)(state->h0 >> 24);
  digest[1] = (uint8_t)(state->h0 >> 16);
  digest[2] = (uint8_t)(state->h0 >> 8);
  digest[3] = (uint8_t)state->h0;
  digest[4] = (uint8_t)(state->h1 >> 24);
  digest[5] = (uint8_t)(state->h1 >> 16);
  digest[6] = (uint8_t)(state->h1 >> 8);
  digest[7] = (uint8_t)state->h1;
  digest[8] = (uint8_t)(state->h2 >> 24);
  digest[9] = (uint8_t)(state->h2 >> 16);
  digest[10] = (uint8_t)(state->h2 >> 8);
  digest[11] = (uint8_t)state->h2;
  digest[12] = (uint8_t)(state->h3 >> 24);
  digest[13] = (uint8_t)(state->h3 >> 16);
  digest[14] = (uint8_t)(state->h3 >> 8);
  digest[15] = (uint8_t)state->h3;
  digest[16] = (uint8_t)(state->h4 >> 24);
  digest[17] = (uint8_t)(state->h4 >> 16);
  digest[18] = (uint8_t)(state->h4 >> 8);
  digest[19] = (uint8_t)state->h4;

  return 0;
}
