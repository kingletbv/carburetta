/* Copyright 2020-2021 Kinglet B.V.
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

#ifndef SNIPPET_H
#define SNIPPET_H

#ifndef STDDEF_H_INCLUDED
#define STDDEF_H_INCLUDED
#include <stddef.h> /* size_t */
#endif

#ifndef STDINT_H_INCLUDED
#define STDINT_H_INCLUDED
#include <stdint.h>
#endif

#ifndef XLTS_H_INCLUDED
#define XLTS_H_INCLUDED
#include "xlts.h"
#endif

#ifndef TOKENIZER_H_INCLUDED
#define TOKENIZER_H_INCLUDED
#include "tokenizer.h"
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
void snippet_pop_first_token(struct snippet *s);
uint64_t snippet_hash(const struct snippet *s);
int snippet_cmp(const struct snippet *left, const struct snippet *right);
char *snippet_dup_xlt(const struct snippet *s);
int snippet_append_to_xlts(struct xlts *x, const struct snippet *s);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* SNIPPET_H */
