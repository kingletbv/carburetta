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

#ifndef DATA_SECTION_H
#define DATA_SECTION_H

#ifdef __cplusplus
extern "C" {
#endif

/* A portion is 0 or more consecutive bytes in the data section. */
struct ds_portion {
  struct ds_portion *chain_;
  size_t size_;
  size_t size_allocated_;
  void *data_;
};

/* A datasection consists of portions which, together, describes all
 * the data in the datasection. */
struct data_section {
  struct ds_portion *portions_; /* tail cyclic */
};

void ds_init(struct data_section *ds);
void ds_cleanup(struct data_section *ds);

struct ds_portion *ds_alloc_portion(struct data_section *ds);

int ds_reserve(struct ds_portion *dsp, size_t num_bytes, void **buf);
int ds_append(struct ds_portion *dsp, size_t num_bytes, void *bytes);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* DATA_SECTION_H */
