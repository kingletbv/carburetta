/* Copyright 2020-2024 Kinglet B.V.
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

#ifndef XLTS_H_INCLUDED
#define XLTS_H_INCLUDED
#include "xlts.h"
#endif

#ifndef TOKENS_H_INCLUDED
#define TOKENS_H_INCLUDED
#include "tokens.h"
#endif

#ifndef REPORT_ERROR_H_INCLUDED
#define REPORT_ERROR_H_INCLUDED
#include "report_error.h"
#endif

#ifndef PRD_GRAM_H_INCLUDED
#define PRD_GRAM_H_INCLUDED
#include "prd_gram.h"
#endif

#ifndef SYMBOL_H_INCLUDED
#define SYMBOL_H_INCLUDED
#include "symbol.h"
#endif

#ifndef REGEX_GRAMMAR_H_INCLUDED
#define REGEX_GRAMMAR_H_INCLUDED
#include "regex_grammar.h"
#endif

#ifndef SNIPPET_H_INCLUDED
#define SNIPPET_H_INCLUDED
#include "snippet.h"
#endif




/* pattern type holds the index of the pattern in g->patterns_ */





struct regex_mode_node {
  struct regex_mode_node *chain_;
  struct xlts name_;
};

struct regex_mode_list {
  struct regex_mode_node *modes_;
};

void regex_mode_list_init(struct regex_mode_list *rml) {
  if (!rml) return;
  rml->modes_ = NULL;
}

void regex_mode_list_cleanup(struct regex_mode_list *rml) {
  struct regex_mode_node *m = rml->modes_;
  struct regex_mode_node *next;
  if (m) {
    next = m->chain_;
    do {
      m = next;
      next = m->chain_;

      xlts_cleanup(&m->name_);

      free(m);
    } while (m != rml->modes_);
    rml->modes_ = NULL;
  }
}

int regex_mode_list_append_mode(struct regex_mode_list *rml, const struct snippet *mode_name) {
  struct regex_mode_node *m = (struct regex_mode_node *)malloc(sizeof(struct regex_mode_node));
  if (!m) {
    return PRD_INTERNAL_ERROR;
  }
  xlts_init(&m->name_);
  int r;
  r = snippet_append_to_xlts(&m->name_, mode_name);
  if (r) return PRD_INTERNAL_ERROR;
  if (rml->modes_) {
    m->chain_ = rml->modes_->chain_;
    rml->modes_->chain_ = m;
  }
  else {
    m->chain_ = m;
  }
  rml->modes_ = m;

  return PRD_SUCCESS;
}

void regex_mode_list_move(struct regex_mode_list *dst, struct regex_mode_list *src) {
  regex_mode_list_cleanup(dst);
  dst->modes_ = src->modes_;
  src->modes_ = NULL;
}

