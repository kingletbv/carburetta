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


#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <assert.h>

#include "gf8.h"

#include "aes.h"

static uint8_t aes_sbox_table[256] = {
  0x63, 0x7C, 0x77, 0x7B, 0xF2, 0x6B, 0x6F, 0xC5, 0x30, 0x01, 0x67, 0x2B, 0xFE, 0xD7, 0xAB, 0x76,
  0xCA, 0x82, 0xC9, 0x7D, 0xFA, 0x59, 0x47, 0xF0, 0xAD, 0xD4, 0xA2, 0xAF, 0x9C, 0xA4, 0x72, 0xC0,
  0xB7, 0xFD, 0x93, 0x26, 0x36, 0x3F, 0xF7, 0xCC, 0x34, 0xA5, 0xE5, 0xF1, 0x71, 0xD8, 0x31, 0x15,
  0x04, 0xC7, 0x23, 0xC3, 0x18, 0x96, 0x05, 0x9A, 0x07, 0x12, 0x80, 0xE2, 0xEB, 0x27, 0xB2, 0x75,
  0x09, 0x83, 0x2C, 0x1A, 0x1B, 0x6E, 0x5A, 0xA0, 0x52, 0x3B, 0xD6, 0xB3, 0x29, 0xE3, 0x2F, 0x84,
  0x53, 0xD1, 0x00, 0xED, 0x20, 0xFC, 0xB1, 0x5B, 0x6A, 0xCB, 0xBE, 0x39, 0x4A, 0x4C, 0x58, 0xCF,
  0xD0, 0xEF, 0xAA, 0xFB, 0x43, 0x4D, 0x33, 0x85, 0x45, 0xF9, 0x02, 0x7F, 0x50, 0x3C, 0x9F, 0xA8,
  0x51, 0xA3, 0x40, 0x8F, 0x92, 0x9D, 0x38, 0xF5, 0xBC, 0xB6, 0xDA, 0x21, 0x10, 0xFF, 0xF3, 0xD2,
  0xCD, 0x0C, 0x13, 0xEC, 0x5F, 0x97, 0x44, 0x17, 0xC4, 0xA7, 0x7E, 0x3D, 0x64, 0x5D, 0x19, 0x73,
  0x60, 0x81, 0x4F, 0xDC, 0x22, 0x2A, 0x90, 0x88, 0x46, 0xEE, 0xB8, 0x14, 0xDE, 0x5E, 0x0B, 0xDB,
  0xE0, 0x32, 0x3A, 0x0A, 0x49, 0x06, 0x24, 0x5C, 0xC2, 0xD3, 0xAC, 0x62, 0x91, 0x95, 0xE4, 0x79,
  0xE7, 0xC8, 0x37, 0x6D, 0x8D, 0xD5, 0x4E, 0xA9, 0x6C, 0x56, 0xF4, 0xEA, 0x65, 0x7A, 0xAE, 0x08,
  0xBA, 0x78, 0x25, 0x2E, 0x1C, 0xA6, 0xB4, 0xC6, 0xE8, 0xDD, 0x74, 0x1F, 0x4B, 0xBD, 0x8B, 0x8A,
  0x70, 0x3E, 0xB5, 0x66, 0x48, 0x03, 0xF6, 0x0E, 0x61, 0x35, 0x57, 0xB9, 0x86, 0xC1, 0x1D, 0x9E,
  0xE1, 0xF8, 0x98, 0x11, 0x69, 0xD9, 0x8E, 0x94, 0x9B, 0x1E, 0x87, 0xE9, 0xCE, 0x55, 0x28, 0xDF,
  0x8C, 0xA1, 0x89, 0x0D, 0xBF, 0xE6, 0x42, 0x68, 0x41, 0x99, 0x2D, 0x0F, 0xB0, 0x54, 0xBB, 0x16
};
static uint8_t aes_inv_sbox_table[256] = {
  0x52, 0x09, 0x6A, 0xD5, 0x30, 0x36, 0xA5, 0x38, 0xBF, 0x40, 0xA3, 0x9E, 0x81, 0xF3, 0xD7, 0xFB,
  0x7C, 0xE3, 0x39, 0x82, 0x9B, 0x2F, 0xFF, 0x87, 0x34, 0x8E, 0x43, 0x44, 0xC4, 0xDE, 0xE9, 0xCB,
  0x54, 0x7B, 0x94, 0x32, 0xA6, 0xC2, 0x23, 0x3D, 0xEE, 0x4C, 0x95, 0x0B, 0x42, 0xFA, 0xC3, 0x4E,
  0x08, 0x2E, 0xA1, 0x66, 0x28, 0xD9, 0x24, 0xB2, 0x76, 0x5B, 0xA2, 0x49, 0x6D, 0x8B, 0xD1, 0x25,
  0x72, 0xF8, 0xF6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xD4, 0xA4, 0x5C, 0xCC, 0x5D, 0x65, 0xB6, 0x92,
  0x6C, 0x70, 0x48, 0x50, 0xFD, 0xED, 0xB9, 0xDA, 0x5E, 0x15, 0x46, 0x57, 0xA7, 0x8D, 0x9D, 0x84,
  0x90, 0xD8, 0xAB, 0x00, 0x8C, 0xBC, 0xD3, 0x0A, 0xF7, 0xE4, 0x58, 0x05, 0xB8, 0xB3, 0x45, 0x06,
  0xD0, 0x2C, 0x1E, 0x8F, 0xCA, 0x3F, 0x0F, 0x02, 0xC1, 0xAF, 0xBD, 0x03, 0x01, 0x13, 0x8A, 0x6B,
  0x3A, 0x91, 0x11, 0x41, 0x4F, 0x67, 0xDC, 0xEA, 0x97, 0xF2, 0xCF, 0xCE, 0xF0, 0xB4, 0xE6, 0x73,
  0x96, 0xAC, 0x74, 0x22, 0xE7, 0xAD, 0x35, 0x85, 0xE2, 0xF9, 0x37, 0xE8, 0x1C, 0x75, 0xDF, 0x6E,
  0x47, 0xF1, 0x1A, 0x71, 0x1D, 0x29, 0xC5, 0x89, 0x6F, 0xB7, 0x62, 0x0E, 0xAA, 0x18, 0xBE, 0x1B,
  0xFC, 0x56, 0x3E, 0x4B, 0xC6, 0xD2, 0x79, 0x20, 0x9A, 0xDB, 0xC0, 0xFE, 0x78, 0xCD, 0x5A, 0xF4,
  0x1F, 0xDD, 0xA8, 0x33, 0x88, 0x07, 0xC7, 0x31, 0xB1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xEC, 0x5F,
  0x60, 0x51, 0x7F, 0xA9, 0x19, 0xB5, 0x4A, 0x0D, 0x2D, 0xE5, 0x7A, 0x9F, 0x93, 0xC9, 0x9C, 0xEF,
  0xA0, 0xE0, 0x3B, 0x4D, 0xAE, 0x2A, 0xF5, 0xB0, 0xC8, 0xEB, 0xBB, 0x3C, 0x83, 0x53, 0x99, 0x61,
  0x17, 0x2B, 0x04, 0x7E, 0xBA, 0x77, 0xD6, 0x26, 0xE1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0C, 0x7D
};

