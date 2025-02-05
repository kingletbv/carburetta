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

#ifdef _WIN32
#ifndef WINDOWS_H_INCLUDED
#define WINDOWS_H_INCLUDED
#include <Windows.h>
#endif
#elif defined(__linux__)
#ifndef PTHREAD_H_INCLUDED
#define PTHREAD_H_INCLUDED
#include <pthread.h>
#endif
#endif

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

#ifndef TIME_H_INCLUDED
#define TIME_H_INCLUDED
#include <time.h>
#endif

#ifndef STDARG_H_INCLUDED
#define STDARG_H_INCLUDED
#include <stdarg.h>
#endif

#ifndef HELPERS_H_INCLUDED
#define HELPERS_H_INCLUDED
#include "helpers.h"
#endif

#include "klt_logger.h"

#ifdef __linux__
pthread_key_t g_thread_level_intercept_tls_;
pthread_key_t g_thread_level_intercept_baton_tls_;
#elif defined(_WIN32)
static DWORD g_thread_level_intercept_tls_;
static DWORD g_thread_level_intercept_baton_tls_;
#else
#error Platform not implemented
#endif

void *g_log_global_lock_ = NULL;
struct klt_log_node *g_log_root_ = NULL;

struct klt_log_anchor g_log_root_anchor_ = {
  "/",
  NULL, NULL, NULL
};

static void klt_log_node_tree_cleanup(struct klt_log_node *node);


static void *klt_log_alloc_cs(void) {
#ifdef __linux__
  void *cs = malloc(sizeof(pthread_mutex_t));
  if (!cs) return NULL;
  pthread_mutex_init(cs, NULL);
  return cs;
#elif defined(_WIN32)
  void *cs = malloc(sizeof(CRITICAL_SECTION));
  if (!cs) return NULL;
  InitializeCriticalSection((LPCRITICAL_SECTION)cs);
  return cs;
#else
#error Platform not implemented
#endif
}

static void klt_log_free_cs(void *cs) {
#ifdef __linux__
  pthread_mutex_destroy((pthread_mutex_t *)cs);
  free(cs);
#elif defined(WIN32)
  DeleteCriticalSection((LPCRITICAL_SECTION)cs);
  free(cs);
#else
#error Platform not implemented
#endif
}

static void klt_log_lock(void *cs) {
#ifdef __linux__
  pthread_mutex_lock((pthread_mutex_t *)cs);
#elif defined(_WIN32)
  EnterCriticalSection((LPCRITICAL_SECTION)cs);
#else
#error Platform not implemented
#endif
}

static void klt_log_unlock(void *cs) {
#ifdef __linux__
  pthread_mutex_unlock((pthread_mutex_t *)cs);
#elif defined(_WIN32)
  LeaveCriticalSection((LPCRITICAL_SECTION)cs);
#else
#error Platform not implemented
#endif
}

void klt_logger_init(void) {
  g_log_global_lock_ = klt_log_alloc_cs();
  g_log_root_ = (struct klt_log_node *)malloc(sizeof(struct klt_log_node));
  klt_log_node_init(g_log_root_);
  g_log_root_->name_ = strdup("/");
#ifdef __linux__
  pthread_key_create(&g_thread_level_intercept_tls_, NULL);
  pthread_key_create(&g_thread_level_intercept_baton_tls_, NULL);
#elif defined(_WIN32)
  g_thread_level_intercept_tls_ = TlsAlloc();
  g_thread_level_intercept_baton_tls_ = TlsAlloc();
#else
#error Platform not implemented
#endif
}

void klt_logger_cleanup(void) {
#ifdef __linux__
  pthread_key_delete(g_thread_level_intercept_tls_);
  pthread_key_delete(g_thread_level_intercept_baton_tls_);
#elif defined(_WIN32)
  TlsFree(g_thread_level_intercept_tls_);
  TlsFree(g_thread_level_intercept_baton_tls_);
#else
#error Platform not implemented
#endif
  klt_log_node_tree_cleanup(g_log_root_);
  g_log_root_ = NULL;
  if (g_log_global_lock_) klt_log_free_cs(g_log_global_lock_);
}

static struct klt_log_node *klt_log_node_construct_path(const char *path) {
  const char *p;
  struct klt_log_node *node;
  p = path;
  node = g_log_root_;

