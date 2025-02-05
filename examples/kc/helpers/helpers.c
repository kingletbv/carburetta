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

#ifndef _MSC_VER
/* We need the declaration for vasprintf() which is a GNU extension,
 * this instructs clang to include the declaration. */
#define _GNU_SOURCE
#endif

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <inttypes.h>
#include <assert.h>

#ifdef __linux__
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#endif

#ifdef _WIN32
#include <tchar.h>

#define STRICT
#include <Windows.h>
#endif

#ifndef _WIN32
#include <termios.h>
#include <unistd.h>
#endif

#include "helpers.h"

#define KLT_LOG_MODULE "/helpers"
#include "klt_logger.h"

void linebuf_init(struct linebuf *lb) {
  lb->line = NULL;
  lb->line_size = 0;
  lb->line_size_allocated;
  lb->current_pos = 0;
  lb->current_buffer = NULL;
  lb->current_buffer_size = 0;
  lb->is_eof = 0;
}

void linebuf_cleanup(struct linebuf *lb) {
  if (lb->line) {
    free(lb->line);
  }
}

static int linebuf_expand_for_extra_char(struct linebuf *lb) {
  if (lb->line_size == lb->line_size_allocated) {
    size_t new_size_allocated;
    if (lb->line_size_allocated) {
      new_size_allocated = lb->line_size_allocated * 2;
    }
    else {
      new_size_allocated = 128;
    }
    
    if (new_size_allocated < lb->line_size_allocated) {
      return -1;
    }
    void *new_line = realloc(lb->line, new_size_allocated);
    if (!new_line) {
      return -1;
    }
    lb->line_size_allocated = new_size_allocated;
    lb->line = (char *)new_line;
  }
  return 0;
}

static int linebuf_append_null_terminator(struct linebuf *lb) {
  int r;
  r = linebuf_expand_for_extra_char(lb);
  if (r) return r;
  lb->line[lb->line_size] = '\0';
  return 0;
}

int linebuf_next_line(struct linebuf *lb) {
  lb->line_size = 0;

  for (;;) {
    if (lb->current_pos == lb->current_buffer_size) {
      if (lb->is_eof) {
        if (lb->line_size == lb->line_size_allocated) {
          if (linebuf_append_null_terminator(lb)) {
            return -1; /* Mem error */
          }
          return 1;
        }
      }
      /* Not EOF; just get more data */
      return 0;
    }
    char c = ((char *)lb->current_buffer)[lb->current_pos++];
    if (linebuf_expand_for_extra_char(lb)) {
      return -1; /* Mem error expanding buffer */
    }
    lb->line[lb->line_size++] = c;
    
    if (c == '\n') {
      if (linebuf_append_null_terminator(lb)) {
        return -1; /* mem error appending null terminator */
      }
      return 1;
    }
  }
}

void linebuf_set_input(struct linebuf *lb, void *buf, size_t bufsize) {
  lb->current_buffer = buf;
  lb->current_buffer_size = bufsize;
  lb->current_pos = 0;
}

void linebuf_finish(struct linebuf *lb) {
  lb->is_eof = 1;
}

int linebuf_is_finished(const struct linebuf *lb) {
  return lb->is_eof && (lb->current_pos == lb->current_buffer_size);
}

size_t linebuf_read_bytes(struct linebuf *lb, void **buf, size_t size_requested) {
  size_t size_available = lb->current_buffer_size - lb->current_pos;
  if (size_available) {
    *buf = ((char *)(lb->current_buffer)) + lb->current_pos;
  }
  if (size_available > size_requested) {
    lb->current_pos += size_requested;
    return size_requested;
  }
  else {
    lb->current_pos = lb->current_buffer_size;
    return size_available;
  }
}

char *linebuf_current_line(struct linebuf *lb) {
  assert(lb->line && "Call linebuf_next_line() at least once before accessing the current line");
  return lb->line;
}

size_t linebuf_current_line_size(const struct linebuf *lb) {
  return lb->line_size;
}


#ifdef _MSC_VER
#ifdef _TCHAR_DEFINED
/* Silence non-sensical deprecation complaints, could also define _CRT_SECURE_NO_WARNINGS project-wide,
 * but that would have to be done for every project using the helpers; this is more local. */
#pragma warning( disable: 4996 )
TCHAR *tmasprintf(const TCHAR *format, ...) {
  va_list args;
  va_start(args, format);
  size_t len = _vsctprintf(format, args) + 1;
  TCHAR *p = (TCHAR *)malloc(len * sizeof(TCHAR));
  _vsntprintf(p, len, format, args);
  va_end(args);
  return p;
}
#endif
#endif

void *arealloc(void *mem, size_t count, size_t size) {
  if (((count >= (((size_t)1) << sizeof(size_t)*4)) ||
       (size >= (((size_t)1) << (sizeof(size_t)*4)))) &&
      (((size_t)-1)/(count) < size)) {
    return NULL;
  }
  return realloc(mem, count * size);
}

#ifdef _MSC_VER
char *vmasprintf(const char *format, va_list args) {
  char *p = (char *)malloc(_vscprintf(format, args) + 1);
  vsprintf(p, format, args);
  return p;
}
#else
char *vmasprintf(const char *format, va_list args) {
  char *p;
  vasprintf(&p, format, args);
  return p;
}
#endif

#ifdef _MSC_VER
char *masprintf(const char *format, ...) {
  va_list args;
  va_start(args, format);
  char *p = (char *)malloc(_vscprintf(format, args) + 1);
  vsprintf(p, format, args);
  va_end(args);
  return p;
}
#else
char *masprintf(const char *format, ...) {
  va_list args;
  va_start(args, format);
  char *p;
  vasprintf(&p, format, args);
  va_end(args);
  return p;
}
#endif

char *bindumpf(const void *data, size_t datasize, size_t baseoffset) {
  /* "0000000000000000  47 45 54 20 2F 20 48 54  54 50 2F 31 2E 31 0D 0A  GET / HTTP/1.1.." + "\n"
   *  OOOOOOOOOOOOOOOOSxxxxxxxxxxxxxxxxxxxxxxxxSxxxxxxxxxxxxxxxxxxxxxxxxSScccccccccccccccc" +  "n"
   * O=offset for row, 16 bytes
   * S=layout Spaces, 4 bytes
   * x=hexadecimals, always 3*16 = 48 bytes per row, triple-space when skipped
   * c=character representation of value; left out if skipped at the end, space if skipped at the start
   * n=single newline at the end of each row including the last
   */
  /* Find the start of the first row, this is the baseoffset truncated down per 16 bytes */
  size_t true_base = baseoffset & ~(size_t)15;
  size_t first_row_skip = baseoffset - true_base;
  size_t num_rows = (datasize + first_row_skip + 15) / 16;
  size_t last_row_size = 1 + ((datasize + first_row_skip - 1) & 15);

  /* XXX: Should check for overflow on size calculation */
  size_t full_size = num_rows * (16 + 4 + 48 + 16 + 1) - 16 + last_row_size + 1 /* null terminator */;
  char *str = (char *)malloc(full_size);
  char *p = str;
  size_t row;
  for (row = 0; row < num_rows; ++row) {
    sprintf(p, "%016zx ", true_base + row * 16);
    p += 16 + 1;
    size_t n;
    for (n = 0; n < 16; ++n) {
      /* byte_offset as counted from true_base, which may be less than baseoffset */
      size_t byte_offset = row * 16 + n;
      if ((byte_offset >= first_row_skip) && (byte_offset < (first_row_skip + datasize))) {
        if (n != 8) {
          sprintf(p, " %02X", (int)(((uint8_t*)data)[byte_offset - first_row_skip]));
          p += 3;
        }
        else {
          sprintf(p, "  %02X", (int)(((uint8_t*)data)[byte_offset - first_row_skip]));
          p += 4;
        }
      }
      else {
        *p++ = ' '; *p++ = ' '; *p++ = ' ';
        if (n == 8) *p++ = ' ';
      }
    }
    *p++ = ' ';
    *p++ = ' ';
    for (n = 0; n < 16; ++n) {
      /* byte_offset as counted from true_base, which may be less than baseoffset */
      size_t byte_offset = row * 16 + n;
      if ((byte_offset >= first_row_skip) && (byte_offset < (first_row_skip + datasize))) {
        char c = ((uint8_t*)data)[byte_offset - first_row_skip];
        if (isprint((int)(uint8_t)c)) {
          *p++ = c;
        }
        else {
          *p++ = '.';
        }
      }
      else if (byte_offset < (first_row_skip + datasize)) {
        *p++ = ' ';
      }
      else {
        /* Reached the end; no need to pad spaces */
        break;
      }
    }
    *p++ = '\n';
  }
  *p++ = '\0';
  assert(((p - str) == full_size) && "Buffer size needed misjudged.");
  return str;
}

char *bindumpc(const void *data, size_t datasize, size_t baseoffset) {
  /* "  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,\n"
   */
  size_t size_needed = ((15 + datasize) / 16) * 98 + 1 /* 0 term. */;
  char *cb = (char *)malloc(size_needed);
  char *p = cb;
  size_t n;
  static const char *hex = "0123456789ABCDEF";
  *p++ = ' ';
  *p++ = ' ';
  const uint8_t *bin = (const uint8_t *)data;
  for (n = 0; n < datasize; ++n) {
    *p++ = '0'; *p++ = 'x'; *p++ = hex[(*bin) >> 4]; *p++ = hex[(*bin) & 0xF];
    bin++;
    if ((n + 1) != datasize) {
      *p++ = ',';
      if (!((n + 1) & 15)) {
        *p++ = '\n';
        *p++ = ' ';
        *p++ = ' ';
      }
      else {
        *p++ = ' ';
      }
    }
  }
  *p++ = '\n';
  *p++ = '\0';
  return cb;
}

int mul64(uint64_t a, uint64_t b, uint64_t *rhi, uint64_t *rlo) {
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
    z2 ++;
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

int mul_size_t(size_t a, size_t b, size_t *rhi, size_t *rlo) {
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
    return mul64(a, b, (uint64_t *)rhi, (uint64_t *)rlo);
  }
}

/* Performs an unsigned division of a 128 bit numerator by a 64 bit denominator, returning 
 * a 128 bit number, split into a high and a low 64 bit number.
 * Return value == -1 for division by zero.
 *                  1 if the result requires more than 64 bits
 *                  0 if the result requires less than 64 bits.
 * *rhi is optional, you can check the return value for 1 to see if the result fits into *rlo.
 */
