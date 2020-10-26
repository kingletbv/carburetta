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

#ifndef REGEX_GRAMMAR_H_INCLUDED
#define REGEX_GRAMMAR_H_INCLUDED
#include "regex_grammar.h"
#endif

#ifndef PRD_GRAM_H_INCLUDED
#define PRD_GRAM_H_INCLUDED
#include "prd_gram.h"
#endif








/* --------- START OF GENERATED CODE ------------ */
#include <stdlib.h> /* realloc(), free(), NULL, size_t */
struct rxg_sym_data {
  int state_;
  struct xlts common_;
};
static const int rxg_minimum_sym = 3;
static const size_t rxg_num_columns = 38;
static const size_t rxg_num_rows = 66;
static const size_t rxg_num_productions = 49;
static const int rxg_parse_table[] = {
  0, -2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,-2, 0, 0, 0, 0, 0, 0,65, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, -3,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,-3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, -4,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,-4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, -5,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,-5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0,  0,  0,  0,  0,  3,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
-27,-27,-27,-27,-27,-27,-27,-27,-27,-27,-27,-27,-27,-27,-27,-27,-27,-27,-27,-27, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
-28,-28,-28,-28,-28,-28,-28,-28,-28,-28,-28,-28,-28,-28,-28,-28,-28,-28,-28,-28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
-29,-29,-29,-29,-29,-29,-29,-29,-29,-29,-29,-29,-29,-29,-29,-29,-29,-29,-29,-29, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
-31,-31,-31,-31,-31,-31,-31,-31,-31,-31,-31,-31,-31,-31,-31,-31,-31,-31,-31,-31, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
-32,-32,-32,-32,-32,-32,-32,-32,-32,-32,-32,-32,-32,-32,-32,-32,-32,-32,-32,-32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
-33,-33,-33,-33,-33,-33,-33,-33,-33,-33,-33,-33,-33,-33,-33,-33,-33,-33,-33,-33, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
-34,-34,-34,-34,-34,-34,-34,-34,-34,-34,-34,-34,-34,-34,-34,-34,-34,-34,-34,-34, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
-35,-35,-35,-35,-35,-35,-35,-35,-35,-35,-35,-35,-35,-35,-35,-35,-35,-35,-35,-35, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
-36,-36,-36,-36,-36,-36,-36,-36,-36,-36,-36,-36,-36,-36,-36,-36,-36,-36,-36,-36, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
-37,-37,-37,-37,-37,-37,-37,-37,-37,-37,-37,-37,-37,-37,-37,-37,-37,-37,-37,-37, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
-38,-38,-38,-38,-38,-38,-38,-38,-38,-38,-38,-38,-38,-38,-38,-38,-38,-38,-38,-38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
-39,-39,-39,-39,-39,-39,-39,-39,-39,-39,-39,-39,-39,-39,-39,-39,-39,-39,-39,-39, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
-40,-40,-40,-40,-40,-40,-40,-40,-40,-40,-40,-40,-40,-40,-40,-40,-40,-40,-40,-40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
-41,-41,-41,-41,-41,-41,-41,-41,-41,-41,-41,-41,-41,-41,-41,-41,-41,-41,-41,-41, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
-42,-42,-42,-42,-42,-42,-42,-42,-42,-42,-42,-42,-42,-42,-42,-42,-42,-42,-42,-42, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
-43,-43,-43,-43,-43,-43,-43,-43,-43,-43,-43,-43,-43,-43,-43,-43,-43,-43,-43,-43, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  5,  6, 23, 21, 26,  0,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
-26,-26,-26,-26,-26,  0,-26,-26,-26,-26,-26,-26,-26,-26,-26,-26,-26,-26,-26,-26, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,22, 0, 0, 0, 0, 0, 0, 0,
-44,-44,-44,-44,-44,-44,-44,-44,-44,-44,-44,-44,-44,-44,-44,-44,-44,-44,-44,-44, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  5,  6, 23,  0, 26, 24,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
-26,-26,-26,  0,-26,-26,-26,-26,-26,-26,-26,-26,-26,-26,-26,-26,-26,-26,-26,-26, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,25, 0, 0, 0, 0, 0, 0, 0,
  5,  6, 23,  0, 26,-48,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0,
-26,-26,-26,  0,-26,-26,-26,-26,-26,-26,-26,-26,-26,-26,-26,-26,-26,-26,-26,-26, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,27, 0, 0, 0, 0, 0, 0, 0,
  0,  0,  0,  0, 28,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, -6,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,-6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0,  0,  0,  0,  0,  0,  0, 30,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0,  0,  0,  0,  0,  0,  0,-48,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,31, 0, 0,
  0,  0,  0,  0,-49,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,32, 0, 0, 0, 0,29, 0,
  0,  0,  0,  0,-47,  0,  0,  2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,33, 0, 0, 0,
  0,  0,  0,  0,-46,  0,  0,-46,  0,  0,  0,  0,  0,  0, 46,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,34, 0, 0, 0, 0,
-10,  0,-10,-10,-10,  0,  0,-10,-10,  0,-10,-10,-10,  0,-10,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
-12,  0,-12,-12,-12,  0,  0,-12,-12,  0,-12,-12,-12,  0,-12,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
-13,  0,-13,-13,-13,  0,  0,-13,-13,  0,-13,-13,-13,  0,-13,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
-14,  0,-14,-14,-14,  0,  0,-14,-14,  0,-14,-14,-14,  0,-14,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
-11,  0,-11,-11,-11,  0,  0,-11,-11,  0,-11,-11,-11,  0,-11, 37, 38, 39,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
-15,  0,-15,-15,-15,  0,  0,-15,-15,  0,-15,-15,-15,  0,-15,-15,-15,-15,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
-16,  0,-16,-16,-16,  0,  0,-16,-16,  0,-16,-16,-16,  0,-16,-16,-16,-16,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
-17,  0,-17,-17,-17,  0,  0,-17,-17,  0,-17,-17,-17,  0,-17,-17,-17,-17,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
-18,  0,-18,-18,-18,  0,  0,-18,-18,  0,-18,-18,-18,  0,-18,-18,-18,-18,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 41,  0, 49, -8, -8,  0,  0, -8, 60,  0, 42, 44, 43,  0, -8,  0,  0,  0,  0,  0, 0, 0, 0, 0,36,40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 -9,  0, -9, -9, -9,  0,  0, -9, -9,  0, -9, -9, -9,  0, -9,  0,  0,  0,  0,  0, 0, 0, 0,45, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
-19,  0,-19,-19,-19,  0,  0,-19,-19,  0,-19,-19,-19,  0,-19,-19,-19,-19,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0,  0,  0, 47,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 46,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 -9,  0, -9, -9,  0,  0,  0,  0, -9,  0, -9, -9, -9,  0, -9,  0,  0,  0,  0,  0, 0, 0,48,61, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
-20,  0,-20,-20,-20,  0,  0,-20,-20,  0,-20,-20,-20,  0,-20,-20,-20,-20,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 58,  0,  0,  0,  0,  0,  0,  0,  0, 50,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0,53, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
-21,  0,-21,-21,-21,  0,  0,-21,-21,  0,-21,-21,-21,  0,-21,-21,-21,-21,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
-22,  0,  0,  0,  0,  0,  0,  0,  0,-22,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 58,  0,  0,  0,  0,  0,  0,  0,  0, 52,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0,53, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
-23,  0,  0,  0,  0,  0,  0,  0,  0,-23,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
-25,  0,  0,  0,  0,  0,  0,  0,  0,-25,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 56,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
-24,  0,  0,  0,  0,  0,  0,  0,  0,-24,  0,  0,  0, 57,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 58,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0,54,55, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 58,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 59,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0,51,55, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 41,  0, 49, -7, -7,  0,  0, -7, 60,  0, 42, 44, 43,  0, -7,  0,  0,  0,  0,  0, 0, 0, 0, 0,36,40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 -9,  0, -9,  0, -9,  0,  0, -9, -9,  0, -9, -9, -9,  0, -9,  0,  0,  0,  0,  0, 0, 0,35,61, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0,  0,  0,  0,  0,  0, 62,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0,  0,  0,  0,  0,  0,-45,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,63, 0, 0, 0, 0, 0,
  0, 64,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,-1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
};
static const size_t rxg_production_lengths[] = {
 1,
 0,
 2,
 6,
 11,
 9,
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
 3,
 3,
 4,
 2,
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
 4,
 4,
 0,
 0,
 0,
 0,
 0
};
static const int rxg_production_syms[] = {
 40,
 31,
 31,
 32,
 32,
 32,
 25,
 25,
 26,
 26,
 27,
 27,
 27,
 27,
 28,
 28,
 28,
 28,
 28,
 28,
 28,
 29,
 29,
 30,
 30,
 33,
 33,
 33,
 33,
 33,
 33,
 33,
 33,
 33,
 33,
 33,
 33,
 33,
 33,
 33,
 33,
 33,
 33,
 33,
 35,
 36,
 37,
 38,
 39
};
static const int rxg_state_syms[] = {
 -1,
 32,
 10,
 8,
 38,
 3,
 4,
 9,
 10,
 11,
 12,
 13,
 14,
 15,
 16,
 17,
 18,
 19,
 20,
 21,
 22,
 6,
 33,
 5,
 8,
 33,
 7,
 33,
 7,
 39,
 10,
 38,
 34,
 37,
 36,
 25,
 27,
 18,
 19,
 20,
 28,
 3,
 13,
 15,
 14,
 26,
 17,
 6,
 25,
 5,
 12,
 29,
 12,
 30,
 29,
 30,
 3,
 16,
 3,
 15,
 11,
 26,
 9,
 35,
 4,
 31
};

