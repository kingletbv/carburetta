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

#ifndef STDIO_H_INCLUDED
#define STDIO_H_INCLUDED
#include <stdio.h>
#endif

#ifndef STDLIB_H_INCLUDED
#define STDLIB_H_INCLUDED
#include <stdlib.h>
#endif

#ifndef STRING_H_INCLUDED
#define STRING_H_INCLUDED
#include <string.h>
#endif

#ifndef ASSERT_H_INCLUDED
#define ASSERT_H_INCLUDED
#include <assert.h>
#endif

#ifndef HELPERS_H_INCLUDED
#define HELPERS_H_INCLUDED
#include "helpers.h"
#endif

#ifndef KLT_LOGGER_H_INCLUDED
#define KLT_LOGGER_H_INCLUDED
#define KLT_LOG_MODULE "symtab"
#include "klt_logger.h"
#endif

#ifndef SYMTAB_H_INCLUDED
#define SYMTAB_H_INCLUDED
#include "symtab.h"
#endif

static int st_count_leaf_depth(struct sym *s) {
  int count = 0;
  if (!s) return 0;
  if (!s->is_red_) {
    count = 1;
  }
  count += st_count_leaf_depth(s->left_);
  return count;
}

static int st_validate_leaf_depth(struct sym *s, int current_depth, int must_match) {
  int next_depth = current_depth;
  if (!s) {
    return (must_match == current_depth);
  }
  if (!s->is_red_) {
    next_depth++;;
  }
  if (!st_validate_leaf_depth(s->left_, next_depth, must_match)) {
    return 0;
  }
  if (!st_validate_leaf_depth(s->right_, next_depth, must_match)) {
    return 0;
  }
  return 1;
}

int st_check_sanity(struct symtab *st) {
  int ret = 1;
  int black_depth = st_count_leaf_depth(st->root_);

  if (!st_validate_leaf_depth(st->root_, 0, black_depth)) {
    LOGERROR("symtab is not balanced");
    ret = 0;
  }

  return ret;
}

static struct sym *st_rotate_left(struct sym *h) {
  struct sym *x = h->right_;
  h->right_ = x->left_;
  x->left_ = h;
  x->is_red_ = h->is_red_;
  h->is_red_ = 1;
  return x;
}

static struct sym *st_rotate_right(struct sym *h) {
  struct sym *x;
  x = h->left_;
  h->left_ = x->right_;
  x->right_ = h;
  x->is_red_ = h->is_red_;
  h->is_red_ = 1;
  return x;
}

static void st_flip_colors(struct sym *h) {
  struct sym *left = h->left_;
  struct sym *right = h->right_;
  h->is_red_ = !h->is_red_;
  left->is_red_ = !left->is_red_;
  right->is_red_ = !right->is_red_;
}

static struct sym *st_move_red_left(struct sym *h) {
  struct sym *right = h->right_;
  st_flip_colors(h);
  if (right && right->left_ && right->left_->is_red_) {
    h->right_ = st_rotate_right(h->right_);
    h = st_rotate_left(h);
    st_flip_colors(h);
  }
  return h;
}

static struct sym *st_move_red_right(struct sym *h) {
  struct sym *left = h->left_;
  st_flip_colors(h);
  if (left && left->left_ && left->left_->is_red_) {
    h = st_rotate_right(h);
    st_flip_colors(h);
  }
  return h;
}

static struct sym *st_fixup(struct sym *h) {
  if (h->right_ && h->right_->is_red_ && (!h->left_ || !h->left_->is_red_)) {
    h = st_rotate_left(h);
  }
  
  if (h->left_ && h->left_->is_red_) {
    if (h->left_->left_ && h->left_->left_->is_red_) {
      h = st_rotate_right(h);
    }
  }

  if (h->left_ && h->left_->is_red_ && h->right_ && h->right_->is_red_) {
    st_flip_colors(h);
  }

  return h;
}

static struct sym *st_alloc(size_t sym_size, const char *ident) {
  size_t ident_len = strlen(ident) + 1;
  struct sym *s = (struct sym *)malloc(sym_size + ident_len);
  if (!s) return NULL;
  s->hash_value_ = 0;
  s->ident_ = ((char *)s) + sym_size; /* Right after the sym */
  memcpy(s->ident_, ident, ident_len);
  s->ident_[ident_len - 1] = '\0';
  s->is_red_ = 0;
  s->left_ = s->right_ = NULL;
  return s;
}

static struct sym *st_find_or_insert_at_node(struct sym *h, uint64_t hash_key, const char *ident, struct sym **result, int *result_is_new, size_t sym_size) {
  if (!h) {
    /* At leaf, did not find an existing sym so allocate a new one */
    struct sym *n = st_alloc(sym_size, ident);
    *result = n;
    if (!n) {
      return NULL;
    }
    *result_is_new = 1;
    n->hash_value_ = hash_key;
    n->is_red_ = 1;
    n->left_ = n->right_ = NULL;
    return n;
  }

  if ((hash_key < h->hash_value_) || ((hash_key == h->hash_value_) && (strcmp(ident, h->ident_) < 0))) {
    struct sym *new_left_branch = st_find_or_insert_at_node(h->left_, hash_key, ident, result, result_is_new, sym_size);
    h->left_ = new_left_branch;
  }
  else if ((hash_key > h->hash_value_) || ((hash_key == h->hash_value_) && (strcmp(ident, h->ident_) > 0))) {
    struct sym *new_right_branch = st_find_or_insert_at_node(h->right_, hash_key, ident, result, result_is_new, sym_size);
    h->right_ = new_right_branch;
  }
  else /* hash_key == h->hash_value_ && value_key == h->value_ */ {
    *result = h;
    *result_is_new = 0;
  }