int div128by64(uint64_t numhi, uint64_t numlo, uint64_t den, uint64_t *rhi, uint64_t *rlo) {
  /* numerator shifted - note that the digit ordering, as is for all arrays, is little endian, with
   * the least significant digit first. */
  uint32_t sn[5] = { 0 };
  sn[0] = (uint32_t)(numlo & 0xFFFFFFFF);
  sn[1] = (uint32_t)(numlo >> 32);
  sn[2] = (uint32_t)(numhi & 0xFFFFFFFF);
  sn[3] = (uint32_t)(numhi >> 32);
  sn[4] = 0;
  
  /* denominator shifted */
  uint32_t sd[4] = { 0 };
  sd[0] = (uint32_t)(den & 0xFFFFFFFF);
  sd[1] = (uint32_t)(den >> 32);
  sd[2] = 0;
  sd[3] = 0;
  
  /* subtraction register */
  uint32_t sr[5];
  /* result register */
  uint32_t r[4] = {0};
  
  uint32_t digit;
  uint64_t acc;
  int shift, lead_den_digit, lead_num_digit, n;

  for (lead_den_digit = 3; lead_den_digit >= 0; --lead_den_digit) {
    if (sd[lead_den_digit]) break;
  }

  if (lead_den_digit == -1) {
    /* Division by zero */
    return -1;
  }

  for (lead_num_digit = 3; lead_num_digit >= 0; --lead_num_digit) {
    if (sn[lead_num_digit]) break;
  }

  if (lead_den_digit > lead_num_digit) {
    /* Denominator is larger than numerator, the result is therefore zero. */
    if (rhi) {
      *rhi = 0x0ULL;
    }
    if (rlo) {
      *rlo = 0x0ULL;
    }
    return 0;
  }

  /* Shift numerator and denominator left until the denominator's lead bit is in the
   * most significant bit slot. We do this so that our "guessing" division in the core
   * division algorithm is as accurate as possible because the denominator of *that*
   * division (the lead digit of the overall denominator) is as big as possible. */
  digit = sd[lead_den_digit];
  if (digit & 0xFFFF0000) {
    shift = 0;
  }
  else {
    shift = 16;
    digit <<= 16;
  }
  if (!(digit & 0xFF000000)) {
    shift += 8;
    digit <<= 8;
  }
  if (!(digit & 0xF0000000)) {
    shift += 4;
    digit <<= 4;
  }
  if (!(digit & 0xC0000000)) {
    shift += 2;
    digit <<= 2;
  }
  if (!(digit & 0x80000000)) {
    shift++;
  }
  /* the "& (-shift >> 31)" part of the equation handles the case for shift == 0,
   * during which we'd attempt a right-shift of (32 - 0) == 32, which is undefined
   * over a 32 bit number (and likely to equal a shift of 0). The desired behavior 
   * is for the entire result to be 0, as-if all 0's would have shifted in. */
  sn[lead_num_digit + 1] = (sn[lead_num_digit] >> (32 - shift)) & (-shift >> 31);
  for (n = lead_num_digit; n > 0; --n) {
    sn[n] = (sn[n] << shift) | ((sn[n - 1] >> (32 - shift)) & (-shift >> 31));
  }
  sn[0] = sn[0] << shift;
  for (n = lead_den_digit; n > 0; --n) {
    sd[n] = (sd[n] << shift) | ((sd[n - 1] >> (32 - shift)) & (-shift >> 31));
  }
  sd[0] = sd[0] << shift;

  for (n = lead_num_digit; n >= lead_den_digit; --n) {
    uint64_t div;
    uint64_t mul;
    int k;
    int sr_is_bigger;
    div = (((uint64_t)sn[n + 1]) << 32) | sn[n];
    div /= sd[lead_den_digit];
    mul = 0;
    for (k = 0; k <= lead_den_digit; ++k) {
      uint64_t factor = div * ((uint64_t)sd[k]);
      mul += factor;
      sr[k] = (uint32_t)mul;
      mul >>= 32;
    }
    sr[k] = (uint32_t)mul;

    do {
      sr_is_bigger = 0;
      for (k = lead_den_digit + 1; k >= 0; --k) {
        if (sr[k] < sn[k - lead_den_digit + n]) {
          sr_is_bigger = 0;
          break;
        }
        else if (sr[k] > sn[k - lead_den_digit + n]) {
          sr_is_bigger = 1;
          break;
        }
      }

      if (sr_is_bigger) {
        /* Estimate is off, subtract denominator from subtraction register */
        acc = 0;
        --div;
        for (k = 0; k <= lead_den_digit; ++k) {
          acc += ((uint64_t)sr[k]) - (uint64_t)sd[k];
          sr[k] = (uint32_t)acc;
          acc = (uint64_t)(((int64_t)acc) >> 32);
        }
        sr[k] += (uint32_t)acc;
      }
    } while (sr_is_bigger);

    /* Subtract the subtraction register (containing the current dividend digit
     * estimate times the denominator) from the numerator to leave the remainder
     * for subsequent digits. */
    acc = 0;
    for (k = 0; k <= (lead_den_digit + 1); ++k) {
      acc += ((uint64_t)sn[k - lead_den_digit + n]) - ((uint64_t)sr[k]);
      sn[k - lead_den_digit + n] = (uint32_t)acc;
      acc = (uint64_t)(((int64_t)acc) >> 32);
    }

    r[n - lead_den_digit] = (uint32_t)div;
  }

  if (rhi) {
    /* Again: notice the little-endian convention for digits */
    *rhi = (((uint64_t)r[3]) << 32) | ((uint64_t)r[2]);
  }
  if (rlo) {
    *rlo = (((uint64_t)r[1]) << 32) | ((uint64_t)r[0]);
  }

  /* Return 0 if result fits in 64 bits, 1 if more bits are needed. */
  return (r[3] || r[2]) ? 1 : 0;
}

void rep_init(struct reentrant_packer *rep) {
  rep->buffer_start = NULL;
  rep->buffer_end = NULL;
  rep->buffer_pos = NULL;
  rep->num_bytes_to_start_of_struct = 0;
  rep->num_bytes_to_next_byte_in_struct = 0;
  rep->num_deepest_pass = (size_t)-1;
}

void rep_set_buffer(struct reentrant_packer *rep, void *buffer, size_t num_bytes) {
  rep->buffer_start = buffer;
  rep->buffer_end = (void *)(((uint8_t *)buffer) + num_bytes);
  rep->buffer_pos = buffer;
}

size_t rep_num_bytes_processed(const struct reentrant_packer *rep) {
  return (size_t)(((uint8_t *)rep->buffer_pos) - ((uint8_t *)rep->buffer_start));
}

size_t rep_num_bytes_remaining(const struct reentrant_packer *rep) {
  return (size_t)(((uint8_t *)rep->buffer_end) - ((uint8_t *)rep->buffer_pos));
}

void rep_reentry_checkpoint(struct reentrant_packer *rep) {
  /* Note that this is a no-op if we are already at the checkpoint */
  rep->num_bytes_to_next_byte_in_struct -= rep->num_bytes_to_start_of_struct;

  /* num_deepest_pass might go unsigned-negative by offsetting it like this, this is
   * fine as we don't do relative (<, <=, >=, >) matching, but only exact matching (!=, ==)
   * see rep_is_first_entry() for the logic. */
  rep->num_deepest_pass -= rep->num_bytes_to_start_of_struct;

  rep->num_bytes_to_start_of_struct = 0;
}

int rep_is_first_entry(struct reentrant_packer *rep) {
  /* If the next byte is immediate, this is first entry, see comments
   * in header for how this creates edge cases.. */
  if (rep->num_bytes_to_next_byte_in_struct == rep->num_bytes_to_start_of_struct) {
    /* Next byte in input is new to the processing logic, however it's possible
     * that we've seen this situation before but just didn't process any further
     * bytes (eg. buffer exhaustion right here would not allow us to process any
     * further bytes but will cause us to resume from this point onwards.).
     * To avoid repetition, we manually check if we already processed at this
     * point. */
    if (rep->num_deepest_pass != rep->num_bytes_to_next_byte_in_struct) {
      /* First time we pass this point, return true and mark the point passed.. */
      rep->num_deepest_pass = rep->num_bytes_to_next_byte_in_struct;
      return 1;
    }
  }
  /* Have already been here before. */
  return 0;
}

static int rep_pack_a_byte(struct reentrant_packer *rep, uint8_t val) {
  if (rep->num_bytes_to_next_byte_in_struct == rep->num_bytes_to_start_of_struct) {
    /* Actively process next byte */
    if (rep->buffer_pos != rep->buffer_end) {
      *(uint8_t *)rep->buffer_pos = val;
      rep->buffer_pos = ((uint8_t *)rep->buffer_pos) + 1;
    }
    else {
      /* Saturated buffer, reset to last checkpoint and return to caller for a new buffer */
      rep->num_bytes_to_start_of_struct = 0;
      return 1;
    }

    rep->num_bytes_to_start_of_struct++;
    rep->num_bytes_to_next_byte_in_struct++;
  }
  else {
    /* If we're not actively processing this byte, we should always be skipping, that is,
    * the current position in our sequence of rep_pack() calls should always be less
    * than the position where we are to continue to actively process bytes.. */
    assert(rep->num_bytes_to_start_of_struct < rep->num_bytes_to_next_byte_in_struct);
    rep->num_bytes_to_start_of_struct++;
  }

  return 0 /* wrote or skipped a byte */;
}

int rep_pack(struct reentrant_packer *rep, const char *format, ...) {
  va_list args;
  uint8_t val_8;
  uint16_t val_16;
  uint32_t val_32;
  uint64_t val_64;

  va_start(args, format);

  while (*format) {
    switch (*format++) {
      case 'b':
      case 'B': /* little and big endian bytes are, of course, the same... */
        val_8 = (uint8_t)(va_arg(args, unsigned int));
        if (rep_pack_a_byte(rep, val_8)) {
          /* Depleted buffer, rep_pack_a_byte() reset us to last checkpoint,
           * return with non-zero to notify caller what happened. */
          va_end(args);
          return 1;
        }
        break;
      case 'w': /* little endian 16 bit word */
        val_16 = (uint16_t)va_arg(args, unsigned int);

        if (rep_pack_a_byte(rep, (uint8_t)(val_16 & 0xFF)) ||
            rep_pack_a_byte(rep, (uint8_t)((val_16 >> 8) & 0xFF))) {
          /* Depleted buffer, rep_pack_a_byte() reset us to last checkpoint,
           * return with non-zero to notify caller what happened. */
          va_end(args);
          return 1;
        }
        break;
      case 'W': /* big endian 16 bit word */
        val_16 = (uint16_t)va_arg(args, unsigned int);

        if (rep_pack_a_byte(rep, (uint8_t)((val_16 >> 8) & 0xFF)) ||
            rep_pack_a_byte(rep, (uint8_t)(val_16 & 0xFF))) {
          /* Depleted buffer, rep_pack_a_byte() reset us to last checkpoint,
           * return with non-zero to notify caller what happened. */
          va_end(args);
          return 1;
        }
        break;

      case 'd': /* little endian 32 bit doubleword */
        val_32 = va_arg(args, uint32_t);
        if (rep_pack_a_byte(rep, (uint8_t)(val_32 & 0xFF)) ||
            rep_pack_a_byte(rep, (uint8_t)((val_32 >> 8) & 0xFF)) ||
            rep_pack_a_byte(rep, (uint8_t)((val_32 >> 16) & 0xFF)) ||
            rep_pack_a_byte(rep, (uint8_t)((val_32 >> 24) & 0xFF))) {
          /* Depleted buffer, rep_pack_a_byte() reset us to last checkpoint,
           * return with non-zero to notify caller what happened. */
          va_end(args);
          return 1;
        }
        break;
      case 'D': /* big endian 32 bit doubleword */
        val_32 = va_arg(args, uint32_t);
        if (rep_pack_a_byte(rep, (uint8_t)((val_32 >> 24) & 0xFF)) ||
            rep_pack_a_byte(rep, (uint8_t)((val_32 >> 16) & 0xFF)) ||
            rep_pack_a_byte(rep, (uint8_t)((val_32 >> 8) & 0xFF)) ||
            rep_pack_a_byte(rep, (uint8_t)(val_32 & 0xFF))) {
          /* Depleted buffer, rep_pack_a_byte() reset us to last checkpoint,
           * return with non-zero to notify caller what happened. */
          va_end(args);
          return 1;
        }
        break;
      case 'q': /* little endian 64 bit quadword */
        val_64 = va_arg(args, uint64_t);
        if (rep_pack_a_byte(rep, (uint8_t)(val_64 & 0xFF)) ||
            rep_pack_a_byte(rep, (uint8_t)((val_64 >> 8) & 0xFF)) ||
            rep_pack_a_byte(rep, (uint8_t)((val_64 >> 16) & 0xFF)) ||
            rep_pack_a_byte(rep, (uint8_t)((val_64 >> 24) & 0xFF)) ||
            rep_pack_a_byte(rep, (uint8_t)((val_64 >> 32) & 0xFF)) ||
            rep_pack_a_byte(rep, (uint8_t)((val_64 >> 40) & 0xFF)) ||
            rep_pack_a_byte(rep, (uint8_t)((val_64 >> 48) & 0xFF)) ||
            rep_pack_a_byte(rep, (uint8_t)((val_64 >> 56) & 0xFF))) {
          /* Depleted buffer, rep_pack_a_byte() reset us to last checkpoint,
           * return with non-zero to notify caller what happened. */
          va_end(args);
          return 1;
        }
        break;
      case 'Q': /* big endian 64 bit quadword */
        val_64 = va_arg(args, uint64_t);
        if (rep_pack_a_byte(rep, (uint8_t)((val_64 >> 56) & 0xFF)) ||
            rep_pack_a_byte(rep, (uint8_t)((val_64 >> 48) & 0xFF)) ||
            rep_pack_a_byte(rep, (uint8_t)((val_64 >> 40) & 0xFF)) ||
            rep_pack_a_byte(rep, (uint8_t)((val_64 >> 32) & 0xFF)) ||
            rep_pack_a_byte(rep, (uint8_t)((val_64 >> 24) & 0xFF)) ||
            rep_pack_a_byte(rep, (uint8_t)((val_64 >> 16) & 0xFF)) ||
            rep_pack_a_byte(rep, (uint8_t)((val_64 >> 8) & 0xFF)) ||
            rep_pack_a_byte(rep, (uint8_t)(val_64 & 0xFF))) {
          /* Depleted buffer, rep_pack_a_byte() reset us to last checkpoint,
           * return with non-zero to notify caller what happened. */
          va_end(args);
          return 1;
        }
        break;
      default:
        /* Unknown format specifier */
        assert(0 && "Unknown format specifier");
        return -1;
    }
  }
  va_end(args);
  return 0;
}

