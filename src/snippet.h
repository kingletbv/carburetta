#ifndef SNIPPET_H
#define SNIPPET_H

#ifndef STDINT_H_INCLUDED
#define STDINT_H_INCLUDED
#include <stdint.h>
#endif

#ifndef XLTS_H_INCLUDED
#define XLTS_H_INCLUDED
#include "xlts.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct snippet_token {
  uintptr_t match_;
  uintptr_t variant_;
  struct xlts text_;
};

struct snippet {
  /* Tokens that make up the snippet of code */
  size_t num_tokens_;
  size_t num_tokens_allocated_;
  struct snippet_token *tokens_;
};

void snippet_init(struct snippet *s);
void snippet_cleanup(struct snippet *s);
void snippet_clear(struct snippet *s);
int snippet_append(struct snippet *s, uintptr_t match, uintptr_t variant, struct xlts *token);
int snippet_append_tkr(struct snippet *s, struct tkr_tokenizer *tkr);
int snippet_append_snippet(struct snippet *d, const struct snippet *s);
void snippet_pop_last_token(struct snippet *s);
uint64_t snippet_hash(const struct snippet *s);
int snippet_cmp(const struct snippet *left, const struct snippet *right);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* SNIPPET_H */
