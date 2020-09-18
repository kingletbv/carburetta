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

#ifndef KLT_LOGGER_H_INCLUDED
#define KLT_LOGGER_H_INCLUDED
#define KLT_LOG_MODULE "snippet"
#include "klt_logger.h"
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