static int rep_unpack_a_byte(struct reentrant_packer *rep, uint8_t *val) {
  if (rep->num_bytes_to_next_byte_in_struct == rep->num_bytes_to_start_of_struct) {
    /* Actively process next byte */
    if (rep->buffer_pos != rep->buffer_end) {
      *val = *(uint8_t *)rep->buffer_pos;
      rep->buffer_pos = ((uint8_t *)rep->buffer_pos) + 1;
    }
    else {
      /* Exhausted buffer, reset to last checkpoint and return to caller for a new buffer */
      rep->num_bytes_to_start_of_struct = 0;
      return 1;
    }

    rep->num_bytes_to_start_of_struct++;
    rep->num_bytes_to_next_byte_in_struct++;
  }
  else {
    /* If we're not actively processing this byte, we should always be skipping, that is,
     * the current position in our sequence of rep_pack() calls should always be less
     * than the position where we are to continue to actively process bytes.. */
    assert(rep->num_bytes_to_start_of_struct < rep->num_bytes_to_next_byte_in_struct);
    rep->num_bytes_to_start_of_struct++;
  }

  return 0 /* wrote or skipped a byte */;
}

int rep_read(struct reentrant_packer *rep, void *buffer, size_t bytes_to_read) {
  size_t buffer_pos = 0;
  size_t bytes_to_skip = rep->num_bytes_to_next_byte_in_struct - rep->num_bytes_to_start_of_struct;
  if (bytes_to_skip >= bytes_to_read) {
    rep->num_bytes_to_start_of_struct += bytes_to_read;
    return 0; /* skipped entire buffer (already read on prior entry) */
  }
  buffer_pos = bytes_to_skip;
  rep->num_bytes_to_start_of_struct += bytes_to_skip;
  /* Should now actively be processing the next byte */
  assert(rep->num_bytes_to_next_byte_in_struct == rep->num_bytes_to_start_of_struct);
  size_t bytes_available = rep_num_bytes_remaining(rep);
  size_t buffer_remaining = bytes_to_read - buffer_pos;
  if (buffer_remaining <= bytes_available) {
    if (buffer) {
      memcpy(((uint8_t *)buffer) + buffer_pos, rep->buffer_pos, buffer_remaining);
    }
    rep->num_bytes_to_start_of_struct += buffer_remaining;
    rep->num_bytes_to_next_byte_in_struct += buffer_remaining;
    rep->buffer_pos = ((uint8_t *)rep->buffer_pos) + buffer_remaining;
    return 0; /* Completed read of buffer.. */
  }
  /* Partial read */
  if (buffer) {
    memcpy(((uint8_t *)buffer) + buffer_pos, rep->buffer_pos, bytes_available);
  }
  rep->num_bytes_to_start_of_struct += bytes_available;
  rep->num_bytes_to_next_byte_in_struct += bytes_available;
  rep->buffer_pos = ((uint8_t *)rep->buffer_pos) + bytes_available;
  /* Exhausted buffer */
  rep->num_bytes_to_start_of_struct = 0;
  return 1;
}

int rep_nest(struct reentrant_packer *dst_rep, struct reentrant_packer *src_rep, size_t bytes_to_nest) {
  size_t buffer_pos = 0;
  size_t bytes_to_skip = src_rep->num_bytes_to_next_byte_in_struct - src_rep->num_bytes_to_start_of_struct;
  if (bytes_to_skip >= bytes_to_nest) {
    src_rep->num_bytes_to_start_of_struct += bytes_to_nest;
    return 0; /* skipped entire buffer (already read on prior entry) */
  }
  buffer_pos = bytes_to_skip;
  src_rep->num_bytes_to_start_of_struct += bytes_to_skip;
  /* Should now actively be processing the next byte */
  assert(src_rep->num_bytes_to_next_byte_in_struct == src_rep->num_bytes_to_start_of_struct);
  size_t bytes_available = rep_num_bytes_remaining(src_rep);
  size_t buffer_remaining = bytes_to_nest - buffer_pos;
  if (buffer_remaining <= bytes_available) {
    if (dst_rep) {
      rep_set_buffer(dst_rep, src_rep->buffer_pos, buffer_remaining);
    }
    src_rep->num_bytes_to_start_of_struct += buffer_remaining;
    src_rep->num_bytes_to_next_byte_in_struct += buffer_remaining;
    src_rep->buffer_pos = ((uint8_t *)src_rep->buffer_pos) + buffer_remaining;
    return 0; /* Completed read of buffer.. */
  }
  /* Partial read */
  if (dst_rep) {
    rep_set_buffer(dst_rep, src_rep->buffer_pos, bytes_available);
  }
  src_rep->num_bytes_to_start_of_struct += bytes_available;
  src_rep->num_bytes_to_next_byte_in_struct += bytes_available;
  src_rep->buffer_pos = ((uint8_t *)src_rep->buffer_pos) + bytes_available;
  /* Exhausted buffer */
  src_rep->num_bytes_to_start_of_struct = 0;
  return 1;
}

int rep_unpack_va(struct reentrant_packer *rep, const char *format, va_list args_) {
  va_list args;
  uint8_t parts[8];
  uint8_t *val_ptr_8;
  uint16_t *val_ptr_16;
  uint32_t *val_ptr_32;
  uint64_t *val_ptr_64;
  int r;

  va_copy(args, args_);

  while (*format) {
    switch (*format++) {
    case 'b':
    case 'B': /* little and big endian bytes are, of course, the same... */
      val_ptr_8 = va_arg(args, uint8_t *);
      if (rep_unpack_a_byte(rep, val_ptr_8)) {
        /* Saturated buffer, rep_unpack_a_byte() reset us to last checkpoint,
         * return with non-zero to notify caller what happened. */
        va_end(args);
        return 1;
      }
      break;
    case 'w': /* little endian 16 bit word */
      val_ptr_16 = va_arg(args, uint16_t *);
      parts[0] = (uint8_t)((*val_ptr_16) & 0xFF);
      parts[1] = (uint8_t)(((*val_ptr_16) >> 8) & 0xFF);
      r = rep_unpack_a_byte(rep, parts + 0) ||
          rep_unpack_a_byte(rep, parts + 1);
      *val_ptr_16 = ((uint16_t)parts[0])
                  | (((uint16_t)parts[1]) << 8);
      if (r) {
        /* Saturated buffer, rep_unpack_a_byte() reset us to last checkpoint,
         * return with non-zero to notify caller what happened. */
        va_end(args);
        return 1;
      }
      break;
    case 'W': /* big endian 16 bit word */
      val_ptr_16 = va_arg(args, uint16_t *);
      parts[0] = (uint8_t)(((*val_ptr_16) >> 8) & 0xFF);
      parts[1] = (uint8_t)((*val_ptr_16) & 0xFF);
      r = rep_unpack_a_byte(rep, parts + 0) ||
          rep_unpack_a_byte(rep, parts + 1);
      *val_ptr_16 = (((uint16_t)parts[0]) << 8)
                  | ((uint16_t)parts[1]);
      if (r) {
        /* Saturated buffer, rep_unpack_a_byte() reset us to last checkpoint,
         * return with non-zero to notify caller what happened. */
        va_end(args);
        return 1;
      }
      break;
    case 'd': /* little endian 32 bit doubleword */
      val_ptr_32 = va_arg(args, uint32_t *);
      parts[0] = (uint8_t)((*val_ptr_32) & 0xFF);
      parts[1] = (uint8_t)(((*val_ptr_32) >> 8) & 0xFF);
      parts[2] = (uint8_t)(((*val_ptr_32) >> 16) & 0xFF);
      parts[3] = (uint8_t)(((*val_ptr_32) >> 24) & 0xFF);
      r = rep_unpack_a_byte(rep, parts + 0) ||
          rep_unpack_a_byte(rep, parts + 1) ||
          rep_unpack_a_byte(rep, parts + 2) ||
          rep_unpack_a_byte(rep, parts + 3);
      *val_ptr_32 = ((uint32_t)parts[0])
                  | (((uint32_t)parts[1]) << 8)
                  | (((uint32_t)parts[2]) << 16)
                  | (((uint32_t)parts[3]) << 24);
      if (r) {
        /* Saturated buffer, rep_unpack_a_byte() reset us to last checkpoint,
         * return with non-zero to notify caller what happened. */
        va_end(args);
        return 1;
      }
      break;
    case 'D': /* big endian 32 bit doubleword */
      val_ptr_32 = va_arg(args, uint32_t *);
      parts[0] = (uint8_t)(((*val_ptr_32) >> 24) & 0xFF);
      parts[1] = (uint8_t)(((*val_ptr_32) >> 16) & 0xFF);
      parts[2] = (uint8_t)(((*val_ptr_32) >> 8) & 0xFF);
      parts[3] = (uint8_t)((*val_ptr_32) & 0xFF);
      r = rep_unpack_a_byte(rep, parts + 0) ||
          rep_unpack_a_byte(rep, parts + 1) ||
          rep_unpack_a_byte(rep, parts + 2) ||
          rep_unpack_a_byte(rep, parts + 3);
      *val_ptr_32 = (((uint32_t)parts[0]) << 24)
                  | (((uint32_t)parts[1]) << 16)
                  | (((uint32_t)parts[2]) << 8)
                  | ((uint32_t)parts[3]);
      if (r) {
        /* Saturated buffer, rep_unpack_a_byte() reset us to last checkpoint,
         * return with non-zero to notify caller what happened. */
        va_end(args);
        return 1;
      }
      break;
    case 'q': /* little endian 64 bit quadword */
      val_ptr_64 = va_arg(args, uint64_t *);
      parts[0] = (uint8_t)((*val_ptr_64) & 0xFF);
      parts[1] = (uint8_t)(((*val_ptr_64) >> 8) & 0xFF);
      parts[2] = (uint8_t)(((*val_ptr_64) >> 16) & 0xFF);
      parts[3] = (uint8_t)(((*val_ptr_64) >> 24) & 0xFF);
      parts[4] = (uint8_t)(((*val_ptr_64) >> 32) & 0xFF);
      parts[5] = (uint8_t)(((*val_ptr_64) >> 40) & 0xFF);
      parts[6] = (uint8_t)(((*val_ptr_64) >> 48) & 0xFF);
      parts[7] = (uint8_t)(((*val_ptr_64) >> 56) & 0xFF);
      r = rep_unpack_a_byte(rep, parts + 0) ||
          rep_unpack_a_byte(rep, parts + 1) ||
          rep_unpack_a_byte(rep, parts + 2) ||
          rep_unpack_a_byte(rep, parts + 3) ||
          rep_unpack_a_byte(rep, parts + 4) ||
          rep_unpack_a_byte(rep, parts + 5) ||
          rep_unpack_a_byte(rep, parts + 6) ||
          rep_unpack_a_byte(rep, parts + 7);
      *val_ptr_64 = ((uint64_t)parts[0])
                  | (((uint64_t)parts[1]) << 8)
                  | (((uint64_t)parts[2]) << 16)
                  | (((uint64_t)parts[3]) << 24)
                  | (((uint64_t)parts[4]) << 32)
                  | (((uint64_t)parts[5]) << 40)
                  | (((uint64_t)parts[6]) << 48)
                  | (((uint64_t)parts[7]) << 56);
      if (r) {
        /* Saturated buffer, rep_unpack_a_byte() reset us to last checkpoint,
         * return with non-zero to notify caller what happened. */
        va_end(args);
        return 1;
      }
      break;
    case 'Q': /* big endian 64 bit quadword */
      val_ptr_64 = va_arg(args, uint64_t *);
      parts[0] = (uint8_t)(((*val_ptr_64) >> 56) & 0xFF);
      parts[1] = (uint8_t)(((*val_ptr_64) >> 48) & 0xFF);
      parts[2] = (uint8_t)(((*val_ptr_64) >> 40) & 0xFF);
      parts[3] = (uint8_t)(((*val_ptr_64) >> 32) & 0xFF);
      parts[4] = (uint8_t)(((*val_ptr_64) >> 24) & 0xFF);
      parts[5] = (uint8_t)(((*val_ptr_64) >> 16) & 0xFF);
      parts[6] = (uint8_t)(((*val_ptr_64) >> 8) & 0xFF);
      parts[7] = (uint8_t)((*val_ptr_64) & 0xFF);
      r = rep_unpack_a_byte(rep, parts + 0) ||
          rep_unpack_a_byte(rep, parts + 1) ||
          rep_unpack_a_byte(rep, parts + 2) ||
          rep_unpack_a_byte(rep, parts + 3) ||
          rep_unpack_a_byte(rep, parts + 4) ||
          rep_unpack_a_byte(rep, parts + 5) ||
          rep_unpack_a_byte(rep, parts + 6) ||
          rep_unpack_a_byte(rep, parts + 7);
      *val_ptr_64 = (((uint64_t)parts[7]) << 56)
                  | (((uint64_t)parts[6]) << 48)
                  | (((uint64_t)parts[5]) << 40)
                  | (((uint64_t)parts[4]) << 32)
                  | (((uint64_t)parts[3]) << 24)
                  | (((uint64_t)parts[2]) << 16)
                  | (((uint64_t)parts[1]) << 8)
                  | ((uint64_t)parts[0]);
      if (r) {
        /* Saturated buffer, rep_unpack_a_byte() reset us to last checkpoint,
         * return with non-zero to notify caller what happened. */
        va_end(args);
        return 1;
      }
      break;
    default:
      /* Unknown format specifier */
      assert(0 && "Unknown format specifier");
      return -1;
    }
  }
  va_end(args);
  return 0;
}