#ifndef CINDER_RXG_SRCREGEX_GRAMMAR_H_INCLUDED
struct rxg_stack {
  int error_recovery_:1;
  int report_error_:1;
  int mute_error_turns_;
  size_t pos_, num_stack_allocated_;
  struct rxg_sym_data *stack_;
};
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
#define RXG_ERROR 23
#define RXG_INPUT_END 24

#define RXG_EXP 25
#define RXG_TERM 26
#define RXG_ELM 27
#define RXG_SYM 28
#define RXG_RANGE 29
#define RXG_RANGE_ELM 30
#define RXG_GRAMMAR 31
#define RXG_PATTERN 32
#define RXG_ACTION_SEQUENCE 33
#define RXG_STMT_ACTION 34
#define RXG_START_REGEX 35
#define RXG_END_REGEX 36
#define RXG_START_C_TOKENIZER 37
#define RXG_END_C_TOKENIZER 38
#define RXG_ACCEPT_WHITESPACE 39
#endif /* CINDER_RXG_SRCREGEX_GRAMMAR_H_INCLUDED */

void rxg_stack_init(struct rxg_stack *stack) {
  stack->error_recovery_ = 0;
  stack->report_error_ = 0;
  stack->mute_error_turns_ = 0;
  stack->pos_ = 0;
  stack->num_stack_allocated_ = 0;
  stack->stack_ = NULL;
}

