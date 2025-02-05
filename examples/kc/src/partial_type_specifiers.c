/* Copyright 2023-2025 Kinglet B.V.
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

#ifndef STDLIB_H_INCLUDED
#define STDLIB_H_INCLUDED
#include <stdlib.h>
#endif

#ifndef ASSERT_H_INCLUDED
#define ASSERT_H_INCLUDED
#include <assert.h>
#endif

#ifndef PARTIAL_TYPE_SPECIFIERS_H_INCLUDED
#define PARTIAL_TYPE_SPECIFIERS_H_INCLUDED
#include "partial_type_specifiers.h"
#endif

static int pts_type_combination_table_[PTS_NUM_SINGULAR * PTS_LAST_TYPE_SPECIFIER ] = {
  // Singular type specifiers: TS_VOID , TS_CHAR , TS_SHORT, ... as columns
  // All type specifiers starting from TS_NONE as rows..
  // Therefore TS_NUM_SINGLULAR - 1 columns and TS_LAST_TYPE_SPECIFIER rows..
  0
};

static const int constituent_parts_[] = {
  PTS_NONE, 0,
  PTS_VOID, PTS_VOID, 0,
  PTS_CHAR, PTS_CHAR, 0,
  PTS_SHORT, PTS_SHORT, 0,
  PTS_INT, PTS_INT, 0,
  PTS_SIGNED, PTS_SIGNED, 0,
  PTS_UNSIGNED, PTS_UNSIGNED, 0,
  PTS_LONG, PTS_LONG, 0,
  PTS_FLOAT, PTS_FLOAT, 0,
  PTS_DOUBLE, PTS_DOUBLE, 0,
  PTS_BOOL, PTS_BOOL, 0,
  PTS_COMPLEX, PTS_COMPLEX, 0,
  PTS_IMAGINARY, PTS_IMAGINARY, 0,
  PTS_TYPEDEF_NAME, PTS_TYPEDEF_NAME, 0,

  PTS_SIGNED_CHAR, PTS_SIGNED, PTS_CHAR, 0,
  PTS_UNSIGNED_CHAR, PTS_UNSIGNED, PTS_CHAR, 0,
  PTS_SIGNED_SHORT, PTS_SIGNED, PTS_SHORT, 0,
  PTS_SHORT_INT, PTS_SHORT, PTS_INT, 0,
  PTS_SIGNED_SHORT_INT, PTS_SIGNED, PTS_SHORT, PTS_INT, 0,
  
  PTS_UNSIGNED_SHORT, PTS_UNSIGNED, PTS_SHORT, 0,
  PTS_UNSIGNED_SHORT_INT, PTS_UNSIGNED, PTS_SHORT, PTS_INT, 0,

  PTS_SIGNED_INT, PTS_SIGNED, PTS_INT, 0,

  PTS_UNSIGNED_INT, PTS_UNSIGNED, PTS_INT, 0,

  PTS_SIGNED_LONG, PTS_SIGNED, PTS_LONG, 0,
  PTS_LONG_INT, PTS_LONG, PTS_INT, 0,
  PTS_SIGNED_LONG_INT, PTS_SIGNED, PTS_LONG, PTS_INT, 0,

  PTS_UNSIGNED_LONG, PTS_UNSIGNED, PTS_LONG, 0,
  PTS_UNSIGNED_LONG_INT, PTS_UNSIGNED, PTS_LONG, PTS_INT, 0,

  PTS_LONG_LONG, PTS_LONG, PTS_LONG, 0,
  PTS_SIGNED_LONG_LONG, PTS_SIGNED, PTS_LONG, PTS_LONG, 0,
  PTS_LONG_LONG_INT, PTS_LONG, PTS_LONG, PTS_INT, 0,
  PTS_SIGNED_LONG_LONG_INT, PTS_SIGNED, PTS_LONG, PTS_LONG, PTS_INT, 0,

  PTS_UNSIGNED_LONG_LONG, PTS_UNSIGNED, PTS_LONG, PTS_LONG,  0,
  PTS_UNSIGNED_LONG_LONG_INT, PTS_UNSIGNED, PTS_LONG, PTS_LONG, PTS_INT, 0,

  PTS_LONG_DOUBLE, PTS_LONG, PTS_DOUBLE, 0,
  PTS_FLOAT_COMPLEX, PTS_FLOAT, PTS_COMPLEX, 0,
  PTS_DOUBLE_COMPLEX, PTS_DOUBLE, PTS_COMPLEX, 0,
  PTS_LONG_DOUBLE_COMPLEX, PTS_LONG, PTS_DOUBLE, PTS_COMPLEX, 0,
  PTS_FLOAT_IMAGINARY, PTS_FLOAT, PTS_IMAGINARY, 0,
  PTS_DOUBLE_IMAGINARY, PTS_DOUBLE, PTS_IMAGINARY, 0,
  PTS_LONG_DOUBLE_IMAGINARY, PTS_LONG, PTS_DOUBLE, PTS_IMAGINARY, 0,

  PTS_LONG_COMPLEX, PTS_LONG, PTS_COMPLEX, 0,
  PTS_LONG_IMAGINARY, PTS_LONG, PTS_IMAGINARY, 0,

  PTS_STRUCT, PTS_STRUCT, 0,
  PTS_UNION, PTS_UNION, 0,
  PTS_ENUM, PTS_ENUM, 0,
};

static pts_type_specifier_t pts_combine_type_slow(pts_type_specifier_t compound, pts_type_specifier_t singular) {
  size_t num_parts = 0;
  pts_type_specifier_t parts[5];
  size_t n;
  if (compound == PTS_NONE) return singular;
  if (singular == PTS_NONE) return compound;
  for (n = 0; n < sizeof(constituent_parts_) / sizeof(*constituent_parts_); ++n) {
    if (compound == constituent_parts_[n]) {
      /* Found compound type specifier, extract its part composition */
      ++n;
      while (constituent_parts_[n]) {
        parts[num_parts++] = constituent_parts_[n++];
      }
      break;
    }
    else {
      /* Not a compound match, skip */
      ++n;
      while (constituent_parts_[n]) {
        n++;
      }
    }
  }
  if (!num_parts) {
    /* compound type specifier could not be found */
    return PTS_NONE;
  }
  /* Add singular to the parts - now the parts make up the combined type; if we can find it */
  parts[num_parts++] = singular;

  size_t num_cmp_parts;
  pts_type_specifier_t cmp_parts[5];
  int candidate;
  for (n = 0; n < sizeof(constituent_parts_) / sizeof(*constituent_parts_); ++n) {
    num_cmp_parts = 0;
    candidate = constituent_parts_[n++];
    while (constituent_parts_[n]) {
      cmp_parts[num_cmp_parts++] = constituent_parts_[n++];
    }
    if (num_cmp_parts == num_parts) {
      /* Potential for match, equal number of parts */
      size_t k;
      for (k = 0; k < num_parts; ++k) {
        /* Find and remove this part from the cmp_parts list */
        size_t h;
        int matched = 0;
        for (h = 0; h < num_cmp_parts; ++h) {
          if (parts[k] == cmp_parts[h]) {
            /* Remove match from list, this ensures parts that appear multiple times will
             * match up correctly counted. */
            cmp_parts[h] = cmp_parts[num_cmp_parts - 1];
            num_cmp_parts--;
            matched = 1;
            break;
          }
        }
        if (!matched) {
          /* part k could not be found in cmp_parts */
          break;
        }
      }
      if (k == num_parts) {
        /* All parts[] accounted for in cmp_parts[] and they're of equal
         * length, so we have a match */
        return candidate;
      }
    }
  }
  /* Failed to find match.. */
  return PTS_NONE;
}