int rep_unpack(struct reentrant_packer *rep, const char *format, ...) {
  va_list args;
  int r;

  va_start(args, format);
  r = rep_unpack_va(rep, format, args);
  va_end(args);
  return r;
}


size_t pack_le(size_t buffer_size, char *buffer, const char *format, ...) {
  va_list args;
  uint8_t val_8;
  uint16_t val_16;
  uint32_t val_32;
  uint64_t val_64;
  size_t len = 0;

  va_start(args, format);

  while (*format) {
    switch (*format++) {
      case 'b':
        val_8 = (uint8_t)(va_arg(args, unsigned int));
        len += 1;
        if (buffer_size) { buffer_size--; *buffer++ = (char)val_8; };
        break;
      case 'w':
        val_16 = (uint16_t)va_arg(args, unsigned int);
        len += 2;
        if (buffer_size) { buffer_size--; *buffer++ = (char)(val_16 & 0xFF); };
        if (buffer_size) { buffer_size--; *buffer++ = (char)((val_16 >> 8) & 0xFF); };
        break;
      case 'd':
        val_32 = va_arg(args, uint32_t);
        len += 4;
        if (buffer_size) { buffer_size--; *buffer++ = (char)(val_32 & 0xFF); };
        if (buffer_size) { buffer_size--; *buffer++ = (char)((val_32 >> 8) & 0xFF); };
        if (buffer_size) { buffer_size--; *buffer++ = (char)((val_32 >> 16) & 0xFF); };
        if (buffer_size) { buffer_size--; *buffer++ = (char)((val_32 >> 24) & 0xFF); };
        break;
      case 'q':
        val_64 = va_arg(args, uint64_t);
        len += 8;
        if (buffer_size) { buffer_size--; *buffer++ = (char)(val_64 & 0xFF); };
        if (buffer_size) { buffer_size--; *buffer++ = (char)((val_64 >> 8) & 0xFF); };
        if (buffer_size) { buffer_size--; *buffer++ = (char)((val_64 >> 16) & 0xFF); };
        if (buffer_size) { buffer_size--; *buffer++ = (char)((val_64 >> 24) & 0xFF); };
        if (buffer_size) { buffer_size--; *buffer++ = (char)((val_64 >> 32) & 0xFF); };
        if (buffer_size) { buffer_size--; *buffer++ = (char)((val_64 >> 40) & 0xFF); };
        if (buffer_size) { buffer_size--; *buffer++ = (char)((val_64 >> 48) & 0xFF); };
        if (buffer_size) { buffer_size--; *buffer++ = (char)((val_64 >> 56) & 0xFF); };
        break;
      default:
        /* Unknown format specifier */
        return 0;
    }
  }
  return len;
}

size_t unpack_le(size_t buffer_size, const char *buffer, const char *format, ...) {
  va_list args;
  uint8_t *val_ptr_8;
  uint16_t *val_ptr_16;
  uint32_t *val_ptr_32;
  uint64_t *val_ptr_64;
  size_t len = buffer_size;

  va_start(args, format);

  while (*format) {
    switch (*format++) {
    case 'b':
      val_ptr_8 = va_arg(args, uint8_t *);
      if (!buffer_size) { return 0; } else { buffer_size--; (*val_ptr_8) = ((uint8_t)*buffer++); }
      break;
    case 'w':
      val_ptr_16 = va_arg(args, uint16_t *);
      if (!buffer_size) { return 0; } else { buffer_size--; (*val_ptr_16) = ((uint16_t)((uint8_t)*buffer++)); }
      if (!buffer_size) { return 0; } else { buffer_size--; (*val_ptr_16) |= ((uint16_t)((uint8_t)*buffer++)) << 8; }
      break;
    case 'd':
      val_ptr_32 = va_arg(args, uint32_t *);
      if (!buffer_size) { return 0; } else { buffer_size--; (*val_ptr_32) = ((uint32_t)((uint8_t)*buffer++)); }
      if (!buffer_size) { return 0; } else { buffer_size--; (*val_ptr_32) |= ((uint32_t)((uint8_t)*buffer++)) << 8; }
      if (!buffer_size) { return 0; } else { buffer_size--; (*val_ptr_32) |= ((uint32_t)((uint8_t)*buffer++)) << 16; }
      if (!buffer_size) { return 0; } else { buffer_size--; (*val_ptr_32) |= ((uint32_t)((uint8_t)*buffer++)) << 24; }
      break;
    case 'q':
      val_ptr_64 = va_arg(args, uint64_t *);
      if (!buffer_size) { return 0; } else { buffer_size--; (*val_ptr_64) = ((uint64_t)((uint8_t)*buffer++)); }
      if (!buffer_size) { return 0; } else { buffer_size--; (*val_ptr_64) |= ((uint64_t)((uint8_t)*buffer++)) << 8; }
      if (!buffer_size) { return 0; } else { buffer_size--; (*val_ptr_64) |= ((uint64_t)((uint8_t)*buffer++)) << 16; }
      if (!buffer_size) { return 0; } else { buffer_size--; (*val_ptr_64) |= ((uint64_t)((uint8_t)*buffer++)) << 24; }
      if (!buffer_size) { return 0; } else { buffer_size--; (*val_ptr_64) |= ((uint64_t)((uint8_t)*buffer++)) << 32; }
      if (!buffer_size) { return 0; } else { buffer_size--; (*val_ptr_64) |= ((uint64_t)((uint8_t)*buffer++)) << 40; }
      if (!buffer_size) { return 0; } else { buffer_size--; (*val_ptr_64) |= ((uint64_t)((uint8_t)*buffer++)) << 48; }
      if (!buffer_size) { return 0; } else { buffer_size--; (*val_ptr_64) |= ((uint64_t)((uint8_t)*buffer++)) << 56; }
      break;
    default:
      return 0;
    }
  }
  return len - buffer_size;
}

size_t convert_utf16_to_utf8(size_t buffer_size, char *buffer,  const uint16_t *src, size_t src_size) {
  const uint16_t *src_end = src + src_size;
  if (src_size == ~(size_t)0) {
    /* String is null-terminated */
    src_end = NULL;
  }
  const char *buffer_end = buffer + buffer_size;
  /* dst holds destination *and* progress: it will continue incrementing even if
   * it points beyond buffer + buffer_size; if it is beyond the buffer, it'll simply
   * stop writing. */
  char *dst = buffer;
  const uint16_t *inp;

#define EMIT_BYTE(c) \
  do { \
    if (dst < buffer_end) { *dst++ = (char)(unsigned char)(c); } else { dst++; } \
  } while (0)
#define EMIT_REPLACEMENT_CHAR \
  do { \
    EMIT_BYTE(0xE0 | (0x3F & (0xFFFD >> 12))); \
    EMIT_BYTE(0x80 | (0x3F & (0xFFFD >> 6))); \
    EMIT_BYTE(0x80 | (0x3F & 0xFFFD)); \
  } while (0)

  for (inp = src; (src_end && (inp < src_end)) || ((!src_end) && *inp); ++inp) {
    unsigned int wc = (unsigned int)*inp;
    if (wc < 0x80) {
      /* 0x0000 - 0x007f; encode as single UTF-8 byte */
      EMIT_BYTE(wc);
    }
    else if (wc < 0x800) {
      /* 0x0080 - 0x07ff; encode as two UTF-8 bytes */
      EMIT_BYTE(0xC0 | (wc >> 6));
      EMIT_BYTE(0x80 | (0x3F & wc));
    }
    else {
      if ((wc >= 0xd800) && (wc <= 0xdfff)) {
        /* First of surrogate pair - grab second */
        if (src_end && ((inp + 1) >= src_end)) {
          /* Second beyond end of input, emit replacement character 0xfffd */
          EMIT_REPLACEMENT_CHAR;
        }
        else if (wc >= 0xdc00) {
          /* Invalid first value for surrogate pair, should be in range
           * 0xd800 to 0xdbff - note that we emit the replacement character
           * and don't encode the value as unicode in the range 0xd800 - 0xdfff
           * is invalid */
          EMIT_REPLACEMENT_CHAR;
        }
        else {
          wchar_t swc = inp[1];
          if ((swc < 0xdc00) || (swc > 0xdfff)) {
            /* Second value of pair is not a proper surrogate, emit replacement
             * character 0xfffd for the first value and evaluate the second value
             * independently. */
            EMIT_REPLACEMENT_CHAR;
          }
          else {
            /* Surrogate pair values are in range */
            inp++; /* Skip first part of pair so we'll next skip the second part. */
            wc = 0x10000 | ((wc & 0x3FF) << 10) | (swc & 0x3FF);
            assert(wc <= 0x10FFFF);
            /* 0x010000 - 0x10FFFF - encode as four UTF-8 bytes */
            EMIT_BYTE(0xF0 | (0x3F & (wc >> 18)));
            EMIT_BYTE(0x80 | (0x3F & (wc >> 12)));
            EMIT_BYTE(0x80 | (0x3F & (wc >> 6)));
            EMIT_BYTE(0x80 | (0x3F & wc));
          }
        }
      }
      else /* (wc <= 0xFFFF) */ {
        assert(wc <= 0xFFFF);
        /* 0x800 - 0xFFFF; encode as three UTF-8 bytes */
        EMIT_BYTE(0xE0 | (0x3F & (wc >> 12)));
        EMIT_BYTE(0x80 | (0x3F & (wc >> 6)));
        EMIT_BYTE(0x80 | (0x3F & wc));
      }
    }
  }
  if (!src_end) {
    /* Null-terminated input should yield null-terminated output. */
    EMIT_BYTE(0);
  }
#undef EMIT_BYTE
#undef EMIT_REPLACEMENT_CHAR
  return dst - buffer;
}

