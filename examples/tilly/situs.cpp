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

#ifndef NEW_INCLUDED
#define NEW_INCLUDED
#include <new>
#endif

#ifndef ALGORITHM_INCLUDED
#define ALGORITHM_INCLUDED
#include <algorithm>
#endif

#ifndef SITUS_H_INCLUDED
#define SITUS_H_INCLUDED
#include "situs.h"
#endif

Situs &Situs::operator=(const Situs &other) {
  clone(&other);
  return *this;
}

Situs &Situs::operator=(Situs &&other) noexcept {
  if (num_spans_ > 1) {
    delete[] u_.many_.spans_;
  }
  num_spans_ = other.num_spans_;
  u_ = other.u_;
  other.num_spans_ = 0;
  return *this;
}

void Situs::reset() {
  if (num_spans_ > 1) {
    delete[] u_.many_.spans_;
  }
  num_spans_ = 0;
}

int Situs::move_range(Situs *from, size_t byte_length, const char *text) {
  Situs chop;

  while (byte_length) {
    size_t num_bytes_chopped = 0;
    chop.chop(from, byte_length, text, &num_bytes_chopped);
    if (!chop.num_spans_) {
      /* Could not complete the chop - not enough bytes available */
      return 1;
    }
    glue(&chop);
    byte_length -= num_bytes_chopped;
    text += num_bytes_chopped;
  }

  return 0;
}

void Situs::clone(const Situs *src) {
  if (src->num_spans_ <= 1) {
    if (num_spans_ > 1) delete [] u_.many_.spans_;
    num_spans_ = src->num_spans_;
    u_.one_ = src->u_.one_;
  }
  else {
    if (num_spans_ > 1) delete[] u_.many_.spans_;
    num_spans_ = src->num_spans_;
    u_.many_.num_spans_allocated_ = src->u_.many_.num_spans_allocated_;
    u_.many_.spans_ = new situs_span[src->u_.many_.num_spans_allocated_];
    std::copy(src->u_.many_.spans_, src->u_.many_.spans_ + src->num_spans_, u_.many_.spans_);
  }
}

void Situs::clone_as_single_span_all_gaps_filled(const Situs *src) {
  if (num_spans_ > 1) delete[] u_.many_.spans_;
  if (!src->num_spans_) {
    num_spans_ = 0;
    return;
  }

  const situs_span *first = src->first_span();
  const situs_span *last = src->last_span();

  num_spans_ = 1;
  situs_span *span = &u_.one_;

  span->filename_ = first->filename_;
  span->start_ = first->start_;
  span->end_ = last->end_;
  span->start_line_ = first->start_line_;
  span->end_line_ = last->end_line_;
  span->start_col_ = first->start_col_;
  span->end_col_ = last->end_col_;
  span->is_substitution_ = 0;
  span->is_aux_ = 0;
  span->num_bytes_ = last->end_ - first->start_;
}

void Situs::concat(const Situs *tail) {
  Situs clone;
  clone.clone(tail);
  glue(&clone);
}

void Situs::skip(size_t byte_length, const char *text) {
  Situs chop;

  while (byte_length) {
    size_t num_bytes_chopped = 0;
    chop.chop(this, byte_length, text, &num_bytes_chopped);
    byte_length -= num_bytes_chopped;
    text += num_bytes_chopped;
    chop.reset();
  }
}

size_t Situs::len() {
  size_t n;
  size_t len = 0;
  situs_span *spans = (num_spans_ > 1) ? u_.many_.spans_ : &u_.one_;
  for (n = 0; n < num_spans_; ++n) {
    situs_span *span = spans + n;
    len += span->num_bytes_;
  }
  return len;
}

void Situs::convert_to_substitution(size_t num_bytes) {
  size_t n;
  situs_span *span = first_span();
  for (n = 1; n < num_spans_; ++n) {
    span[n].is_aux_ = 1;
    span[n].num_bytes_ = 0;
  }
  span->is_substitution_ = 1;
  span->is_aux_ = 0;
  span->num_bytes_ = num_bytes;
}

bool Situs::get_byte_range(size_t *start, size_t *end) const {
  if (!num_spans_) {
    *start = *end = 0;
    return false;
  }

  const situs_span *spans = first_span();
  const situs_span *last_span = spans + num_spans_ - 1;

  *start = spans->start_;
  *end = last_span->end_;
  return true;
}