/* --------- START OF GENERATED CODE ------------ */
#include <stdlib.h> /* realloc(), free(), NULL, size_t */
#include <string.h> /* memcpy() */
#include <stddef.h> /* size_t */
#include <stdint.h> /* SIZE_MAX */
struct rxg_sym_data {
  int state_;
  struct snippet common_;
  union {
    size_t uv0_;
    struct regex_mode_list uv2_;
    struct { size_t start_ , end_ ; } uv3_;
  } v_;
};
static const int rxg_minimum_sym = 3;
static const size_t rxg_num_columns = 48;
static const size_t rxg_num_rows = 111;
static const size_t rxg_num_productions = 70;
static const int rxg_parse_table[] = {
    0, -2,  0,  0,  0,  0, -2,  0,  0,  0,  0, -2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -2,  0,  0,  0, 0, -2, 0, 0, 0, 0, 0, 0,110, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
    0, -3,  0,  0,  0,  0, -3,  0,  0,  0,  0, -3,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -3,  0,  0,  0, 0, -3, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
    0, -4,  0,  0,  0,  0, -4,  0,  0,  0,  0, -4,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -4,  0,  0,  0, 0, -4, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
    0, -5,  0,  0,  0,  0, -5,  0,  0,  0,  0, -5,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -5,  0,  0,  0, 0, -5, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
    0, -9,  0,  0,  0,  0, -9,  0,  0,  0,  0, -9,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -9,  0,  0,  0, 0, -9, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
    0,-10,  0,  0,  0,  0,-10,  0,  0,  0,  0,-10,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,-10,  0,  0,  0, 0,-10, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
    0,-12,  0,  0,  0,-12,-12,  0,  0,  0,  0,-12,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
    0, 23,  0,  0,  0,  5,-66,  0,  0,  0,  0,101,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0,  0, 6, 0, 0,67, 0, 0, 0, 0,  0,  0, 0, 0, 0,
    0,-11,  0,  0,  0,-11,-11,  0,  0,  0,  0,-11,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
    0,-13,  0,  0,  0,-13,-13,  0,  0,  0,  0,-13,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,-13,  0,  0,  0, 0,-13, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
    0,-14,  0,  0,  0,-14,-14,  0,  0,  0,  0,-14,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,-14,  0,  0,  0, 0,-14, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
    0,  0,  0,  0,  0, 10,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
   70, 72, 92,  0, 95,-69, 73, 74, 75, 76, 77, 81, 82, 83, 84, 85, 86, 87, 88, 89,  0,  0, 79, 80, 78, 71, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0,11, 0,  0,  0, 0, 0, 0,
  -43,-43,-43,  0,-43,-43,-43,-43,-43,-43,-43,-43,-43,-43,-43,-43,-43,-43,-43,-43,  0,  0,-43,-43,-43,-43, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0,12, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
    0,  0,  0,  0, 13,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
    0,-15,  0,  0,  0,-15,-15,  0,  0,  0,  0,-15,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,-15,  0,  0,  0, 0,-15, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
    0,  0,  0,  0,  0,  0,  0, 15,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
    0,  0,  0,  0,  0,  0,  0,-69,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0,16, 0,  0,  0, 0, 0, 0,
    0,  0,  0,  0,-70,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0,17, 0, 0, 0, 0,14,  0,  0, 0, 0, 0,
    0,  0,  0,  0,-68,  0,  0,  9,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0,18, 0, 0,  0,  0, 0, 0, 0,
    0,  0,  0,  0,-67,  0,  0,-67,  0,  0,  0,  0,  0,  0, 48,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0,19, 0, 0, 0,  0,  0, 0, 0, 0,
  -22,  0,-22,  0,-22,  0,  0,-22,-22,  0,-22,-22,-22,  0,-22,  0,  0,  0,  0,  0,-22,-22,  0,  0,  0,-22, 0,  0,20,65, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
    0,  0,  0,  0,  0,  0, 21,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
    0,  0,  0,  0,  0,  0,-66,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0,22, 0, 0, 0, 0,  0,  0, 0, 0, 0,
    0,-16,  0,  0,  0,-16,-16,  0,  0,  0,  0,-16,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,-16,  0,  0,  0, 0,-16, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
    0,-17,  0,  0,  0,-17,-17,  0,  0,  0,  0,-17,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,-17,  0,  0,  0, 0,-17, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
    0,  0,  0,  0,  0, 25,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
   70, 72, 92,  0, 95,-69, 73, 74, 75, 76, 77, 81, 82, 83, 84, 85, 86, 87, 88, 89,  0,  0, 79, 80, 78, 71, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0,26, 0,  0,  0, 0, 0, 0,
  -43,-43,-43,  0,-43,-43,-43,-43,-43,-43,-43,-43,-43,-43,-43,-43,-43,-43,-43,-43,  0,  0,-43,-43,-43,-43, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0,27, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
    0,  0,  0,  0, 28,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
    0,-18,  0,  0,  0,-18,-18,  0,  0,  0,  0,-18,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,-18,  0,  0,  0, 0,-18, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
    0,  0,  0,  0,  0,  0,  0, 30,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
    0,  0,  0,  0,  0,  0,  0,-69,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0,31, 0,  0,  0, 0, 0, 0,
    0,  0,  0,  0,-70,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0,32, 0, 0, 0, 0,29,  0,  0, 0, 0, 0,
    0,  0,  0,  0,-68,  0,  0, 24,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0,33, 0, 0,  0,  0, 0, 0, 0,
    0,  0,  0,  0,-67,  0,  0,-67,  0,  0,  0,  0,  0,  0, 48,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0,34, 0, 0, 0,  0,  0, 0, 0, 0,
  -23,  0,-23,-23,-23,  0,  0,-23,-23,  0,-23,-23,-23,  0,-23,  0,  0,  0,  0,  0,-23,-23,  0,  0,  0,-23, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
  -25,  0,-25,-25,-25,  0,  0,-25,-25,  0,-25,-25,-25,  0,-25,  0,  0,  0,  0,  0,-25,-25,  0,  0,  0,-25, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
  -26,  0,-26,-26,-26,  0,  0,-26,-26,  0,-26,-26,-26,  0,-26,  0,  0,  0,  0,  0,-26,-26,  0,  0,  0,-26, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
  -27,  0,-27,-27,-27,  0,  0,-27,-27,  0,-27,-27,-27,  0,-27,  0,  0,  0,  0,  0,-27,-27,  0,  0,  0,-27, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
  -24,  0,-24,-24,-24,  0,  0,-24,-24,  0,-24,-24,-24,  0,-24, 37, 38, 39,  0,  0,-24,-24,  0,  0,  0,-24, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
  -28,  0,-28,-28,-28,  0,  0,-28,-28,  0,-28,-28,-28,  0,-28,-28,-28,-28,  0,  0,-28,-28,  0,  0,  0,-28, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
  -29,  0,-29,-29,-29,  0,  0,-29,-29,  0,-29,-29,-29,  0,-29,-29,-29,-29,  0,  0,-29,-29,  0,  0,  0,-29, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
  -30,  0,-30,-30,-30,  0,  0,-30,-30,  0,-30,-30,-30,  0,-30,-30,-30,-30,  0,  0,-30,-30,  0,  0,  0,-30, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
  -31,  0,-31,-31,-31,  0,  0,-31,-31,  0,-31,-31,-31,  0,-31,-31,-31,-31,  0,  0,-31,-31,  0,  0,  0,-31, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
  -32,  0,-32,-32,-32,  0,  0,-32,-32,  0,-32,-32,-32,  0,-32,-32,-32,-32,  0,  0,-32,-32,  0,  0,  0,-32, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
  -33,  0,-33,-33,-33,  0,  0,-33,-33,  0,-33,-33,-33,  0,-33,-33,-33,-33,  0,  0,-33,-33,  0,  0,  0,-33, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
   41,  0, 51,-21,-21,  0,  0,-21, 63,  0, 42, 44, 43,  0,-21,  0,  0,  0,  0,  0, 45, 46,  0,  0,  0, 64, 0,  0, 0, 0,36,40, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
  -22,  0,-22,-22,-22,  0,  0,-22,-22,  0,-22,-22,-22,  0,-22,  0,  0,  0,  0,  0,-22,-22,  0,  0,  0,-22, 0,  0, 0,47, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
  -34,  0,-34,-34,-34,  0,  0,-34,-34,  0,-34,-34,-34,  0,-34,-34,-34,-34,  0,  0,-34,-34,  0,  0,  0,-34, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
    0,  0,  0, 49,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 48,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
  -22,  0,-22,-22,  0,  0,  0,  0,-22,  0,-22,-22,-22,  0,-22,  0,  0,  0,  0,  0,-22,-22,  0,  0,  0,-22, 0,  0,50,65, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
  -35,  0,-35,-35,-35,  0,  0,-35,-35,  0,-35,-35,-35,  0,-35,-35,-35,-35,  0,  0,-35,-35,  0,  0,  0,-35, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
   60,  0,  0,  0,  0,  0,  0,  0,  0, 52,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 61, 0,  0, 0, 0, 0, 0, 0,55,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
  -36,  0,-36,-36,-36,  0,  0,-36,-36,  0,-36,-36,-36,  0,-36,-36,-36,-36,  0,  0,-36,-36,  0,  0,  0,-36, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
  -38,  0,  0,  0,  0,  0,  0,  0,  0,-38,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,-38, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
   60,  0,  0,  0,  0,  0,  0,  0,  0, 54,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 61, 0,  0, 0, 0, 0, 0, 0,55,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
  -39,  0,  0,  0,  0,  0,  0,  0,  0,-39,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,-39, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
  -42,  0,  0,  0,  0,  0,  0,  0,  0,-42,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,-42, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
   58,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
  -40,  0,  0,  0,  0,  0,  0,  0,  0,-40,  0,  0,  0, 59,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,-40, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
  -41,  0,  0,  0,  0,  0,  0,  0,  0,-41,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,-41, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
   60,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 61, 0,  0, 0, 0, 0, 0,56,57,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
   60,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 62,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 61, 0,  0, 0, 0, 0, 0,53,57,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
  -37,  0,-37,-37,-37,  0,  0,-37,-37,  0,-37,-37,-37,  0,-37,-37,-37,-37,  0,  0,-37,-37,  0,  0,  0,-37, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
   41,  0, 51,-20,-20,  0,  0,-20, 63,  0, 42, 44, 43,  0,-20,  0,  0,  0,  0,  0, 45, 46,  0,  0,  0, 64, 0,  0, 0, 0,36,40, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
  -22,  0,-22,  0,-22,  0,  0,-22,-22,  0,-22,-22,-22,  0,-22,  0,  0,  0,  0,  0,-22,-22,  0,  0,  0,-22, 0,  0,35,65, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
    0,  0,  0,  0,  0,  0, 66,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
    0,-19,  0,  0,  0,-19,-19,  0,  0,  0,  0,-19,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,-19,  0,  0,  0, 0,-19, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
    0,  0,  0,  0,  0, 68,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
  -44,-44,-44,-44,-44,-44,-44,-44,-44,-44,-44,-44,-44,-44,-44,-44,-44,-44,-44,-44,  0,  0,-44,-44,-44,-44, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
  -45,-45,-45,-45,-45,-45,-45,-45,-45,-45,-45,-45,-45,-45,-45,-45,-45,-45,-45,-45,  0,  0,-45,-45,-45,-45, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
  -46,-46,-46,-46,-46,-46,-46,-46,-46,-46,-46,-46,-46,-46,-46,-46,-46,-46,-46,-46,  0,  0,-46,-46,-46,-46, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
  -47,-47,-47,-47,-47,-47,-47,-47,-47,-47,-47,-47,-47,-47,-47,-47,-47,-47,-47,-47,  0,  0,-47,-47,-47,-47, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
  -48,-48,-48,-48,-48,-48,-48,-48,-48,-48,-48,-48,-48,-48,-48,-48,-48,-48,-48,-48,  0,  0,-48,-48,-48,-48, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
  -49,-49,-49,-49,-49,-49,-49,-49,-49,-49,-49,-49,-49,-49,-49,-49,-49,-49,-49,-49,  0,  0,-49,-49,-49,-49, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
  -50,-50,-50,-50,-50,-50,-50,-50,-50,-50,-50,-50,-50,-50,-50,-50,-50,-50,-50,-50,  0,  0,-50,-50,-50,-50, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
  -51,-51,-51,-51,-51,-51,-51,-51,-51,-51,-51,-51,-51,-51,-51,-51,-51,-51,-51,-51,  0,  0,-51,-51,-51,-51, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
  -52,-52,-52,-52,-52,-52,-52,-52,-52,-52,-52,-52,-52,-52,-52,-52,-52,-52,-52,-52,  0,  0,-52,-52,-52,-52, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
  -53,-53,-53,-53,-53,-53,-53,-53,-53,-53,-53,-53,-53,-53,-53,-53,-53,-53,-53,-53,  0,  0,-53,-53,-53,-53, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
  -54,-54,-54,-54,-54,-54,-54,-54,-54,-54,-54,-54,-54,-54,-54,-54,-54,-54,-54,-54,  0,  0,-54,-54,-54,-54, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
  -55,-55,-55,-55,-55,-55,-55,-55,-55,-55,-55,-55,-55,-55,-55,-55,-55,-55,-55,-55,  0,  0,-55,-55,-55,-55, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
  -56,-56,-56,-56,-56,-56,-56,-56,-56,-56,-56,-56,-56,-56,-56,-56,-56,-56,-56,-56,  0,  0,-56,-56,-56,-56, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
  -57,-57,-57,-57,-57,-57,-57,-57,-57,-57,-57,-57,-57,-57,-57,-57,-57,-57,-57,-57,  0,  0,-57,-57,-57,-57, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
  -58,-58,-58,-58,-58,-58,-58,-58,-58,-58,-58,-58,-58,-58,-58,-58,-58,-58,-58,-58,  0,  0,-58,-58,-58,-58, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
  -59,-59,-59,-59,-59,-59,-59,-59,-59,-59,-59,-59,-59,-59,-59,-59,-59,-59,-59,-59,  0,  0,-59,-59,-59,-59, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
  -60,-60,-60,-60,-60,-60,-60,-60,-60,-60,-60,-60,-60,-60,-60,-60,-60,-60,-60,-60,  0,  0,-60,-60,-60,-60, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
  -61,-61,-61,-61,-61,-61,-61,-61,-61,-61,-61,-61,-61,-61,-61,-61,-61,-61,-61,-61,  0,  0,-61,-61,-61,-61, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
  -62,-62,-62,-62,-62,-62,-62,-62,-62,-62,-62,-62,-62,-62,-62,-62,-62,-62,-62,-62,  0,  0,-62,-62,-62,-62, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
  -63,-63,-63,-63,-63,-63,-63,-63,-63,-63,-63,-63,-63,-63,-63,-63,-63,-63,-63,-63,  0,  0,-63,-63,-63,-63, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
  -64,-64,-64,-64,-64,-64,-64,-64,-64,-64,-64,-64,-64,-64,-64,-64,-64,-64,-64,-64,  0,  0,-64,-64,-64,-64, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
   70, 72, 92, 90, 95,  0, 73, 74, 75, 76, 77, 81, 82, 83, 84, 85, 86, 87, 88, 89,  0,  0, 79, 80, 78, 71, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
  -43,-43,-43,-43,-43,  0,-43,-43,-43,-43,-43,-43,-43,-43,-43,-43,-43,-43,-43,-43,  0,  0,-43,-43,-43,-43, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0,91, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
  -65,-65,-65,-65,-65,-65,-65,-65,-65,-65,-65,-65,-65,-65,-65,-65,-65,-65,-65,-65,  0,  0,-65,-65,-65,-65, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
   70, 72, 92,  0, 95, 93, 73, 74, 75, 76, 77, 81, 82, 83, 84, 85, 86, 87, 88, 89,  0,  0, 79, 80, 78, 71, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
  -43,-43,-43,  0,-43,-43,-43,-43,-43,-43,-43,-43,-43,-43,-43,-43,-43,-43,-43,-43,  0,  0,-43,-43,-43,-43, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0,94, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
   70, 72, 92,  0, 95,-69, 73, 74, 75, 76, 77, 81, 82, 83, 84, 85, 86, 87, 88, 89,  0,  0, 79, 80, 78, 71, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0,69, 0,  0,  0, 0, 0, 0,
  -43,-43,-43,  0,-43,-43,-43,-43,-43,-43,-43,-43,-43,-43,-43,-43,-43,-43,-43,-43,  0,  0,-43,-43,-43,-43, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0,96, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
    0,  0,  0,  0, 97,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
    0,  0,  0,  0,-70,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,98,  0,  0, 0, 0, 0,
    0,  0,  0,  0,-68,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0,99, 0, 0,  0,  0, 0, 0, 0,
    0,  0,  0,  0,  0,  0,100,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
    0, 23,  0,  0,  0,  4,-66,  0,  0,  0,  0,101,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0,  0, 8, 0, 0,67, 0, 0, 0, 0,  0,  0, 0, 7, 0,
    0, 23,  0,  0,102,  0,-66,  0,  0,  0,  0,101,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0,  0, 3, 0, 0,67, 0, 0, 0, 0,  0,  0, 0, 0, 0,
    0, -6,  0,  0, -6,  0, -6,  0,  0,  0,  0, -6,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -8, -8,  0, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
    0,105,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,104,106,  0, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, -7, -7,  0, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0,
    0,108,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,  0,107, 0, 0, 0,
    0, 23,  0,  0,  0,  0,-66,  0,  0,  0,  0,101,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,109,  0,  0,  0, 0, -1, 0, 0, 0, 0, 0, 0,  0, 1, 0, 0,67, 0, 0, 0, 0,103,  0, 2, 0, 0,
};
static const size_t rxg_production_lengths[] = {
  1,
  0,
  2,
  2,
  3,
  3,
  1,
  3,
  3,
  4,
  1,
  2,
  6,
  11,
  9,
  5,
  10,
  8,
  8,
  1,
  3,
  0,
  2,
  1,
  2,
  2,
  2,
  1,
  1,
  1,
  1,
  1,
  1,
  3,
  3,
  4,
  1,
  2,
  1,
  1,
  1,
  3,
  0,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  4,
  4,
  0,
  0,
  0,
  0,
  0
};
static const int rxg_production_syms[] = {
  50,
  37,
  37,
  37,
  37,
  46,
  47,
  47,
  48,
  48,
  49,
  49,
  38,
  38,
  38,
  38,
  38,
  38,
  38,
  31,
  31,
  32,
  32,
  33,
  33,
  33,
  33,
  34,
  34,
  34,
  34,
  34,
  34,
  34,
  34,
  34,
  34,
  35,
  35,
  36,
  36,
  36,
  39,
  39,
  39,
  39,
  39,
  39,
  39,
  39,
  39,
  39,
  39,
  39,
  39,
  39,
  39,
  39,
  39,
  39,
  39,
  39,
  39,
  39,
  39,
  41,
  42,
  43,
  44,
  45
};