int codepoint_as_utf8(char *dst, uint32_t codepoint) {
  uint8_t *dst_utf8 = (uint8_t *)dst;
  if (codepoint <= 0x7F) {
    *dst_utf8++ = (uint8_t)codepoint;
  }
  else if (codepoint <= 0x7FF) {
    *dst_utf8++ = (uint8_t)((codepoint & 0x7C0) >> 6) | 0xC0;
    *dst_utf8++ = (uint8_t)(codepoint & 0x3F) | 0x80;
  }
  else if (codepoint <= 0xFFFF) {
    *dst_utf8++ = (uint8_t)((codepoint & 0xF000) >> 12) | 0xE0;
    *dst_utf8++ = (uint8_t)((codepoint & 0x0FC0) >> 6) | 0x80;
    *dst_utf8++ = (uint8_t)(codepoint & 0x003F) | 0x80;
  }
  else if (codepoint <= 0x10FFFF) {
    *dst_utf8++ = (uint8_t)((codepoint & 0x1C0000) >> 18) | 0xF0;
    *dst_utf8++ = (uint8_t)((codepoint & 0x03F000) >> 12) | 0x80;
    *dst_utf8++ = (uint8_t)((codepoint & 0x000FC0) >> 6) | 0x80;
    *dst_utf8++ = (uint8_t)(codepoint & 0x00003F) | 0x80;
  }
  else {
    // Invalid codepoint
  }
  return (int)(((char *)dst_utf8) - dst);
}

int get_next_utf8_codepoint(const char **pputf8) {
  uint8_t *utf8 = (uint8_t *)*pputf8;
  if ((*utf8) < 0x80) {
    *pputf8 += 1;
    return utf8[0];
  }
  if ((*utf8) < 0xC0) {
    /* 0x80 - 0xBF - in the middle of a sequence, not at its start.. */
    *pputf8 += 1;
    return 0xFFFD /* REPLACEMENT CHAR */;
  }
  if ((*utf8) < 0xE0) {
    if ((utf8[1] & 0xC0) == 0x80) {
      int codepoint = (((int)utf8[0] & 0x1F) << 6) | ((int)(utf8[1]) & 0x3F);
      *pputf8 += 2;
      return codepoint;
    }
    else {
      /* Second byte of sequence is not 10xxxxxx binary */
      *pputf8 += 1;
      return 0xFFFD /* REPLACEMENT CHAR */;
    }
  }
  if ((*utf8) < 0xF0) {
    if (((utf8[1] & 0xC0) == 0x80) &&
        ((utf8[2] & 0xC0) == 0x80)) {
      int codepoint = (((int)utf8[0] & 0x0F) << 12) | (((int)utf8[1] & 0x3F) << 6) | ((int)(utf8[2]) & 0x3F);
      *pputf8 += 3;
      return codepoint;
    }
    else {
      /* Either second or third byte of sequence is not 10xxxxxx binary */
      *pputf8 += 1;
      return 0xFFFD /* REPLACEMENT CHAR */;
    }
  }
  if ((*utf8) < 0xF8) {
    if (((utf8[1] & 0xC0) == 0x80) &&
        ((utf8[2] & 0xC0) == 0x80) &&
        ((utf8[3] & 0xC0) == 0x80)) {
      int codepoint = (((int)utf8[0] & 0x07) << 18) | (((int)utf8[1] & 0x3F) << 12) | (((int)utf8[2] & 0x3F) << 6) | ((int)(utf8[3]) & 0x3F);
      *pputf8 += 4;
      return codepoint;
    }
    else {
      /* Either second or third byte of sequence is not 10xxxxxx binary */
      *pputf8 += 1;
      return 0xFFFD /* REPLACEMENT CHAR */;
    }
  }
  /* Incorrect byte for leading a sequence */
  *pputf8 += 1;
  return 0xFFFD /* REPLACEMENT CHAR */;
}

size_t convert_utf8_to_utf16(size_t buffer_size, uint16_t *buffer, const char *src, size_t src_size) {
  const char *src_end = src + src_size;
  if (src_size == ~(size_t)0) {
    /* String is null-terminated */
    src_end = NULL;
  }
  const uint16_t *buffer_end = buffer + buffer_size;
  /* dst holds destination *and* progress: it will continue incrementing even if
   * it points beyond buffer + buffer_size; if it is beyond the buffer, it'll simply
   * stop writing. */
  uint16_t *dst = buffer;
  const char *inp;

#define EMIT_SHORT(c) \
  do { \
    if (dst < buffer_end) { *dst++ = (uint16_t)(c); } else { dst++; } \
  } while (0)
#define EMIT_REPLACEMENT_CHAR \
  do { \
    EMIT_SHORT(0xFFFD); \
  } while (0)

  for (inp = src; (src_end && (inp < src_end)) || ((!src_end) && *inp); ++inp) {
    if (((uint8_t)(inp[0])) < 0x80) {
      EMIT_SHORT(inp[0]);
    }
    else if (((uint8_t)(inp[0])) < 0xC0) {
      /* 0x80 - 0xBF - in the middle of a sequence, not at its start.. */
      EMIT_REPLACEMENT_CHAR;
    }
    else if (((uint8_t)(inp[0])) < 0xE0) {
      /* 0xC0 - 0xDF - 2-byte encoding */
      if (src_end && ((inp + 1) >= src_end)) {
        /* Second byte beyond input */
        EMIT_REPLACEMENT_CHAR;
      }
      else if (((uint16_t)(inp[1] & 0xC0)) == 0x80) {
        uint16_t codepoint = (((uint16_t)(inp[0] & 0x1F)) << 6) | (uint16_t)(inp[1] & 0x3F);
        EMIT_SHORT(codepoint);
        inp++;
      }
      else {
        /* Second byte of sequence is not 10xxxxxx binary */
        EMIT_REPLACEMENT_CHAR;
      }
    }
    else if (((uint8_t)(inp[0])) < 0xF0) {
      /* 0xE0 - 0xEF - 3-byte encoding */
      if (src_end && (((inp + 1) >= src_end) || ((inp + 2) >= src_end))) {
        /* Either 3rd byte or both 2nd and 3rd byte of encoding are beyond the input. */
        EMIT_REPLACEMENT_CHAR;
      }
      else if ((((uint16_t)(inp[1] & 0xC0)) == 0x80) &&
               (((uint16_t)(inp[2] & 0xC0)) == 0x80)) {
        uint32_t codepoint = ((((uint32_t)inp[0]) & 0x0F) << 12) | ((((uint32_t)inp[1]) & 0x3F) << 6) | (((uint32_t)inp[2]) & 0x3F);
        if ((codepoint < 0xD800) || (codepoint >= 0xE000)) {
          /* Regular codepoint */
          EMIT_SHORT(((uint16_t)codepoint));
          inp += 2;
        }
        else {
          /* Problematic codepoint in illegal range 0xD800 - 0xDFFF, emit as illegal character */
          EMIT_REPLACEMENT_CHAR;
        }
      }
      else {
        /* Second or third byte in sequence is not 10xxxxxx binary */
        EMIT_REPLACEMENT_CHAR;
      }
    }
    else if (((uint8_t)(inp[0])) < 0xF8) {
      /* 0xF0 - 0xF7 - 4 byte encoding */
      if (src_end && (((inp + 1) >= src_end) || ((inp + 2) >= src_end) || ((inp + 3) >= src_end))) {
        /* Either 3rd byte or both 2nd and 3rd byte of encoding are beyond the input. */
        EMIT_REPLACEMENT_CHAR;
      }
      else if ((((uint16_t)(inp[1] & 0xC0)) == 0x80) &&
               (((uint16_t)(inp[2] & 0xC0)) == 0x80) &&
               (((uint16_t)(inp[3] & 0xC0)) == 0x80)) {
        uint32_t codepoint = ((((uint32_t)inp[0]) & 0x07) << 18) | ((((uint32_t)inp[1]) & 0x3F) << 12) | ((((uint32_t)inp[2]) & 0x3F) << 6) | (((uint32_t)inp[3]) & 0x3F);
        if (codepoint >= 0x10000) {
          codepoint -= 0x10000;
          EMIT_SHORT((uint16_t)(0xD800 + (codepoint >> 10)));
          EMIT_SHORT((uint16_t)(0xDC00 + (codepoint & 0x3FF)));
          inp += 3;
        }
        else {
          /* Overlong encoding, do not accept */
          EMIT_REPLACEMENT_CHAR;
        }
      }
      else {
        /* Second, third, or fourth byte in sequence is not 10xxxxxx binary */
        EMIT_REPLACEMENT_CHAR;
      }
    }
  }
  if (!src_end) {
    /* Append null-terminator on output if we used the null-terminator for length determination */
    EMIT_SHORT(0);
  }

#undef EMIT_SHORT
#undef EMIT_REPLACEMENT_CHAR
  return dst - buffer;
}

#ifdef _MSC_VER
#ifdef _TCHAR_DEFINED
char *tstr2acstr(const TCHAR *tsrc) {
#ifdef _UNICODE
  size_t len_needed = convert_utf16_to_utf8(0, NULL, tsrc, ~(size_t)0);
  char *p = (char *)malloc(len_needed);
  convert_utf16_to_utf8(len_needed, p, tsrc, ~(size_t)0);
  return p;
#else
  size_t len_needed = 1 + strlen(tsrc);
  char *p = malloc(len_needed);
  strcpy(p, tsrc);
return p;
#endif
}

TCHAR *cstr2atstr(const char *csrc) {
#ifdef _UNICODE
  size_t len_needed = convert_utf8_to_utf16(0, NULL, csrc, ~(size_t)0);
  TCHAR *p = (TCHAR *)malloc(sizeof(TCHAR) * len_needed);
  convert_utf8_to_utf16(len_needed, p, csrc, ~(size_t)0);
  return p;
#else
  size_t len_needed = 1 + strlen(csrc);
  char *p = malloc(len_needed);
  strcpy(p, csrc);
  return p;
#endif
}

TCHAR *wstr2atstr(const wchar_t *wsrc) {
#ifdef _UNICODE
  return _tcsdup(wsrc);
#else
  size_t len_needed = convert_utf16_to_utf8(0, NULL, tsrc, ~(size_t)0);
  char *p = (char *)malloc(len_needed);
  convert_utf16_to_utf8(len_needed, p, tsrc, ~(size_t)0);
  return p;
#endif
}
#endif
#endif

