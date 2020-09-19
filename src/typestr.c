#ifndef STDIO_H_INCLUDED
#define STDIO_H_INCLUDED
#include <stdio.h>
#endif

#ifndef STDLIB_H_INCLUDED
#define STDLIB_H_INCLUDED
#include <stdlib.h>
#endif

#ifndef STDINT_H_INCLUDED
#define STDINT_H_INCLUDED
#include <stdint.h>
#endif

#ifndef SNIPPET_H_INCLUDED
#define SNIPPET_H_INCLUDED
#include "snippet.h"
#endif

#ifndef KLT_LOGGER_H_INCLUDED
#define KLT_LOGGER_H_INCLUDED
#define KLT_LOG_MODULE "typestr"
#include "klt_logger.h"
#endif

#ifndef TYPESTR_H_INCLUDED
#define TYPESTR_H_INCLUDED
#include "typestr.h"
#endif

static void typestr_init(struct typestr *ts);
static void typestr_cleanup(struct typestr *ts);

void typestr_table_init(struct typestr_table *tt) {
  size_t n;
  for (n = 0; n < TYPESTR_TABLE_SIZE; ++n) {
    tt->hash_table_[n] = NULL;
  }
  tt->num_typestrs_ = tt->num_typestrs_allocated_ = 0;
  tt->typestrs_ = NULL;
}

void typestr_table_cleanup(struct typestr_table *tt) {
  size_t n;
  for (n = 0; n < tt->num_typestrs_; ++n) {
    struct typestr *ts = tt->typestrs_[n];

    typestr_cleanup(ts);
    free(ts);
  }

  if (tt->typestrs_) free(tt->typestrs_);
}

static void typestr_init(struct typestr *ts) {
  ts->hash_chain_ = NULL;
  ts->hash_ = 0;
  snippet_init(&ts->typestr_snippet_);
  ts->ordinal_ = 0;
}

static void typestr_cleanup(struct typestr *ts) {
  snippet_cleanup(&ts->typestr_snippet_);
}

struct typestr *typestr_find_or_add(struct typestr_table *tt, const struct snippet *typestr_snippet, int *is_new) {
  int r;
  uint64_t hash_value = snippet_hash(typestr_snippet);
  int idx = (int)(hash_value % TYPESTR_TABLE_SIZE);
  if (!typestr_snippet->num_tokens_) {
    printf("Empty typestr attempted\n");
  }
  struct typestr *ts, *last;
  ts = last = tt->hash_table_[idx];
  if (ts) {
    do {
      ts = ts->hash_chain_;

      if (hash_value == ts->hash_) {
        if (!snippet_cmp(typestr_snippet, &ts->typestr_snippet_)) {
          *is_new = 0;
          return ts;
        }
      }

    } while (ts != last);
  }

  /* Add new */
  if (tt->num_typestrs_ == tt->num_typestrs_allocated_) {
    size_t new_num = tt->num_typestrs_allocated_ * 2 + 1;
    if (new_num <= tt->num_typestrs_allocated_) {
      LOGERROR("Error: overflow\n");
      return NULL;
    }
    if (new_num > (SIZE_MAX / sizeof(struct typestr *))) {
      LOGERROR("Error: overflow\n");
      return NULL;
    }
    size_t alloc_size = new_num * sizeof(struct typestr *);
    void *p = realloc(tt->typestrs_, alloc_size);
    if (!p) {
      LOGERROR("Error: no memory\n");
      return NULL;
    }
    tt->typestrs_ = (struct typestr **)p;
    tt->num_typestrs_allocated_ = new_num;
  }

  ts = (struct typestr *)malloc(sizeof(struct typestr));
  if (!ts) {
    return NULL;
  }
  typestr_init(ts);
  tt->typestrs_[tt->num_typestrs_] = ts;
  ts->ordinal_ = tt->num_typestrs_++;
  
  if (tt->hash_table_[idx]) {
    ts->hash_chain_ = tt->hash_table_[idx]->hash_chain_;
    tt->hash_table_[idx]->hash_chain_ = ts;
  }
  else {
    ts->hash_chain_ = ts;
  }
  tt->hash_table_[idx] = ts;
  ts->hash_ = hash_value;
  r = snippet_append_snippet(&ts->typestr_snippet_, typestr_snippet);
  if (r) {
    typestr_cleanup(ts);
    free(ts);
    return NULL;
  }
  *is_new = 1;
  return ts;
}