  for (;;) {
    while (*p == '/') *p++;
    if (!*p) {
      return node;
    }

    const char *start_ident = p;
    while ((*p) && (*p != '/')) p++;

    struct klt_log_node *sib = node->first_child_;
    if (sib) {
      do {
        if (!strncmp(sib->name_, start_ident, p - start_ident)) {
          if (!sib->name_[p - start_ident]) {
            node = sib;
          }
        }
      } while (sib != node->first_child_);
    }
    if (node != sib) {
      /* Failed to find node matching path, so build our own path. */
      sib = (struct klt_log_node *)malloc(sizeof(struct klt_log_node));
      if (!sib) {
        return NULL;
      }
      klt_log_node_init(sib);
      sib->name_ = (char *)malloc(1 + p - start_ident);
      if (!sib->name_) {
        free(sib);
        return NULL;
      }
      memcpy(sib->name_, p, p - start_ident);
      sib->name_[p - start_ident] = '\0';
      sib->parent_ = node;
      if (sib->parent_->first_child_) {
        sib->next_sibling_ = sib->parent_;
        sib->prev_sibling_ = sib->next_sibling_->prev_sibling_;
        sib->prev_sibling_->next_sibling_ = sib;
        sib->next_sibling_->prev_sibling_ = sib;
      }
      else {
        sib->next_sibling_ = sib->prev_sibling_ = sib->parent_->first_child_ = sib;
      }
      sib->flags_ = node->flags_ & KLT_LOG_NODE_FLAG_ENABLED;
      sib->log_fn_ptr_ = node->log_fn_ptr_;
      sib->log_fn_baton_ = node->log_fn_baton_;

      node = sib;
    }
  }
}


void klt_log_node_init(struct klt_log_node *node) {
  node->name_ = NULL;

  node->parent_ = NULL;
  node->next_sibling_ = node->prev_sibling_ = node;
  node->first_child_ = NULL;

  node->flags_ = 0;

  node->log_fn_ptr_ = NULL;
  node->log_fn_baton_ = NULL;

  node->first_anchor_ = NULL;
}

void klt_log_node_cleanup(struct klt_log_node *node) {
  if (node == g_log_root_) {
    g_log_root_ = NULL;
  }
  if (node->name_) {
    free(node->name_);
  }
  if (node->parent_) {
    if (node->parent_->first_child_ == node) {
      if (node->next_sibling_ == node) {
        node->parent_->first_child_ = NULL;
      }
      else {
        node->parent_->first_child_ = node->next_sibling_;
        node->next_sibling_->prev_sibling_ = node->prev_sibling_;
        node->prev_sibling_->next_sibling_ = node->next_sibling_;
      }
    }
    else {
      node->next_sibling_->prev_sibling_ = node->prev_sibling_;
      node->prev_sibling_->next_sibling_ = node->next_sibling_;
    }
    node->parent_ = node->next_sibling_ = node->prev_sibling_ = NULL;
  }
  if (node->first_child_) {
    /* detach all children from parent, this leaves them lingering
     * with a NULL parent but avoids having a stray pointer. It is not
     * the role or desired behavior of klt_log_node_cleanup() to clear
     * out the entire tree; see klt_log_node_tree_cleanup(). */
    struct klt_log_node *child;
    child = node->first_child_;
    do {
      struct klt_log_node *next = child->next_sibling_;
      child->parent_ = NULL;
      child->next_sibling_ = child->prev_sibling_ = NULL;

      child = next;
    } while (child != node->first_child_);
    node->first_child_ = NULL;
  }
  if (node->first_anchor_) {
    struct klt_log_anchor *anchor;
    anchor = node->first_anchor_;
    do {
      struct klt_log_anchor *next = anchor->next_in_node_;
      anchor->node_ = NULL;
      anchor->next_in_node_ = anchor->prev_in_node_ = NULL;

      anchor = next;
    } while (anchor != node->first_anchor_);
    node->first_anchor_ = NULL;
  }
}

static void klt_log_node_tree_cleanup(struct klt_log_node *node) {
  struct klt_log_node *root = node;
  for (;;) {
    if (node->first_child_) {
      /* We have children, enter sub-tree. */
      node = node->first_child_;
    }
    else {
      struct klt_log_node *doomed = node;
      if (node->next_sibling_ != node) {
        /* We have siblings but no children, take out this node, move to sibling */
        node = node->next_sibling_;
      }
      else {
        /* No children, no siblings, take out this node, move to parent */
        node = node->parent_;
      }
      klt_log_node_cleanup(doomed);
      free(doomed);
      if (doomed == root) return;
    }
  }
}