wchar_t *cstr2awstr(const char *csrc) {
  size_t len_needed = convert_utf8_to_utf16(0, NULL, csrc, ~(size_t)0);
  wchar_t *p = (wchar_t *)malloc(sizeof(wchar_t) * len_needed);
  convert_utf8_to_utf16(len_needed, (uint16_t *)p, csrc, ~(size_t)0);
  return p;
}

char *wstr2acstr(const wchar_t *wsrc) {
  size_t len_needed = convert_utf16_to_utf8(0, NULL, (uint16_t *)wsrc, ~(size_t)0);
  char *p = (char *)malloc(len_needed);
  convert_utf16_to_utf8(len_needed, p, (uint16_t *)wsrc, ~(size_t)0);
  return p;
}

#ifdef _HRESULT_DEFINED
char *hr2cstr(HRESULT hr) {
  hr = HRESULT_CODE(hr);
  LPVOID msgbuf;
  FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
    NULL, (DWORD)hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&msgbuf, 0, NULL);
  char *msg = tstr2acstr(msgbuf);
  LocalFree(msgbuf);
  return msg;
}
#endif


struct dir_enum_handle {
  char *error_msg;
  int want_directories;
  int has_finished;
#ifdef __linux__
  char *real_base_path;
  char *full_path;
  DIR *dir;
  struct dirent *ent;
#elif defined(_WIN32)
  TCHAR *fullpath;
  char *cfullpath;
  WIN32_FIND_DATA wfd;
  HANDLE hfind;
  char *current_file;
#endif
};

const char *dir_enum_get_error(void *dir_enum_handle) {
  struct dir_enum_handle *deh = (struct dir_enum_handle *)dir_enum_handle;
  return deh->error_msg;
}

const char *dir_enum_full_path(void *dir_enum_handle) {
  struct dir_enum_handle *deh = (struct dir_enum_handle *)dir_enum_handle;
#ifdef __linux__
  if (!deh->ent) {
    return NULL;
  }
  if (deh->full_path) {
    free(deh->full_path);
  }
  deh->full_path = masprintf("%s/%s", deh->real_base_path, deh->ent->d_name);
  return deh->full_path;
#elif defined(_WIN32)
  return deh->cfullpath;
#else
#error Implement non-win32/posix version
#endif
}

void *dir_enum_begin(const char *directory_path) {
  return dir_enum_begin2(directory_path, 1);
}

void *dir_enum_begin2(const char *directory_path, int want_directories) {
  struct dir_enum_handle *deh = (struct dir_enum_handle *)malloc(sizeof(struct dir_enum_handle));
  if (!deh) return NULL;
  deh->error_msg = NULL;
  deh->want_directories = !!want_directories;
  deh->has_finished = 0;
#ifdef __linux__
  deh->real_base_path = realpath(directory_path, NULL);
  if (!deh->real_base_path) {
    deh->error_msg = strerror(errno);
    return deh;
  }
  deh->full_path = NULL;
  deh->dir = opendir(directory_path);
  if (!deh->dir) {
    deh->error_msg = strdup(strerror(errno));
    return deh;
  }
#elif defined(_WIN32)
  deh->cfullpath = NULL;
  deh->fullpath = NULL;
  memset(&deh->wfd, 0, sizeof(WIN32_FIND_DATA));
  deh->hfind = INVALID_HANDLE_VALUE;
  deh->current_file = NULL;

  /* Determine the full (absolute) path name to remove any
   * relative path */
  TCHAR *tpath = cstr2atstr(directory_path);
  TCHAR *filepart = NULL;
  DWORD retval = GetFullPathName(tpath, 0, NULL, &filepart);
  if (!retval) {
    LPTSTR lpMsgBuf;
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                  FORMAT_MESSAGE_FROM_SYSTEM |
                  FORMAT_MESSAGE_IGNORE_INSERTS,
                  NULL,
                  GetLastError(),
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                  (LPTSTR)&lpMsgBuf,
                  0, NULL);
    deh->error_msg = tstr2acstr(lpMsgBuf);
    /* Cut trailing newline if we find it in the error message */
    if (strlen(deh->error_msg) && (deh->error_msg[strlen(deh->error_msg) - 1] == '\n')) {
      deh->error_msg[strlen(deh->error_msg) - 1] = '\0';
    }
    LocalFree(lpMsgBuf);
    free(tpath);
    return deh;
  }
  /* Allocate appropriate path size */
  deh->fullpath = (TCHAR *)malloc(sizeof(TCHAR) * retval);
  retval = GetFullPathName(tpath, retval, deh->fullpath, &filepart);
  if (!retval) {
    LPTSTR lpMsgBuf;
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                  FORMAT_MESSAGE_FROM_SYSTEM |
                  FORMAT_MESSAGE_IGNORE_INSERTS,
                  NULL,
                  GetLastError(),
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                  (LPTSTR)&lpMsgBuf,
                  0, NULL);
    deh->error_msg = tstr2acstr(lpMsgBuf);
    /* Cut trailing newline if we find it in the error message */
    if (strlen(deh->error_msg) && (deh->error_msg[strlen(deh->error_msg) - 1] == '\n')) {
      deh->error_msg[strlen(deh->error_msg) - 1] = '\0';
    }
    LocalFree(lpMsgBuf);
    free(tpath);
    return deh;
  }
  free(tpath);

  /* Get the attributes, make sure it is a directory; and also
   * make sure the path does not contain any wildcards (which would
   * make FindFirstFile/FindNextFile to act differently.)*/
  DWORD attrs = GetFileAttributes(deh->fullpath);
  if (attrs == INVALID_FILE_ATTRIBUTES) {
    LPTSTR lpMsgBuf;
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                  FORMAT_MESSAGE_FROM_SYSTEM |
                  FORMAT_MESSAGE_IGNORE_INSERTS,
                  NULL,
                  GetLastError(),
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                  (LPTSTR)&lpMsgBuf,
                  0, NULL);
    deh->error_msg = tstr2acstr(lpMsgBuf);
    /* Cut trailing newline if we find it in the error message */
    if (strlen(deh->error_msg) && (deh->error_msg[strlen(deh->error_msg) - 1] == '\n')) {
      deh->error_msg[strlen(deh->error_msg) - 1] = '\0';
    }
    LocalFree(lpMsgBuf);
    return deh;
  }
  if ((attrs & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY) {
    deh->error_msg = tstr2acstr(L"Not a directory");
    return deh;
  }

  size_t pathlen = _tcslen(deh->fullpath);
  assert(pathlen);
  TCHAR *filesearch;
  if (deh->fullpath[pathlen - 1] != '\\') {
    /* Append backslash so the fullpath will always be consistent later */
    TCHAR *new_fullpath = tmasprintf(_T("%s\\"), deh->fullpath);
    free(deh->fullpath);
    deh->fullpath = new_fullpath;
  }
  deh->cfullpath = tstr2acstr(deh->fullpath);

  filesearch = tmasprintf(_T("%s*"), deh->fullpath);

  deh->hfind = FindFirstFile(filesearch, &deh->wfd);
  if (deh->hfind == INVALID_HANDLE_VALUE) {
    DWORD last_error = GetLastError();
    free(filesearch);
    if (last_error == ERROR_FILE_NOT_FOUND) {
      /* Not so much an error; just no files found */
      deh->has_finished = 1;
      return deh;
    }
    LPTSTR lpMsgBuf;
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                  FORMAT_MESSAGE_FROM_SYSTEM |
                  FORMAT_MESSAGE_IGNORE_INSERTS,
                  NULL,
                  last_error,
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                  (LPTSTR)&lpMsgBuf,
                  0, NULL);
    deh->error_msg = tstr2acstr(lpMsgBuf);
    /* Cut trailing newline if we find it in the error message */
    if (strlen(deh->error_msg) && (deh->error_msg[strlen(deh->error_msg) - 1] == '\n')) {
      deh->error_msg[strlen(deh->error_msg) - 1] = '\0';
    }
    LocalFree(lpMsgBuf);
    return deh;
  }
  free(filesearch);
  do {
    if (deh->want_directories || !(deh->wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
      if (_tcscmp(deh->wfd.cFileName, _T(".")) && _tcscmp(deh->wfd.cFileName, _T(".."))) {
        /* Have something to return.. */
        return deh;
      }
    }
  } while (FindNextFile(deh->hfind, &deh->wfd));
  DWORD last_error = GetLastError();
  if (last_error == ERROR_NO_MORE_FILES) {
    /* Normal finish condition, no files found in directory */
    deh->has_finished = 1;
    return deh;
  }
  LPTSTR lpMsgBuf;
  FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                FORMAT_MESSAGE_FROM_SYSTEM |
                FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL,
                last_error,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPTSTR)&lpMsgBuf,
                0, NULL);
  deh->error_msg = tstr2acstr(lpMsgBuf);
  /* Cut trailing newline if we find it in the error message */
  if (strlen(deh->error_msg) && (deh->error_msg[strlen(deh->error_msg) - 1] == '\n')) {
    deh->error_msg[strlen(deh->error_msg) - 1] = '\0';
  }
  LocalFree(lpMsgBuf);
  return deh;
#else
#error Implement non-win32/posix version
#endif
}

const char *dir_enum_next(void *dir_enum_handle) {
  int is_dir;
  return dir_enum_next2(dir_enum_handle, &is_dir);
}