static int parity(uint8_t v) {
  v = v ^ (v >> 4);
  v = v ^ (v >> 2);
  v = v ^ (v >> 1);
  return !!(v&1);
}

KLT_AES_OPTIONAL_STATIC void aes_gen_sbox(void) {
  uint8_t v;
  gf8_init_tables(0x1B);
  v = 0;
  do {
    uint8_t inv;
    uint8_t part_sub;
    uint8_t result;
    int n;
    static uint8_t bitmtx[8] = {
      0xF1,
      0xE3,
      0xC7,
      0x8F,
      0x1F,
      0x3E,
      0x7C,
      0xF8
    };
    if (!v) inv = 0;
    else inv = gf8_tbl_inverse(v);

    part_sub = 0;
    for (n = 0; n < 8; n++) {
      if (parity(inv & bitmtx[n])) {
        part_sub |= 1 << n;
      }
    }

    result = part_sub ^ 0x63;

    aes_sbox_table[v] = result;
    aes_inv_sbox_table[result] = v;

    v++;
  } while (v);
}

KLT_AES_OPTIONAL_STATIC void aes_gen_c(void) {
  size_t n;
  printf("static uint8_t aes_sbox_table[256] = {\n");
  for (n = 0; n < sizeof(aes_sbox_table) / sizeof(*aes_sbox_table); ++n) {
    if (!(n % 16)) {
      if (n) {
        printf(",\n  ");
      }
      else {
        printf("  ");
      }
    }
    else {
      printf(", ");
    }
    printf("0x%02X", aes_sbox_table[n]);
  }
  printf("\n};\n");

  printf("static uint8_t aes_inv_sbox_table[256] = {\n");
  for (n = 0; n < sizeof(aes_inv_sbox_table) / sizeof(*aes_inv_sbox_table); ++n) {
    if (!(n % 16)) {
      if (n) {
        printf(",\n  ");
      }
      else {
        printf("  ");
      }
    }
    else {
      printf(", ");
    }
    printf("0x%02X", aes_inv_sbox_table[n]);
  }
  printf("\n};\n");
}

