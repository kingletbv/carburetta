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

#ifndef STDLIB_H_INCLUDED
#define STDLIB_H_INCLUDED
#include <stdlib.h>
#endif

#ifndef STRING_H_INCLUDED
#define STRING_H_INCLUDED
#include <string.h>
#endif

#ifndef REX_SET_RANGE_H_INCLUDED
#define REX_SET_RANGE_H_INCLUDED
#include "rex_set_range.h"
#endif

#ifndef UC_CAT_RANGES_H_INCLUDED
#define UC_CAT_RANGES_H_INCLUDED
#include "uc_cat_ranges.h"
#endif

/* helper function when debugging; checks whether invariants are met. */
static int rex_set_range_sanity(struct rex_set_range *rng) {
  size_t n;
  for (n = 0; n < rng->num_intervals_; ++n) {
    struct rex_set_interval *iv = rng->intervals_ + n;
    if (iv->from_ > iv->to_) return -1;
    if (n && (iv->from_ <= (iv - 1)->to_)) return -1;
    if (iv->from_ < 0) return -1;
    if (iv->from_ > 0x10FFFF) return -1;
    if (iv->to_ < 0) return -1;
    if (iv->to_ > 0x10FFFF) return -1;
  }
  return 0;
}

void rex_set_range_init(struct rex_set_range *range) {
  range->num_intervals_ = range->num_intervals_allocated_ = 0;
  range->intervals_ = NULL;
}

void rex_set_range_cleanup(struct rex_set_range *range) {
  if (range->intervals_) free(range->intervals_);
}

void rex_set_range_swap(struct rex_set_range *a, struct rex_set_range *b) {
  struct rex_set_range tmp = *a;
  *a = *b;
  *b = tmp;
}

static int rex_set_range_ensure_space(struct rex_set_range *range) {
  if (range->num_intervals_ == range->num_intervals_allocated_) {
    size_t new_num_intervals_allocated = range->num_intervals_allocated_ ? range->num_intervals_allocated_ * 2 : 4;
    void *buf = realloc(range->intervals_, new_num_intervals_allocated * sizeof(struct rex_set_interval));
    if (!buf) return -1;
    range->intervals_ = (struct rex_set_interval *)buf;
    range->num_intervals_allocated_ = new_num_intervals_allocated;
  }
  return 0;
}

/* Adds the given range to the set; from and to are inclusive. */
int rex_set_range_add(struct rex_set_range *range, int from, int to) {
  if (!range->num_intervals_) {
    if (rex_set_range_ensure_space(range)) {
      return -1;
    }
    range->intervals_[0].from_ = from;
    range->intervals_[0].to_ = to;
    range->num_intervals_ = 1;
    return 0;
  }

  size_t n;

  // Loop to find the insertion
  // -> If it overlaps an existing range, that becomes the insertion
  // -> If from is in a gap, a new range becomes the insertion.
  // The insertion is then merged with adjacent (succeeding) ranges,
  // if a succeeding range's from is <= the insertion's to.

  /* Find insertion. */
  for (n = 0; n < range->num_intervals_; ++n) {
    struct rex_set_interval *iv = range->intervals_ + n;
    if (from <= iv->from_) {
      /* Start of new interval at 'from' */
      if ((to + 1) >= iv->from_) {
        /* Range merges with iv starting at from */
        iv->from_ = from;
        break;
      }
      else {
        /* Range inserts before iv and does not overlap */
        if (rex_set_range_ensure_space(range)) {
          return -1;
        }
        iv = range->intervals_ + n;
        memmove(iv + 1, iv, (range->num_intervals_ - n) * sizeof(struct rex_set_interval));
        iv->from_ = from;
        iv->to_ = to;
        range->num_intervals_++;
        return 0;
      }
    }
    else /* (from > iv->from_) */ {
      if (from <= (iv->to_ + 1)) {
        /* Range merges with iv, starting at iv->from_ */
        break;
      }
      else /* (from > iv->to_) */ {
        /* Range lies beyond iv, step to next. */
        continue;
      }
    }
  }

  if (n == range->num_intervals_) {
    /* Append to the end of it */
    if (rex_set_range_ensure_space(range)) {
      return -1;
    }
    range->intervals_[range->num_intervals_].from_ = from;
    range->intervals_[range->num_intervals_].to_ = to;
    range->num_intervals_++;
    return 0;
  }

  /* Merging from range->intervals_ + n including to to. */
  struct rex_set_interval *iv = range->intervals_ + n;
  if (iv->to_ < to) {
    iv->to_ = to;
  }

  for (++n; n < range->num_intervals_; ++n) {
    struct rex_set_interval *miv = range->intervals_ + n;
    if ((iv->to_ + 1) >= miv->from_) {
      /* Adjacent interval miv overlaps with newly formed interval iv */
      if (iv->to_ < miv->to_) {
        /* Adjacent interval miv extends newly formed interval iv; this also
         * implies we need not look beyond miv. */
        iv->to_ = miv->to_;
        /* done. */
        memmove(iv + 1, miv + 1, (range->num_intervals_ - n - 1) * sizeof(struct rex_set_interval));
        range->num_intervals_ -= miv - iv;
        return 0;
      }
    }
    else /* ((iv->to_ + 1) < miv->from_) */ {
      memmove(iv + 1, miv, (range->num_intervals_ - n) * sizeof(struct rex_set_interval));
      range->num_intervals_ -= miv - iv - 1;
      return 0;
    }
  }

  /* Set last interval to be iv as all the other intervals were overlapping
   * if we get here. */
  range->num_intervals_ = iv + 1 - range->intervals_;
  return 0;
}