static void klt_log_node_ascending_unused_cleanup(struct klt_log_node *node) {
  /* Keep removing this node and its parens while no function is defined, no anchor is linked and
   * no other children are attached to it */
  while (!(node->first_child_) && !(node->first_anchor_) && !(node->flags_ & KLT_LOG_NODE_FLAG_DEFINITIONS_MASK)) {
    struct klt_log_node *doomed = node;
    node = node->parent_;
    klt_log_node_cleanup(doomed);
    free(doomed);
  }
}

static void klt_log_node_cascade(struct klt_log_node *root, int flags_carried) {
  struct klt_log_node *node = root;
  node->flags_carried_ = flags_carried & KLT_LOG_NODE_FLAG_DEFINITIONS_MASK;
  for (;;) {
    if (node->first_child_ && (node->flags_carried_ & ~node->first_child_->flags_)) {
      /* Child has fields we need to propagate to, enter sub-tree */
      node->first_child_->flags_carried_ = node->flags_carried_ & ~node->first_child_->flags_;
      node = node->first_child_;
    }
    else {
      /* No valid child, enter sibling if we have one */
      while ((node != root) && (node->next_sibling_ == node->parent_->first_child_)) {
        /* No sibling after us, step to parent and keep looking a sibling. Note we
         * have already visited all parents up to root before this. */
        node = node->parent_;
      }
      if (node == root) {
        /* Done */
        return;
      }

      node = node->next_sibling_;
      node->flags_carried_ = node->parent_->flags_carried_ & ~node->flags_;
    }

    if (node->flags_carried_ & KLT_LOG_NODE_FLAG_FUNCTION_DEFINITION) {
      node->log_fn_ptr_ = root->log_fn_ptr_;
      node->log_fn_baton_ = root->log_fn_baton_;
    }
    if (node->flags_carried_ & KLT_LOG_NODE_FLAG_ENABLED_DEFINITION) {
      node->flags_ &= ~KLT_LOG_NODE_FLAG_ENABLED;
      node->flags_ |= root->flags_ & KLT_LOG_NODE_FLAG_ENABLED;
    }
  }
}

static void klt_log_node_set_function(struct klt_log_node *node, klt_log_fn log_fn_ptr, void *log_fn_baton) {
  /* Overwrite whatever may have been there before us. Clear function definition flag
   * to ensure klt_log_node_cascade_function() is not a no-op. */
  node->flags_ |= KLT_LOG_NODE_FLAG_FUNCTION_DEFINITION;
  node->log_fn_ptr_ = log_fn_ptr;
  node->log_fn_baton_ = log_fn_baton;
  klt_log_node_cascade(node, KLT_LOG_NODE_FLAG_FUNCTION_DEFINITION);
}

static void klt_log_node_clear_function(struct klt_log_node *node) {
  node->flags_ &= ~KLT_LOG_NODE_FLAG_FUNCTION_DEFINITION;
  if (node->parent_) {
    node->log_fn_ptr_ = node->parent_->log_fn_ptr_;
    node->log_fn_baton_ = node->parent_->log_fn_baton_;
  }
  else {
    node->log_fn_ptr_ = NULL;
    node->log_fn_baton_ = NULL;
  }
  klt_log_node_cascade(node, KLT_LOG_NODE_FLAG_FUNCTION_DEFINITION);
  /* Clean up if nothing uses the node after clearing the function */
  klt_log_node_ascending_unused_cleanup(node);
}

void klt_log_set_function(const char *module, klt_log_fn fn, void *fn_baton) {
  if (g_log_global_lock_) {
    klt_log_lock(g_log_global_lock_);

    struct klt_log_node *module_node = klt_log_node_construct_path(module);
    if (!module_node) {
      klt_log_unlock(g_log_global_lock_);
      return;
    }
    if (fn) {
      klt_log_node_set_function(module_node, fn, fn_baton);
    }
    else {
      klt_log_node_clear_function(module_node);
    }

    klt_log_unlock(g_log_global_lock_);
  }
}

void klt_log_clear_function(const char *module) {
  if (g_log_global_lock_) {
    klt_log_lock(g_log_global_lock_);

    struct klt_log_node *module_node = klt_log_node_construct_path(module);
    if (!module_node) {
      klt_log_unlock(g_log_global_lock_);
      return;
    }
    klt_log_node_clear_function(module_node);

    klt_log_unlock(g_log_global_lock_);
  }
}

