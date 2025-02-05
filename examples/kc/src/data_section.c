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

#ifndef STRING_H_INCLUDED
#define STRING_H_INCLUDED
#include <string.h>
#endif

#ifndef DATA_SECTION_H_INCLUDED
#define DATA_SECTION_H_INCLUDED
#include "data_section.h"
#endif

void ds_init(struct data_section *ds) {
  ds->portions_ = NULL;
}

void ds_cleanup(struct data_section *ds) {
  struct ds_portion *dsp = ds->portions_;
  if (dsp) {
    struct ds_portion *next = dsp->chain_;
    do {
       dsp = next;
       next = dsp->chain_;

      if (dsp->data_) {
        free(dsp->data_);
      }
      free(dsp);

    } while (dsp != ds->portions_);
  }
}


struct ds_portion *ds_alloc_portion(struct data_section *ds) {
  struct ds_portion *dsp = (struct ds_portion *)malloc(sizeof(struct ds_portion));
  if (ds->portions_) {
    dsp->chain_ = ds->portions_->chain_;
    ds->portions_->chain_ = dsp;
    ds->portions_ = dsp;
  }
  else {
    dsp->chain_ = dsp;
    ds->portions_ = dsp;
  }
  dsp->data_ = NULL;
  dsp->size_ = 0;
  dsp->size_allocated_ = 0;
  return dsp;
}

int ds_reserve(struct ds_portion *dsp, size_t num_bytes, void **pbuf) {
  size_t size_needed = dsp->size_ + num_bytes;
  if (size_needed < num_bytes) {
    return -1; /* overflow */
  }
  if (size_needed > dsp->size_allocated_) {
    size_t size_to_allocate = dsp->size_allocated_ + dsp->size_allocated_ + 1;
    if (size_to_allocate <= dsp->size_allocated_) {
      return -1; /* overflow / maximum size_t exceeded. */
    }
    if (size_to_allocate < size_needed) {
      size_to_allocate = size_needed;
    }
    void *buf = realloc(dsp->data_, size_to_allocate);
    if (!buf) {
      return -2; /* no mem */
    }
    memset(((char *)buf) + dsp->size_allocated_, 0, (size_to_allocate - dsp->size_allocated_));
    dsp->data_ = buf;
    dsp->size_allocated_ = size_to_allocate;
  }
  *pbuf = ((char *)dsp->data_) + dsp->size_;
  dsp->size_ += num_bytes;
  return 0;
}

int ds_append(struct ds_portion *dsp, size_t num_bytes, void *bytes) {
  int r;
  void *buf;
  r = ds_reserve(dsp, num_bytes, &buf);
  if (r) return r;
  memcpy(((char *)dsp->data_) + dsp->size_, bytes, num_bytes);
  return 0;
}

