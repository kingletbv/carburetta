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

#ifndef STDIO_H_INCLUDED
#define STDIO_H_INCLUDED
#include <stdio.h>
#endif

#ifndef STRING_H_INCLUDED
#define STRING_H_INCLUDED
#include <string.h>
#endif

#ifndef ASSERT_H_INCLUDED
#define ASSERT_H_INCLUDED
#include <assert.h>
#endif

#ifndef C_COMPILER_H_INCLUDED
#define C_COMPILER_H_INCLUDED
#include "c_compiler.h"
#endif

#ifndef STMT_H_INCLUDED
#define STMT_H_INCLUDED
#include "stmt.h"
#endif

#ifndef TEMPL_PARSER_H_INCLUDED
#define TEMPL_PARSER_H_INCLUDED
#include "templ_parser.h"
#endif

#ifndef NAME_SPACE_H_INCLUDED
#define NAME_SPACE_H_INCLUDED
#include "name_space.h"
#endif

struct stmt *stmt_alloc(struct c_compiler *cc, enum stmt_type st, struct situs *location) {
  struct stmt *s = (struct stmt *)malloc(sizeof(struct stmt));
  if (!s) {
    cc_no_memory(cc);
    return NULL;
  }
  struct name_space *ns = NULL;
  if (st == ST_BLOCK) {
    ns = (struct name_space *)malloc(sizeof(struct name_space));
    if (!ns) {
      free(s);
      return NULL;
    }
    ns_init(ns);
  }
  s->type_ = st;
  situs_init(&s->location_);
  if (situs_clone(&s->location_, location)) {
    free(s);
    if (ns) free(ns);
    return NULL;
  }
  s->parent_ = NULL;
  s->next_ = s->prev_ = s;
  s->parent_block_ = NULL;
  s->child0_ = s->child1_ = NULL;
  s->label_ = NULL;
  s->ns_ = ns;
  situs_init(&s->label_range_);
  s->expr0_ = NULL;
  s->expr1_ = NULL;
  s->expr2_ = NULL;
  s->parent_switch_ = NULL;
  s->break_parent_ = NULL;
  s->continue_parent_ = NULL;
  switch_case_map_init(&s->cases_);
  s->goto_destination_ = NULL;
  return s;
}

void stmt_free(struct stmt *s) {
  do {
    if (!s) return;
    if (s->next_ == s) {
      if (s->parent_) {
        if (s->parent_->child0_ == s) {
          s->parent_->child0_ = NULL;
        }
        if (s->parent_->child1_ == s) {
          s->parent_->child1_ = NULL;
        }
      }
    }
    else {
      s->next_->prev_ = s->prev_;
      s->prev_->next_ = s->next_;
      if (s->parent_) {
        if (s->parent_->child0_ == s) {
          s->parent_->child0_ = s->next_;
        }
        if (s->parent_->child1_ == s) {
          s->parent_->child1_ = s->next_;
        }
      }
    }
    struct stmt *next = NULL;

    if (!s->parent_) {
      /* No parent, may have to delete entire block (all siblings) */
      if (s->next_ != s) {
        next = s->next_;
      }
    }

    while (s->child0_) {
      stmt_free(s->child0_);
    }
    while (s->child1_) {
      stmt_free(s->child1_);
    }

    if (s->label_) free(s->label_);

    if (s->ns_) {
      ns_cleanup(s->ns_);
      free(s->ns_);
    }

    expr_free(s->expr0_);
    expr_free(s->expr1_);
    expr_free(s->expr2_);

    switch_case_map_cleanup(&s->cases_);

    situs_cleanup(&s->location_);
    situs_cleanup(&s->label_range_);

    free(s);

    s = next;
  } while (s);
}

void stmt_set_parent(struct stmt *s, struct stmt *new_parent) {
  struct stmt *p = s;
  if (p) {
    do {
      p = p->next_;

      p->parent_ = new_parent;
      if (p->ns_) {
        p->ns_->parent_ = new_parent->ns_;
      }

    } while (p != s);
  }
}

int stmt_prepare(struct c_compiler *cc, struct stmt *root) {
  /* Iterate through s and all its children, resolve goto's to their labels; as
   * defined by cx->current_func_'s namespace. */
  struct stmt *enter = root, *s;
  int failure = 0;
  for (;;) {
    do {
      s = enter;
      enter = NULL;

      if (s->expr0_) expr_prepare(s->expr0_);
      if (s->expr1_) expr_prepare(s->expr1_);
      if (s->expr2_) expr_prepare(s->expr2_);

      switch (s->type_) {
        case ST_GOTO:
          /* Perform lookup of label */
          if (cc->ctx_.current_func_) {
            struct ns_label_sym *nls = (struct ns_label_sym *)st_find(&cc->ctx_.current_func_->body_->ns_->stmt_labels_, s->label_);
            if (nls) {
              s->goto_destination_ = nls->labeled_stmt_;
            }
            else {
              cc_printf(cc, "error: label \"%s\" not found\n", s->label_);
              if (!failure) failure = -1;
            }
          }
          else {
            cc_printf(cc, "cannot resolve label \"%s\" outside function scope\n", s->label_);
            if (!failure) failure = -1;
          }
          break;
      }

      if (s->child0_) {
        enter = s->child0_;
      }
    } while (enter);

    do {
      if (s == root) {
        /* Finished at root, don't try to escape out, don't try to execute root's 
         * next sibling, just exit. */
        return 0;
      }
      if (s->parent_) {
        if ((s->next_ != s->parent_->child0_) && (s->next_ != s->parent_->child1_)) {
          /* Step to next */
          enter = s->next_;
        }
        else if ((s->next_ == s->parent_->child0_) && (s->parent_->child1_)) {
          /* Step to second child branch, for instance, in the case of an "if-else",
           * we get here if we just processed the if-clause body and should next process
           * the else-clause body. */
          enter = s->parent_->child1_;
        }
        else {
          s = s->parent_;
        }
      }
      else {
        return 0;
      }
    } while (!enter);
  }

  return failure;
}