  return st_fixup(h);
}

static uint64_t st_hash(const char *value) {
  uint64_t hash_value = 0;
  while (*value) {
    hash_value = (hash_value << 7) | (hash_value >> ((-7) & 63));
    hash_value += *(uint8_t *)value;
    value++;
  }
  return hash_value;
}

static struct sym *st_delete_and_retrieve_min(struct sym *h, struct sym **min_sym) {
  if (h->left_ == NULL) {
    *min_sym = h;
    return NULL;
  }
  
  if ((!h->left_ || !h->left_->is_red_) && (!h->left_ || !h->left_->left_ || !h->left_->left_->is_red_)) {
    h = st_move_red_left(h);
  }

  h->left_ = st_delete_and_retrieve_min(h->left_, min_sym);

  return st_fixup(h);
}

static struct sym *st_delete_at(struct sym *h, uint64_t hash_key, const char *value_key, struct sym **sym_deleted) {
  if ((hash_key < h->hash_value_) || (hash_key == h->hash_value_) && (strcmp(value_key, h->ident_) < 0)) {
    if (!h->left_) {
      /* key not found in tree */
      *sym_deleted = NULL;
      return h;
    }
    if ((!h->left_->is_red_) && (!h->left_->left_ || !h->left_->left_->is_red_)) {
      h = st_move_red_left(h);
    }
    h->left_ = st_delete_at(h->left_, hash_key, value_key, sym_deleted);
  }
  else {
    if (h->left_ && h->left_->is_red_) {
      h = st_rotate_right(h);
    }

    if (((hash_key == h->hash_value_) && !strcmp(value_key, h->ident_)) && !h->right_) {
      /* Found match at leaf */
      *sym_deleted = h;
      return NULL;
    }

    if ((!h->right_ || (!h->right_->is_red_)) && (!h->right_ || !h->right_->left_ || !h->right_->left_->is_red_)) {
      h = st_move_red_right(h);
    }

    if ((hash_key == h->hash_value_) && !strcmp(value_key, h->ident_)) {
      /* Found match at inner node */
      *sym_deleted = h;
      struct sym *sub;
      struct sym *subs_right_arm = st_delete_and_retrieve_min(h->right_, &sub);
      sub->left_ = h->left_;
      /* Preserve read-ness of this node, even as we're replacing this node. */
      sub->is_red_ = h->is_red_;
      h = sub;
      h->right_ = subs_right_arm;
    }
    else {
      if (!h->right_) {
        /* key not found in tree */
        *sym_deleted = NULL;
        return h;
      }
      h->right_ = st_delete_at(h->right_, hash_key, value_key, sym_deleted);
    }
  }

  return st_fixup(h);
}


struct sym *st_find_or_insert(struct symtab *st, const char *ident, int *s_is_new, size_t sym_size) {
  struct sym *s = NULL;
  /* Value -1 should never survive, if it does, something is broken in st_find_or_insert_at_node() */
  *s_is_new = -1;
  st->root_ = st_find_or_insert_at_node(st->root_, st_hash(ident), ident, &s, s_is_new, sym_size);
  if (st->root_) st->root_->is_red_ = 0;
  if (!s) { return NULL; }
  if (*s_is_new) {
    /* Append new s to tail of sequence */
    if (st->seq_) {
      s->next_ = st->seq_;
      s->prev_ = st->seq_->prev_;
      s->next_->prev_ = s->prev_->next_ = s;
    }
    else {
      s->next_ = s->prev_ = st->seq_ = s;
    }
  }
  return s;
}

struct sym *st_find(struct symtab *st, const char *value_key) {
  uint64_t hash = st_hash(value_key);
  struct sym *s = st->root_;
  for (;;) {
    if (!s) return NULL; /* not found */
    if ((hash < s->hash_value_) || ((hash == s->hash_value_) && (strcmp(value_key, s->ident_) < 0))) {
      s = s->left_;
    }
    else if ((hash > s->hash_value_) || ((hash == s->hash_value_) && (strcmp(value_key, s->ident_) > 0))) {
      s = s->right_;
    }
    else /* hash == s->hash_value_ && value_key == s->value_ */ {
      return s;
    }
  }
}

int st_remove(struct symtab *st, struct sym *s) {
  struct sym *s_deleted;
  st->root_ = st_delete_at(st->root_, s->hash_value_, s->ident_, &s_deleted);
  if (st->root_) st->root_->is_red_ = 0;
  assert((s == s_deleted) || !s_deleted); /* if a match exists in the table, it must be s and not some other sym */
  if (s_deleted) {
    if (s_deleted->next_ == s) {
      st->seq_ = NULL;
    }
    else {
      if (st->seq_ == s_deleted) {
        st->seq_ = s_deleted->next_;
      }
      s_deleted->next_->prev_ = s_deleted->prev_;
      s_deleted->prev_->next_ = s_deleted->next_;
    }
    s->next_ = s->prev_ = NULL;
    
    return 1;
  }
  return 0;
}

void st_init(struct symtab *st) {
  st->root_ = NULL;
  st->seq_ = NULL;
}

void st_cleanup(struct symtab *st) {
  while (st->root_) {
    st_remove(st, st->root_);
  }
}