int rex_set_range_add_range(struct rex_set_range *range, const struct rex_set_range *other) {
  size_t n;
  /* XXX: Make this O(n) instead of O(n^2) */
  for (n = 0; n < other->num_intervals_; ++n) {
    if (rex_set_range_add(range, other->intervals_[n].from_, other->intervals_[n].to_)) {
      /* Propagate failure */
      return -1;
    }
  }
  return 0;
}

int rex_set_range_unicode(struct rex_set_range *range, size_t first, size_t last, int invert) {
  range->num_intervals_ = 0;
  
  size_t rng_index;

  for (rng_index = first; rng_index <= last; ++rng_index) {
    const struct uc_cat_range *rng = g_uc_cat_ranges_[rng_index];
    size_t n;
    for (n = 0; n < rng->num_intervals_; ++n) {
      const struct uc_cat_interval *iv = rng->intervals_ + n;
      if (rex_set_range_add(range, iv->from_, iv->to_)) {
        return -1;
      }
    }
  }
  if (invert) {
    if (rex_set_range_unicode_invert(range)) {
      return -1;
    }
  }
  return 0;
}

/* Inverts the range over the unicode codepoint range; e.g. if the old set range had only
 * the 0x20 space character, the new range would be 0-0x1F; 0x21-0x10FFFF. */
int rex_set_range_unicode_invert(struct rex_set_range *range) {
  struct rex_set_range inverted;
  rex_set_range_init(&inverted);
  if (!range->num_intervals_) {
    if (rex_set_range_add(&inverted, 0x0, 0x10FFFF)) {
      rex_set_range_cleanup(&inverted);
      return -1;
    }
    rex_set_range_swap(range, &inverted);
    rex_set_range_cleanup(&inverted);
    return 0;
  }
  size_t n;
  for (n = 0; n <= range->num_intervals_; ++n) {
    int from, to;
    if (!n) {
      from = 0;
    }
    else {
      from = range->intervals_[n - 1].to_ + 1;
    }
    if (n == range->num_intervals_) {
      to = 0x10FFFF;
    }
    else {
      to = range->intervals_[n].from_ - 1;
    }
    if (to >= from) {
      if (rex_set_range_add(&inverted, from, to)) {
        rex_set_range_cleanup(&inverted);
        return -1;
      }
    }
  }
  rex_set_range_swap(range, &inverted);
  rex_set_range_cleanup(&inverted);
  return 0;
}
