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

#ifndef INCLUDED_STDLIB_H
#define INCLUDED_STDLIB_H
#include <stdlib.h>
#endif

#ifndef INCLUDED_STDINT_H
#define INCLUDED_STDINT_H
#include <stdint.h>
#endif

#ifndef INCLUDED_STDIO_H
#define INCLUDED_STDIO_H
#include <stdio.h>
#endif

#include "gf8.h"

/* Defaults use 0x11B as default irreducible polynomial, as you'd use for AES.
 * To override with another, invoke gf8_init_tables() */
static uint8_t gf8_log_table[256] = {
  0x00, 0x00, 0x19, 0x01, 0x32, 0x02, 0x1A, 0xC6, 0x4B, 0xC7, 0x1B, 0x68,
  0x33, 0xEE, 0xDF, 0x03, 0x64, 0x04, 0xE0, 0x0E, 0x34, 0x8D, 0x81, 0xEF,
  0x4C, 0x71, 0x08, 0xC8, 0xF8, 0x69, 0x1C, 0xC1, 0x7D, 0xC2, 0x1D, 0xB5,
  0xF9, 0xB9, 0x27, 0x6A, 0x4D, 0xE4, 0xA6, 0x72, 0x9A, 0xC9, 0x09, 0x78,
  0x65, 0x2F, 0x8A, 0x05, 0x21, 0x0F, 0xE1, 0x24, 0x12, 0xF0, 0x82, 0x45,
  0x35, 0x93, 0xDA, 0x8E, 0x96, 0x8F, 0xDB, 0xBD, 0x36, 0xD0, 0xCE, 0x94,
  0x13, 0x5C, 0xD2, 0xF1, 0x40, 0x46, 0x83, 0x38, 0x66, 0xDD, 0xFD, 0x30,
  0xBF, 0x06, 0x8B, 0x62, 0xB3, 0x25, 0xE2, 0x98, 0x22, 0x88, 0x91, 0x10,
  0x7E, 0x6E, 0x48, 0xC3, 0xA3, 0xB6, 0x1E, 0x42, 0x3A, 0x6B, 0x28, 0x54,
  0xFA, 0x85, 0x3D, 0xBA, 0x2B, 0x79, 0x0A, 0x15, 0x9B, 0x9F, 0x5E, 0xCA,
  0x4E, 0xD4, 0xAC, 0xE5, 0xF3, 0x73, 0xA7, 0x57, 0xAF, 0x58, 0xA8, 0x50,
  0xF4, 0xEA, 0xD6, 0x74, 0x4F, 0xAE, 0xE9, 0xD5, 0xE7, 0xE6, 0xAD, 0xE8,
  0x2C, 0xD7, 0x75, 0x7A, 0xEB, 0x16, 0x0B, 0xF5, 0x59, 0xCB, 0x5F, 0xB0,
  0x9C, 0xA9, 0x51, 0xA0, 0x7F, 0x0C, 0xF6, 0x6F, 0x17, 0xC4, 0x49, 0xEC,
  0xD8, 0x43, 0x1F, 0x2D, 0xA4, 0x76, 0x7B, 0xB7, 0xCC, 0xBB, 0x3E, 0x5A,
  0xFB, 0x60, 0xB1, 0x86, 0x3B, 0x52, 0xA1, 0x6C, 0xAA, 0x55, 0x29, 0x9D,
  0x97, 0xB2, 0x87, 0x90, 0x61, 0xBE, 0xDC, 0xFC, 0xBC, 0x95, 0xCF, 0xCD,
  0x37, 0x3F, 0x5B, 0xD1, 0x53, 0x39, 0x84, 0x3C, 0x41, 0xA2, 0x6D, 0x47,
  0x14, 0x2A, 0x9E, 0x5D, 0x56, 0xF2, 0xD3, 0xAB, 0x44, 0x11, 0x92, 0xD9,
  0x23, 0x20, 0x2E, 0x89, 0xB4, 0x7C, 0xB8, 0x26, 0x77, 0x99, 0xE3, 0xA5,
  0x67, 0x4A, 0xED, 0xDE, 0xC5, 0x31, 0xFE, 0x18, 0x0D, 0x63, 0x8C, 0x80,
  0xC0, 0xF7, 0x70, 0x07
};

