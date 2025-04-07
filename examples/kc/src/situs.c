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

#ifndef STDINT_H_INCLUDED
#define STDINT_H_INCLUDED
#include <stdint.h>
#endif

#ifndef STRING_H_INCLUDED
#define STRING_H_INCLUDED
#include <string.h>
#endif

#ifndef ASSERT_H_INCLUDED
#define ASSERT_H_INCLUDED
#include <assert.h>
#endif

#ifndef SITUS_H_INCLUDED
#define SITUS_H_INCLUDED
#include "situs.h"
#endif

void situs_init(struct situs *s) {
  s->num_spans_ = 0;
}

void situs_cleanup(struct situs *s) {
  if (s->num_spans_ > 1) {
    free(s->u_.many_.spans_);
  }
}

/* note: num_cols will contain the number of columns, it resets to _zero_ after a newline, 
 *       and unlike most column representations, is therefore zero-based */
static void situs_find_offset(const char *bytes, size_t byte_size, int *num_lines, int *num_cols) {
  *num_lines = 0;
  *num_cols = 0;
  size_t at = 0;
  while (at < byte_size) {
    uint8_t byte = (uint8_t)(bytes[at]);
    if (byte <= 0x7F) {
      if (byte != '\n') {
        (*num_cols)++;
      }
      else {
        (*num_cols) = 0;
        (*num_lines)++;
      }
      at++;
    }
    else if (byte <= 0xDF) {
      (*num_cols)++;
      at += 2;
    }
    else if (byte <= 0xEF) {
      (*num_cols)++;
      at += 3;
    }
    else if (byte <= 0xF7) {
      (*num_cols)++;
      at += 4;
    }
    else {
      (*num_cols)++;
      at++; /* invalid encoding */
    }
  }
}

/* Chops at most the first byte_length bytes from the from situs and puts them into the chop situs.
 * The length of bytes chopped is returned in chop_length; this can be less than byte_length if
 * from consists of multiple spans. The code seeks to copy a single span, unless that span is a
 * substitution consisting of multiple auxiliary spans, in which case all spans of the substitution
 * will be copied. The goal is to make progress, not to complete all in one shot.
 * If it tried to chop but there was no memory, -1 is returned. 0 is returned otherwise.
 */
