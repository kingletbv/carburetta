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

#ifndef SITUS_H
#define SITUS_H

#ifndef STDDEF_H_INCLUDED
#define STDDEF_H_INCLUDED
#include <stddef.h>
#endif

struct situs_position {
  const char *filename_;
  size_t offset_;
  int line_, col_;
};

struct situs_span {
  /* Filename of source in which the span appears */
  const char *filename_;

  /* Start and end byte offsets in the original source in which the span appears */
  size_t start_, end_;

  /* Start line and column, both 1 based, of the span of text */
  int start_line_, start_col_;

  /* End line and column, both 1 based, of the span of text */
  int end_line_, end_col_;

  /* If non-zero, this is a substitution, num_bytes_ indicates the size of text the
   * original span of text has been substituted by. If false, num_bytes_ is always
   * equal to end_ - start_. */
  int is_substitution_ : 1;

  /* For a substitution, the range being substituted may consist of multiple spans
   * that are not adjacent. When that is the case, the first has a non-zero num_bytes_,
   * and the successive spans have num_bytes_ to 0 with is_aux_ non-zero. */
  int is_aux_ : 1;

  /* Number of bytes of text that this situs_span describes. */
  size_t num_bytes_;
};

struct situs_dynamic {
  size_t num_spans_allocated_;
  struct situs_span *spans_;
};

struct situs {
  size_t num_spans_;
  union {
    struct situs_dynamic many_; /* dynamic array if num_spans_ > 1 for mutliple situs spans */
    struct situs_span one_;     /* static situs_span if num_spans_ == 1 (which is the likely case for normal sourcecode) */
  } u_;
};

void situs_init(struct situs *s);
void situs_cleanup(struct situs *s);

int situs_move_range(struct situs *to, struct situs *from, size_t byte_length, const char *text);
int situs_clone(struct situs *dst, const struct situs *src);
int situs_concat(struct situs *dsthead, const struct situs *tail);
int situs_skip(struct situs *from, size_t byte_length, const char *text);

size_t situs_len(const struct situs *s);

int situs_line(const struct situs *s);
int situs_col(const struct situs *s);
const char *situs_filename(const struct situs *s);

void situs_convert_to_substitution(struct situs *s, size_t num_bytes);

/* Initializes situs s to tbe at the point after src_after; the span contains no bytes,
 * (so the span's start_ and end_ are identical) but it may represent sourcecode text
 * (and hence num_bytes is non-zero). Useful for edge-case situations where code is 
 * inserted by the compiler.
 * Note that the resulting span is always, by definition, a substitution. */
void situs_init_from_after(struct situs *s, const struct situs *src_after, size_t num_bytes);

/* Ideally used like C++'s std::swap() in that we can move things without incurring
 * memory allocations and still ensuring clean destruction. */
void situs_swap(struct situs *a, struct situs *b);

#endif /* SITUS_H */
