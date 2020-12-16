/* Copyright 2020 Kinglet B.V.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http ://www.apache.org/licenses/LICENSE-2.0
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

#ifndef XLTS_H_INCLUDED
#define XLTS_H_INCLUDED
#include "xlts.h"
#endif

#ifndef ASSERT_H_INCLUDED
#define ASSERT_H_INCLUDED
#include <assert.h>
#endif

#ifndef MODE_H_INCLUDED
#define MODE_H_INLUDED
#include "mode.h"
#endif



void mode_table_init(struct mode_table *mt) {
  memset(mt->mode_table_, 0, sizeof(mt->mode_table_));
  mt->modes_ = NULL;
}

void mode_table_cleanup(struct mode_table *mt) {
  struct mode *m, *next;
  m = mt->modes_;
  if (m) {
    next = m->next_;
    do {
      m = next;
      next = m->next_;

      mode_cleanup(m);
      free(m);

    } while (m != mt->modes_);
    mt->modes_ = NULL;
  }
}


void mode_init(struct mode *m) {
  xlts_init(&m->def_);
  m->next_ = NULL;
  m->hash_chain_ = NULL;
  m->rex_mode_ = NULL;
}

void mode_cleanup(struct mode *m) {
  xlts_cleanup(&m->def_);
}

/* Follows same logic as in symbol.c */
static uint8_t mode_standardize_char(uint8_t c) {
  if (c == '-') c = '_';
  if ((c >= 'a') && (c <= 'z')) c = c + 'A' - 'a';
  return c;
}

static int mode_hash(const char *id) {
  uint64_t hash_value = 0;
  while (*id) {
    uint8_t c = mode_standardize_char(*(uint8_t *)id);
    /* Dashes convert to underscores, letters are all upper-cased, to ensure the symbols are unique *after* hashing */
    hash_value = (hash_value << 7) | (hash_value >> ((-7) & 63));
    hash_value += c;
    id++;
  }
  return (int)(hash_value % MODE_TABLE_SIZE);
}

static int mode_cmp(const char *left, const char *right) {
  while (*left && *right) {
    uint8_t left_value = mode_standardize_char((uint8_t)*left);
    uint8_t right_value = mode_standardize_char((uint8_t)*right);
    if (left_value < right_value) return -1;
    if (left_value > right_value) return 1;
    left++;
    right++;
  }
  if (*left) {
    return 1;
  }
  if (*right) {
    return -1;
  }
  return 0;
}


struct mode *mode_find_or_add(struct mode_table *mt, struct xlts *id, int *is_new) {
  int idx = mode_hash(id->translated_);
  struct mode *m, *last;
  m = last = mt->mode_table_[idx];
  if (m) {
    do {
      m = m->hash_chain_;

      if (!mode_cmp(m->def_.translated_, id->translated_)) {
        *is_new = 0;
        return m;
      }

    } while (m != last);
  }

  m = (struct mode *)malloc(sizeof(struct mode));
  if (!m) {
    return NULL;
  }
  mode_init(m);
  if (last) {
    m->hash_chain_ = last->hash_chain_;
    last->hash_chain_ = m;
  }
  else {
    m->hash_chain_ = m;
  }
  mt->mode_table_[idx] = m;

  if (mt->modes_) {
    m->next_ = mt->modes_->next_;
    mt->modes_->next_ = m;
  }
  else {
    m->next_ = m;
  }
  mt->modes_ = m;

  xlts_append(&m->def_, id);

  *is_new = 1;
  return m;
}

struct mode *mode_find(struct mode_table *mt, const char *id) {
  int idx = mode_hash(id);
  struct mode *m, *last;
  m = last = mt->mode_table_[idx];
  if (m) {
    do {
      m = m->hash_chain_;

      if (!mode_cmp(m->def_.translated_, id)) {
        return m;
      }
    } while (m != last);
  }

  return NULL; /* not found */
}
