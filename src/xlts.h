/* Copyright 2020-2024 Kinglet B.V.
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

#ifndef XLTS_H
#define XLTS_H

#ifndef STDDEF_H_INCLUDED
#define STDDEF_H_INCLUDED
#include <stddef.h> /* size_t */
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define XLTS_INTERNAL_ERROR 1

typedef enum xlts_chunk_type_enum {
  XLTS_ORIGINAL,
  XLTS_XLAT,
  XLTS_EQUAL
} xlts_chunk_type_t;

struct xlts_chunk {
  xlts_chunk_type_t ct_;

  /* Each chunk can span, at most, 1 line. Multiple
   * lines are always divided up into chunks to ensure that
   * any partial chunk is always on a single line (thus preventing
   * having to scan the bytes to determine the partial chunk's line
   * number.)
   * The newline (\n) is deemed to be fully on the line at which 
   * it appears.
   * These positions are relative to the original stream. */
  int line_;
  int col_;
  size_t offset_;

  /* Pointer to a filename, the xlts module considers this a static
   * value, not an allocated value it somehow owns. */
  const char *filename_;

  size_t num_original_bytes_;
  size_t num_translated_bytes_;
};

struct xlts_clamp {
  char ori_term_;
  size_t ori_num_;
  char xlt_term_;
  size_t xlt_num_;
  struct xlts_chunk chunk_tail_;
  size_t chunk_num_;
};

struct xlts {
  size_t num_original_;
  size_t num_original_allocated_;
  char *original_;

  size_t num_translated_;
  size_t num_translated_allocated_;
  char *translated_;

  size_t num_chunks_;
  size_t num_chunks_allocated_;
  struct xlts_chunk *chunks_;
};


void xlts_init(struct xlts *x);
void xlts_cleanup(struct xlts *x);
void xlts_reset(struct xlts *x);

int xlts_append_xlat(struct xlts *x, size_t num_bytes, const char *bytes);
int xlts_append_equal(struct xlts *x, const char *filename, int line, int col, size_t offset, size_t num_bytes, const char *bytes);
int xlts_append_original(struct xlts *x, const char *filename, int line, int col, size_t offset, size_t num_bytes, const char *bytes);

int xlts_append(struct xlts *dst, const struct xlts *src);
int xlts_append_as_original(struct xlts *dst, const struct xlts *src);
int xlts_append_mid_translated(struct xlts *dst, const struct xlts *src, size_t start_byte, size_t end_byte);
int xlts_append_left_translated(struct xlts *dst, const struct xlts *src, size_t num_translated_bytes);
void xlts_strip_left_translated(struct xlts *x, size_t num_translated_bytes);

/* xlts_clamp allows you to, temporarily, shorten the size of the string contained in an xlts, without
 * this causing re-allocations. It does this by inserting null terminators and resizing the original
 * and translated strings at locations corresponding to a position in the translated string.
 * After processing has been done, the original string can be recovered using xlts_clamp_remove().
 * The xlts 'x' passed in should be treated as read-only until xlts_clamp_remove() is called, or the
 * call to xlts_clamp_remove() will produce unpredictable results. */
int xlts_clamp_left_translated(struct xlts *x, size_t num_translated_bytes, struct xlts_clamp *clamp);
void xlts_clamp_remove(struct xlts *x, struct xlts_clamp *clamp);

void xlts_swap(struct xlts *a, struct xlts *b);

/* Returns the line number at which the string starts, or 0 if this cannot be located (eg. string is
 * empty or has no XLTS_EQUAL chunks */
int xlts_line(struct xlts *x);

/* Returns the line, column and offset after which the string ends. If the last character of the
 * string is a newline, the line and character position returned will be /after/ the newline,
 * on the next line. */
int xlts_end_location(struct xlts *x, int *endline, int *endcolumn, size_t *endoffset, const char **filename);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* XLTS_H */