static uint8_t gf8_exp_table[256] = {
  0x01, 0x03, 0x05, 0x0F, 0x11, 0x33, 0x55, 0xFF, 0x1A, 0x2E, 0x72, 0x96,
  0xA1, 0xF8, 0x13, 0x35, 0x5F, 0xE1, 0x38, 0x48, 0xD8, 0x73, 0x95, 0xA4,
  0xF7, 0x02, 0x06, 0x0A, 0x1E, 0x22, 0x66, 0xAA, 0xE5, 0x34, 0x5C, 0xE4,
  0x37, 0x59, 0xEB, 0x26, 0x6A, 0xBE, 0xD9, 0x70, 0x90, 0xAB, 0xE6, 0x31,
  0x53, 0xF5, 0x04, 0x0C, 0x14, 0x3C, 0x44, 0xCC, 0x4F, 0xD1, 0x68, 0xB8,
  0xD3, 0x6E, 0xB2, 0xCD, 0x4C, 0xD4, 0x67, 0xA9, 0xE0, 0x3B, 0x4D, 0xD7,
  0x62, 0xA6, 0xF1, 0x08, 0x18, 0x28, 0x78, 0x88, 0x83, 0x9E, 0xB9, 0xD0,
  0x6B, 0xBD, 0xDC, 0x7F, 0x81, 0x98, 0xB3, 0xCE, 0x49, 0xDB, 0x76, 0x9A,
  0xB5, 0xC4, 0x57, 0xF9, 0x10, 0x30, 0x50, 0xF0, 0x0B, 0x1D, 0x27, 0x69,
  0xBB, 0xD6, 0x61, 0xA3, 0xFE, 0x19, 0x2B, 0x7D, 0x87, 0x92, 0xAD, 0xEC,
  0x2F, 0x71, 0x93, 0xAE, 0xE9, 0x20, 0x60, 0xA0, 0xFB, 0x16, 0x3A, 0x4E,
  0xD2, 0x6D, 0xB7, 0xC2, 0x5D, 0xE7, 0x32, 0x56, 0xFA, 0x15, 0x3F, 0x41,
  0xC3, 0x5E, 0xE2, 0x3D, 0x47, 0xC9, 0x40, 0xC0, 0x5B, 0xED, 0x2C, 0x74,
  0x9C, 0xBF, 0xDA, 0x75, 0x9F, 0xBA, 0xD5, 0x64, 0xAC, 0xEF, 0x2A, 0x7E,
  0x82, 0x9D, 0xBC, 0xDF, 0x7A, 0x8E, 0x89, 0x80, 0x9B, 0xB6, 0xC1, 0x58,
  0xE8, 0x23, 0x65, 0xAF, 0xEA, 0x25, 0x6F, 0xB1, 0xC8, 0x43, 0xC5, 0x54,
  0xFC, 0x1F, 0x21, 0x63, 0xA5, 0xF4, 0x07, 0x09, 0x1B, 0x2D, 0x77, 0x99,
  0xB0, 0xCB, 0x46, 0xCA, 0x45, 0xCF, 0x4A, 0xDE, 0x79, 0x8B, 0x86, 0x91,
  0xA8, 0xE3, 0x3E, 0x42, 0xC6, 0x51, 0xF3, 0x0E, 0x12, 0x36, 0x5A, 0xEE,
  0x29, 0x7B, 0x8D, 0x8C, 0x8F, 0x8A, 0x85, 0x94, 0xA7, 0xF2, 0x0D, 0x17,
  0x39, 0x4B, 0xDD, 0x7C, 0x84, 0x97, 0xA2, 0xFD, 0x1C, 0x24, 0x6C, 0xB4,
  0xC7, 0x52, 0xF6, 0x00
};

KLT_GF8_OPTIONAL_STATIC uint8_t gf8_xtime(uint8_t val, uint8_t irreducible) {
  if (val & 0x80) {
    return (val + val) ^ irreducible;
  }
  return val + val;
}

KLT_GF8_OPTIONAL_STATIC uint8_t gf8_mul(uint8_t lvalue, uint8_t rvalue, uint8_t irreducible) {
  int n;
  uint8_t xt = lvalue;
  uint8_t r = 0;
  for (n = 0; n < 8; n++) {
    if (rvalue & 1) {
      r = r ^ xt;
    }
    xt = gf8_xtime(xt, irreducible);
    rvalue >>= 1;
  }
  
  return r;
}

KLT_GF8_OPTIONAL_STATIC void gf8_init_tables(uint8_t irreducible) {
  uint8_t val, n;
  n = 0;
  val = 1;
  do {
    gf8_log_table[val] = n;
    gf8_exp_table[n] = val;
    /* using 0x3 as the value being exponented, 
     * though that's irrelevant as long as its
     * exponentiation modulo the irreducible
     * generates the entire value range. */
    val = gf8_mul(val, 0x3, irreducible);
    n++;
  } while (val != 1);
}

KLT_GF8_OPTIONAL_STATIC uint8_t gf8_tbl_inverse(uint8_t x) {
  if (!x) return 0;
  if (x == 1) return 1;
  /* return multiplier M that, when viewed from
   * its log equivalent, would result in x * M == 1.
   */
  return gf8_exp_table[255 - gf8_log_table[x]];
}

KLT_GF8_OPTIONAL_STATIC uint8_t gf8_tbl_divide(uint8_t lvalue, uint8_t rvalue) {
  uint8_t log_lvalue;
  if (!lvalue) return 0;
  if (!rvalue) return 0; /* really invalid */
  if (rvalue == 1) return lvalue;
  log_lvalue = gf8_log_table[lvalue];
  if (gf8_log_table[rvalue] > log_lvalue) {
    return gf8_exp_table[log_lvalue + 255 - gf8_log_table[rvalue]];
  }
  return gf8_exp_table[log_lvalue - gf8_log_table[rvalue]];
}

KLT_GF8_OPTIONAL_STATIC uint8_t gf8_tbl_multiply(uint8_t lvalue, uint8_t rvalue) {
  uint16_t log_values;
  if (!lvalue) return 0;
  if (!rvalue) return 0;
  log_values = ((uint16_t)gf8_log_table[lvalue]) +
               ((uint16_t)gf8_log_table[rvalue]);
  if (log_values >= 255) {
    return gf8_exp_table[log_values - 255];
  }
  return gf8_exp_table[log_values];
}

KLT_GF8_OPTIONAL_STATIC void gf8_gen_c(void) {
  size_t n;
  /* Use 0x11B as default irreducible polynomial, as you'd use for AES */
  gf8_init_tables(0x1B);

  printf("static uint8_t gf8_log_table[256] = {\n");
  for (n = 0; n < sizeof(gf8_log_table) / sizeof(*gf8_log_table); ++n) {
    if (!(n % 12)) {
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
    printf("0x%02X", gf8_log_table[n]);
  }
  printf("\n};\n");

  printf("static uint8_t gf8_exp_table[256] = {\n");
  for (n = 0; n < sizeof(gf8_exp_table) / sizeof(*gf8_exp_table); ++n) {
    if (!(n % 12)) {
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
    printf("0x%02X", gf8_exp_table[n]);
  }
  printf("\n};\n");
}
