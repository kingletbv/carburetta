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

#ifndef SWITCH_H_INCLUDED
#define SWITCH_H_INCLUDED
#include "switch.h"
#endif

void switch_case_map_init(struct switch_case_map *scm) {
  scm->first_ = NULL;
  scm->root_ = NULL;
  scm->default_ = NULL;
}

void switch_case_map_cleanup(struct switch_case_map *scm) {
  struct switch_case *sc;
  /* Free all switch_cases without a care for the red black tree */
  sc = scm->first_;
  if (sc) {
    do {
      struct switch_case *next = sc->next_;

      free(sc);

      sc = next;
    } while (sc != scm->first_);
  }
}

static struct switch_case *switch_find_or_insert_at_node(struct switch_case *h, uint64_t case_val, struct switch_case **result);
static struct switch_case *switch_delete_at(struct switch_case *h, uint64_t case_val, struct switch_case **deleted);
static struct switch_case *switch_alloc_switch_case_struct(void);

struct switch_case *switch_alloc_case(struct switch_case_map *scm, struct stmt *case_stmt, uint64_t case_value) {
  struct switch_case *sc = NULL;
  scm->root_ = switch_find_or_insert_at_node(scm->root_, case_value, &sc);

  if (sc && !sc->case_stmt_) {
    /* New insertion */
    sc->case_stmt_ = case_stmt;
  
    if (scm->first_) {
      sc->next_ = scm->first_;
      sc->prev_ = scm->first_->prev_;
      sc->next_->prev_ = sc->prev_->next_ = sc;
    }
    else {
      sc->next_ = sc->prev_ = scm->first_ = sc;
    }
  }

  return sc;
}

struct switch_case *switch_alloc_default(struct switch_case_map *scm, struct stmt *default_stmt) {
  if (scm->default_) {
    return scm->default_;
  }

  struct switch_case *sc = switch_alloc_switch_case_struct();
  if (!sc) {
    return NULL;
  }
  
  scm->default_ = sc;

  sc->case_stmt_ = default_stmt;
  if (scm->first_) {
    sc->next_ = scm->first_;
    sc->prev_ = scm->first_->prev_;
    sc->next_->prev_ = sc->prev_->next_ = sc;
  }
  else {
    sc->next_ = sc->prev_ = scm->first_ = sc;
  }

  return sc;
}


void switch_free_case(struct switch_case_map *scm, uint64_t case_value) {
  struct switch_case *deleted;
  scm->root_ = switch_delete_at(scm->root_, case_value, &deleted);
  if (scm->root_) scm->root_->is_red_ = 0;
  if (deleted) {
    if (deleted->next_ == deleted) {
      scm->first_ = NULL;
    }
    else {
      if (scm->first_ == deleted) {
        scm->first_ = deleted->next_;
      }
      deleted->next_->prev_ = deleted->prev_;
      deleted->prev_->next_ = deleted->next_;
    }
  }
  free(deleted);
}

struct switch_case *switch_rotate_left(struct switch_case *h) {
  struct switch_case *x = h->right_;
  h->right_ = x->left_;
  x->left_ = h;
  x->is_red_ = h->is_red_;
  h->is_red_ = 1;
  return x;
}

struct switch_case *switch_rotate_right(struct switch_case *h) {
  struct switch_case *x;
  x = h->left_;
  h->left_ = x->right_;
  x->right_ = h;
  x->is_red_ = h->is_red_;
  h->is_red_ = 1;
  return x;
}

void switch_flip_colors(struct switch_case *h) {
  struct switch_case *left = h->left_;
  struct switch_case *right = h->right_;
  h->is_red_ = !h->is_red_;
  left->is_red_ = !left->is_red_;
  right->is_red_ = !right->is_red_;
}

static struct switch_case *switch_alloc_switch_case_struct(void) {
  struct switch_case *n = (struct switch_case *)malloc(sizeof(struct switch_case));
  if (!n) {
    return NULL;
  }
  n->case_value_ = 0;
  n->is_red_ = 0;
  n->left_ = n->right_ = NULL;
  n->next_ = n->prev_ = NULL;
  n->case_stmt_ = NULL;
  return n;
}