int stmt_exec(struct c_compiler *cc, struct stmt *root, void *local_base, void *param_base, void *return_value_ptr) {
  int r;
  struct expr_temp result;
  struct stmt *s = NULL;
  struct stmt *enter = root;
  for (;;) {
    do {
      /* Entering for execution */
      s = enter;
      enter = NULL;

      switch (s->type_) {
        case ST_LABEL:
        case ST_CASE:
        case ST_DEFAULT:
          enter = s->child0_;
          break;
        case ST_BLOCK:
          /* Enter block */
          enter = s->child0_;
          break;
        case ST_EXPR:
          r = expr_eval(cc, &result, s->expr0_, 0, NULL, local_base, param_base, return_value_ptr);
          if (r) {
            return r;
          }
#if 0
          expr_debug_print(cc, s->expr0_);
          cc_printf(cc, ": ");
          expr_print_temp(cc, s->expr0_, &result);
          cc_printf(cc, "\n");
#endif
          break;
        case ST_IF_ELSE:
          r = expr_eval(cc, &result, s->expr0_, 0, NULL, local_base, param_base, return_value_ptr);
          if (r) {
            return r;
          }
          if (result.v_.u64_) {
            enter = s->child0_;
          }
          else {
            enter = s->child1_;
          }
          break;
        case ST_SWITCH: {
          r = expr_eval(cc, &result, s->expr0_, 0, NULL, local_base, param_base, return_value_ptr);
          if (r) {
            return r;
          }
          struct switch_case *sc = switch_find_case(&s->cases_, result.v_.u64_);
          if (sc) {
            enter = sc->case_stmt_;
          }
          else if (s->cases_.default_) {
            enter = s->cases_.default_->case_stmt_;
          }
          else {
            /* fall through statement after switch */
          }
          break;
        }
        case ST_CONTINUE:
          s = s->continue_parent_;
          /* s = ST_WHILE, ST_DO_WHILE or ST_FOR; we would like to /continue/ the loop,
           * whereas if we leave it as is, we will /exit/ the loop. To continue the loop
           * set s to the last statement inside the loop body, causing loop conditions to
           * be re-evaluated below. Contrast this with the break statement, where we
           * do want to exit. s->child0_ is either a single statement or an ST_BLOCK. */
          s = s->child0_;
          break;
        case ST_BREAK:
          s = s->break_parent_;
          break;
        case ST_IF:
        case ST_WHILE:
          r = expr_eval(cc, &result, s->expr0_, 0, NULL, local_base, param_base, return_value_ptr);
          if (r) {
            return r;
          }
          if (result.v_.u64_) {
            enter = s->child0_;
          }
          break;
        case ST_DO_WHILE:
          /* Enter block */
          enter = s->child0_;
          break;
        case ST_FOR:
          if (s->expr0_) {
            r = expr_eval(cc, &result, s->expr0_, 0, NULL, local_base, param_base, return_value_ptr);
            if (r) {
              return r;
            }
          }
          if (s->expr1_) {
            r = expr_eval(cc, &result, s->expr1_, 0, NULL, local_base, param_base, return_value_ptr);
            if (r) {
              return r;
            }
            if (result.v_.u64_) {
              enter = s->child0_;
            }
          }
          else {
            enter = s->child0_;
          }
          break;
        case ST_GOTO:
          /* Label statement is stored in s->child0_, even though strictly this is not a child.. */
          enter = s->goto_destination_;
          break;
        case ST_RETURN:
          if (s->expr0_) {
            r = expr_eval(cc, &result, s->expr0_, 0, NULL, local_base, param_base, return_value_ptr);
            if (r) {
              return r;
            }
          }
          /* Break out of statement evaluation */
          return 0;
          break;
        case ST_NOP:
          /* Do nothing */
          break;
      }
    } while (enter);
    do {
      if (s == root) {
        /* Completed execution, break out */
        return 0;
      }
      if (s->parent_ && (s->next_ != s->parent_->child0_) && (s->next_ != s->parent_->child1_)) {
        /* Step to next */
        enter = s->next_;
      }
      else {
        s = s->parent_;
        if (!s) {
          return 0;
        }
        if (s->type_ == ST_FOR) {
          if (s->expr2_) {
            r = expr_eval(cc, &result, s->expr2_, 0, NULL, local_base, param_base, return_value_ptr);
            if (r) {
              return r;
            }
          }

          if (s->expr1_) {
            r = expr_eval(cc, &result, s->expr1_, 0, NULL, local_base, param_base, return_value_ptr);
            if (r) {
              return r;
            }
            if (result.v_.u64_) {
              enter = s->child0_;
            }
          }
          else {
            enter = s->child0_;
          }
        }
        else if ((s->type_ == ST_WHILE) || (s->type_ == ST_DO_WHILE)) {
          r = expr_eval(cc, &result, s->expr0_, 0, NULL, local_base, param_base, return_value_ptr);
          if (r) {
            return r;
          }
          if (result.v_.u64_) {
            enter = s->child0_;
          }
        }
      }
    } while (!enter);

  }
}