static void aes_sbox(uint8_t *state) {
  size_t n;
  for (n = 0; n < 16; n++) {
    state[n] = aes_sbox_table[state[n]];
  }
}

static void aes_inv_sbox(uint8_t *state) {
  size_t n;
  for (n = 0; n < 16; n++) {
    state[n] = aes_inv_sbox_table[state[n]];
  }
}

static void aes_subword(uint8_t *word) {
  size_t n;
  for (n = 0; n < 4; n++) {
    word[n] = aes_sbox_table[word[n]];
  }
}

static void aes_rotword(uint8_t *word) {
  uint8_t swap = word[0];
  word[0] = word[1];
  word[1] = word[2];
  word[2] = word[3];
  word[3] = swap;
}

static void aes_shiftrows(uint8_t *state) {
  /** Mapping of state to rows and columns:
   * 0 4 8 C
   * 1 5 9 D
   * 2 6 A E
   * 3 7 B F
   * with this in mind, swap becomes:
   * 0 4 8 C
   * 5 9 D 1
   * A E 2 6
   * F 3 7 B
   */
  uint8_t swap;
  /* first row unaffected, second row: */
  swap = state[0x1];
  state[0x1] = state[0x5];
  state[0x5] = state[0x9];
  state[0x9] = state[0xD];
  state[0xD] = swap;
  /* third row: */
  swap = state[0x2];
  state[0x2] = state[0xA];
  state[0xA] = swap;
  swap = state[0x6];
  state[0x6] = state[0xE];
  state[0xE] = swap;
  /* fourth row: */
  swap = state[0x3];
  state[0x3] = state[0xF];
  state[0xF] = state[0xB];
  state[0xB] = state[0x7];
  state[0x7] = swap;
}

static void aes_inv_shiftrows(uint8_t *state) {
  /** Mapping of state to rows and columns:
   * 0 4 8 c
   * 1 5 9 d
   * 2 6 a e
   * 3 7 b f
   * swap becomes
   * 0 4 8 c
   * d 1 5 9
   * a e 2 6
   * 7 b f 3
   */
  uint8_t swap;
  /* second row */
  swap = state[0x1];
  state[0x1] = state[0xd];
  state[0xd] = state[0x9];
  state[0x9] = state[0x5];
  state[0x5] = swap;
  /* third row */
  swap = state[0x2];
  state[0x2] = state[0xa];
  state[0xa] = swap;
  swap = state[0x6];
  state[0x6] = state[0xe];
  state[0xe] = swap;
  /* fourth row */
  swap = state[0x3];
  state[0x3] = state[0x7];
  state[0x7] = state[0xb];
  state[0xb] = state[0xf];
  state[0xf] = swap;
}

static void aes_addroundkey(uint8_t *state, uint8_t *w, int nb, int round) {
  int n;
  for (n = 0; n < nb; n++) {
    state[n * 4 + 0] ^= w[(nb * round + n) * 4 + 0];
    state[n * 4 + 1] ^= w[(nb * round + n) * 4 + 1];
    state[n * 4 + 2] ^= w[(nb * round + n) * 4 + 2];
    state[n * 4 + 3] ^= w[(nb * round + n) * 4 + 3];
  }
}


static void aes_mul_gf8_poly_mod_x4_1(uint8_t *result, uint8_t *a, uint8_t *b) {
  uint8_t d[4];
  d[0] = gf8_tbl_multiply(a[0],b[0]) ^ gf8_tbl_multiply(a[3],b[1]) ^ gf8_tbl_multiply(a[2],b[2]) ^ gf8_tbl_multiply(a[1],b[3]);
  d[1] = gf8_tbl_multiply(a[1],b[0]) ^ gf8_tbl_multiply(a[0],b[1]) ^ gf8_tbl_multiply(a[3],b[2]) ^ gf8_tbl_multiply(a[2],b[3]);
  d[2] = gf8_tbl_multiply(a[2],b[0]) ^ gf8_tbl_multiply(a[1],b[1]) ^ gf8_tbl_multiply(a[0],b[2]) ^ gf8_tbl_multiply(a[3],b[3]);
  d[3] = gf8_tbl_multiply(a[3],b[0]) ^ gf8_tbl_multiply(a[2],b[1]) ^ gf8_tbl_multiply(a[1],b[2]) ^ gf8_tbl_multiply(a[0],b[3]);
  result[0] = d[0];
  result[1] = d[1];
  result[2] = d[2];
  result[3] = d[3];
}