void rxg_stack_cleanup(struct rxg_stack *stack) {
  size_t n;
  for (n = 1; n < stack->pos_; ++n) {
    switch (stack->stack_[n].state_) {
    } /* switch */
    {
       xlts_cleanup(&((stack->stack_ + n)->common_));

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
        return -1;
      }
    }
    else {
      new_num_allocated = 16;
    }

    if (new_num_allocated > (SIZE_MAX / sizeof(struct rxg_sym_data))) {
      /* Overflow in allocation */
      return -1;
    }

    void *p = realloc(stack->stack_, new_num_allocated * sizeof(struct rxg_sym_data));
    if (!p) {
      /* Out of memory */
      return -2;
    }
    stack->stack_ = (struct rxg_sym_data *)p;
    stack->num_stack_allocated_ = new_num_allocated;
  }
  stack->stack_[stack->pos_++].state_ = state;
  return 0;
}
int rxg_stack_reset(struct rxg_stack *stack) {
  size_t n;
  for (n = 0; n < stack->pos_; ++n) {
    switch (stack->stack_[n].state_) {
    } /* switch */
    {
       xlts_cleanup(&((stack->stack_ + n)->common_));

    }
  }
  stack->pos_ = 0;
  stack->error_recovery_ = 0;
  stack->report_error_ = 0;
  stack->mute_error_turns_ = 0;
  switch (rxg_push_state(stack, 0)) {
    case -1: /* overflow */ {
      return -2;
    }
    break;
    case -2: /* out of memory */ {
      return -2;
    }
    break;
  }
  return 0;
}