void Situs::init_from_after(const Situs *src_after, size_t num_bytes) {
  reset();
  const situs_span *spans = src_after->first_span();
  const situs_span *last_span = spans + src_after->num_spans_ - 1;
  situs_span *span = &u_.one_;
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

int Situs::line() {
  if (!num_spans_) return 0;
  return first_span()->start_line_;
}

int Situs::col() {
  if (!num_spans_) return 0;
  return first_span()->start_col_;
}

const char *Situs::filename() {
  if (!num_spans_) return "";
  return first_span()->filename_;
}

/* Chops at most the first byte_length bytes from the from situs and puts them into the chop situs.
 * The length of bytes chopped is returned in chop_length; this can be less than byte_length if
 * from consists of multiple spans. The code seeks to copy a single span, unless that span is a
 * substitution consisting of multiple auxiliary spans, in which case all spans of the substitution
 * will be copied. The goal is to make progress, not to complete all in one shot.
 */
void Situs::chop(Situs *from, size_t byte_length, const char *text, size_t *chop_length) {
  struct situs_span *sf = from->num_spans_ > 1 ? from->u_.many_.spans_ : &from->u_.one_;
  struct situs_span *sflast = sf;
  if (num_spans_ > 1) delete [] u_.many_.spans_;
  num_spans_ = 0;
  if (from->num_spans_ == 0) {
    *chop_length = 0;
    return;
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
      num_spans_ = 1;
      u_.one_ = *sf;
    }
    else {
      num_spans_ = num_spans;
      u_.many_.num_spans_allocated_ = num_spans;
      u_.many_.spans_ = new situs_span[num_spans];
      std::copy(sf, sf + num_spans, u_.many_.spans_);
    }
    if ((from->num_spans_ - num_spans) >= 2) {
      std::move(sflast + 1, sflast + 1 + from->num_spans_ - num_spans, sf);
      from->num_spans_ -= num_spans;
    }
    else if ((from->num_spans_ - num_spans) == 1) {
      struct situs_span *p = from->u_.many_.spans_;
      from->u_.one_ = p[num_spans];
      delete[] p;
      from->num_spans_ = 1;
    }
    else {
      from->num_spans_ = 0;
    }
    *chop_length = byte_length;
    return;
  }
  if (sf->is_substitution_) {
    sf->num_bytes_ -= byte_length;
    size_t num_spans = (sflast + 1) - sf;
    if (num_spans == 1) {
      num_spans_ = 1;
      u_.one_ = *sf;
      u_.one_.num_bytes_ = byte_length;
    }
    else {
      num_spans_ = num_spans;
      u_.many_.num_spans_allocated_ = num_spans;
      u_.many_.spans_ = new situs_span[num_spans];
      std::copy(sf, sf + num_spans, u_.many_.spans_);
      u_.many_.spans_->num_bytes_ = byte_length;
    }
    *chop_length = byte_length;
    return;
  }
  /* from-span is larger than byte length matched, so break it apart */
  int num_lines = 0, num_cols = 0;
  find_offset(text, byte_length, &num_lines, &num_cols);
  num_spans_ = 1;
  u_.one_.filename_ = sf->filename_;
  u_.one_.is_substitution_ = 0;
  u_.one_.num_bytes_ = byte_length;
  u_.one_.start_ = sf->start_;
  u_.one_.start_line_ = sf->start_line_;
  u_.one_.start_col_ = sf->start_col_;

  sf->start_ += byte_length;
  sf->num_bytes_ -= byte_length;
  sf->start_line_ += num_lines;
  sf->start_col_ = num_lines ? 1 + num_cols : (sf->start_col_ + num_cols);

  u_.one_.end_ = sf->start_;
  u_.one_.end_line_ = sf->start_line_;
  u_.one_.end_col_ = sf->start_col_;

  *chop_length = byte_length;
}

/* Note that, unlike situs_chop(), here the requirement is that all of chop be appended 
 * to to. Otherwise situs_concat() will not work. */