#ifndef CARB_RXG_REGEX_GRAMMAR_H_INCLUDED
struct rxg_stack {
  int error_recovery_:1;
  int pending_reset_:1;
  int discard_remaining_actions_:1;
  int slot_1_has_sym_data_:1;
  int slot_1_has_common_data_:1;
  int top_of_stack_has_sym_data_:1;
  int top_of_stack_has_common_data_:1;
  int slot_1_sym_;
  int continue_at_;
  int mute_error_turns_;
  size_t pos_, num_stack_allocated_;
  struct rxg_sym_data *stack_;
  struct rxg_sym_data *sym_data_;
  size_t current_production_length_;
  int current_production_nonterminal_;
};
#define _RXG_FINISH 0
#define _RXG_MATCH 1
#define _RXG_OVERFLOW 2
#define _RXG_NO_MEMORY 3
#define _RXG_FEED_ME 4
#define _RXG_END_OF_INPUT 5
#define _RXG_SYNTAX_ERROR 6
#define _RXG_LEXICAL_ERROR 7
#define _RXG_INTERNAL_ERROR 8

#define RXG_CHAR 3
#define RXG_IDENT 4
#define RXG_PAR_OPEN 5
#define RXG_PAR_CLOSE 6
#define RXG_CUR_OPEN 7
#define RXG_CUR_CLOSE 8
#define RXG_COLON 9
#define RXG_SEMICOLON 10
#define RXG_SQ_OPEN 11
#define RXG_SQ_CLOSE 12
#define RXG_DOT 13
#define RXG_DOLLAR 14
#define RXG_CARET 15
#define RXG_DASH 16
#define RXG_BAR 17
#define RXG_ASTERISK 18
#define RXG_PLUS 19
#define RXG_QUESTION_MARK 20
#define RXG_TOKEN 21
#define RXG_WHITESPACE 22
#define RXG_START_OF_INPUT_ESC 23
#define RXG_END_OF_INPUT_ESC 24
#define RXG_LESS_THAN 25
#define RXG_GREATER_THAN 26
#define RXG_COMMA 27
#define RXG_CATEGORY 28
#define RXG_ERROR 29
#define RXG_INPUT_END 30

#define RXG_EXP 31
#define RXG_TERM 32
#define RXG_ELM 33
#define RXG_SYM 34
#define RXG_RANGE 35
#define RXG_RANGE_ELM 36
#define RXG_GRAMMAR 37
#define RXG_PATTERN 38
#define RXG_ACTION_SEQUENCE 39
#define RXG_STMT_ACTION 40
#define RXG_START_REGEX 41
#define RXG_END_REGEX 42
#define RXG_START_C_TOKENIZER 43
#define RXG_END_C_TOKENIZER 44
#define RXG_ACCEPT_WHITESPACE 45
#define RXG_MODE 46
#define RXG_MODE_LIST 47
#define RXG_MODE_GROUP 48
#define RXG_PATTERN_LIST 49


#endif /* CARB_RXG_REGEX_GRAMMAR_H_INCLUDED */

void rxg_stack_init(struct rxg_stack *stack) {
  stack->error_recovery_ = 0;
  stack->pending_reset_ = 1;
  stack->discard_remaining_actions_ = 0;
  stack->slot_1_has_sym_data_ = stack->slot_1_has_common_data_ = 0;
  stack->slot_1_sym_ = 0;
  stack->top_of_stack_has_sym_data_ = stack->top_of_stack_has_common_data_ = 0;
  stack->continue_at_ = 0;
  stack->mute_error_turns_ = 0;
  stack->pos_ = 0;
  stack->num_stack_allocated_ = 0;
  stack->stack_ = NULL;
  stack->sym_data_ = NULL;
  stack->current_production_length_ = 0;
  stack->current_production_nonterminal_ = 0;
}

void rxg_stack_cleanup(struct rxg_stack *stack) {
  size_t n;
  for (n = 2; n < stack->pos_; ++n) {
    if ((n != (stack->pos_ - 1)) || stack->top_of_stack_has_sym_data_) {
      switch (stack->stack_[n].state_) {
        case 103: /* mode */
        case 107: /* mode-list */
        {
          {
             regex_mode_list_cleanup(&((stack->stack_ + n)->v_.uv2_));
          }
          
        }
        break;
      } /* switch */
    }
    if ((n != (stack->pos_ - 1)) || stack->top_of_stack_has_common_data_) {
      {
         snippet_cleanup(&((stack->stack_ + n)->common_));
      }
      
    }
  }
  if (stack->slot_1_has_sym_data_) {
    switch (stack->slot_1_sym_) {
      case 38:
      break;
      case 46:
      case 47:
      {
        {
           regex_mode_list_cleanup(&(stack->stack_[1].v_.uv2_));
        }
        
      }
      break;
      case 49:
      break;
    } /* switch */
  }
  if (stack->slot_1_has_common_data_) {
    {
       snippet_cleanup(&(stack->stack_[1].common_));
    }
    
  }
  
  if (stack->stack_) free(stack->stack_);
}

static int rxg_push_state(struct rxg_stack *stack, int state) {
  if (stack->num_stack_allocated_ == stack->pos_) {
    size_t new_num_allocated = 0;
    if (stack->num_stack_allocated_) {
      new_num_allocated = stack->num_stack_allocated_ * 2;
      if (new_num_allocated <= stack->num_stack_allocated_) {
        /* Overflow in allocation */
        return _RXG_OVERFLOW;
      }
    }
    else {
      new_num_allocated = 16;
    }
    
    if (new_num_allocated > (SIZE_MAX / sizeof(struct rxg_sym_data))) {
      /* Overflow in allocation */
      return _RXG_OVERFLOW;
    }
    
    void *p = realloc(stack->stack_, new_num_allocated * sizeof(struct rxg_sym_data));
    if (!p) {
      /* Out of memory */
      return _RXG_NO_MEMORY;
    }
    stack->stack_ = (struct rxg_sym_data *)p;
    stack->num_stack_allocated_ = new_num_allocated;
  }
  stack->stack_[stack->pos_++].state_ = state;
  return 0;
}

