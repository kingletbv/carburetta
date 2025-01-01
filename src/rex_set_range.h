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

#ifndef REX_SET_RANGE_H
#define REX_SET_RANGE_H

#ifdef __cplusplus
extern "C" {
#endif

struct uc_cat_range;

struct rex_set_interval {
  int from_, to_;
};

struct rex_set_range {
  size_t num_intervals_;
  size_t num_intervals_allocated_;
  struct rex_set_interval *intervals_;
};

void rex_set_range_init(struct rex_set_range *range);
void rex_set_range_cleanup(struct rex_set_range *range);

/* Swaps the contents of the two rex set ranges */
void rex_set_range_swap(struct rex_set_range *a, struct rex_set_range *b);

/* Adds the given range to the set; from and to are inclusive. */
int rex_set_range_add(struct rex_set_range *range, int from, int to);

/* Merges the two ranges */
int rex_set_range_add_range(struct rex_set_range *range, const struct rex_set_range *other);

/* Resets "range", adds the given unicode ranges to it, combining them, and then optionally inverts the range.
 * first and last index g_uc_cat_ranges_[] (see uc_cat_ranges.h). */
int rex_set_range_unicode(struct rex_set_range *range, size_t first, size_t last, int invert);

/* Inverts the range over the unicode codepoint range; e.g. if the old set range had only
 * the 0x20 space character, the new range would be 0-0x1F; 0x21-0x10FFFF. */
int rex_set_range_unicode_invert(struct rex_set_range *range);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* REX_SET_RANGE_H */
