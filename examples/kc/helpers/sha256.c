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

#include "sha256.h"

#define KLT_SHA256_ROR32(v, s)\
 (((v) >> s) | ((v) << (32 - s)))

KLT_SHA256_OPTIONAL_STATIC void sha256_init(struct sha256_inner_state *state) {
  state->h0 = 0x6a09e667;
  state->h1 = 0xbb67ae85;
  state->h2 = 0x3c6ef372;
  state->h3 = 0xa54ff53a;
  state->h4 = 0x510e527f;
  state->h5 = 0x9b05688c;
  state->h6 = 0x1f83d9ab;
  state->h7 = 0x5be0cd19;
  state->message_bit_length = 0;
  state->current_w_index = 0;
  state->current_w = 0;
}

static void sha256_inner_process_chunk(struct sha256_inner_state *state) {
  static const uint32_t k[] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
  };

  size_t i;
  for (i = 16; i < 64; ++i) {
    uint32_t s0 = KLT_SHA256_ROR32(state->w[i - 15], 7) ^ KLT_SHA256_ROR32(state->w[i - 15], 18) ^ (state->w[i - 15] >> 3);
    uint32_t s1 = KLT_SHA256_ROR32(state->w[i - 2], 17) ^ KLT_SHA256_ROR32(state->w[i - 2], 19) ^ (state->w[i - 2] >> 10);
    state->w[i] = state->w[i - 16] + s0 + state->w[i - 7] + s1;
  }

  uint32_t a = state->h0;
  uint32_t b = state->h1;
  uint32_t c = state->h2;
  uint32_t d = state->h3;
  uint32_t e = state->h4;
  uint32_t f = state->h5;
  uint32_t g = state->h6;
  uint32_t h = state->h7;

  for (i = 0; i < 64; ++i) {
    uint32_t s1 = KLT_SHA256_ROR32(e, 6) ^ KLT_SHA256_ROR32(e, 11) ^ KLT_SHA256_ROR32(e, 25);
    uint32_t ch = (e & f) ^ ((~e) & g);
    uint32_t temp1 = h + s1 + ch + k[i] + state->w[i];
    uint32_t s0 = KLT_SHA256_ROR32(a, 2) ^ KLT_SHA256_ROR32(a, 13) ^ KLT_SHA256_ROR32(a, 22);
    uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
    uint32_t temp2 = s0 + maj;

    h = g;
    g = f;
    f = e;
    e = d + temp1;
    d = c;
    c = b;
    b = a;
    a = temp1 + temp2;
  }

  state->h0 += a;
  state->h1 += b;
  state->h2 += c;
  state->h3 += d;
  state->h4 += e;
  state->h5 += f;
  state->h6 += g;
  state->h7 += h;
}

KLT_SHA256_OPTIONAL_STATIC int sha256_process(struct sha256_inner_state *state, const void *message_part, size_t message_part_size) {
  const uint8_t *msg = (const uint8_t *)(message_part);
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
        sha256_inner_process_chunk(state);
      }
    }
  }

  return 0;
}


KLT_SHA256_OPTIONAL_STATIC int sha256_finish(struct sha256_inner_state *state, uint8_t *digest) {
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
  sha256_process(state, padding, (size_t)(message_padded_byte_length - message_byte_length - 8));
  sha256_process(state, message_length_big_endian, 8);

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
  digest[20] = (uint8_t)(state->h5 >> 24);
  digest[21] = (uint8_t)(state->h5 >> 16);
  digest[22] = (uint8_t)(state->h5 >> 8);
  digest[23] = (uint8_t)state->h5;
  digest[24] = (uint8_t)(state->h6 >> 24);
  digest[25] = (uint8_t)(state->h6 >> 16);
  digest[26] = (uint8_t)(state->h6 >> 8);
  digest[27] = (uint8_t)state->h6;
  digest[28] = (uint8_t)(state->h7 >> 24);
  digest[29] = (uint8_t)(state->h7 >> 16);
  digest[30] = (uint8_t)(state->h7 >> 8);
  digest[31] = (uint8_t)state->h7;

  return 0;
}

KLT_SHA256_OPTIONAL_STATIC int sha256_hmac(const void *key, size_t key_size, const void *message, size_t message_size, uint8_t *hmac) {
  struct sha256_inner_state state;
  uint8_t clamped_key[64];
  int n;
  int r;

  if (key_size >= 64) {
    sha256_init(&state);
    r = sha256_process(&state, key, key_size);
    if (r) return r;
    r = sha256_finish(&state, clamped_key); /* first 32 bytes filled */
    if (r) return r;
    memset(clamped_key + 32, 0, 32); /* pad remaining 32 bytes to complete to block length */
  }
  else {
    /* key fits within a block, ensure it is padded */
    memcpy(clamped_key, key, key_size);
    memset(clamped_key + key_size, 0, (64 - key_size));
  }
  for (n = 0; n < 64; ++n) {
    /* clamped_key XORed with ipad */
    clamped_key[n] ^= 0x36; 
  }
  sha256_init(&state);
  /* H(K XOR ipad, text) */
  r = sha256_process(&state, clamped_key, 64);
  if (r) return r;
  r = sha256_process(&state, message, message_size);
  if (r) return r;
  uint8_t inner_hash[32];
  r = sha256_finish(&state, inner_hash);
  if (r) return r;
  for (n = 0; n < 64; ++n) {
    /* clamped_key remove ipad XOR and XOR with opad */
    clamped_key[n] ^= (0x36 ^ 0x5C);
  }
  sha256_init(&state);
  r = sha256_process(&state, clamped_key, 64);
  if (r) return r;
  r = sha256_process(&state, inner_hash, 32);
  if (r) return r;
  r = sha256_finish(&state, hmac);
  return r;
}