int rxg_stack_reset(struct rxg_stack *stack) {
  size_t n;
  stack->pending_reset_ = 0;
  stack->discard_remaining_actions_ = 0;
  for (n = 2; n < stack->pos_; ++n) {
    if ((n != (stack->pos_ - 1)) || stack->top_of_stack_has_sym_data_) {
      switch (stack->stack_[n].state_) {
        case 103: /* mode */
        case 107: /* mode-list */
        {
          {
             regex_mode_list_cleanup(&((stack->stack_ + n)->v_.uv2_));
          }
          
        }
        break;
      } /* switch */
    }
    if ((n != (stack->pos_ - 1)) || stack->top_of_stack_has_common_data_) {
      {
         snippet_cleanup(&((stack->stack_ + n)->common_));
      }
      
    }
  }
  if (stack->slot_1_has_sym_data_) {
    switch (stack->slot_1_sym_) {
      case 38:
      break;
      case 46:
      case 47:
      {
        {
           regex_mode_list_cleanup(&(stack->stack_[1].v_.uv2_));
        }
        
      }
      break;
      case 49:
      break;
    } /* switch */
  }
  if (stack->slot_1_has_common_data_) {
    {
       snippet_cleanup(&(stack->stack_[1].common_));
    }
    
  }
  stack->slot_1_has_sym_data_ = stack->slot_1_has_common_data_ = 0;
  stack->slot_1_sym_ = 0;
  stack->top_of_stack_has_sym_data_ = 0;
  stack->top_of_stack_has_common_data_ = 0;
  stack->sym_data_ = NULL;
  stack->current_production_length_ = 0;
  stack->current_production_nonterminal_ = 0;
  stack->pos_ = 0;
  stack->error_recovery_ = 0;
  stack->continue_at_ = 0;
  stack->mute_error_turns_ = 0;
  /* reserve slot [0] for the token sym data */
  switch (rxg_push_state(stack, 0)) {
    case _RXG_OVERFLOW:{
      return _RXG_OVERFLOW;
    }
    break;
    case _RXG_NO_MEMORY: {
      return _RXG_NO_MEMORY;
    }
    break;
  }
  /* reserve slot [1] for the initial state 0, corresponding to the synthetic S production
  ** sym_data in slot [1] is used for temporary nonterminal data storage */
  switch (rxg_push_state(stack, 0)) {
    case _RXG_OVERFLOW:{
      return _RXG_OVERFLOW;
    }
    break;
    case _RXG_NO_MEMORY: {
      return _RXG_NO_MEMORY;
    }
    break;
  }
  return 0;
}

int rxg_stack_can_recover(struct rxg_stack *stack) {
  return !!stack->error_recovery_;
}

int rxg_stack_accepts(struct rxg_stack *stack, int sym) {
  if (!stack->pos_) return 0;
  return 0 != rxg_parse_table[rxg_num_columns * stack->stack_[stack->pos_ - 1].state_ + (sym - rxg_minimum_sym)];}

