/* Copyright 2020-2023 Kinglet B.V.
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

#ifndef STDINT_H_INCLUDED
#define STDINT_H_INCLUDED
#include <stdint.h>
#endif

#ifndef MUL_H_INCLUDED
#define MUL_H_INCLUDED
#include "mul.h"
#endif

int multiply_uint64(uint64_t a, uint64_t b, uint64_t *rhi, uint64_t *rlo) {
  /* Multiplication:
   * xy = z2 b^2 + z1 b + z0
   * where:
   * z2 = x1y1
   * z1 = x1y0 + x0y1
   * z0 = x0y0
   * and b is base 2^32 */
  uint32_t x0 = a & 0xFFFFFFFF;
  uint32_t x1 = (a >> 32) & 0xFFFFFFFF;
  uint32_t y0 = b & 0xFFFFFFFF;
  uint32_t y1 = (b >> 32) & 0xFFFFFFFF;

  uint64_t z2 = ((uint64_t)x1) * ((uint64_t)y1);
  uint64_t z0 = ((uint64_t)x0) * ((uint64_t)y0);
  uint64_t x1y0 = ((uint64_t)x1) * ((uint64_t)y0);
  uint64_t x0y1 = ((uint64_t)x0) * ((uint64_t)y1);
  uint64_t z1 = x1y0 + x0y1;
  if (z1 < x1y0) {
    /* Carry overflow from z1; add in to z2, note also that z2
     * is at most 0xFFFFFFFE00000001 and so consequently would
     * hereby become 0xFFFFFFFF00000001 in the worst case (we
     * therefore don't need to check for overflow) */
    z2 += ((uint64_t)1) << 32;
  }
  uint64_t result0;
  uint64_t result1;
  result0 = z0 + (z1 << 32);
  if (result0 < z0) {
    /* Overflow from z0 + loword(z1) into upper word of result */
    z2++;
  }
  result1 = (z1 >> 32) + z2;

  if (rlo) {
    *rlo = result0;
  }
  if (rhi) {
    /* note: can skip z2 if not interested in rhi */
    *rhi = result1;
  }

  return !!result1;
}

int multiply_size_t(size_t a, size_t b, size_t *rhi, size_t *rlo) {
  /* Rely on compiler to optimize out this constant branch */
  if (sizeof(size_t) == 4) {
    /* size_t is 32 bits */
    uint64_t ab = ((uint64_t)a) * ((uint64_t)b);
    if (rlo) *rlo = (size_t)ab;
    size_t hi = (size_t)(ab >> 32);
    if (rhi) *rhi = hi;
    return !!hi;
  }
  else {
    /* size_t is 64 bits */
    return multiply_uint64(a, b, (uint64_t *)rhi, (uint64_t *)rlo);
  }
}