int rxg_parse(struct rxg_stack *stack, int sym, struct prd_grammar *g, struct tkr_tokenizer *tkr, struct symbol_table *st, char char_value) {
  if (stack->mute_error_turns_) stack->mute_error_turns_--;
  for (;;) {
    if (!stack->error_recovery_) {
      int action = rxg_parse_table[rxg_num_columns * stack->stack_[stack->pos_ - 1].state_ + (sym - rxg_minimum_sym)];
      if (action > 0) {
        switch (rxg_push_state(stack, action /* action for a shift is the ordinal */)) {
          case -1: /* overflow */ {
            re_error_tkr(tkr, "Error: internal error\n"); return PRD_INTERNAL_ERROR;
          }
          break;
          case -2: /* out of memory */ {
            re_error_tkr(tkr, "Error: no memory"); return PRD_INTERNAL_ERROR;
          }
          break;
        } /* switch */

        /* Fill in the sym from the tokenizer */
        struct rxg_sym_data *sym_data = stack->stack_ + stack->pos_ - 1;
        {
           xlts_init(&(sym_data->common_));

        }
        if (stack->report_error_) {
          /* We're shifting this sym following an error recovery on the same sym, report syntax error */
          stack->report_error_ = 0;
          /* Syntax error */ \
  if (sym != PRD_INPUT_END) {\
    re_error_tkr(tkr, "Syntax error \"%s\" not expected", tkr->xmatch_.translated_); \
  } \
  else { \
    re_error_tkr(tkr, "Syntax error: end of input not expected");   \
  } \
  return PRD_SYNTAX_ERROR;
        }
        else {
          return PRD_NEXT;
        }
      } /* action > 0 */
      else if (action < 0) {
        int production = -action - 1;
        int discard_action = 0;
        size_t production_length = rxg_production_lengths[production];
        int nonterminal = rxg_production_syms[production];
        if (0 == production) {
          return PRD_SUCCESS;
        }

        struct rxg_sym_data nonterminal_sym_data_reduced_to = { 0 };
        { /* scope guard */
          struct rxg_sym_data *sym_data = stack->stack_ + stack->pos_ - production_length;
          switch (production) {
            /* grammar: */
            case 1: {
     xlts_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              
            }
            break;
            /* grammar: grammar pattern */
            case 2: {
     xlts_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              
            }
            break;
            /* pattern: IDENT start-regex COLON exp end-regex SEMICOLON */
            case 3: {
     xlts_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              
            }
            break;
            /* pattern: IDENT start-regex COLON exp end-regex start-c-tokenizer accept-whitespace CUR_OPEN action-sequence end-c-tokenizer CUR_CLOSE */
            case 4: {
     xlts_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              
            }
            break;
            /* pattern: IDENT start-regex COLON exp end-regex start-c-tokenizer stmt-action end-c-tokenizer SEMICOLON */
            case 5: {
     xlts_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              
            }
            break;
            /* exp: term */
            case 6: {
     xlts_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    xlts_append(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* exp: exp BAR term */
            case 7: {
     xlts_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    xlts_append(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* term: */
            case 8: {
     xlts_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    xlts_append(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* term: term elm */
            case 9: {
     xlts_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    xlts_append(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* elm: sym */
            case 10: {
     xlts_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    xlts_append(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* elm: sym ASTERISK */
            case 11: {
     xlts_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    xlts_append(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* elm: sym PLUS */
            case 12: {
     xlts_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    xlts_append(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* elm: sym QUESTION_MARK */
            case 13: {
     xlts_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    xlts_append(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* sym: CHAR */
            case 14: {
     xlts_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    xlts_append(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* sym: DOT */
            case 15: {
     xlts_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    xlts_append(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* sym: CARET */
            case 16: {
     xlts_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    xlts_append(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* sym: DOLLAR */
            case 17: {
     xlts_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    xlts_append(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* sym: PAR_OPEN exp PAR_CLOSE */
            case 18: {
     xlts_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    xlts_append(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* sym: SQ_OPEN range SQ_CLOSE */
            case 19: {
     xlts_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    xlts_append(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* sym: SQ_OPEN CARET range SQ_CLOSE */
            case 20: {
     xlts_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    xlts_append(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* range: range range-elm */
            case 21: {
     xlts_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    xlts_append(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* range: range-elm */
            case 22: {
     xlts_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    xlts_append(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* range-elm: CHAR */
            case 23: {
     xlts_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    xlts_append(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* range-elm: CHAR DASH CHAR */
            case 24: {
     xlts_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    xlts_append(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* action-sequence: */
            case 25: {
     xlts_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    xlts_append(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* action-sequence: action-sequence CHAR */
            case 26: {
     xlts_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    xlts_append(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* action-sequence: action-sequence IDENT */
            case 27: {
     xlts_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    xlts_append(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* action-sequence: action-sequence COLON */
            case 28: {
     xlts_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    xlts_append(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* action-sequence: action-sequence SEMICOLON */
            case 29: {
     xlts_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    xlts_append(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* action-sequence: action-sequence SQ_OPEN */
            case 30: {
     xlts_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    xlts_append(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* action-sequence: action-sequence SQ_CLOSE */
            case 31: {
     xlts_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    xlts_append(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* action-sequence: action-sequence DOT */
            case 32: {
     xlts_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    xlts_append(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* action-sequence: action-sequence DOLLAR */
            case 33: {
     xlts_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    xlts_append(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* action-sequence: action-sequence CARET */
            case 34: {
     xlts_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    xlts_append(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* action-sequence: action-sequence DASH */
            case 35: {
     xlts_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    xlts_append(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* action-sequence: action-sequence BAR */
            case 36: {
     xlts_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    xlts_append(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* action-sequence: action-sequence ASTERISK */
            case 37: {
     xlts_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    xlts_append(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* action-sequence: action-sequence PLUS */
            case 38: {
     xlts_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    xlts_append(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* action-sequence: action-sequence QUESTION_MARK */
            case 39: {
     xlts_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    xlts_append(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* action-sequence: action-sequence TOKEN */
            case 40: {
     xlts_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    xlts_append(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* action-sequence: action-sequence WHITESPACE */
            case 41: {
     xlts_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    xlts_append(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* action-sequence: action-sequence PAR_OPEN action-sequence PAR_CLOSE */
            case 42: {
     xlts_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    xlts_append(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* action-sequence: action-sequence CUR_OPEN action-sequence CUR_CLOSE */
            case 43: {
     xlts_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    xlts_append(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* start-regex: */
            case 44: {
     xlts_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  tok_switch_to_regex(tkr);
}
            }
            break;
            /* end-regex: */
            case 45: {
     xlts_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  tok_switch_to_nonterminal_idents(tkr);
}
            }
            break;
            /* start-c-tokenizer: */
            case 46: {
     xlts_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  tok_switch_to_c_idents(tkr);
}
            }
            break;
            /* end-c-tokenizer: */
            case 47: {
     xlts_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  g->accept_whitespace_ = 0; /* ignore whitespace */
  tok_switch_to_nonterminal_idents(tkr);
}
            }
            break;
            /* accept-whitespace: */
            case 48: {
     xlts_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  g->accept_whitespace_ = 1;
}
            }
            break;
          } /* switch */
        } /* scope guard */

        /* Free symdata for every symbol in the production, including the first slot where we will soon
         * push nonterminal_data_reduced_to */
        size_t rxg_sym_idx;
        for (rxg_sym_idx = stack->pos_ - production_length; rxg_sym_idx < stack->pos_; ++rxg_sym_idx) {
          switch (stack->stack_[rxg_sym_idx].state_) {
          } /* switch */
        {
           xlts_cleanup(&((stack->stack_ + rxg_sym_idx)->common_));

        }
        } /* for */
        stack->pos_ -= production_length;
        action = rxg_parse_table[rxg_num_columns * stack->stack_[stack->pos_ - 1].state_ + (nonterminal - rxg_minimum_sym)];
        if (action <= 0) {
          re_error_tkr(tkr, "Error: internal error\n"); return PRD_INTERNAL_ERROR;
        }
        switch (rxg_push_state(stack, action /* action for a "goto" shift is the ordinal */)) {
          case -1: /* overflow */ {
            re_error_tkr(tkr, "Error: internal error\n"); return PRD_INTERNAL_ERROR;
          }
          break;
          case -2: /* out of memory */ {
            re_error_tkr(tkr, "Error: no memory"); return PRD_INTERNAL_ERROR;
          }
          break;
        } /* switch */
        struct rxg_sym_data *sd = stack->stack_ + stack->pos_ - 1;
        *sd = nonterminal_sym_data_reduced_to;
        sd->state_ = action;
      } /* action < 0 */
      else /* action == 0 */ {
        stack->error_recovery_ = 1;
        stack->report_error_ = !stack->mute_error_turns_;
        stack->mute_error_turns_ = 3;
      }
    } /* !stack->error_recovery_ */
    if (stack->error_recovery_) {
      size_t n;
      n = stack->pos_;
      if (n) {
        do {
          --n;
          /* Can we shift an error token? */
          int err_action = rxg_parse_table[rxg_num_columns * stack->stack_[n].state_ + (23 /* error token */ - rxg_minimum_sym)];
          if (err_action > 0) {
            /* Does the resulting state accept the current symbol? */
            int err_sym_action = rxg_parse_table[rxg_num_columns * stack->stack_[n].state_ + (sym - rxg_minimum_sym)];
            if (err_sym_action) {
              /* Current symbol is accepted, recover error condition by shifting the error token and then process the symbol as usual */
              /* Free symdata for every symbol up to the state where we will shift the error token */
              size_t rxg_sym_idx;
              for (rxg_sym_idx = n + 1; rxg_sym_idx < stack->pos_; ++rxg_sym_idx) {
                switch (stack->stack_[rxg_sym_idx].state_) {
                } /* switch */
                {
                   xlts_cleanup(&((stack->stack_ + rxg_sym_idx)->common_));

                }
              } /* for */
              stack->pos_ = n + 1;
              /* Push the state of the error transition */
              switch (rxg_push_state(stack, err_action /* action for a shift is the state */)) {
                case -1: /* overflow */ {
                  re_error_tkr(tkr, "Error: internal error\n"); return PRD_INTERNAL_ERROR;
                }
                break;
                case -2: /* out of memory */ {
                  re_error_tkr(tkr, "Error: no memory"); return PRD_INTERNAL_ERROR;
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
      if (stack->report_error_) {
        stack->report_error_ = 0;
        /* Syntax error */ \
  if (sym != PRD_INPUT_END) {\
    re_error_tkr(tkr, "Syntax error \"%s\" not expected", tkr->xmatch_.translated_); \
  } \
  else { \
    re_error_tkr(tkr, "Syntax error: end of input not expected");   \
  } \
  return PRD_SYNTAX_ERROR;
      }
      else {
        return PRD_NEXT;
      }
    } /* stack->error_recovery_ */
  } /* for (;;) */
}
/* --------- END OF GENERATED CODE ------------ */

int rxg_parse_tkr(struct rxg_stack *stack, struct prd_grammar *g, struct tkr_tokenizer *tkr, int end_of_input, struct symbol_table *st) {
  int sym;
  char char_value = 0;

  if (!end_of_input) {
    token_type_t tkt = (token_type_t)tkr->best_match_variant_;
    if (!g->accept_whitespace_ && (tkt == TOK_WHITESPACE)) {
      /* Eat whitespace */
      return PRD_NEXT;
    }

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
    case TOK_ESC_CHAR: sym = RXG_CHAR; char_value = tkr->xmatch_.translated_[1]; break;
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
    case TOK_CHAR: {
      sym = RXG_CHAR;
      char_value = tkr->xmatch_.translated_[0];
      break;
    }
    case TOK_WHITESPACE: sym = RXG_WHITESPACE; break;
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