static void klt_log_node_set_module_enable(struct klt_log_node *node, int enable) {
  node->flags_ |= KLT_LOG_NODE_FLAG_ENABLED_DEFINITION;
  node->flags_ &= ~KLT_LOG_NODE_FLAG_ENABLED;
  if (enable) node->flags_ |= KLT_LOG_NODE_FLAG_ENABLED;
  klt_log_node_cascade(node, KLT_LOG_NODE_FLAG_ENABLED_DEFINITION);
}

static void klt_log_node_clear_module_enable(struct klt_log_node *node) {
  node->flags_ &= ~KLT_LOG_NODE_FLAG_ENABLED_DEFINITION;
  node->flags_ &= ~KLT_LOG_NODE_FLAG_ENABLED;
  if (node->parent_) {
    node->flags_ |= KLT_LOG_NODE_FLAG_ENABLED & node->parent_->flags_;
  }
  klt_log_node_cascade(node, KLT_LOG_NODE_FLAG_ENABLED_DEFINITION);
  /* Clean up if nothing uses the node after clearing the function */
  klt_log_node_ascending_unused_cleanup(node);
}

void klt_log_set_module_enable(const char *module, int enable) {
  if (g_log_global_lock_) {
    klt_log_lock(g_log_global_lock_);

    struct klt_log_node *module_node = klt_log_node_construct_path(module);
    if (!module_node) {
      klt_log_unlock(g_log_global_lock_);
      return;
    }
    klt_log_node_set_module_enable(module_node, enable);

    klt_log_unlock(g_log_global_lock_);
  }
}

void klt_log_clear_module_enable(const char *module) {
  if (g_log_global_lock_) {
    klt_log_lock(g_log_global_lock_);

    struct klt_log_node *module_node = klt_log_node_construct_path(module);
    if (!module_node) {
      klt_log_unlock(g_log_global_lock_);
      return;
    }
    klt_log_node_clear_module_enable(module_node);

    klt_log_unlock(g_log_global_lock_);
  }
}

void klt_log(struct klt_log_anchor *origin_anchor, int level, const char *file, int line, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);

#ifdef __linux__
  klt_log_fn thread_local_fn = (klt_log_fn)pthread_getspecific(g_thread_level_intercept_tls_);
  void *thread_local_baton = pthread_getspecific(g_thread_level_intercept_baton_tls_);
#elif defined(_WIN32)
  klt_log_fn thread_local_fn = (klt_log_fn)TlsGetValue(g_thread_level_intercept_tls_);
  void *thread_local_baton = TlsGetValue(g_thread_level_intercept_baton_tls_);
#else
#error Platform not implemented
#endif

  if (thread_local_fn) {
    thread_local_fn(origin_anchor->node_, thread_local_baton, level, file, line, fmt, args);
    va_end(args);
    return;
  }

  if (g_log_global_lock_) {
    klt_log_lock(g_log_global_lock_);

    if (!origin_anchor->node_) {
      /* Create hierarchy to node for the anchor path if we don't have it already */
      origin_anchor->node_ = klt_log_node_construct_path(origin_anchor->full_path_initializer_);
    }

    if (origin_anchor->node_ && origin_anchor->node_->log_fn_ptr_) {
      /* Log if we have a function.. */
      origin_anchor->node_->log_fn_ptr_(origin_anchor->node_, origin_anchor->node_->log_fn_baton_, level, file, line, fmt, args);
    }

    klt_log_unlock(g_log_global_lock_);
  }

  va_end(args);
}


klt_log_fn klt_log_set_current_thread_intercept(klt_log_fn fn, void *baton, void **original_baton) {
#ifdef __linux__
  klt_log_fn original_fn = (klt_log_fn)pthread_getspecific(g_thread_level_intercept_tls_);
  if (original_baton) {
    *original_baton = (void *)pthread_getspecific(g_thread_level_intercept_baton_tls_);
  }
  pthread_setspecific(g_thread_level_intercept_tls_, fn);
  pthread_setspecific(g_thread_level_intercept_baton_tls_, baton);
  return original_fn;
#elif defined(_WIN32)
  klt_log_fn original_fn = (klt_log_fn)TlsGetValue(g_thread_level_intercept_tls_);
  if (original_baton) {
    *original_baton = (void *)TlsGetValue(g_thread_level_intercept_baton_tls_);
  }
  TlsSetValue(g_thread_level_intercept_tls_, fn);
  TlsSetValue(g_thread_level_intercept_baton_tls_, baton);
  return original_fn;
#else
#error Platform not implemented
#endif
}

klt_log_fn klt_log_clear_current_thread_intercept(void **original_baton) {
  return klt_log_set_current_thread_intercept(NULL, NULL, original_baton);
}