void pts_init() {
  int compound, singular;
  for (compound = PTS_NONE; compound < PTS_LAST_TYPE_SPECIFIER; ++compound) {
    for (singular = PTS_NONE; singular <= PTS_NUM_SINGULAR; ++singular) {
      pts_type_combination_table_[compound * PTS_NUM_SINGULAR + singular] = pts_combine_type_slow(compound, singular);
    }
  }
}

const char *pts_str(pts_type_specifier_t ts) {
#define xx(is_valid, id, dt_type, str) case id: return str;
  switch (ts) {
    ENUM_PARTIAL_TYPE_SPECIFIERS
#undef xx
  }
  return "???";
}

enum type_kind pts_type_kind(pts_type_specifier_t ts) {
#define xx(is_valid, id, c_type_kind, str) case id: return c_type_kind;
  switch (ts) {
    ENUM_PARTIAL_TYPE_SPECIFIERS
#undef xx
  }
  return PTS_INVALID;
}

int pts_combine_type(pts_type_specifier_t compound, pts_type_specifier_t singular) {
  assert((compound >= PTS_NONE) && (compound <= PTS_LAST_TYPE_SPECIFIER) && "compound value out of range");
  assert((singular >= PTS_NONE) && (singular <= PTS_NUM_SINGULAR) && "singular value out of range");
  pts_type_specifier_t r;
  r = pts_type_combination_table_[compound * PTS_NUM_SINGULAR + singular];
  return r;
}