static size_t situs_chop(struct situs *chop, struct situs *from, size_t byte_length, const char *text, size_t *chop_length) {
  /* Returns non-zero if chop was performed.. */
  struct situs_span *sf = from->num_spans_ > 1 ? from->u_.many_.spans_ : &from->u_.one_;
  struct situs_span *sflast = sf;
  if (chop->num_spans_ > 1) free(chop->u_.many_.spans_);
  chop->num_spans_ = 0;
  if (from->num_spans_ == 0) {
    *chop_length = 0;
    return 0;
  }

  if (sf->is_substitution_) {
    while ((from->num_spans_ > (size_t)((sflast + 1) - sf)) && (sflast + 1)->is_aux_) {
      sflast++;
    }
  }

  if (byte_length >= sf->num_bytes_) {
    byte_length = sf->num_bytes_;
    size_t num_spans = (sflast + 1) - sf;
    if (num_spans == 1) {
      chop->num_spans_ = 1;
      chop->u_.one_ = *sf;
    }    
    else {
      chop->num_spans_ = num_spans;
      chop->u_.many_.num_spans_allocated_ = num_spans;
      chop->u_.many_.spans_ = malloc(sizeof(struct situs_span) * num_spans);
      if (!chop->u_.many_.spans_) {
        /* Caller can check chop->num_spans_, if non-zero and yet 0 is returned, it's a fail */
        *chop_length = 0;
        return -1;
      }
      memmove(chop->u_.many_.spans_, sf, sizeof(struct situs_span) * num_spans);
    }
    if ((from->num_spans_ - num_spans) >= 2) {
      memmove(sf, sflast + 1, sizeof(struct situs_span) * (from->num_spans_ - num_spans));
      from->num_spans_ -= num_spans;
    }
    else if ((from->num_spans_ - num_spans) == 1) {
      struct situs_span *p = from->u_.many_.spans_;
      from->u_.one_ = p[num_spans];
      free(p);
      from->num_spans_ = 1;
    }
    else {
      from->num_spans_ = 0;
    }
    *chop_length = byte_length;
    return 0;
  }
  if (sf->is_substitution_) {
    sf->num_bytes_ -= byte_length;
    size_t num_spans = (sflast + 1) - sf;
    if (num_spans == 1) {
      chop->num_spans_ = 1;
      chop->u_.one_ = *sf;
      chop->u_.one_.num_bytes_ = byte_length;
    }
    else {
      chop->num_spans_ = num_spans;
      chop->u_.many_.num_spans_allocated_ = num_spans;
      chop->u_.many_.spans_ = malloc(sizeof(struct situs_span) * num_spans);
      if (!chop->u_.many_.spans_) {
        /* Caller can check chop->num_spans_, if non-zero and yet 0 is returned, it's a fail */
        return 0;
      }
      memmove(chop->u_.many_.spans_, sf, sizeof(struct situs_span) * num_spans);
      chop->u_.many_.spans_->num_bytes_ = byte_length;
    }
    *chop_length = byte_length;
    return 0;
  }
  /* from-span is larger than byte length matched, so break it apart */
  int num_lines = 0, num_cols = 0;
  situs_find_offset(text, byte_length, &num_lines, &num_cols);
  chop->num_spans_ = 1;
  chop->u_.one_.filename_ = sf->filename_;
  chop->u_.one_.is_substitution_ = 0;
  chop->u_.one_.num_bytes_ = byte_length;
  chop->u_.one_.start_ = sf->start_;
  chop->u_.one_.start_line_ = sf->start_line_;
  chop->u_.one_.start_col_ = sf->start_col_;

  sf->start_ += byte_length;
  sf->num_bytes_ -= byte_length;
  sf->start_line_ += num_lines;
  sf->start_col_ = num_lines ? 1 + num_cols : (sf->start_col_ + num_cols);

  chop->u_.one_.end_ = sf->start_;
  chop->u_.one_.end_line_ = sf->start_line_;
  chop->u_.one_.end_col_ = sf->start_col_;

  *chop_length = byte_length;
  return 0;
}

/* Note that, unlike situs_chop(), here the requirement is that all of chop be appended 
 * to to. Otherwise situs_concat() will not work. */