const char *dir_enum_next2(void *dir_enum_handle, int *is_directory) {
  struct dir_enum_handle *deh = (struct dir_enum_handle *)dir_enum_handle;
  if (deh->has_finished) {
    return NULL;
  }
#ifdef __linux__
  do {
    errno = 0;
    deh->ent = readdir(deh->dir);
    if (!deh->ent) {
      if (errno) deh->error_msg = strdup(strerror(errno));
      deh->has_finished = 1;
      return NULL;
    }
  } while ((!deh->want_directories) && (deh->ent->d_type == DT_DIR));
  return deh->ent->d_name;
#elif defined(_WIN32)
  if (deh->current_file) {
    free(deh->current_file);
  }
  char *filename = tstr2acstr(deh->wfd.cFileName);
  deh->current_file = masprintf("%s%s", deh->cfullpath, filename);
  *is_directory = !!(deh->wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
  free(filename);

  while (FindNextFile(deh->hfind, &deh->wfd)) {
    if (deh->want_directories || !(deh->wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
      if (_tcscmp(deh->wfd.cFileName, _T(".")) && _tcscmp(deh->wfd.cFileName, _T(".."))) {
        /* We'll have something to return on the next invocation */
return deh->current_file;
      }
    }
  }
  DWORD last_error = GetLastError();
  if (last_error == ERROR_NO_MORE_FILES) {
    /* Normal finish condition; nothing to return on next invocation */
    deh->has_finished = 1;
    return deh->current_file;
  }
  /* Error condition, find error message and keep it; we'll return
   * something now and end on the next run */
  LPTSTR lpMsgBuf;
  FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
    FORMAT_MESSAGE_FROM_SYSTEM |
    FORMAT_MESSAGE_IGNORE_INSERTS,
    NULL,
    last_error,
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
    (LPTSTR)&lpMsgBuf,
    0, NULL);
  deh->error_msg = tstr2acstr(lpMsgBuf);
  LocalFree(lpMsgBuf);
  deh->has_finished = 1;
  return deh->current_file;
#else
#error Implement non-win32/posix version
#endif
}

void dir_enum_end(void *dir_enum_handle) {
  struct dir_enum_handle *deh = (struct dir_enum_handle *)dir_enum_handle;
  if (!deh) return;
  if (deh->error_msg) {
    free(deh->error_msg);
  }
#ifdef __linux__
  if (deh->real_base_path) {
    free(deh->real_base_path);
  }
  if (deh->full_path) {
    free(deh->full_path);
  }
#elif defined(_WIN32)
  if (deh->fullpath) {
    free(deh->fullpath);
  }
  if (deh->cfullpath) {
    free(deh->cfullpath);
  }
  if (deh->hfind != INVALID_HANDLE_VALUE) {
    FindClose(deh->hfind);
  }
  if (deh->current_file) {
    free(deh->current_file);
  }
  free(deh);
#else
#error Implement non-win32/posix version
#endif
}

static int find_directory_parents(char ***parents, size_t *num_parents, const char *directory_path) {
#ifdef __linux__
  char *path = realpath(directory_path, NULL);
  if (!path) {
    return errno;
  }

  const char *cp = path;
  int has_leading_slash = 0;
  if (*cp == '/') {
    has_leading_slash = 1;
    do {
      cp++;
    } while (*cp == '/');
  }
  int num_components = 0;
  while (*cp) {
    if (*cp != '/') {
      /* Component */
      num_components++;
      do {
        cp++;
      } while (*cp && (*cp != '/'));
    }
    while (*cp == '/') {
      /* Separator */
      cp++;
    }
  }
  *num_parents = has_leading_slash + num_components;
  *parents = (char **)malloc(sizeof(char *) * (1 + *num_parents));
  if (!*parents) {
    free(path);
    return ENOMEM;
  }
  size_t dir_index;
  for (dir_index = 0; dir_index <= *num_parents; ++dir_index) {
    (*parents)[dir_index] = NULL;
  }
  cp = path;
  while (*cp) {
    if (*cp != '/') {
      do {
        cp++;
      } while (*cp && (*cp != '/'));
      (*parents)[dir_index] = (char *)malloc(1 + cp - path);
      if (!(*parents)[dir_index]) {
        while (dir_index > 0) {
          dir_index--;
          free((*parents)[dir_index]);
        }
        free(*parents);
        *parents = NULL;
        free(path);
        return ENOMEM;
      }
      memcpy((*parents)[dir_index], path, cp - path);
      ((*parents)[dir_index])[cp - path] = '\0';
    }
    while (*cp == '/') {
      cp++;
    }
  }
  free(path);
  return 0;
#elif defined(_WIN32)
  /* Determine the full (absolute) path name to remove any
   * relative path */
  TCHAR *tpath = cstr2atstr(directory_path);
  TCHAR *filepart = NULL;
  DWORD retval = GetFullPathName(tpath, 0, NULL, &filepart);
  if (!retval) {
    LPTSTR lpMsgBuf;
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                  FORMAT_MESSAGE_FROM_SYSTEM |
                  FORMAT_MESSAGE_IGNORE_INSERTS,
                  NULL,
                  GetLastError(),
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                  (LPTSTR)&lpMsgBuf,
                  0, NULL);
    /* XXX: Find way to return error */
    *num_parents = 0;
    *parents = NULL;
    char *error_msg = tstr2acstr(lpMsgBuf);
    LocalFree(lpMsgBuf);
    free(tpath);
    fprintf(stderr, "Failed to get full path for \"%s\", error:%s\n", directory_path, error_msg);
    free(error_msg);
    return 1;
  }
  TCHAR *tfullpath = (TCHAR *)malloc(sizeof(TCHAR) * retval);
  retval = GetFullPathName(tpath, retval, tfullpath, &filepart);
  if (!retval) {
    LPTSTR lpMsgBuf;
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                  FORMAT_MESSAGE_FROM_SYSTEM |
                  FORMAT_MESSAGE_IGNORE_INSERTS,
                  NULL,
                  GetLastError(),
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                  (LPTSTR)&lpMsgBuf,
                  0, NULL);
    /* XXX: Find way to return error */
    *num_parents = 0;
    *parents = NULL;
    char *error_msg = tstr2acstr(lpMsgBuf);
    LocalFree(lpMsgBuf);
    free(tpath);
    free(tfullpath);
    fprintf(stderr, "Failed to get full path for \"%s\", error:%s\n", directory_path, error_msg);
    free(error_msg);
    return 1;
  }
  free(tpath);
  char *fullpath = tstr2acstr(tfullpath);
  free(tfullpath);

  /* Iterate through path discovering all separators, this helps us count the total
   * number of paths to allocate in parents. */
  const char *p = fullpath;
  const char win32_path_length_extender[] = "\\\\?\\";
  if (!strncmp(win32_path_length_extender, p, sizeof(win32_path_length_extender) - 1)) {
    /* Skip forward; the path length extender does not count */
    p += sizeof(win32_path_length_extender) - 1;
  }
  const char *proper_path_start = p;
  size_t num_parents_found = 0;
  const char *last_path_ended_on = NULL;
  while (*p) {
    if ((*p == '/') || (*p == '\\')) {
      num_parents_found++;
      last_path_ended_on = p;
    }
    p++;
  }
  if ((last_path_ended_on + 1) < p) {
    /* Last path is more than one character away, which
     * can only mean there is a path ending in a non-slash or backslash
     * character we haven't counted yet. */
    num_parents_found++;
  }
  *parents = (char **)malloc(sizeof(char *) * (num_parents_found + 1));
  *num_parents = num_parents_found;
  (*parents)[num_parents_found] = NULL;
  p = proper_path_start;
  last_path_ended_on = NULL;
  size_t current_path_index = 0;
  while (*p) {
    if ((*p == '/') || (*p == '\\')) {
      if (current_path_index) {
        /* Copy up to, but excluding, the slash (or backslash) */
        (*parents)[current_path_index] = (char *)malloc(sizeof(char) * (p - fullpath + 1));
        memcpy((*parents)[current_path_index], fullpath, p - fullpath);
        ((*parents)[current_path_index])[p - fullpath] = '\0';
        last_path_ended_on = p;
      }
      else {
        /* First path is inclusive of the backslash (eg. "C:\" not "C:" which has a different meaning) */
        (*parents)[current_path_index] = (char *)malloc(sizeof(char) * (p - fullpath + 2));
        memcpy((*parents)[current_path_index], fullpath, p - fullpath + 1);
        ((*parents)[current_path_index])[p - fullpath + 1] = '\0';
        last_path_ended_on = p;
      }
      current_path_index++;
    }
    p++;
  }
  if ((last_path_ended_on + 1) < p) {
    /* Final directory in path not yet matched (the path did not end in a slash or backslash.)
     * Make sure we add it to the list. */
    (*parents)[current_path_index] = (char *)malloc(sizeof(char) * (p - fullpath + 1));
    memcpy((*parents)[current_path_index], fullpath, p - fullpath);
    ((*parents)[current_path_index])[p - fullpath] = '\0';
  }
  return 0;
#else
#error Implement non-win32/posix version
#endif
}

int dir_is_dir(const char *possible_directory_path) {
#ifdef _WIN32
  TCHAR *tpath = cstr2atstr(possible_directory_path);
  DWORD attrs = GetFileAttributes(tpath);
  free(tpath);
  return ((attrs & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY);
#elif defined(__linux__)
  struct stat s;
  if (lstat(possible_directory_path, &s) == -1) {
    return 0; /* not a dir */
  }
  return (s.st_mode & S_IFMT) == S_IFDIR;
#else
#error Implement non-win32/posix version
#endif
}

int make_dirp(const char *directory_path) {
#ifdef __linux__
  char **paths_from_root = NULL;
  size_t num_paths = 0;
  int r;
  if (0 != (r = find_directory_parents(&paths_from_root, &num_paths, directory_path))) {
    /* Failed to find parent directories */
    return 1;
  }
  size_t n = num_paths;
  if (num_paths) {
    do {
      n--;
      errno = 0;
      r = mkdir(paths_from_root[n], 0777);
      if (!r || (errno == EEXIST)) {
        /* Directory created successfully or already existed. Create child directories */
        for (n = n + 1; n < num_paths; ++n) {
          r = mkdir(paths_from_root[n], 0777);
          if (r) {
            /* This should have succeeded; return error */
            LOGERROR("Failed to create directory for \"%s\", error:%s\n", paths_from_root[n], strerror(errno));
            char **p = paths_from_root;
            while (*p) {
              free(*p++);
            }
            free(paths_from_root);
            return 1;
          }
        }
        /* All done */
        return 0;
      }
      if (errno != ENOENT) {
        LOGERROR("Failed to create directory for \"%s\", error:%s\n", paths_from_root[n], strerror(errno));
        char **p = paths_from_root;
        while (*p) {
          free(*p++);
        }
        free(paths_from_root);
        return 1;
      }
    } while (n);
  }
  LOGERROR("Failed to find starting point for creating directory: \"%s\"\n", directory_path);
  return 1;
#elif defined(_WIN32)
  char **paths_from_root = NULL;
  size_t num_paths = 0;
  int r;
  if (0 != (r = find_directory_parents(&paths_from_root, &num_paths, directory_path))) {
    /* Failed finding parent directories */
    return 1;
  }
  size_t n = num_paths;
  if (num_paths) {
    do {
      n--;

      TCHAR *intermediate_dir = cstr2atstr(paths_from_root[n]);
      BOOL success = CreateDirectory(intermediate_dir, NULL);
      DWORD last_error = success ? ERROR_SUCCESS : GetLastError();
      if ((last_error == ERROR_SUCCESS) || (last_error == ERROR_ALREADY_EXISTS)) {
        /* Directory created succesfully, now create all child directories */
        free(intermediate_dir);
        for (n = n + 1; n < num_paths; ++n) {
          intermediate_dir = cstr2atstr(paths_from_root[n]);
          if (!CreateDirectory(intermediate_dir, NULL)) {
            /* This *should* have succeeded, error out. */
            /* XXX: Figure out how we'd return errors */
            LPTSTR lpMsgBuf;
            FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                          FORMAT_MESSAGE_FROM_SYSTEM |
                          FORMAT_MESSAGE_IGNORE_INSERTS,
                          NULL,
                          GetLastError(),
                          MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                          (LPTSTR)&lpMsgBuf,
                          0, NULL);
            char *error_msg = tstr2acstr(lpMsgBuf);
            LocalFree(lpMsgBuf);
            LOGERROR("Failed to create directory for \"%s\", error:%s\n", paths_from_root[n], error_msg);
            free(error_msg);
            free(intermediate_dir);
            char **p = paths_from_root;
            while (*p) {
              free(*p++);
            }
            free(paths_from_root);
            return 1;
          }
        }
        /* All done if we got here.. */
        return 0;
      }
      
      if (last_error != ERROR_PATH_NOT_FOUND) {
        /* Serious error occurred.. */
        LPTSTR lpMsgBuf;
        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                      FORMAT_MESSAGE_FROM_SYSTEM |
                      FORMAT_MESSAGE_IGNORE_INSERTS,
                      NULL,
                      last_error,
                      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                      (LPTSTR)&lpMsgBuf,
                      0, NULL);
        char *error_msg = tstr2acstr(lpMsgBuf);
        LocalFree(lpMsgBuf);
        LOGERROR("Failed to create directory for \"%s\", error:%s\n", paths_from_root[n], error_msg);
        free(error_msg);
        free(intermediate_dir);
        char **p = paths_from_root;
        while (*p) {
          free(*p++);
        }
        free(paths_from_root);
        return 1;
      }

      /* ERROR_PATH_NOT_FOUND; continue up the chain until we created
       * the intermediate directory that could not be found, or, alternatively,
       * find an error. */
      free(intermediate_dir);
    } while (n);

  }
  /* Went all the way to the top, but could not create any
   * directories (they're all ERROR_PATH_NOT_FOUND or some
   * other error occurred..) */
  LOGERROR("Failed to find starting point for creating directory: \"%s\"\n", directory_path);
  return 1;

#else
#error implement non-win32/posix version
#endif
}

