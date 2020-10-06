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

#ifndef STRING_H_INCLUDED
#define STRING_H_INCLUDED
#include <string.h>
#endif

#ifndef ERRNO_H_INCLUDED
#define ERNNO_H_INCLUDED
#include <errno.h>
#endif

#ifndef ASSERT_H_INCLUDED
#define ASSERT_H_INCLUDED
#include <assert.h>
#endif

#ifndef TOKENIZER_H_INCLUDED
#define TOKENIZER_H_INCLUDED
#include "tokenizer.h"
#endif

#ifndef TOKENS_H_INCLUDED
#define TOKENS_H_INCLUDED
#include "tokens.h"
#endif

#ifndef REPORT_ERROR_H_INCLUDED
#define REPORT_ERROR_H_INCLUDED
#include "report_error.h"
#endif

#ifndef SNIPPET_H_INCLUDED
#define SNIPPET_H_INCLUDED
#include "snippet.h"
#endif

void snippet_init(struct snippet *s) {
  s->num_tokens_ = s->num_tokens_allocated_ = 0;
  s->tokens_ = NULL;
}

void snippet_cleanup(struct snippet *s) {
  if (s->tokens_) {
    size_t n;
    for (n = 0; n < s->num_tokens_; ++n) {
      struct snippet_token *stk = s->tokens_ + n;
      xlts_cleanup(&stk->text_);
    }
    free(s->tokens_);
  }
}

void snippet_clear(struct snippet *s) {
  size_t n;
  for (n = 0; n < s->num_tokens_; ++n) {
    struct snippet_token *stk = s->tokens_ + n;
    xlts_cleanup(&stk->text_);
  }
  s->num_tokens_ = 0;
}

static int snippet_check_reserve(struct snippet *s, struct xlts *loc) {
  if (s->num_tokens_ == s->num_tokens_allocated_) {
    size_t new_num = s->num_tokens_allocated_ * 2 + 1;
    if (new_num < 7) new_num = 7;
    if (new_num <= s->num_tokens_allocated_) {
      re_error(loc, "Internal error, overflow on allocation");
      return -1;
    }
    if (new_num > (SIZE_MAX / sizeof(struct snippet_token))) {
      re_error(loc, "Internal error, overflow on allocation");
      return -1;
    }
    size_t size_to_alloc = new_num * sizeof(struct snippet_token);
    void *p = realloc(s->tokens_, size_to_alloc);
    if (!p) {
      re_error(loc, "Internal error, no memory");
      return -1;
    }
    s->tokens_ = (struct snippet_token *)p;
    s->num_tokens_allocated_ = new_num;
  }
  return 0;
}


int snippet_append(struct snippet *s, uintptr_t match, uintptr_t variant, struct xlts *token) {
  int r;
  if (s->num_tokens_ == s->num_tokens_allocated_) {
    r = snippet_check_reserve(s, token);
    if (r) return r;
  }
  struct snippet_token *stk = s->tokens_ + s->num_tokens_;
  stk->match_ = match;
  stk->variant_ = variant;
  xlts_init(&stk->text_);
  r = xlts_append(&stk->text_, token);
  if (r) return r;
  s->num_tokens_++;
  return 0;
}

int snippet_append_tkr(struct snippet *s, struct tkr_tokenizer *tkr) {
  return snippet_append(s, tkr->best_match_action_, tkr->best_match_variant_, &tkr->xmatch_);
}

int snippet_append_snippet(struct snippet *d, const struct snippet *s) {
  size_t n;
  int r;
  size_t original_num_tokens = d->num_tokens_;
  for (n = 0; n < s->num_tokens_; ++n) {
    r = snippet_append(d, s->tokens_[n].match_, s->tokens_[n].variant_, &s->tokens_[n].text_);
    if (r) goto break_down;
  }
  return 0;

break_down:
  /* Revert partial construction before returning error */
  for (n = original_num_tokens; n < d->num_tokens_; ++n) {
    xlts_cleanup(&d->tokens_[n].text_);
  }
  d->num_tokens_ = original_num_tokens;
  return r;
}

void snippet_pop_last_token(struct snippet *s) {
  if (!s->num_tokens_) return;
  xlts_cleanup(&s->tokens_[s->num_tokens_ - 1].text_);
  s->num_tokens_--;
}

uint64_t snippet_hash(const struct snippet *s) {
  uint64_t hash_value = 0;
  size_t n;
  for (n = 0; n < s->num_tokens_; ++n) {
    struct snippet_token *st = s->tokens_ + n;
    hash_value = (hash_value << 7) | (hash_value >> ((-7) & 63));
    hash_value += st->match_;
    hash_value = (hash_value << 7) | (hash_value >> ((-7) & 63));
    hash_value += st->variant_;
    const char *p;
    for (p = st->text_.translated_; p != st->text_.translated_ + st->text_.num_translated_; ++p) {
      uint8_t c = (uint8_t)*p;
      hash_value = (hash_value << 7) | (hash_value >> ((-7) & 63));
      hash_value += c;
    }
  }
  return hash_value;
}

int snippet_cmp(const struct snippet *left, const struct snippet *right) {
  size_t n;
  size_t min_len = (left->num_tokens_ < right->num_tokens_) ? left->num_tokens_ : right->num_tokens_;
  for (n = 0; n < min_len; ++n) {
    struct snippet_token *stl = left->tokens_ + n;
    struct snippet_token *str = right->tokens_ + n;
    if (stl->match_ < str->match_) {
      return -1;
    }
    if (stl->match_ > str->match_) {
      return 1;
    }
    if (stl->variant_ < str->variant_) {
      return -1;
    }
    if (stl->variant_ > str->variant_) {
      return 1;
    }
    size_t min_len = (stl->text_.num_translated_ < str->text_.num_translated_) ? stl->text_.num_translated_ : str->text_.num_translated_;
    size_t k;
    for (k = 0; k < min_len; ++k) {
      if (stl->text_.translated_[k] < str->text_.translated_[k]) return -1;
      if (stl->text_.translated_[k] > str->text_.translated_[k]) return 1;
    }
    if (stl->text_.num_translated_ < str->text_.num_translated_) return -1;
    if (stl->text_.num_translated_ > str->text_.num_translated_) return 1;
  }
  if (left->num_tokens_ < right->num_tokens_) return -1;
  if (left->num_tokens_ > right->num_tokens_) return 1;
  return 0;
}