void Situs::glue(Situs *chop) {
  if (!chop->num_spans_) return;
  if (!num_spans_) {
    u_ = chop->u_;
    num_spans_ = chop->num_spans_;
    chop->num_spans_ = 0;
    return;
  }
  situs_span *tohead, *tolast;
  situs_span *tofirst = num_spans_ > 1 ? u_.many_.spans_ : &u_.one_;
  tolast = (num_spans_ > 1) ? u_.many_.spans_ + num_spans_ - 1 : &u_.one_;
  tohead = tolast;
  while (tohead->is_aux_ && tohead > tofirst) {
    --tohead;
  }
  situs_span *choplast = (chop->num_spans_ > 1) ? chop->u_.many_.spans_ + chop->num_spans_ - 1 : &chop->u_.one_;
  situs_span *chopfirst = chop->num_spans_ > 1 ? chop->u_.many_.spans_ : &chop->u_.one_;

  /* If they're identical substitutions, expand the num_bytes_ of the substitution
   * If they're adjacent ranges, expand the first range to end on the end of the second.
   * Otherwise append the second to the first.
   */
  if (tohead->is_substitution_ && 
      ((choplast - chopfirst) == (tolast - tohead)) &&
      std::equal(tohead, tohead + (1 + choplast - chopfirst), chopfirst)) { 
    /* Identical substitutions. */
    tohead->num_bytes_ += chopfirst->num_bytes_;
    if (chop->num_spans_ > 1) {
      delete [] chop->u_.many_.spans_;
    }
    chop->num_spans_ = 0;
    return;
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
      size_t new_num_spans = num_spans_ + num_extra_spans;
      if (num_spans_ == 1) {
        situs_span *sp = new situs_span[new_num_spans];
        *sp = u_.one_;
        std::copy(chopfirst + 1, chopfirst + 1 + num_extra_spans, sp + 1);
        num_spans_ = new_num_spans;
        u_.many_.num_spans_allocated_ = new_num_spans;
        u_.many_.spans_ = sp;
      }
      else /* (to->num_spans_ >= 2) */ {
        if (new_num_spans > u_.many_.num_spans_allocated_) {
          size_t new_num_alloc = u_.many_.num_spans_allocated_ * 2 + 1;
          if (new_num_alloc < new_num_spans) new_num_alloc = new_num_spans;
          situs_span *sp = new situs_span[new_num_alloc];
          std::move(u_.many_.spans_, u_.many_.spans_ + num_spans_, sp);
          delete [] u_.many_.spans_;
          u_.many_.num_spans_allocated_ = new_num_alloc;
          u_.many_.spans_ = sp;
        }

        std::copy(chopfirst + 1, chopfirst + 1 + num_extra_spans, u_.many_.spans_ + num_spans_);
        num_spans_ = new_num_spans;
      }
    }
    return;
  }
  /* If we get here, all of the chop needs to be appended to the situs */
  size_t num_extra_spans = (choplast + 1) - chopfirst;
  size_t new_num_spans = num_spans_ + num_extra_spans;
  if (num_spans_ == 1) {
    situs_span *sp = new situs_span[new_num_spans];
    *sp = u_.one_;
    std::copy(chopfirst, chopfirst + num_extra_spans, sp + 1);
    num_spans_ = new_num_spans;
    u_.many_.num_spans_allocated_ = new_num_spans;
    u_.many_.spans_ = sp;
  }
  else /* (to->num_spans_ >= 2) */ {
    if (new_num_spans > u_.many_.num_spans_allocated_) {
      size_t new_num_alloc = u_.many_.num_spans_allocated_ * 2 + 1;
      if (new_num_alloc < new_num_spans) new_num_alloc = new_num_spans;
      situs_span *sp = new situs_span[new_num_alloc];
      std::move(u_.many_.spans_, u_.many_.spans_ + num_spans_, sp);
      delete[] u_.many_.spans_;
      u_.many_.num_spans_allocated_ = new_num_alloc;
      u_.many_.spans_ = sp;
    }
    std::copy(chopfirst, chopfirst + num_extra_spans, u_.many_.spans_ + num_spans_);
    num_spans_ = new_num_spans;
  }
}


/* note: num_cols will contain the number of columns, it resets to _zero_ after a newline,
 *       and unlike most column representations, is therefore zero-based */
void Situs::find_offset(const char *bytes, size_t byte_size, int *num_lines, int *num_cols) {
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