static uint32_t fake_to_day(uint32_t fake_day) {
  uint32_t day = fake_day;

  // Remove fake November 31st
  day -= (day + 372 - 31 * 10 - 30) / 372;

  // Remove fake September 31st
  day -= (day + 371 - 31 * 8 - 30) / 371;

  // Remove fake June 31st
  day -= (day + 370 - 31 * 5 - 30) / 370;

  // Remove fake April 31st
  day -= (day + 369 - 31 * 3 - 30) / 369;

  // Remove fake February 30th and 31st
  day -= 2 * ((day + 368 - 31 - 29) / 368);

  // Every year now has 366 days, remove fake leap days
  uint32_t years = (day + 366 * 400 - 31 - 29) / 366;

  // Remove fake leap days.
  day -= years - years / 400 + years / 100 - years / 4 - 303;

  return day;
}

// The following functions convert to and from a "fake" calendar,
// hence the term "fake days". The fake calendar is special in
// that every year has 372 days and every month has 31 days, 
// correcting for differences in month lengths and leap years.
// This greatly simplifies working with years and months at the
// expense of having impossible days representable. When converting
// back, impossible days are clamped down to a valid day (no special
// validation occurs.)

// Make every year have a leap day, respecting the actual 
// leap days, and skipping the "virtual" fake leap days.
// In addition, the year is shifted as follows:
// - When returned, input day 0 is output as November 2nd,
//   year 399, consequently, input day March 1st year 0 is
//   output as January 1st year 400, such that each year is
//   366 days long.
static uint32_t insert_fake_leap_days(uint32_t day) {
  day += 146097; // Leap day on year 0; so move forward to year 400 (we cycle every 400 years..)
  day -= 31 + 29; // offset to the day after February 28th as day we add skipped leap days behind Feb 28

  uint32_t remainder;
  // Add skipped leap days for every 400 year period.
  remainder = day % 146097;
  day += 303 * (day / 146097);
  // If 399 years + 365 days have passed, subsequent logic will
  // assume a full (time-shifted) year has passed when there is 
  // still Feb 29 - the 400 year leap day - to go. Detect this
  // and complete adding all skipped leap days.
  if (remainder == 146096) {
    day += 303;
  }
  else {
    // Add skipped leap days for every 100 year period.
    day += 76 * (remainder / 36524);
    remainder = remainder % 36524;

    // Add skipped leap days for every 4 year period.
    day += 3 * (remainder / 1461);
    remainder = remainder % 1461;

    // If 3 years and 365 days have passed, subsequent logic will
    // assume 4 full (time-shifted) year have passed when there is
    // still Feb 29 - the leap day - to go. Detect this and 
    // complete adding all skipped leap days.
    if (remainder == 1460) {
      day += 3;
    }
    else {
      // Add skipped leap days for every year
      day += remainder / 365;
    }
  }
  return day;
}

static uint32_t day_to_fake(uint32_t day) {
  day = insert_fake_leap_days(day);

  // January 1st year 0 back at January 1st year 400
  day += 31 + 29;

  // Note that february always has 29 days courtesy of the
  // fake leap days insertion.
  // Expand february to always have 31 days (from 29 days)
  day += 2 * ((day + 366 - 31 - 29) / 366);

  // Expand April to always have 31 days (from 30 days)
  day += ((day + 368 - 31 * 3 - 30) / 368);

  // Expand June to always have 31 days (from 30 days)
  day += ((day + 369 - 31 * 5 - 30) / 369);

  // Expand September to always have 31 days (from 30 days)
  day += ((day + 370 - 31 * 8 - 30) / 370);

  // Expand November to always have 31 days (from 30 days)
  day += ((day + 371 - 31 * 10 - 30) / 371);

  // Every year now has 372 days, every month now has 31 days.
  return day - 372 * 400;
}

static uint32_t days_since_0000_from_yyyymmdd(uint32_t year, uint32_t month, uint32_t day) {
  month -= 1;
  day -= 1;
  // In our "fake" calendar, every year has 372 days, and every month has 31 days;
  // this greatly simplifies our math.. fake_to_day converts this simplified fake calendar
  // to the number of "actual" days since calendar root 0000, correcting for leap years.
  uint32_t days = fake_to_day(year * 372 + month * 31 + day);
  return days;
}

static void yyyymmdd_from_days_since_0000(uint32_t *year, uint32_t *month, uint32_t *day, uint32_t days_since_0000) {
  // Convert days_since_0000 to our "fake" calendar, this reduces year month and day to simple division / remainder logic.
  uint32_t fake_days = day_to_fake(days_since_0000);
  *year = fake_days / 372;
  *month = (fake_days / 31) % 12 + 1;
  *day = (fake_days % 31) + 1;
}

uint64_t seconds_since_0000_from_yyyymmddhhmmss(uint32_t year, uint32_t month, uint32_t day, uint32_t hour, uint32_t minute, uint32_t second) {
  uint32_t current_days = days_since_0000_from_yyyymmdd(year, month, day);
  uint32_t current_seconds = hour * 60 * 60 + minute * 60 + second;
  uint64_t seconds_since_0000 = (uint64_t)(current_days) * 24ULL * 60ULL * 60ULL + (uint64_t)(current_seconds);
  return seconds_since_0000;
}

void yyyymmddhhmmss_from_seconds_since_0000(uint32_t *year, uint32_t *month, uint32_t *day, uint32_t *hour, uint32_t *minute, uint32_t *second, uint64_t seconds_since_0000) {
  uint32_t current_seconds = (uint32_t)(seconds_since_0000 % (24 * 60 * 60));
  uint32_t days_since_0000 = (uint32_t)(seconds_since_0000 / (24 * 60 * 60));
  *second = current_seconds % 60;
  *minute = (current_seconds / 60) % 60;
  *hour = current_seconds / 3600;
  yyyymmdd_from_days_since_0000(year, month, day, days_since_0000);
}

static void set_terminal_echo(int enable) {
#ifdef _WIN32
  HANDLE hstdin = GetStdHandle(STD_INPUT_HANDLE);
  DWORD mode;
  GetConsoleMode(hstdin, &mode);
  if (enable) {
    SetConsoleMode(hstdin, mode | ENABLE_ECHO_INPUT);
  }
  else {
    SetConsoleMode(hstdin, mode & ~ENABLE_ECHO_INPUT);
  }
#else
  struct termios tty;
  tcgetattr(STDIN_FILENO, &tty);
  if (enable) {
    tty.c_lflag |= ECHO;
  }
  else {
    tty.c_lflag &= ~ECHO;
  }
  tcsetattr(STDIN_FILENO, TCSANOW, &tty);
#endif
}

char *read_password_line() {
  size_t pw_len = 0;
  size_t pw_len_allocated = 128;
  set_terminal_echo(0);
  char *pw = (char *)malloc(pw_len_allocated);
  char c;
  do {
    c = fgetc(stdin);
    if ((c == EOF) || (c == '\n')) {
      /* Terminate string and exit */
      c = 0;
    }
    if (pw_len == pw_len_allocated) {
      size_t new_pw_len_allocated = pw_len_allocated + 128;
      void *new_pw = realloc(pw, new_pw_len_allocated);
      if (!new_pw) {
        /* Out of memory */
        set_terminal_echo(1);
        free(pw);
        return NULL;
      }
      pw = (char *)new_pw;
      pw_len_allocated = new_pw_len_allocated;
    }
    pw[pw_len++] = c;
  } while (c);
  set_terminal_echo(1);
  return pw;
}

static char sixbits_to_char(uint8_t six_bits) {
  six_bits &= 0x3F;
  static const char digits[64 + 1 /* null terminator */] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  return digits[six_bits];
}

static uint8_t char_to_sixbits(char c) {
  /* This is the reverse of the table in sixbits_to_char, with 0xFF for invalid digits */
  static const uint8_t ascii_to_value[256] = {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3E, 0xFF, 0xFF, 0xFF, 0x3F,
    0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E,
    0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
    0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
  uint8_t digit = ascii_to_value[(uint8_t)c];
  return digit;
}

void base64_bin_to_b64(size_t num_bin_bytes, const uint8_t *bin_bytes, char *str) {
  size_t n;
  int bit_buf = 0, num_bits_in_buf = 0;
  size_t out_idx = 0;
  for (n = 0; n < num_bin_bytes; ++n) {
    bit_buf <<= 8;
    bit_buf |= (int)bin_bytes[n];
    num_bits_in_buf += 8;
    while (num_bits_in_buf >= 6) {
      num_bits_in_buf -= 6;
      uint8_t base64_digit = 0x3F & (uint8_t)(bit_buf >> num_bits_in_buf);
      char ascii_digit = sixbits_to_char(base64_digit);
      str[out_idx++] = ascii_digit;
    }
  }
  if (num_bits_in_buf) {
    /* Pad with 0 bits, consequently shift left. Note that if num_bits_in_buf
     * was >= 6, it would already have been processed above, so at this point
     * we *know* there are 0 bits necessary. */
    uint8_t base64_digit = 0x3F & (uint8_t)(bit_buf << (6 - num_bits_in_buf));
    char ascii_digit = sixbits_to_char(base64_digit);
    str[out_idx++] = ascii_digit;
  }
  /* pad ASCII with = to next multiple of four */
  while (out_idx & 3) {
    str[out_idx++] = '=';
  }
}

int base64_b64_to_bin(size_t base64_str_length, const char *base64_str, uint8_t *bin_bytes) {
  size_t n;
  int bit_buf = 0;
  int num_bits_in_buf = 0;
  size_t out_idx = 0;
  for (n = 0; n < base64_str_length; ++n) {
    char ascii_digit = base64_str[n];
    uint8_t sixbits = char_to_sixbits(ascii_digit);
    if (sixbits == 0xFF) {
      return 0; /* Invalid digit */
    }
    bit_buf = (bit_buf << 6) | (int)sixbits;
    num_bits_in_buf += 6;
    if (num_bits_in_buf >= 8) {
      num_bits_in_buf -= 8;
      bin_bytes[out_idx++] = (uint8_t)(bit_buf >> num_bits_in_buf);
    }
  }
  /* Any bits remaining (num_bits_in_buf > 0) are padding and should be ignored. */
  return 1;
}

size_t base64_len_as_bin(size_t base64_str_length, const char *base64_str) {
  while (base64_str_length && (base64_str[base64_str_length - 1] == '=')) {
    base64_str_length--;
  }

  /* Any truncation here is due to padding */
  return base64_str_length * 6 / 8;
}

size_t base64_len_as_b64(size_t num_bin_bytes) {
  /* Every 4 digits of base64 contains 3 bytes of 8 bit data (4 * 6 = 3 * 8),
   * add the +2 so any trailing bytes are appropriately rounded up to include padding;
   * this padding includes the '=' digits. */
  return ((num_bin_bytes + 2) / 3) * 4;
}

int get_random_bytes(void *buf, size_t num_bytes) {
#ifdef __linux__
  FILE *fp = fopen("/dev/urandom", "rb");
  if (!fp) {
    return errno;
  }
  if (fp) {
    if (num_bytes != fread(buf, 1, num_bytes, fp)) {
      fclose(fp);
      return errno;
    }
  }
  fclose(fp);
  return 0;
#elif defined(_WIN32)
  HCRYPTPROV prov;
  if (!CryptAcquireContext(&prov, NULL, NULL, PROV_RSA_FULL, 0)) {
    if (NTE_BAD_KEYSET == GetLastError()) {
      if (!CryptAcquireContext(&prov, NULL, NULL, PROV_RSA_FULL, CRYPT_NEWKEYSET)) {
        prov = 0;
        return GetLastError();
      }
      /* ok */
    }
    else {
      return GetLastError();
    }
  }
  if (!CryptGenRandom(prov, (DWORD)num_bytes, (BYTE *)buf)) {
    return GetLastError();
  }

  CryptReleaseContext(prov, 0);

  return ERROR_SUCCESS;
#else
#error Implement non-win32/posix version
#endif
}