static void aes_mixcolumns(uint8_t *state) {
  uint8_t newstate[4*4];
  int n;
  for (n = 0; n < 4; n++) {
    newstate[4 * n + 0] = gf8_tbl_multiply(0x02, state[4*n+0])
                        ^ gf8_tbl_multiply(0x03, state[4*n+1])
                        ^ state[4*n+2]
                        ^ state[4*n+3];
    newstate[4 * n + 1] = state[4*n+0]
                        ^ gf8_tbl_multiply(0x02, state[4*n+1])
                        ^ gf8_tbl_multiply(0x03, state[4*n+2])
                        ^ state[4*n+3];
    newstate[4 * n + 2] = state[4*n+0]
                        ^ state[4*n+1]
                        ^ gf8_tbl_multiply(0x02, state[4*n+2])
                        ^ gf8_tbl_multiply(0x03, state[4*n+3]);
    newstate[4 * n + 3] = gf8_tbl_multiply(0x03, state[4*n+0])
                        ^ state[4*n+1]
                        ^ state[4*n+2]
                        ^ gf8_tbl_multiply(0x02, state[4*n+3]);
  }
  for (n = 0; n < (4*4); n++) {
    state[n] = newstate[n];
  }
}

static void aes_inv_mixcolumns(uint8_t *state) {
  uint8_t newstate[4*4];
  int n;
  for (n = 0; n < 4; n++) {
    newstate[4 * n + 0] = gf8_tbl_multiply(0xe, state[4 * n + 0])
                        ^ gf8_tbl_multiply(0xb, state[4 * n + 1])
                        ^ gf8_tbl_multiply(0xd, state[4 * n + 2])
                        ^ gf8_tbl_multiply(0x9, state[4 * n + 3]);
    newstate[4 * n + 1] = gf8_tbl_multiply(0x9, state[4 * n + 0])
                        ^ gf8_tbl_multiply(0xe, state[4 * n + 1])
                        ^ gf8_tbl_multiply(0xb, state[4 * n + 2])
                        ^ gf8_tbl_multiply(0xd, state[4 * n + 3]);
    newstate[4 * n + 2] = gf8_tbl_multiply(0xd, state[4 * n + 0])
                        ^ gf8_tbl_multiply(0x9, state[4 * n + 1])
                        ^ gf8_tbl_multiply(0xe, state[4 * n + 2])
                        ^ gf8_tbl_multiply(0xb, state[4 * n + 3]);
    newstate[4 * n + 3] = gf8_tbl_multiply(0xb, state[4 * n + 0])
                        ^ gf8_tbl_multiply(0xd, state[4 * n + 1])
                        ^ gf8_tbl_multiply(0x9, state[4 * n + 2])
                        ^ gf8_tbl_multiply(0xe, state[4 * n + 3]);
  }
  for (n = 0; n < (4 * 4); n++) {
    state[n] = newstate[n];
  }
}

static void aes_dumpstate(uint8_t *state) {
  int row, col;
  for (row = 0; row < 4; row++) {
    for (col = 0; col < 4; col++) {
      printf("%02x%c", state[row + col * 4], (col == 3) ? '\n' : ' ');
    }
  }
}


/* Nb = block-length (eg. 128) divided by 32 (always 4 for AES-128, AES-192 and AES-256) */
/* Nr = 10 for 128 bit (Nk=4), 12 for 192 bit, 14 for 256 bit.
 * Nk = 4 for 128 bit, 6 for 192 bit, 8 for 256 bit */
static void aes_keyexpansion(uint8_t *key/*[4 * Nk]*/,
                             uint8_t *w/*[4 * Nb*(Nr+1)]*/,
                             int nk,
                             int nr) {
  uint8_t temp[4];
  int i;
  uint8_t rcon = 0x01;

  /* copy key to first round in w.. */
  /* very obvious simplification possible, but sticking
   * closer to fips 197 this way */
  for (i = 0; i < nk; i++) {
    w[4 * i + 0] = key[4 * i];
    w[4 * i + 1] = key[4 * i + 1];
    w[4 * i + 2] = key[4 * i + 2];
    w[4 * i + 3] = key[4 * i + 3];
  }
  
  for (i = nk; i < 4 * (nr + 1); i++) {
    temp[0] = w[4 * (i - 1) + 0];
    temp[1] = w[4 * (i - 1) + 1];
    temp[2] = w[4 * (i - 1) + 2];
    temp[3] = w[4 * (i - 1) + 3];
    if (!(i % nk)) {
      aes_rotword(temp);
      aes_subword(temp);
      temp[0] ^= rcon;
      rcon = gf8_tbl_multiply(rcon, 0x02);
    }
    else if ((nk > 6) && ((i % nk) == 4)) {
      aes_subword(temp);
    }
    w[4 * i + 0] = w[4 * (i - nk) + 0] ^ temp[0];
    w[4 * i + 1] = w[4 * (i - nk) + 1] ^ temp[1];
    w[4 * i + 2] = w[4 * (i - nk) + 2] ^ temp[2];
    w[4 * i + 3] = w[4 * (i - nk) + 3] ^ temp[3];
  }
}

