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
#ifndef UC_CAT_RANGES_H
#define UC_CAT_RANGES_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef STDINT_H_INCLUDED
#define STDINT_H_INCLUDED
#include <stdint.h>
#endif

struct uc_cat_interval { 
  int from_, to_; 
};

struct uc_cat_range {
  const char *short_name_, *long_name_;
  const char *short_group_, *long_group_;
  size_t num_intervals_;
  const struct uc_cat_interval *intervals_;
};

extern const struct uc_cat_range *g_uc_cat_ranges_[];
extern const size_t g_uc_cat_range_num_;

/* Looks in the g_uc_cat_ranges_[] array for a match on name on any of short_name_, long_name_, short_group_ and long_group_,
 * and returns the index-range of the ranges that match in first_range_index and last_range_index respectively.
 * If no range matches, returns 0, if at least one matching range is found, non-zero is returned.
 * If you examine g_uc_cat_ranges_[] in the C file, you'll notice that short_name_ and long_name_ are all unique, and only
 * short_group_ and long_group_ are shared between ranges, so you'll only get multiple ranges when specifying a group.
 * Finally, note that name can be the substring of a larger string (and so does not need to be null-terminated) as its
 * length is explicitly passed in. */
int uc_cat_ranges_resolve_codepoint_category(const char *name, size_t len, size_t *first_range_index, size_t *last_range_index);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* UC_CAT_RANGES_H */