static int situs_glue(struct situs *to, struct situs *chop) {
  if (!chop->num_spans_) return 0;
  if (!to->num_spans_) {
    *to = *chop;
    chop->num_spans_ = 0;
    return 0;
  }
  struct situs_span *tohead, *tolast;
  struct situs_span *tofirst = to->num_spans_ > 1 ? to->u_.many_.spans_ : &to->u_.one_;
  tolast = (to->num_spans_ > 1) ? to->u_.many_.spans_ + to->num_spans_ - 1 : &to->u_.one_;
  tohead = tolast;
  while (tohead->is_aux_ && tohead > tofirst) {
    --tohead;
  }
  struct situs_span *choplast = (chop->num_spans_ > 1) ? chop->u_.many_.spans_ + chop->num_spans_ - 1 : &chop->u_.one_;
  struct situs_span *chopfirst = chop->num_spans_ > 1 ? chop->u_.many_.spans_ : &chop->u_.one_;

  /* If they're identical substitutions, expand the num_bytes_ of the substitution
   * If they're adjacent ranges, expand the first range to end on the end of the second.
   * Otherwise append the second to the first.
   */
  if (tohead->is_substitution_ && 
      ((choplast - chopfirst) == (tolast - tohead)) &&
      !memcmp(tohead, chopfirst, (1 + choplast - chopfirst) * sizeof(*chopfirst))) { 
    /* Identical substitutions. */
    tohead->num_bytes_ += chopfirst->num_bytes_;
    if (chop->num_spans_ > 1) {
      free(chop->u_.many_.spans_);
    }
    chop->num_spans_ = 0;
    return 0;
  }
  if ((tohead->filename_ == chopfirst->filename_) &&
      (!tohead->is_substitution_ && !chopfirst->is_substitution_) &&
      (tolast->end_ == chopfirst->start_)) {
    /* Span is adjacent to the last span, extend last span. */
    tolast->end_ = chopfirst->end_;
    tolast->end_line_ = chopfirst->end_line_;
    tolast->end_col_ = chopfirst->end_col_;
    tohead->num_bytes_ += chopfirst->num_bytes_;

    /* Now insert remaining chopfirst+1 .. choplast auxiliaries if any. */
    size_t num_extra_spans = choplast - chopfirst;
    if (num_extra_spans) {
      size_t new_num_spans = to->num_spans_ + num_extra_spans;
      if (to->num_spans_ == 1) {
        struct situs_span *sp = (struct situs_span *)malloc(new_num_spans * sizeof(*sp));
        if (!sp) return -1;
        memcpy(sp, &to->u_.one_, sizeof(*sp));
        memmove(sp + 1, chopfirst + 1, num_extra_spans * sizeof(*sp));
        to->num_spans_ = new_num_spans;
        to->u_.many_.num_spans_allocated_ = new_num_spans;
        to->u_.many_.spans_ = sp;
      }
      else /* (to->num_spans_ >= 2) */ {
        if (new_num_spans > to->u_.many_.num_spans_allocated_) {
          size_t new_num_alloc = to->u_.many_.num_spans_allocated_ * 2 + 1;
          if (new_num_alloc < new_num_spans) new_num_alloc = new_num_spans;
          struct situs_span *sp = (struct situs_span *)realloc(to->u_.many_.spans_, new_num_alloc * sizeof(*sp));
          if (!sp) return -1;
          to->u_.many_.num_spans_allocated_ = new_num_alloc;
          to->u_.many_.spans_ = sp;
        }

        memcpy(to->u_.many_.spans_ + to->num_spans_, chopfirst + 1, num_extra_spans * sizeof(struct situs_span));
        to->num_spans_ = new_num_spans;
      }
    }
    return 0;
  }
  /* If we get here, all of the chop needs to be appended to the to situs */
  size_t num_extra_spans = (choplast + 1) - chopfirst;
  size_t new_num_spans = to->num_spans_ + num_extra_spans;
  if (to->num_spans_ == 1) {
    struct situs_span *sp = (struct situs_span *)malloc(new_num_spans * sizeof(*sp));
    if (!sp) return -1;
    memcpy(sp, &to->u_.one_, sizeof(*sp));
    memmove(sp + 1, chopfirst, num_extra_spans * sizeof(*sp));
    to->num_spans_ = new_num_spans;
    to->u_.many_.num_spans_allocated_ = new_num_spans;
    to->u_.many_.spans_ = sp;
  }
  else /* (to->num_spans_ >= 2) */ {
    if (new_num_spans > to->u_.many_.num_spans_allocated_) {
      size_t new_num_alloc = to->u_.many_.num_spans_allocated_ * 2 + 1;
      if (new_num_alloc < new_num_spans) new_num_alloc = new_num_spans;
      struct situs_span *sp = (struct situs_span *)realloc(to->u_.many_.spans_, new_num_alloc * sizeof(*sp));
      if (!sp) return -1;
      to->u_.many_.num_spans_allocated_ = new_num_alloc;
      to->u_.many_.spans_ = sp;
    }
    memcpy(to->u_.many_.spans_ + to->num_spans_, chopfirst, num_extra_spans * sizeof(struct situs_span));
    to->num_spans_ = new_num_spans;
  }
  return 0;
}

static struct situs_span *situs_first_span(struct situs *sit) {
  return sit->num_spans_ > 1 ? sit->u_.many_.spans_ : &sit->u_.one_;
}

static const struct situs_span *situs_first_span_const(const struct situs *sit) {
  return sit->num_spans_ > 1 ? sit->u_.many_.spans_ : &sit->u_.one_;
}

int situs_move_range(struct situs *to, struct situs *from, size_t byte_length, const char *text) {
  struct situs chop;
  situs_init(&chop);
  while (byte_length) {
    size_t num_bytes_chopped = 0;
    if (situs_chop(&chop, from, byte_length, text, &num_bytes_chopped)) {
      situs_cleanup(&chop);
      return -1;
    }
    if (!chop.num_spans_) {
      /* Could not complete the chop - not enough bytes available */
      return 1;
    }
    if (situs_glue(to, &chop)) {
      situs_cleanup(&chop);
      return -1;
    }
    byte_length -= num_bytes_chopped;
    text += num_bytes_chopped;
  }
  return 0;
}