/* Nb == 4 */
static void aes_cipher(uint8_t *in/*[4*Nb]*/, uint8_t *out/*[4*Nb]*/, uint8_t *w/*[Nb*(Nr+1)*4]*/, int nr) {
  uint8_t state[4*4];
  int n, round;

  for (n = 0; n < (4*4); n++) {
    state[n] = in[n];
  }
  aes_addroundkey(state, w, 4, 0);
  for (round = 1; round < nr; round++) {
    aes_sbox(state);
    aes_shiftrows(state);
    aes_mixcolumns(state);
    aes_addroundkey(state, w, 4, round);
  }
  aes_sbox(state);
  aes_shiftrows(state);
  aes_addroundkey(state, w, 4, nr);

  for (n = 0; n < (4 * 4); n++) {
    out[n] = state[n];
  }
}

/* Nb == 4 */
static void aes_inv_cipher(uint8_t *in/*[4*Nb]*/, uint8_t *out/*[4*Nb]*/, uint8_t *w/*[Nb*(Nr+1)*4]*/, int nr) {
  uint8_t state[4 * 4];
  int n, round;
  
  for (n = 0; n < (4 * 4); n++) {
    state[n] = in[n];
  }

  aes_addroundkey(state, w, 4, nr);

  for (round = (nr - 1); round >= 1; round--) {
    aes_inv_shiftrows(state);
    aes_inv_sbox(state);
    aes_addroundkey(state, w, 4, round);
    aes_inv_mixcolumns(state);
  }

  aes_inv_shiftrows(state);
  aes_inv_sbox(state);
  aes_addroundkey(state, w, 4, 0);

  for (n = 0; n < (4 * 4); n++) {
    out[n] = state[n];
  }
}

KLT_AES_OPTIONAL_STATIC void aes128_keyexpansion(uint8_t *key/*[16]*/, uint8_t *w/*[176]*/) {
  aes_keyexpansion(key, w, 4, 10);
}

KLT_AES_OPTIONAL_STATIC void aes128_cipher(uint8_t *in/*[16]*/, uint8_t *out/*[16]*/, uint8_t *w/*[176]*/) {
  aes_cipher(in, out, w, 10);
}

KLT_AES_OPTIONAL_STATIC void aes128_inv_cipher(uint8_t *in/*[16]*/, uint8_t *out/*[16]*/, uint8_t *w/*[176]*/) {
  aes_inv_cipher(in, out, w, 10);
}


KLT_AES_OPTIONAL_STATIC void aes192_keyexpansion(uint8_t *key/*[24]*/, uint8_t *w/*[208]*/) {
  aes_keyexpansion(key, w, 6, 12);
}

KLT_AES_OPTIONAL_STATIC void aes192_cipher(uint8_t *in/*[16]*/, uint8_t *out/*[16]*/, uint8_t *w/*[208]*/) {
  aes_cipher(in, out, w, 12);
}

KLT_AES_OPTIONAL_STATIC void aes192_inv_cipher(uint8_t *in/*[16]*/, uint8_t *out/*[16]*/, uint8_t *w/*[208]*/) {
  aes_inv_cipher(in, out, w, 12);
}


KLT_AES_OPTIONAL_STATIC void aes256_keyexpansion(uint8_t *key/*[32]*/, uint8_t *w/*[240]*/) {
  aes_keyexpansion(key, w, 8, 14);
}

KLT_AES_OPTIONAL_STATIC void aes256_cipher(uint8_t *in/*[16]*/, uint8_t *out/*[16]*/, uint8_t *w/*[240]*/) {
  aes_cipher(in, out, w, 14);
}

KLT_AES_OPTIONAL_STATIC void aes256_inv_cipher(uint8_t *in/*[16]*/, uint8_t *out/*[16]*/, uint8_t *w/*[240]*/) {
  aes_inv_cipher(in, out, w, 14);
}