int rxg_parse(struct rxg_stack *stack, int sym, struct prd_grammar *g, struct tkr_tokenizer *tkr, struct symbol_table *st, int char_value) {
  if (stack->pending_reset_) {
    int r;
    r = rxg_stack_reset(stack);
    if (r) return r;
  }
    switch (stack->continue_at_) {
    case 0: break; /* fall through to regular code path */
    case 1: goto C1;
    case 2: goto C2;
    case 3: goto C3;
    case 4: goto C4;
    case 5: goto C5;
    case 6: goto C6;
    case 7: goto C7;
    case 8: goto C8;
    case 9: goto C9;
    case 10: goto C10;
    case 11: goto C11;
    case 12: goto C12;
    case 13: goto C13;
    case 14: goto C14;
    case 15: goto C15;
    case 16: goto C16;
    case 17: goto C17;
    case 18: goto C18;
    case 19: goto C19;
    case 20: goto C20;
    case 21: goto C21;
    case 22: goto C22;
    case 23: goto C23;
    case 24: goto C24;
    case 25: goto C25;
    case 26: goto C26;
    case 27: goto C27;
    case 28: goto C28;
    case 29: goto C29;
    case 30: goto C30;
    case 31: goto C31;
    case 32: goto C32;
    case 33: goto C33;
    case 34: goto C34;
    case 35: goto C35;
    case 36: goto C36;
    case 37: goto C37;
    case 38: goto C38;
    case 39: goto C39;
    case 40: goto C40;
    case 41: goto C41;
    case 42: goto C42;
    case 43: goto C43;
    case 44: goto C44;
    case 45: goto C45;
    case 46: goto C46;
    case 47: goto C47;
    case 48: goto C48;
    case 49: goto C49;
    case 50: goto C50;
    case 51: goto C51;
    case 52: goto C52;
    case 53: goto C53;
    case 54: goto C54;
    case 55: goto C55;
    case 56: goto C56;
    case 57: goto C57;
    case 58: goto C58;
    case 59: goto C59;
    case 60: goto C60;
    case 61: goto C61;
    case 62: goto C62;
    case 63: goto C63;
    case 64: goto C64;
    case 65: goto C65;
    case 66: goto C66;
    case 67: goto C67;
    case 68: goto C68;
    case 69: goto C69;
    case 70: goto C70;
    case 71: goto C71;
    case 72: goto C72;
    case 73: goto C73;
    case 74: goto C74;
    case 75: goto C75;
    case 76: goto C76;
    case 77: goto C77;
    case 78: goto C78;
    case 79: goto C79;
    case 80: goto C80;
    case 81: goto C81;
    case 82: goto C82;
    case 83: goto C83;
    case 84: goto C84;
    case 85: goto C85;
    case 86: goto C86;
    case 87: goto C87;
    case 88: goto C88;
    case 89: goto C89;
    case 90: goto C90;
    case 91: goto C91;
    case 92: goto C92;
    case 93: goto C93;
    case 94: goto C94;
    case 95: goto C95;
    case 96: goto C96;
    case 97: goto C97;
    case 98: goto C98;
    case 99: goto C99;
    case 100: goto C100;
    case 101: goto C101;
    case 102: goto C102;
    case 103: goto C103;
    case 104: goto C104;
    case 105: goto C105;
    case 106: goto C106;
    case 107: goto C107;
    case 108: goto C108;
    case 109: goto C109;
    case 110: goto C110;
    case 111: goto C111;
    case 112: goto C112;
    case 113: goto C113;
    case 114: goto C114;
    case 115: goto C115;
    case 116: goto C116;
    case 117: goto C117;
    case 118: goto C118;
    case 119: goto C119;
    case 120: goto C120;
    case 121: goto C121;
    case 122: goto C122;
    case 123: goto C123;
    case 124: goto C124;
    case 125: goto C125;
    case 126: goto C126;
    case 127: goto C127;
    case 128: goto C128;
    case 129: goto C129;
    case 130: goto C130;
    case 131: goto C131;
    case 132: goto C132;
    case 133: goto C133;
    case 134: goto C134;
    case 135: goto C135;
    case 136: goto C136;
    case 137: goto C137;
    case 138: goto C138;
    case 139: goto C139;
    case 140: goto C140;
    case 141: goto C141;
    case 142: goto C142;
    case 143: goto C143;
    case 144: goto C144;
    case 145: goto C145;
    case 146: goto C146;
    case 147: goto C147;
    case 148: goto C148;
    case 149: goto C149;
    case 150: goto C150;
    case 151: goto C151;
    case 152: goto C152;
    case 153: goto C153;
  } /* continuation switch */
if (stack->mute_error_turns_) stack->mute_error_turns_--;
  for (;;) {
    if (!stack->error_recovery_) {
      int action = rxg_parse_table[rxg_num_columns * stack->stack_[stack->pos_ - 1].state_ + (sym - rxg_minimum_sym)];
      if (action > 0) {
        switch (rxg_push_state(stack, action)) {
          case _RXG_OVERFLOW: {
            stack->continue_at_ = 0;
            {
              re_error_tkr(tkr, "Error: internal error\n"); return PRD_INTERNAL_ERROR;
              
            }
          }
          break;
          case _RXG_NO_MEMORY: {
            stack->continue_at_ = 0;
            {
              re_error_tkr(tkr, "Error: no memory"); return PRD_INTERNAL_ERROR;
              
            }
          }
          break;
        } /* switch */
        
        stack->sym_data_ = stack->stack_ + stack->pos_ - 1;
        stack->top_of_stack_has_sym_data_ = 0;
        stack->top_of_stack_has_common_data_ = 1;
        stack->continue_at_ = 1;
        {
           snippet_init(&(stack->sym_data_->common_));
        }
        C1:;
        stack->continue_at_ = 2;
        {
           snippet_append_tkr(&(stack->sym_data_->common_), tkr);
        }
        C2:;
        switch (sym) {
        } /* switch */
        stack->continue_at_ = 0;
        {
          return PRD_NEXT;
          
        }
      } /* action > 0 */
      else if (action < 0) {
        int production = -action - 1;
        stack->discard_remaining_actions_ = 0;
        stack->current_production_length_ = rxg_production_lengths[production];
        stack->current_production_nonterminal_ = rxg_production_syms[production];
        if (0 == production) {
          stack->pending_reset_ = 1;
          return PRD_SUCCESS;
        }
        
        memset(&stack->stack_[1], 0, sizeof(struct rxg_sym_data));
        { /* scope guard */
          stack->sym_data_ = stack->stack_ + stack->pos_ - stack->current_production_length_;
          int r; \
          struct prd_pattern *pat;
          switch (production) {
            /* grammar: */
            case 1: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 3;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C3:;
            }
            break;
            /* grammar: grammar pattern */
            case 2: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 4;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C4:;
            }
            break;
            /* grammar: grammar mode-group */
            case 3: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 5;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C5:;
            }
            break;
            /* grammar: grammar mode pattern */
            case 4: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 6;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C6:;
              stack->continue_at_ = 7;
              {
                {
  r = prd_grammar_check_mode_group_reserve(g);
  if (r) return r;
  struct prd_mode_group *mg = g->mode_groups_ + g->num_mode_groups_++;
  prd_mode_group_init(mg);
  struct regex_mode_node *m = (stack->sym_data_[1].v_.uv2_).modes_;
  if (m) {
    do {
      m = m->chain_;

      r = prd_mode_group_check_mode_reserve(mg, &m->name_);
      if (r) return r;
      struct prd_mode *mgm = mg->modes_ + mg->num_modes_++;
      prd_mode_init(mgm);
      xlts_append(&mgm->id_, &m->name_);
    } while (m != (stack->sym_data_[1].v_.uv2_).modes_);
  }
  if ((stack->sym_data_[2].v_.uv0_) != SIZE_MAX) {
    /* Not a common action (which looks like a pattern but is only an action attribute.) */
    mg->pattern_start_index_ = (stack->sym_data_[2].v_.uv0_);
    mg->pattern_end_index_ = (stack->sym_data_[2].v_.uv0_) + 1;
  }
}
              }
              C7:;
            }
            break;
            /* mode: LESS_THAN mode-list GREATER_THAN */
            case 5: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 8;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C8:;
              stack->slot_1_has_sym_data_ = 1;
              stack->slot_1_sym_ = RXG_MODE;
              stack->continue_at_ = 9;
              {
                 regex_mode_list_init(&(stack->stack_[1].v_.uv2_));
              }
              C9:;
              stack->continue_at_ = 10;
              {
                { regex_mode_list_move(&(stack->stack_[1].v_.uv2_), &(stack->sym_data_[1].v_.uv2_)); }
              }
              C10:;
            }
            break;
            /* mode-list: IDENT */
            case 6: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 11;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C11:;
              stack->slot_1_has_sym_data_ = 1;
              stack->slot_1_sym_ = RXG_MODE_LIST;
              stack->continue_at_ = 12;
              {
                 regex_mode_list_init(&(stack->stack_[1].v_.uv2_));
              }
              C12:;
              stack->continue_at_ = 13;
              {
                {
  regex_mode_list_append_mode(&(stack->stack_[1].v_.uv2_), &(stack->sym_data_[0].common_));
}
              }
              C13:;
            }
            break;
            /* mode-list: mode-list COMMA IDENT */
            case 7: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 14;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C14:;
              stack->slot_1_has_sym_data_ = 1;
              stack->slot_1_sym_ = RXG_MODE_LIST;
              stack->continue_at_ = 15;
              {
                 regex_mode_list_init(&(stack->stack_[1].v_.uv2_));
              }
              C15:;
              stack->continue_at_ = 16;
              {
                {
  regex_mode_list_move(&(stack->stack_[1].v_.uv2_), &(stack->sym_data_[0].v_.uv2_));
  regex_mode_list_append_mode(&(stack->stack_[1].v_.uv2_), &(stack->sym_data_[2].common_));
}
              }
              C16:;
            }
            break;
            /* mode-group: mode CUR_OPEN CUR_CLOSE */
            case 8: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 17;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C17:;
              stack->continue_at_ = 18;
              {
                {
  r = prd_grammar_check_mode_group_reserve(g);
  if (r) return r;
  struct prd_mode_group *mg = g->mode_groups_ + g->num_mode_groups_++;
  prd_mode_group_init(mg);
  struct regex_mode_node *m = (stack->sym_data_[0].v_.uv2_).modes_;
  if (m) {
    do {
      m = m->chain_;

      r = prd_mode_group_check_mode_reserve(mg, &m->name_);
      if (r) return r;
      struct prd_mode *mgm = mg->modes_ + mg->num_modes_++;
      prd_mode_init(mgm);
      xlts_append(&mgm->id_, &m->name_);
    } while (m != (stack->sym_data_[0].v_.uv2_).modes_);
  }
}
              }
              C18:;
            }
            break;
            /* mode-group: mode CUR_OPEN pattern-list CUR_CLOSE */
            case 9: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 19;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C19:;
              stack->continue_at_ = 20;
              {
                {
  r = prd_grammar_check_mode_group_reserve(g);
  if (r) return r;
  struct prd_mode_group *mg = g->mode_groups_ + g->num_mode_groups_++;
  prd_mode_group_init(mg);
  struct regex_mode_node *m = (stack->sym_data_[0].v_.uv2_).modes_;
  if (m) {
    do {
      m = m->chain_;

      r = prd_mode_group_check_mode_reserve(mg, &m->name_);
      if (r) return r;
      struct prd_mode *mgm = mg->modes_ + mg->num_modes_++;
      prd_mode_init(mgm);
      xlts_append(&mgm->id_, &m->name_);
    } while (m != (stack->sym_data_[0].v_.uv2_).modes_);
  }
  mg->pattern_start_index_ = (stack->sym_data_[2].v_.uv3_).start_;
  mg->pattern_end_index_ = (stack->sym_data_[2].v_.uv3_).end_;
}
              }
              C20:;
            }
            break;
            /* pattern-list: pattern */
            case 10: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 21;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C21:;
              stack->slot_1_has_sym_data_ = 1;
              stack->slot_1_sym_ = RXG_PATTERN_LIST;
              stack->continue_at_ = 22;
              {
                 (stack->stack_[1].v_.uv3_).start_ = (stack->stack_[1].v_.uv3_).end_ = SIZE_MAX;
              }
              C22:;
              stack->continue_at_ = 23;
              {
                { 
  if ((stack->sym_data_[0].v_.uv0_) != SIZE_MAX) {
    (stack->stack_[1].v_.uv3_).start_ = (stack->sym_data_[0].v_.uv0_); 
    (stack->stack_[1].v_.uv3_).end_ = (stack->sym_data_[0].v_.uv0_) + 1;
  }
}
              }
              C23:;
            }
            break;
            /* pattern-list: pattern-list pattern */
            case 11: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 24;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C24:;
              stack->slot_1_has_sym_data_ = 1;
              stack->slot_1_sym_ = RXG_PATTERN_LIST;
              stack->continue_at_ = 25;
              {
                 (stack->stack_[1].v_.uv3_).start_ = (stack->stack_[1].v_.uv3_).end_ = SIZE_MAX;
              }
              C25:;
              stack->continue_at_ = 26;
              {
                { 
  if ((stack->sym_data_[1].v_.uv0_) != SIZE_MAX) {
    if ((stack->sym_data_[0].v_.uv3_).start_ != (stack->sym_data_[0].v_.uv3_).end_) {
      (stack->stack_[1].v_.uv3_) = (stack->sym_data_[0].v_.uv3_);
      (stack->stack_[1].v_.uv3_).end_ = (stack->sym_data_[1].v_.uv0_) + 1;
    }
    else {
      (stack->sym_data_[0].v_.uv3_).start_ = (stack->sym_data_[1].v_.uv0_);
      (stack->sym_data_[0].v_.uv3_).end_ = (stack->sym_data_[1].v_.uv0_) + 1;
    }
  }
  else {
    (stack->stack_[1].v_.uv3_) = (stack->sym_data_[0].v_.uv3_);
  }
}
              }
              C26:;
            }
            break;
            /* pattern: IDENT start-regex COLON exp end-regex SEMICOLON */
            case 12: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 27;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C27:;
              stack->slot_1_has_sym_data_ = 1;
              stack->slot_1_sym_ = RXG_PATTERN;
              stack->continue_at_ = 28;
              {
                 (stack->stack_[1].v_.uv0_) = SIZE_MAX;
              }
              C28:;
              stack->continue_at_ = 29;
              {
                {
  r = prd_grammar_check_pattern_reserve(g);
  if (r) return r;
  char *regex = snippet_dup_xlt(&(stack->sym_data_[3].common_));
  if (!regex) {
    return PRD_INTERNAL_ERROR;
  }
  (stack->stack_[1].v_.uv0_) = g->num_patterns_;
  pat = g->patterns_ + g->num_patterns_++;
  prd_pattern_init(pat);
  pat->regex_ = regex;
  r = snippet_append_to_xlts(&pat->term_.id_, &(stack->sym_data_[0].common_));
  if (r) return r;
  r = snippet_append_snippet(&pat->common_action_sequence_, &g->current_common_pattern_action_sequence_);
  if (r) return r;
}
              }
              C29:;
            }
            break;
            /* pattern: IDENT start-regex COLON exp end-regex start-c-tokenizer accept-whitespace CUR_OPEN action-sequence end-c-tokenizer CUR_CLOSE */
            case 13: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 30;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C30:;
              stack->slot_1_has_sym_data_ = 1;
              stack->slot_1_sym_ = RXG_PATTERN;
              stack->continue_at_ = 31;
              {
                 (stack->stack_[1].v_.uv0_) = SIZE_MAX;
              }
              C31:;
              stack->continue_at_ = 32;
              {
                {
  r = prd_grammar_check_pattern_reserve(g);
  if (r) return r;
  char *regex = snippet_dup_xlt(&(stack->sym_data_[3].common_));
  if (!regex) {
    return PRD_INTERNAL_ERROR;
  }
  (stack->stack_[1].v_.uv0_) = g->num_patterns_;
  pat = g->patterns_ + g->num_patterns_++;
  prd_pattern_init(pat);
  pat->regex_ = regex;
  r = snippet_append_to_xlts(&pat->term_.id_, &(stack->sym_data_[0].common_));
  if (r) return r;
  r = snippet_append_snippet(&pat->action_sequence_, &(stack->sym_data_[8].common_));
  if (r) return r;
  r = snippet_append_snippet(&pat->common_action_sequence_, &g->current_common_pattern_action_sequence_);
  if (r) return r;
}
              }
              C32:;
            }
            break;
            /* pattern: IDENT start-regex COLON exp end-regex start-c-tokenizer stmt-action end-c-tokenizer SEMICOLON */
            case 14: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 33;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C33:;
              stack->slot_1_has_sym_data_ = 1;
              stack->slot_1_sym_ = RXG_PATTERN;
              stack->continue_at_ = 34;
              {
                 (stack->stack_[1].v_.uv0_) = SIZE_MAX;
              }
              C34:;
              stack->continue_at_ = 35;
              {
                {
  r = prd_grammar_check_pattern_reserve(g);
  if (r) return r;
  char *regex = snippet_dup_xlt(&(stack->sym_data_[3].common_));
  if (!regex) {
    return PRD_INTERNAL_ERROR;
  }
  (stack->stack_[1].v_.uv0_) = g->num_patterns_;
  pat = g->patterns_ + g->num_patterns_++;
  prd_pattern_init(pat);
  pat->regex_ = regex;
  r = snippet_append_to_xlts(&pat->term_.id_, &(stack->sym_data_[0].common_));
  if (r) return r;
  r = snippet_append_snippet(&pat->action_sequence_, &(stack->sym_data_[6].common_));
  if (r) return r;
  /* Pop the '=' token from stmt-action as it is not part of the action sequence itself. */
  if (pat->action_sequence_.num_tokens_ && (pat->action_sequence_.tokens_[0].match_ == TOK_EQUALS)) {
    snippet_pop_first_token(&pat->action_sequence_);
  }
  r = snippet_append_snippet(&pat->common_action_sequence_, &g->current_common_pattern_action_sequence_);
  if (r) return r;
}
              }
              C35:;
            }
            break;
            /* pattern: start-regex COLON exp end-regex SEMICOLON */
            case 15: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 36;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C36:;
              stack->slot_1_has_sym_data_ = 1;
              stack->slot_1_sym_ = RXG_PATTERN;
              stack->continue_at_ = 37;
              {
                 (stack->stack_[1].v_.uv0_) = SIZE_MAX;
              }
              C37:;
              stack->continue_at_ = 38;
              {
                {
  r = prd_grammar_check_pattern_reserve(g);
  if (r) return r;
  char *regex = snippet_dup_xlt(&(stack->sym_data_[2].common_));
  if (!regex) {
    return PRD_INTERNAL_ERROR;
  }
  (stack->stack_[1].v_.uv0_) = g->num_patterns_;
  pat = g->patterns_ + g->num_patterns_++;
  prd_pattern_init(pat);
  pat->regex_ = regex;
  r = snippet_append_snippet(&pat->common_action_sequence_, &g->current_common_pattern_action_sequence_);
  if (r) return r;
}
              }
              C38:;
            }
            break;
            /* pattern: start-regex COLON exp end-regex start-c-tokenizer accept-whitespace CUR_OPEN action-sequence end-c-tokenizer CUR_CLOSE */
            case 16: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 39;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C39:;
              stack->slot_1_has_sym_data_ = 1;
              stack->slot_1_sym_ = RXG_PATTERN;
              stack->continue_at_ = 40;
              {
                 (stack->stack_[1].v_.uv0_) = SIZE_MAX;
              }
              C40:;
              stack->continue_at_ = 41;
              {
                {
  r = prd_grammar_check_pattern_reserve(g);
  if (r) return r;
  char *regex = snippet_dup_xlt(&(stack->sym_data_[2].common_));
  if (!regex) {
    return PRD_INTERNAL_ERROR;
  }
  (stack->stack_[1].v_.uv0_) = g->num_patterns_;
  pat = g->patterns_ + g->num_patterns_++;
  prd_pattern_init(pat);
  pat->regex_ = regex;
  r = snippet_append_snippet(&pat->action_sequence_, &(stack->sym_data_[7].common_));
  if (r) return r;
  r = snippet_append_snippet(&pat->common_action_sequence_, &g->current_common_pattern_action_sequence_);
  if (r) return r;
}
              }
              C41:;
            }
            break;
            /* pattern: start-regex COLON exp end-regex start-c-tokenizer stmt-action end-c-tokenizer SEMICOLON */
            case 17: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 42;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C42:;
              stack->slot_1_has_sym_data_ = 1;
              stack->slot_1_sym_ = RXG_PATTERN;
              stack->continue_at_ = 43;
              {
                 (stack->stack_[1].v_.uv0_) = SIZE_MAX;
              }
              C43:;
              stack->continue_at_ = 44;
              {
                {
  r = prd_grammar_check_pattern_reserve(g);
  if (r) return r;
  char *regex = snippet_dup_xlt(&(stack->sym_data_[2].common_));
  if (!regex) {
    return PRD_INTERNAL_ERROR;
  }
  (stack->stack_[1].v_.uv0_) = g->num_patterns_;
  pat = g->patterns_ + g->num_patterns_++;
  prd_pattern_init(pat);
  pat->regex_ = regex;
  r = snippet_append_snippet(&pat->action_sequence_, &(stack->sym_data_[5].common_));
  if (r) return r;
  /* Pop the '=' token from stmt-action as it is not part of the action sequence itself. */
  if (pat->action_sequence_.num_tokens_ && (pat->action_sequence_.tokens_[0].match_ == TOK_EQUALS)) {
    snippet_pop_first_token(&pat->action_sequence_);
  }
  r = snippet_append_snippet(&pat->common_action_sequence_, &g->current_common_pattern_action_sequence_);
  if (r) return r;
}
              }
              C44:;
            }
            break;
            /* pattern: DOLLAR COLON start-c-tokenizer accept-whitespace CUR_OPEN action-sequence end-c-tokenizer CUR_CLOSE */
            case 18: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 45;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C45:;
              stack->slot_1_has_sym_data_ = 1;
              stack->slot_1_sym_ = RXG_PATTERN;
              stack->continue_at_ = 46;
              {
                 (stack->stack_[1].v_.uv0_) = SIZE_MAX;
              }
              C46:;
              stack->continue_at_ = 47;
              {
                {
  /* Analogous to regular pattern actions, except we're moving it into the grammar's
   * current common action sequence. */
  snippet_clear(&g->current_common_pattern_action_sequence_);

  /* If action-sequence is empty, then we'd like to preserve this empty-ness by making the common-action 
   * empty as well. This allows generating code to discover that there is no common-action */
  if ((stack->sym_data_[5].common_).num_tokens_) {
    r = snippet_append_snippet(&g->current_common_pattern_action_sequence_, &(stack->sym_data_[5].common_));
    if (r) return r;
  }
}
              }
              C47:;
            }
            break;
            /* exp: term */
            case 19: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 48;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C48:;
              stack->continue_at_ = 49;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C49:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* exp: exp BAR term */
            case 20: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 50;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C50:;
              stack->continue_at_ = 51;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C51:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* term: */
            case 21: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 52;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C52:;
              stack->continue_at_ = 53;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C53:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* term: term elm */
            case 22: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 54;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C54:;
              stack->continue_at_ = 55;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C55:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* elm: sym */
            case 23: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 56;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C56:;
              stack->continue_at_ = 57;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C57:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* elm: sym ASTERISK */
            case 24: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 58;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C58:;
              stack->continue_at_ = 59;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C59:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* elm: sym PLUS */
            case 25: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 60;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C60:;
              stack->continue_at_ = 61;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C61:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* elm: sym QUESTION_MARK */
            case 26: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 62;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C62:;
              stack->continue_at_ = 63;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C63:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* sym: CHAR */
            case 27: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 64;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C64:;
              stack->continue_at_ = 65;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C65:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* sym: DOT */
            case 28: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 66;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C66:;
              stack->continue_at_ = 67;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C67:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* sym: CARET */
            case 29: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 68;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C68:;
              stack->continue_at_ = 69;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C69:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* sym: DOLLAR */
            case 30: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 70;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C70:;
              stack->continue_at_ = 71;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C71:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* sym: START_OF_INPUT_ESC */
            case 31: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 72;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C72:;
              stack->continue_at_ = 73;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C73:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* sym: END_OF_INPUT_ESC */
            case 32: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 74;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C74:;
              stack->continue_at_ = 75;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C75:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* sym: PAR_OPEN exp PAR_CLOSE */
            case 33: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 76;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C76:;
              stack->continue_at_ = 77;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C77:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* sym: SQ_OPEN range SQ_CLOSE */
            case 34: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 78;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C78:;
              stack->continue_at_ = 79;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C79:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* sym: SQ_OPEN CARET range SQ_CLOSE */
            case 35: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 80;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C80:;
              stack->continue_at_ = 81;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C81:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* sym: CATEGORY */
            case 36: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 82;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C82:;
              stack->continue_at_ = 83;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C83:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* range: range range-elm */
            case 37: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 84;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C84:;
              stack->continue_at_ = 85;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C85:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* range: range-elm */
            case 38: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 86;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C86:;
              stack->continue_at_ = 87;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C87:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* range-elm: CHAR */
            case 39: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 88;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C88:;
              stack->continue_at_ = 89;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C89:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* range-elm: CATEGORY */
            case 40: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 90;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C90:;
              stack->continue_at_ = 91;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C91:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* range-elm: CHAR DASH CHAR */
            case 41: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 92;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C92:;
              stack->continue_at_ = 93;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C93:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* action-sequence: */
            case 42: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 94;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C94:;
              stack->continue_at_ = 95;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C95:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* action-sequence: action-sequence CHAR */
            case 43: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 96;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C96:;
              stack->continue_at_ = 97;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C97:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* action-sequence: action-sequence CATEGORY */
            case 44: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 98;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C98:;
              stack->continue_at_ = 99;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C99:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* action-sequence: action-sequence IDENT */
            case 45: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 100;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C100:;
              stack->continue_at_ = 101;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C101:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* action-sequence: action-sequence COLON */
            case 46: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 102;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C102:;
              stack->continue_at_ = 103;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C103:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* action-sequence: action-sequence SEMICOLON */
            case 47: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 104;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C104:;
              stack->continue_at_ = 105;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C105:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* action-sequence: action-sequence SQ_OPEN */
            case 48: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 106;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C106:;
              stack->continue_at_ = 107;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C107:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* action-sequence: action-sequence SQ_CLOSE */
            case 49: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 108;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C108:;
              stack->continue_at_ = 109;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C109:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* action-sequence: action-sequence DOT */
            case 50: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 110;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C110:;
              stack->continue_at_ = 111;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C111:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* action-sequence: action-sequence COMMA */
            case 51: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 112;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C112:;
              stack->continue_at_ = 113;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C113:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* action-sequence: action-sequence LESS_THAN */
            case 52: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 114;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C114:;
              stack->continue_at_ = 115;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C115:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* action-sequence: action-sequence GREATER_THAN */
            case 53: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 116;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C116:;
              stack->continue_at_ = 117;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C117:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* action-sequence: action-sequence DOLLAR */
            case 54: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 118;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C118:;
              stack->continue_at_ = 119;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C119:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* action-sequence: action-sequence CARET */
            case 55: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 120;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C120:;
              stack->continue_at_ = 121;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C121:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* action-sequence: action-sequence DASH */
            case 56: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 122;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C122:;
              stack->continue_at_ = 123;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C123:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* action-sequence: action-sequence BAR */
            case 57: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 124;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C124:;
              stack->continue_at_ = 125;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C125:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* action-sequence: action-sequence ASTERISK */
            case 58: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 126;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C126:;
              stack->continue_at_ = 127;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C127:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* action-sequence: action-sequence PLUS */
            case 59: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 128;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C128:;
              stack->continue_at_ = 129;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C129:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* action-sequence: action-sequence QUESTION_MARK */
            case 60: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 130;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C130:;
              stack->continue_at_ = 131;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C131:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* action-sequence: action-sequence TOKEN */
            case 61: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 132;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C132:;
              stack->continue_at_ = 133;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C133:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* action-sequence: action-sequence WHITESPACE */
            case 62: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 134;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C134:;
              stack->continue_at_ = 135;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C135:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* action-sequence: action-sequence PAR_OPEN action-sequence PAR_CLOSE */
            case 63: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 136;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C136:;
              stack->continue_at_ = 137;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C137:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* action-sequence: action-sequence CUR_OPEN action-sequence CUR_CLOSE */
            case 64: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 138;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C138:;
              stack->continue_at_ = 139;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C139:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* start-regex: */
            case 65: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 140;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C140:;
              stack->continue_at_ = 141;
              {
                {
  tok_switch_to_regex(tkr);
}
              }
              C141:;
            }
            break;
            /* end-regex: */
            case 66: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 142;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C142:;
              stack->continue_at_ = 143;
              {
                {
  tok_switch_to_nonterminal_idents(tkr);
}
              }
              C143:;
            }
            break;
            /* start-c-tokenizer: */
            case 67: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 144;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C144:;
              stack->continue_at_ = 145;
              {
                {
  tok_switch_to_c_idents(tkr);
}
              }
              C145:;
            }
            break;
            /* end-c-tokenizer: */
            case 68: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 146;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C146:;
              stack->continue_at_ = 147;
              {
                {
  g->accept_whitespace_ = 0; /* ignore whitespace */
  tok_switch_to_nonterminal_idents(tkr);
}
              }
              C147:;
            }
            break;
            /* accept-whitespace: */
            case 69: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 148;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C148:;
              stack->continue_at_ = 149;
              {
                {
  g->accept_whitespace_ = 1;
}
              }
              C149:;
            }
            break;
          } /* switch */
        } /* scope guard */
        
        /* Free symdata for every symbol in the production, including the first slot where we will soon
        * push nonterminal_data_reduced_to */
        size_t rxg_sym_idx;
        for (rxg_sym_idx = stack->pos_ - stack->current_production_length_; rxg_sym_idx < stack->pos_; ++rxg_sym_idx) {
          switch (stack->stack_[rxg_sym_idx].state_) {
            case 103: /* mode */
            case 107: /* mode-list */
            stack->continue_at_ = 150;
            {
               regex_mode_list_cleanup(&((stack->stack_ + rxg_sym_idx)->v_.uv2_));
            }
            C150:;
            break;
          } /* switch */
          stack->continue_at_ = 151;
          {
             snippet_cleanup(&((stack->stack_ + rxg_sym_idx)->common_));
          }
          C151:;
        } /* for */
        stack->pos_ -= stack->current_production_length_;
        action = rxg_parse_table[rxg_num_columns * stack->stack_[stack->pos_ - 1].state_ + (stack->current_production_nonterminal_ - rxg_minimum_sym)];
        if (action <= 0) {
          stack->continue_at_ = 0;
          {
            re_error_tkr(tkr, "Error: internal error\n"); return PRD_INTERNAL_ERROR;
            
          }
        }
        switch (rxg_push_state(stack, action /* action for a "goto" shift is the ordinal */)) {
          case _RXG_OVERFLOW: {
            stack->continue_at_ = 0;
            {
              re_error_tkr(tkr, "Error: internal error\n"); return PRD_INTERNAL_ERROR;
              
            }
          }
          break;
          case _RXG_NO_MEMORY: {
            stack->continue_at_ = 0;
            {
              re_error_tkr(tkr, "Error: no memory"); return PRD_INTERNAL_ERROR;
              
            }
          }
          break;
        } /* switch */
        struct rxg_sym_data *sd = stack->stack_ + stack->pos_ - 1;
        *sd = stack->stack_[1];
        sd->state_ = action;
        stack->slot_1_has_common_data_ = 0;
        stack->slot_1_has_sym_data_ = 0;
        stack->top_of_stack_has_common_data_ = 1;
        stack->top_of_stack_has_sym_data_ = 1;
      } /* action < 0 */
      else /* action == 0 */ {
        /* check if we can recover using an error token. */
        size_t n;
        for (n = 0; n < stack->pos_; ++n) {
          int err_action = rxg_parse_table[rxg_num_columns * stack->stack_[n].state_ + (29 /* error token */ - rxg_minimum_sym)];
          if (err_action > 0) {
            /* we can transition on the error token somewhere on the stack */
            break;
          }
        }
        /* Enter error-token recovery mode given that such a recovery is possible */
        stack->error_recovery_ = (n != stack->pos_);
        /* Issue the error here */
        if (!stack->mute_error_turns_) {
          stack->mute_error_turns_ = 3;
          stack->continue_at_ = 0;
          {
            /* Syntax error */ \
            if (sym != RXG_INPUT_END) {\
              re_error_tkr(tkr, "Syntax error \"%s\" not expected", tkr->xmatch_.translated_); \
              } \
            else { \
              re_error_tkr(tkr, "Syntax error: end of input not expected");   \
              } \
            return PRD_SYNTAX_ERROR;
            
          }
        }
        else {
          stack->mute_error_turns_--;
          if (sym == RXG_INPUT_END) {
            /* EOF means we cannot shift to recover, and errors are muted, so return completion */
            stack->pending_reset_ = 1;
            return PRD_SUCCESS;
          }
          /* Retain EOF but discard any other sym so we make progress */
          stack->continue_at_ = 0;
          {
            return PRD_NEXT;
            
          }
        }
      }
    } /* !stack->error_recovery_ */
    if (stack->error_recovery_) {
      size_t n;
      n = stack->pos_;
      if (n) {
        do {
          --n;
          /* Can we shift an error token? */
          int err_action = rxg_parse_table[rxg_num_columns * stack->stack_[n].state_ + (29 /* error token */ - rxg_minimum_sym)];
          if (err_action > 0) {
            /* Does the resulting state accept the current symbol? */
            int err_sym_action = rxg_parse_table[rxg_num_columns * err_action + (sym - rxg_minimum_sym)];
            if (err_sym_action) {
              /* Current symbol is accepted, recover error condition by shifting the error token and then process the symbol as usual */
              /* Free symdata for every symbol up to the state where we will shift the error token */
              size_t rxg_sym_idx;
              for (rxg_sym_idx = n + 1; rxg_sym_idx < stack->pos_; ++rxg_sym_idx) {
                switch (stack->stack_[rxg_sym_idx].state_) {
                  case 103: /* mode */
                  case 107: /* mode-list */
                  stack->continue_at_ = 152;
                  {
                     regex_mode_list_cleanup(&((stack->stack_ + rxg_sym_idx)->v_.uv2_));
                  }
                  C152:;
                  break;
                } /* switch */
                stack->continue_at_ = 153;
                {
                   snippet_cleanup(&((stack->stack_ + rxg_sym_idx)->common_));
                }
                C153:;
              } /* for */
              stack->pos_ = n + 1;
              /* Push the state of the error transition */
              switch (rxg_push_state(stack, err_action)) {
                case _RXG_OVERFLOW: {
                  stack->continue_at_ = 0;
                  {
                    re_error_tkr(tkr, "Error: internal error\n"); return PRD_INTERNAL_ERROR;
                    
                  }
                }
                break;
                case _RXG_NO_MEMORY: {
                  stack->continue_at_ = 0;
                  {
                    re_error_tkr(tkr, "Error: no memory"); return PRD_INTERNAL_ERROR;
                    
                  }
                }
                break;
              } /* switch */
              stack->error_recovery_ = 0;
              /* Break out of do { .. } while loop, we've recovered */
              break;
            } /* if (err_sym_action) (if the current sym can continue after an error transition) */
          } /* if (err_action) (if the state at position 'n' can accept an error transition) */
        } while (n);
      }
      if (stack->error_recovery_) {
        /* Did not yet recover, discard current sym and get next */
        if (sym == RXG_INPUT_END) {
          /* EOF means we cannot shift to recover, so return completion */
          stack->pending_reset_ = 1;
          return PRD_SUCCESS;
        }
        stack->continue_at_ = 0;
        {
          return PRD_NEXT;
          
        }
      }
    } /* stack->error_recovery_ */
  } /* for (;;) */
}
/* --------- END OF GENERATED CODE ------------ */