int situs_skip(struct situs *from, size_t byte_length, const char *text) {
  struct situs chop;
  situs_init(&chop);
  while (byte_length) {
    size_t num_bytes_chopped = 0;
    if (situs_chop(&chop, from, byte_length, text, &num_bytes_chopped)) {
      situs_cleanup(&chop);
      return -1;
    }
    byte_length -= num_bytes_chopped;
    text += num_bytes_chopped;
    situs_cleanup(&chop);
    situs_init(&chop);
  }
  situs_cleanup(&chop);
  return 0;
}

int situs_clone(struct situs *dst, const struct situs *src) {
  if (src->num_spans_ <= 1) {
    if (dst->num_spans_ > 1) free(dst->u_.many_.spans_);
    dst->num_spans_ = src->num_spans_;
    dst->u_.one_ = src->u_.one_;
  }
  else {
    if (dst->num_spans_ > 1) free(dst->u_.many_.spans_);
    dst->num_spans_ = src->num_spans_;
    dst->u_.many_.num_spans_allocated_ = src->u_.many_.num_spans_allocated_;
    dst->u_.many_.spans_ = (struct situs_span *)malloc(src->u_.many_.num_spans_allocated_ * sizeof(*src->u_.many_.spans_));
    if (!dst->u_.many_.spans_) {
      return -1;
    }
    memcpy(dst->u_.many_.spans_, src->u_.many_.spans_, src->num_spans_ * sizeof(*src->u_.many_.spans_));
  }
  return 0;
}

int situs_concat(struct situs *dsthead, const struct situs *tail) {
  struct situs clone;
  situs_init(&clone);
  if (situs_clone(&clone, tail)) {
    goto fail;
  }
  if (situs_glue(dsthead, &clone)) {
    goto fail;
  }

  situs_cleanup(&clone);
  return 0;
fail:
  situs_cleanup(&clone);
  return -1;
}

size_t situs_len(const struct situs *s) {
  size_t n;
  size_t len = 0;
  const struct situs_span *spans = (s->num_spans_ > 1) ? s->u_.many_.spans_ : &s->u_.one_;
  for (n = 0; n < s->num_spans_; ++n) {
    const struct situs_span *span = spans + n;
    len += span->num_bytes_;
  }
  return len;
}

int situs_line(const struct situs *s) {
  if (!s->num_spans_) return 0;
  return situs_first_span_const(s)->start_line_;
}

int situs_col(const struct situs *s) {
  if (!s->num_spans_) return 0;
  return situs_first_span_const(s)->start_col_;
}

const char *situs_filename(const struct situs *s) {
  if (!s->num_spans_) return "";
  return situs_first_span_const(s)->filename_;
}

void situs_convert_to_substitution(struct situs *s, size_t num_bytes) {
  size_t n;
  struct situs_span *span = situs_first_span(s);
  for (n = 1; n < s->num_spans_; ++n) {
    span[n].is_aux_ = 1;
    span[n].num_bytes_ = 0;
  }
  span->is_substitution_ = 1;
  span->is_aux_ = 0;
  span->num_bytes_ = num_bytes;
}

void situs_init_from_after(struct situs *s, const struct situs *src_after, size_t num_bytes) {
  situs_init(s);
  const struct situs_span *spans = (src_after->num_spans_ > 1) ? src_after->u_.many_.spans_ : &src_after->u_.one_;
  const struct situs_span *last_span = spans + src_after->num_spans_ - 1;
  struct situs_span *span = &s->u_.one_;
  span->filename_ = last_span->filename_;
  span->start_ = last_span->end_;
  span->start_line_ = last_span->end_line_;
  span->start_col_ = last_span->end_col_;
  span->end_ = span->start_;
  span->end_line_ = span->start_line_;
  span->end_col_ = span->start_col_;
  span->is_substitution_ = 1;
  span->is_aux_ = 0;
  span->num_bytes_ = num_bytes;
}

void situs_swap(struct situs *a, struct situs *b) {
  struct situs tmp = *a;
  *a = *b;
  *b = tmp;
}