static struct switch_case *switch_fixup(struct switch_case *h) {
  if (h->right_ && h->right_->is_red_ && !(h->left_ && h->left_->is_red_)) {
    h = switch_rotate_left(h);
  }

  if (h->left_ && h->left_->is_red_) {
    struct switch_case *left = h->left_;
    if (left->left_ && left->left_->is_red_) {
      h = switch_rotate_right(h);
    }
  }

  if (h->left_ && h->left_->is_red_ && h->right_ && h->right_->is_red_) {
    switch_flip_colors(h);
  }

  return h;
}

static struct switch_case *switch_find_or_insert_at_node(struct switch_case *h, uint64_t case_val, struct switch_case **result) {
  if (!h) {
    /* At leaf, no match, allocate new */
    struct switch_case *n = switch_alloc_switch_case_struct();
    if (!n) {
      *result = NULL;
      return NULL;
    }
    n->case_value_ = case_val;
    n->is_red_ = 1;
    *result = n;
    return n;
  }

  if (case_val < h->case_value_) {
    struct switch_case *new_left_branch = switch_find_or_insert_at_node(h->left_, case_val, result);
    h->left_ = new_left_branch;
  }
  else if (case_val > h->case_value_) {
    struct switch_case *new_right_branch = switch_find_or_insert_at_node(h->right_, case_val, result);
    h->right_ = new_right_branch;
  }
  else /* (case_val == h->case_value_) */ {
    *result = h;
    return h;
  }

  return switch_fixup(h);
}

static struct switch_case *switch_move_red_left(struct switch_case *h) {
  switch_flip_colors(h);
  if (h->right_ && h->right_->left_ && h->right_->left_->is_red_) {
    h->right_ = switch_rotate_right(h->right_);
    h = switch_rotate_left(h);
    switch_flip_colors(h);
  }
  return h;
}

static struct switch_case *switch_move_red_right(struct switch_case *h) {
  switch_flip_colors(h);
  if (h->left_ && h->left_->left_ && h->left_->left_->is_red_) {
    h = switch_rotate_right(h);
    switch_flip_colors(h);
  }
  return h;
}

static struct switch_case *switch_delete_and_retrieve_min(struct switch_case *h, struct switch_case **minfound) {
  if (!h->left_) {
    *minfound = h;
    return NULL;
  }

  if ((!h->left_ || !h->left_->is_red_) && (!h->left_ || !h->left_->left_ || !h->left_->left_->is_red_)) {
    h = switch_move_red_left(h);
  }

  h->left_ = switch_delete_and_retrieve_min(h->left_, minfound);

  return switch_fixup(h);
}

static struct switch_case *switch_delete_at(struct switch_case *h, uint64_t case_val, struct switch_case **deleted) {
  if (case_val < h->case_value_) {
    if (!h->left_) {
      /* Key not in tree */
      *deleted = NULL;
      return h;
    }
    if ((h->left_->is_red_) && (!h->left_->left_ || !h->left_->left_->is_red_)) {
      h = switch_move_red_left(h);
    }
    h->left_ = switch_delete_at(h->left_, case_val, deleted);
  }
  else {
    if (h->left_ && h->left_->is_red_) {
      h = switch_rotate_right(h);
    }

    if ((case_val == h->case_value_) && !h->right_) {
      *deleted = h;
      return NULL;
    }

    if ((!h->right_ || !h->right_->is_red_) && (!h->right_ || !h->right_->left_ || !h->right_->left_->is_red_)) {
      h = switch_move_red_right(h);
    }

    if (case_val == h->case_value_) {
      /* Match at inner (non-leaf) node */
      *deleted = h;
      struct switch_case *substitute;
      struct switch_case *substitute_right_arm = switch_delete_and_retrieve_min(h->right_, &substitute);
      h = substitute;
      h->right_ = substitute_right_arm;
    }
    else {
      h->right_ = switch_delete_at(h, case_val, deleted);
    }
  }

  return switch_fixup(h);
}

struct switch_case *switch_find_case(struct switch_case_map *scm, uint64_t case_value) {
  struct switch_case *n = scm->root_;
  while (n) {
    if (case_value < n->case_value_) {
      n = n->left_;
    }
    else if (case_value > n->case_value_) {
      n = n->right_;
    }
    else {
      return n;
    }
  }
  return NULL;
}