int rxg_parse_tkr(struct rxg_stack *stack, struct prd_grammar *g, struct tkr_tokenizer *tkr, int end_of_input, struct symbol_table *st) {
  int sym;
  char char_value = 0;

  if (!end_of_input) {
    if (!g->accept_whitespace_ && (tkr->best_match_variant_ == TOK_WHITESPACE)) {
      /* Eat whitespace */
      return PRD_NEXT;
    }
    token_type_t tkt = (token_type_t)tkr->best_match_action_;

    switch (tkt) {
    case TOK_IDENT: sym = RXG_IDENT; break;
    case TOK_PAR_OPEN: sym = RXG_PAR_OPEN; break;
    case TOK_PAR_CLOSE: sym = RXG_PAR_CLOSE; break;
    case TOK_CUBRACE_OPEN: sym = RXG_CUR_OPEN; break;
    case TOK_CUBRACE_CLOSE: sym = RXG_CUR_CLOSE; break;
    case TOK_COLON: sym = RXG_COLON; break;
    case TOK_SEMICOLON: sym = RXG_SEMICOLON; break;
    case TOK_SQBRACE_OPEN: sym = RXG_SQ_OPEN; break;
    case TOK_SQBRACE_CLOSE: sym = RXG_SQ_CLOSE; break;
    case TOK_DOT: sym = RXG_DOT; break;
    case TOK_DOLLAR: sym = RXG_DOLLAR; break;
    case TOK_CARET: sym = RXG_CARET; break;
    case TOK_MINUS: sym = RXG_DASH; break;
    case TOK_BAR: sym = RXG_BAR; break;
    case TOK_ASTERISK: sym = RXG_ASTERISK; break;
    case TOK_PLUS: sym = RXG_PLUS; break;
    case TOK_QUESTION_MARK: sym = RXG_QUESTION_MARK; break;
    case TOK_RESERVED_REGEX_CHAR: {
      return PRD_SYNTAX_ERROR;
    }
    case TOK_ESC_A: sym = RXG_CHAR; char_value = '\a'; break;
    case TOK_ESC_B: sym = RXG_CHAR; char_value = '\b'; break;
    case TOK_ESC_F: sym = RXG_CHAR; char_value = '\f'; break;
    case TOK_ESC_N: sym = RXG_CHAR; char_value = '\n'; break;
    case TOK_ESC_R: sym = RXG_CHAR; char_value = '\r'; break;
    case TOK_ESC_T: sym = RXG_CHAR; char_value = '\t'; break;
    case TOK_ESC_V: sym = RXG_CHAR; char_value = '\v'; break;
    case TOK_ESC_CHAR: {
      sym = RXG_CHAR; char_value = tkr->xmatch_.translated_[1]; break;
    }
    case TOK_ESC_HEX1: {
      sym = RXG_CHAR; 
      char c = tkr->xmatch_.translated_[2];
      if ((c >= '0') && (c <= '9')) char_value = c - '0';
      if ((c >= 'a') && (c <= 'f')) char_value = 0xA + c - 'a';
      if ((c >= 'A') && (c <= 'F')) char_value = 0xA + c - 'A';
      break;
    }
    case TOK_ESC_HEX2: {
      sym = RXG_CHAR; 
      char c = tkr->xmatch_.translated_[2];
      if ((c >= '0') && (c <= '9')) char_value = c - '0';
      if ((c >= 'a') && (c <= 'f')) char_value = 0xA + c - 'a';
      if ((c >= 'A') && (c <= 'F')) char_value = 0xA + c - 'A';
      c = tkr->xmatch_.translated_[3];
      if ((c >= '0') && (c <= '9')) char_value = (char_value << 4) + c - '0';
      if ((c >= 'a') && (c <= 'f')) char_value = (char_value << 4) + 0xA + c - 'a';
      if ((c >= 'A') && (c <= 'F')) char_value = (char_value << 4) + 0xA + c - 'A';
      break;
    }
    case TOK_ESC_OCT1: {
      sym = RXG_CHAR;
      char c = tkr->xmatch_.translated_[1];
      char_value = c - '0';
      break;
    }
    case TOK_ESC_OCT2: {
      sym = RXG_CHAR;
      char c = tkr->xmatch_.translated_[1];
      char_value = c - '0';
      c = tkr->xmatch_.translated_[2];
      char_value = (char_value << 3) + c - '0';
      break;
    }
    case TOK_ESC_OCT3: {
      sym = RXG_CHAR;
      char c = tkr->xmatch_.translated_[1];
      char_value = c - '0';
      c = tkr->xmatch_.translated_[2];
      char_value = (char_value << 3) + c - '0';
      c = tkr->xmatch_.translated_[3];
      char_value = (char_value << 3) + c - '0';
      break;
    }
    case TOK_ESC_UNI1: {
      sym = RXG_CHAR;
      int n = 3; /* position after "\u{" */
      char_value = 0;
      while (tkr->xmatch_.translated_[n] != '}') {
        char c = tkr->xmatch_.translated_[n++];
        if ((c >= '0') && (c <= '9')) char_value = (char_value << 4) + c - '0';
        if ((c >= 'a') && (c <= 'f')) char_value = (char_value << 4) + 0xA + c - 'a';
        if ((c >= 'A') && (c <= 'F')) char_value = (char_value << 4) + 0xA + c - 'A';
      }
      break;
    }
    case TOK_ESC_UNI2: {
      sym = RXG_CHAR;
      int n;
      char_value = 0;
      for (n = 2; n < 6; ++n) {
        char c = tkr->xmatch_.translated_[n];
        if ((c >= '0') && (c <= '9')) char_value = (char_value << 4) + c - '0';
        if ((c >= 'a') && (c <= 'f')) char_value = (char_value << 4) + 0xA + c - 'a';
        if ((c >= 'A') && (c <= 'F')) char_value = (char_value << 4) + 0xA + c - 'A';
      }
      break;
    }
    case TOK_REGULAR_CATEGORY: {
      sym = RXG_CATEGORY;
      break;
    }
    case TOK_INVERTED_CATEGORY: {
      sym = RXG_CATEGORY;
      break;
    }
    case TOK_ESC_INVALID_ESCAPE: {
      re_error_tkr(tkr, "Error, invalid escape \"\\%c\"", tkr->xmatch_.translated_[1]);
      return PRD_SYNTAX_ERROR;
    }
    case TOK_CHAR: {
      sym = RXG_CHAR;
      char_value = tkr->xmatch_.translated_[0];
      break;
    }
    case TOK_WHITESPACE: sym = RXG_WHITESPACE; break;
    case TOK_SPECIAL_IDENT_STR: 
      if (!strcmp("$", tkr_str(tkr))) {
        /* Single '$' is recognized as a dollar.. */
        sym = RXG_DOLLAR;
      }
      else {
        sym = RXG_TOKEN;
      }
      break;
    case TOK_ESC_START_OF_INPUT_ESC: sym = RXG_START_OF_INPUT_ESC; break;
    case TOK_ESC_END_OF_INPUT_ESC: sym = RXG_END_OF_INPUT_ESC; break;
    case TOK_LT: sym = RXG_LESS_THAN; break;
    case TOK_GT: sym = RXG_GREATER_THAN; break;
    case TOK_COMMA: sym = RXG_COMMA; break;
    default: {
      sym = RXG_TOKEN;
    }
    }
  }
  else {
    sym = RXG_INPUT_END;
  }

  return rxg_parse(stack, sym, g, tkr, st, char_value);
}

