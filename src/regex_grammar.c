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








/* --------- START OF GENERATED CODE ------------ */
#include <stdlib.h> /* realloc(), free(), NULL, size_t */
#include <string.h> /* memcpy() */
#include <stddef.h> /* size_t */
#include <stdint.h> /* SIZE_MAX */
struct rxg_sym_data {
  int state_;
  struct snippet common_;
};
static const size_t rxg_scan_table_rex[] = {
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2,
   4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 4, 4, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 6, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3,
   4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 4, 4, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 6, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
   4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 4, 4, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 6, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5,
   7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 8, 7, 7, 8, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 9, 7, 7, 7, 7,10, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 6, 6, 6, 6,
   4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 4, 4, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 6, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 7, 7, 7, 7,
   4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 4, 4, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 6, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 8, 8, 8, 8,
   7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 8, 7, 7, 8, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 9, 7, 7, 7, 7, 0, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 9, 9, 9, 9,
  10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10
};
static const size_t rxg_scan_actions_rex[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 };
static const int rxg_minimum_sym = 3;
static const size_t rxg_num_columns = 38;
static const size_t rxg_num_rows = 88;
static const size_t rxg_num_productions = 53;
static const int rxg_parse_table[] = {
    0, -2,  0,  0,  0,  0, -2,  0,  0,  0,  0, -2,  0,  0,  0,  0,  0,  0,  0,  0, 0, -2, 0, 0, 0, 0, 0, 0,87, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, -3,  0,  0,  0,  0, -3,  0,  0,  0,  0, -3,  0,  0,  0,  0,  0,  0,  0,  0, 0, -3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, -4,  0,  0,  0,  0, -4,  0,  0,  0,  0, -4,  0,  0,  0,  0,  0,  0,  0,  0, 0, -4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, -5,  0,  0,  0,  0, -5,  0,  0,  0,  0, -5,  0,  0,  0,  0,  0,  0,  0,  0, 0, -5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0,  0,  0,  0,  0,  3,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   59, 60, 77,  0, 80,-52, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0,
  -30,-30,-30,  0,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0,
    0,  0,  0,  0,  6,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, -6,  0,  0,  0,  0, -6,  0,  0,  0,  0, -6,  0,  0,  0,  0,  0,  0,  0,  0, 0, -6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0,  0,  0,  0,  0,  0,  0,  8,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0,  0,  0,  0,  0,  0,  0,-52,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 0, 0,
    0,  0,  0,  0,-53,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,10, 0, 0, 0, 0, 7, 0,
    0,  0,  0,  0,-51,  0,  0,  2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,11, 0, 0, 0,
    0,  0,  0,  0,-50,  0,  0,-50,  0,  0,  0,  0,  0,  0, 39,  0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,12, 0, 0, 0, 0,
  -13,  0,-13,  0,-13,  0,  0,-13,-13,  0,-13,-13,-13,  0,-13,  0,  0,  0,  0,  0, 0,  0,13,54, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0,  0,  0,  0,  0,  0, 14,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0,  0,  0,  0,  0,  0,-49,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,15, 0, 0, 0, 0, 0,
    0, -7,  0,  0,  0,  0, -7,  0,  0,  0,  0, -7,  0,  0,  0,  0,  0,  0,  0,  0, 0, -7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, -8,  0,  0,  0,  0, -8,  0,  0,  0,  0, -8,  0,  0,  0,  0,  0,  0,  0,  0, 0, -8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0,  0,  0,  0,  0, 18,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   59, 60, 77,  0, 80,-52, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,19, 0, 0,
  -30,-30,-30,  0,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,20, 0, 0, 0, 0, 0, 0, 0,
    0,  0,  0,  0, 21,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, -9,  0,  0,  0,  0, -9,  0,  0,  0,  0, -9,  0,  0,  0,  0,  0,  0,  0,  0, 0, -9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0,  0,  0,  0,  0,  0,  0, 23,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0,  0,  0,  0,  0,  0,  0,-52,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,24, 0, 0,
    0,  0,  0,  0,-53,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,25, 0, 0, 0, 0,22, 0,
    0,  0,  0,  0,-51,  0,  0, 17,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,26, 0, 0, 0,
    0,  0,  0,  0,-50,  0,  0,-50,  0,  0,  0,  0,  0,  0, 39,  0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,27, 0, 0, 0, 0,
  -14,  0,-14,-14,-14,  0,  0,-14,-14,  0,-14,-14,-14,  0,-14,  0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  -16,  0,-16,-16,-16,  0,  0,-16,-16,  0,-16,-16,-16,  0,-16,  0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  -17,  0,-17,-17,-17,  0,  0,-17,-17,  0,-17,-17,-17,  0,-17,  0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  -18,  0,-18,-18,-18,  0,  0,-18,-18,  0,-18,-18,-18,  0,-18,  0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  -15,  0,-15,-15,-15,  0,  0,-15,-15,  0,-15,-15,-15,  0,-15, 30, 31, 32,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  -19,  0,-19,-19,-19,  0,  0,-19,-19,  0,-19,-19,-19,  0,-19,-19,-19,-19,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  -20,  0,-20,-20,-20,  0,  0,-20,-20,  0,-20,-20,-20,  0,-20,-20,-20,-20,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  -21,  0,-21,-21,-21,  0,  0,-21,-21,  0,-21,-21,-21,  0,-21,-21,-21,-21,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  -22,  0,-22,-22,-22,  0,  0,-22,-22,  0,-22,-22,-22,  0,-22,-22,-22,-22,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   34,  0, 42,-12,-12,  0,  0,-12, 53,  0, 35, 37, 36,  0,-12,  0,  0,  0,  0,  0, 0,  0, 0, 0,29,33, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  -13,  0,-13,-13,-13,  0,  0,-13,-13,  0,-13,-13,-13,  0,-13,  0,  0,  0,  0,  0, 0,  0, 0,38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  -23,  0,-23,-23,-23,  0,  0,-23,-23,  0,-23,-23,-23,  0,-23,-23,-23,-23,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0,  0,  0, 40,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 39,  0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  -13,  0,-13,-13,  0,  0,  0,  0,-13,  0,-13,-13,-13,  0,-13,  0,  0,  0,  0,  0, 0,  0,41,54, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  -24,  0,-24,-24,-24,  0,  0,-24,-24,  0,-24,-24,-24,  0,-24,-24,-24,-24,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   51,  0,  0,  0,  0,  0,  0,  0,  0, 43,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0,46, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  -25,  0,-25,-25,-25,  0,  0,-25,-25,  0,-25,-25,-25,  0,-25,-25,-25,-25,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  -26,  0,  0,  0,  0,  0,  0,  0,  0,-26,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   51,  0,  0,  0,  0,  0,  0,  0,  0, 45,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0,46, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  -27,  0,  0,  0,  0,  0,  0,  0,  0,-27,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  -29,  0,  0,  0,  0,  0,  0,  0,  0,-29,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   49,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  -28,  0,  0,  0,  0,  0,  0,  0,  0,-28,  0,  0,  0, 50,  0,  0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   51,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0,47,48, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   51,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 52,  0,  0,  0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0,44,48, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   34,  0, 42,-11,-11,  0,  0,-11, 53,  0, 35, 37, 36,  0,-11,  0,  0,  0,  0,  0, 0,  0, 0, 0,29,33, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  -13,  0,-13,  0,-13,  0,  0,-13,-13,  0,-13,-13,-13,  0,-13,  0,  0,  0,  0,  0, 0,  0,28,54, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0,  0,  0,  0,  0,  0, 55,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0,-10,  0,  0,  0,  0,-10,  0,  0,  0,  0,-10,  0,  0,  0,  0,  0,  0,  0,  0, 0,-10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0,  0,  0,  0,  0, 57,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  -31,-31,-31,-31,-31,-31,-31,-31,-31,-31,-31,-31,-31,-31,-31,-31,-31,-31,-31,-31, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  -32,-32,-32,-32,-32,-32,-32,-32,-32,-32,-32,-32,-32,-32,-32,-32,-32,-32,-32,-32, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  -33,-33,-33,-33,-33,-33,-33,-33,-33,-33,-33,-33,-33,-33,-33,-33,-33,-33,-33,-33, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  -34,-34,-34,-34,-34,-34,-34,-34,-34,-34,-34,-34,-34,-34,-34,-34,-34,-34,-34,-34, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  -35,-35,-35,-35,-35,-35,-35,-35,-35,-35,-35,-35,-35,-35,-35,-35,-35,-35,-35,-35, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  -36,-36,-36,-36,-36,-36,-36,-36,-36,-36,-36,-36,-36,-36,-36,-36,-36,-36,-36,-36, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  -37,-37,-37,-37,-37,-37,-37,-37,-37,-37,-37,-37,-37,-37,-37,-37,-37,-37,-37,-37, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  -38,-38,-38,-38,-38,-38,-38,-38,-38,-38,-38,-38,-38,-38,-38,-38,-38,-38,-38,-38, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  -39,-39,-39,-39,-39,-39,-39,-39,-39,-39,-39,-39,-39,-39,-39,-39,-39,-39,-39,-39, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  -40,-40,-40,-40,-40,-40,-40,-40,-40,-40,-40,-40,-40,-40,-40,-40,-40,-40,-40,-40, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  -41,-41,-41,-41,-41,-41,-41,-41,-41,-41,-41,-41,-41,-41,-41,-41,-41,-41,-41,-41, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  -42,-42,-42,-42,-42,-42,-42,-42,-42,-42,-42,-42,-42,-42,-42,-42,-42,-42,-42,-42, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  -43,-43,-43,-43,-43,-43,-43,-43,-43,-43,-43,-43,-43,-43,-43,-43,-43,-43,-43,-43, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  -44,-44,-44,-44,-44,-44,-44,-44,-44,-44,-44,-44,-44,-44,-44,-44,-44,-44,-44,-44, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  -45,-45,-45,-45,-45,-45,-45,-45,-45,-45,-45,-45,-45,-45,-45,-45,-45,-45,-45,-45, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  -46,-46,-46,-46,-46,-46,-46,-46,-46,-46,-46,-46,-46,-46,-46,-46,-46,-46,-46,-46, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  -47,-47,-47,-47,-47,-47,-47,-47,-47,-47,-47,-47,-47,-47,-47,-47,-47,-47,-47,-47, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   59, 60, 77, 75, 80,  0, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  -30,-30,-30,-30,-30,  0,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,76, 0, 0, 0, 0, 0, 0, 0,
  -48,-48,-48,-48,-48,-48,-48,-48,-48,-48,-48,-48,-48,-48,-48,-48,-48,-48,-48,-48, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   59, 60, 77,  0, 80, 78, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  -30,-30,-30,  0,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,79, 0, 0, 0, 0, 0, 0, 0,
   59, 60, 77,  0, 80,-52, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,58, 0, 0,
  -30,-30,-30,  0,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,81, 0, 0, 0, 0, 0, 0, 0,
    0,  0,  0,  0, 82,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0,  0,  0,  0,-53,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,83, 0,
    0,  0,  0,  0,-51,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,84, 0, 0, 0,
    0,  0,  0,  0,  0,  0, 85,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 16,  0,  0,  0,  0,-49,  0,  0,  0,  0, 86,  0,  0,  0,  0,  0,  0,  0,  0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,56, 0, 0, 0, 0, 0,
};
static const size_t rxg_production_lengths[] = {
  1,
  0,
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
  32,
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

#ifndef CARB_RXG_SRCREGEX_GRAMMAR_H_INCLUDED
struct rxg_stack {
  int error_recovery_:1;
  int pending_reset_:1;
  int discard_remaining_actions_:1;
  int slot_1_has_sym_data_:1;
  int slot_1_has_common_data_:1;
  int top_of_stack_has_sym_data_:1;
  int top_of_stack_has_common_data_:1;
  int need_sym_:1;
  int is_final_input_:1;
  int slot_0_has_current_sym_data_:1;
  int slot_0_has_common_data_:1;
  int current_sym_;
  size_t input_size_;
  const char *input_;
  int slot_1_sym_;
  int continue_at_;
  int mute_error_turns_;
  size_t pos_, num_stack_allocated_;
  struct rxg_sym_data *stack_;
  struct rxg_sym_data *sym_data_;
  size_t current_production_length_;
  int current_production_nonterminal_;
  size_t scan_state_;
  size_t match_index_;
  size_t best_match_action_;
  size_t best_match_size_;
  size_t input_index_;
  size_t input_offset_;
  size_t match_buffer_size_;
  size_t match_buffer_size_allocated_;
  /* offset, line and column at the start of match_buffer_ */
  size_t match_offset_;
  int match_line_;
  int match_col_;
  /* offset, line and column at the zero-termination (best_match_size_) of a match 
  * in match_buffer_ (the actual buffer may well be longer.) */
  size_t best_match_offset_;
  int best_match_line_;
  int best_match_col_;
  size_t token_size_;
  char *match_buffer_;
  char terminator_repair_;
  int input_line_;
  int input_col_;
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
#endif /* CARB_RXG_SRCREGEX_GRAMMAR_H_INCLUDED */

void rxg_stack_init(struct rxg_stack *stack) {
  stack->error_recovery_ = 0;
  stack->pending_reset_ = 1;
  stack->discard_remaining_actions_ = 0;
  stack->need_sym_ = 1;
  stack->current_sym_ = 0;
  stack->input_ = NULL;
  stack->input_size_ = 0;
  stack->is_final_input_ = 0;
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
  stack->slot_0_has_current_sym_data_ = stack->slot_0_has_common_data_ = 0;
  stack->scan_state_ = 1;
  stack->input_index_ = 0;
  stack->input_offset_ = 0;
  stack->input_line_ = 1;
  stack->input_col_ = 1;
  stack->match_index_ = 0;
  stack->match_buffer_ = NULL;
  stack->match_buffer_size_ = 0;
  stack->match_buffer_size_allocated_ = 0;
  stack->terminator_repair_ = '\0';
  stack->token_size_ = 0;
  stack->match_offset_ = 0;
  stack->match_line_ = 1;
  stack->match_col_ = 1;
  stack->best_match_action_ = 0;
  stack->best_match_size_ = 0;
  stack->best_match_offset_ = 0;
  stack->best_match_line_ = 1;
  stack->best_match_col_ = 1;
}

void rxg_stack_cleanup(struct rxg_stack *stack) {
  size_t n;
  for (n = 2; n < stack->pos_; ++n) {
    if ((n != (stack->pos_ - 1)) || stack->top_of_stack_has_sym_data_) {
      switch (stack->stack_[n].state_) {
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
    } /* switch */
  }
  if (stack->slot_1_has_common_data_) {
    {
       snippet_cleanup(&(stack->stack_[0].common_));
    }
    
  }
  if (stack->slot_0_has_current_sym_data_) {
    /* Deconstruct placeholder location for terminal not yet shifted */
    switch (stack->current_sym_) {
    } /* switch */
    if (stack->slot_0_has_common_data_) {
      {
         snippet_cleanup(&(stack->stack_[0].common_));
      }
      
    }
  }
  
  if (stack->stack_) free(stack->stack_);
  if (stack->match_buffer_) free(stack->match_buffer_);
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
  if (stack->slot_0_has_current_sym_data_) {
    /* Deconstruct placeholder location for terminal not yet shifted */
    switch (stack->current_sym_) {
    } /* switch */
    if (stack->slot_0_has_common_data_) {
      {
         snippet_cleanup(&(stack->stack_[0].common_));
      }
      
    }
  }
  stack->slot_0_has_current_sym_data_ = stack->slot_0_has_common_data_ = 0;
  stack->sym_data_ = NULL;
  stack->current_production_length_ = 0;
  stack->current_production_nonterminal_ = 0;
  stack->pos_ = 0;
  stack->error_recovery_ = 0;
  stack->need_sym_ = 1;
  stack->current_sym_ = 0;
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
  stack->scan_state_ = 1;
  stack->input_offset_ = 0;
  stack->input_line_ = 1;
  stack->input_col_ = 1;
  stack->match_index_ = 0;
  stack->match_buffer_size_ = 0;
  stack->terminator_repair_ = '\0';
  stack->token_size_ = 0;
  stack->match_offset_ = 0;
  stack->match_line_ = 1;
  stack->match_col_ = 1;
  stack->best_match_action_ = 0;
  stack->best_match_size_ = 0;
  stack->best_match_offset_ = 0;
  stack->best_match_line_ = 1;
  stack->best_match_col_ = 1;
  return 0;
}

int rxg_stack_can_recover(struct rxg_stack *stack) {
  return !!stack->error_recovery_;
}

int rxg_stack_accepts(struct rxg_stack *stack, int sym) {
  if (!stack->pos_) return 0;
  return 0 != rxg_parse_table[rxg_num_columns * stack->stack_[stack->pos_ - 1].state_ + (sym - rxg_minimum_sym)];}

static int rxg_append_match_buffer(struct rxg_stack *stack, const char *s, size_t len) {
  size_t size_needed = len;
  size_needed += stack->match_buffer_size_;
  if (size_needed < stack->match_buffer_size_) {
    return _RXG_OVERFLOW;
  }
  if (size_needed == SIZE_MAX) {
    /* cannot fit null terminator */
    return _RXG_OVERFLOW;
  }
  size_needed++; /* null terminator */
  if (size_needed < 128) {
    size_needed = 128;
  }
  if (size_needed > stack->match_buffer_size_allocated_) {
    /* intent of code: grow buffer size by powers of 2-1, unless our needs require more now. */
    size_t size_to_allocate = stack->match_buffer_size_allocated_ * 2 + 1;
    if (size_to_allocate <= stack->match_buffer_size_allocated_) {
      return _RXG_OVERFLOW;
    }
    if (size_to_allocate < size_needed) {
      size_to_allocate = size_needed;
    }
    void *buf = realloc(stack->match_buffer_, size_to_allocate);
    if (!buf) {
      return _RXG_NO_MEMORY;
    }
    stack->match_buffer_ = (char *)buf;
    stack->match_buffer_size_allocated_ = size_to_allocate;
  }
  
  memcpy(stack->match_buffer_ + stack->match_buffer_size_, s, len);
  stack->match_buffer_size_ += len;
  stack->match_buffer_[stack->match_buffer_size_] = '\0';
  return 0;
}

void rxg_set_input(struct rxg_stack *stack, const char *input, size_t input_size, int is_final_input) {
  stack->input_ = input;
  stack->input_size_ = input_size;
  stack->is_final_input_ = is_final_input;
  stack->input_index_ = 0;
}

void rxg_set_location(struct rxg_stack *stack, int line, int col, size_t offset) {
  if (stack->token_size_) {
    /* Parsing of next token not in progress, set end location of this token as
    ** it will be the start of the next token. */
    stack->input_line_ = stack->input_line_ - stack->best_match_line_ + line;
    stack->input_col_ = stack->input_col_ - stack->best_match_col_ + col;
    stack->input_offset_ = stack->input_offset_ - stack->best_match_offset_ + offset;
    
    stack->best_match_line_ = line;
    stack->best_match_col_ = col;
    stack->best_match_offset_ = offset;
    return;
  }
  /* Parsing of token in progress, dynamically move the start of the token, as
  ** well as the relative current partial end of the token, to the desired location. */
  stack->input_line_ = stack->input_line_ - stack->match_line_ + line;
  stack->input_col_ = stack->input_col_ - stack->match_col_ + col;
  stack->input_offset_ = stack->input_offset_ - stack->match_offset_ + offset;
  
  stack->best_match_line_ = stack->best_match_line_ - stack->match_line_ + line;
  stack->best_match_col_ = stack->best_match_col_ - stack->match_col_ + col;
  stack->best_match_offset_ = stack->best_match_offset_ - stack->match_offset_ + offset;
  stack->match_line_ = line;
  stack->match_col_ = col;
  stack->match_offset_ = offset;
}

const char *rxg_text(struct rxg_stack *stack) {
  return stack->match_buffer_;
}

size_t rxg_len(struct rxg_stack *stack) {
  return stack->token_size_;
}

int rxg_line(struct rxg_stack *stack) {
  return stack->match_line_;
}

int rxg_column(struct rxg_stack *stack) {
  return stack->match_col_;
}

size_t rxg_offset(struct rxg_stack *stack) {
  return stack->match_offset_;
}

int rxg_endline(struct rxg_stack *stack) {
  return stack->best_match_line_;
}

int rxg_endcolumn(struct rxg_stack *stack) {
  return stack->best_match_col_;
}

size_t rxg_endoffset(struct rxg_stack *stack) {
  return stack->best_match_offset_;
}

int rxg_lex(struct rxg_stack *stack) {
  int r;
  unsigned char c;
  const char *input = stack->input_;
  size_t input_size = stack->input_size_;
  int is_final_input = !!stack->is_final_input_;
  size_t scan_state = stack->scan_state_;
  const size_t *transition_table = rxg_scan_table_rex;
  const size_t *actions = rxg_scan_actions_rex;
  const size_t row_size = 260;
  const size_t default_action = 0;
  const size_t start_state = 1;
  const size_t start_action = 0;
  
  size_t match_index = stack->match_index_;
  
  size_t best_match_action = stack->best_match_action_;
  size_t best_match_size = stack->best_match_size_;
  size_t best_match_offset = stack->best_match_offset_;
  int best_match_line = stack->best_match_line_;
  int best_match_col = stack->best_match_col_;
  
  size_t input_index = stack->input_index_;
  size_t input_offset = stack->input_offset_;
  int input_line = stack->input_line_;
  int input_col = stack->input_col_;
  
  /* Move any prior token out of the way */
  if (stack->token_size_) {
    stack->match_buffer_[stack->token_size_] = stack->terminator_repair_;
    
    memcpy(stack->match_buffer_, stack->match_buffer_ + stack->token_size_, stack->match_buffer_size_ - stack->token_size_);
    stack->match_buffer_size_ -= stack->token_size_;
    stack->match_offset_ = stack->best_match_offset_;
    stack->match_line_ = stack->best_match_line_;
    stack->match_col_ = stack->best_match_col_;
    
    /* Reset scanner to get ready for next token */
    stack->match_index_ = 0;
    stack->best_match_action_ = best_match_action = start_action;
    stack->best_match_size_ = best_match_size = 0;
    stack->scan_state_ = scan_state = start_state;
    stack->token_size_ = 0;
    
  }
  
  size_t at_match_index_offset = stack->match_offset_;
  int at_match_index_line = stack->match_line_;
  int at_match_index_col = stack->match_col_;
  while (match_index < stack->match_buffer_size_) {
    c = (unsigned char)stack->match_buffer_[match_index];
    for (;;) {
      /* Check for start of input */
      if ((transition_table[row_size * scan_state + 256] != scan_state) && (!at_match_index_offset)) {
        scan_state = transition_table[row_size * scan_state + 256];
      }
      /* Check for start of line */
      else if ((transition_table[row_size * scan_state + 257] != scan_state) && (at_match_index_col == 1)) {
        scan_state = transition_table[row_size * scan_state + 257];
      }
      /* Check for end of line */
      else if ((transition_table[row_size * scan_state + 258] != scan_state) && ('\n' == c)) {
        scan_state = transition_table[row_size * scan_state + 258];
      }
      /* (No need to check for end of input; we have at least 1 character ahead) */
      else {
        break;
      }
    }
    size_t state_action = actions[scan_state];
    if (state_action != default_action) /* replace with actual */ {
      best_match_action = state_action;
      best_match_size = match_index;
      best_match_offset = at_match_index_offset;
      best_match_line = at_match_index_line;
      best_match_col = at_match_index_col;
    }
    scan_state = transition_table[row_size * scan_state + c];
    if (scan_state) {
      at_match_index_offset++;
      if (c != '\n') {
        at_match_index_col++;
      }
      else {
        at_match_index_col = 1;
        at_match_index_line++;
      }
      
      match_index++;
    }
    else {
      /* error, or, end of token, depending on whether we have a match before */
      if (best_match_action == default_action) {
        goto syntax_error;
      }
      
      /* Ensure token match is null terminated */
      stack->terminator_repair_ = stack->match_buffer_[best_match_size];
      stack->match_buffer_[best_match_size] = '\0';
      stack->token_size_ = best_match_size;
      stack->best_match_action_ = best_match_action;
      stack->best_match_size_ = best_match_size;
      stack->best_match_offset_ = best_match_offset;
      stack->best_match_line_ = best_match_line;
      stack->best_match_col_ = best_match_col;
      
      stack->input_index_ = input_index;
      stack->input_offset_ = input_offset;
      stack->input_line_ = input_line;
      stack->input_col_ = input_col;
      return _RXG_MATCH;
    }
  }
  
  while (input_index < input_size) {
    c = (unsigned char)input[input_index];
    for (;;) {
      /* Check for start of input */
      if ((transition_table[row_size * scan_state + 256] != scan_state) && (!input_offset)) {
        scan_state = transition_table[row_size * scan_state + 256];
      }
      /* Check for start of line */
      else if ((transition_table[row_size * scan_state + 257] != scan_state) && (input_col == 1)) {
        scan_state = transition_table[row_size * scan_state + 257];
      }
      /* Check for end of line */
      else if ((transition_table[row_size * scan_state + 258] != scan_state) && ('\n' == c)) {
        scan_state = transition_table[row_size * scan_state + 258];
      }
      /* (No need to check for end of input; we have at least 1 character ahead) */
      else {
        break;
      }
    }
    size_t state_action = actions[scan_state];
    if (state_action != default_action) /* replace with actual */ {
      best_match_action = state_action;
      best_match_size = stack->match_buffer_size_ + input_index - stack->input_index_;
      best_match_offset = input_offset;
      best_match_col = input_col;
      best_match_line = input_line;
    }
    scan_state = transition_table[row_size * scan_state + c];
    if (scan_state) {
      input_offset++;
      if (c != '\n') {
        input_col++;
      }
      else {
        input_col = 1;
        input_line++;
      }
      input_index++;
    }
    else {
      /* Append from stack->input_index_ to input_index, excluding input_index itself */
      r = rxg_append_match_buffer(stack, input + stack->input_index_, input_index - stack->input_index_);
      if (r) return r;
      
      if (best_match_action == default_action) {
        goto syntax_error;
      }
      
      /* Ensure token match is null terminated, note that the size we just appended may
      * (likely) be longer than the last section we matched. */
      stack->terminator_repair_ = stack->match_buffer_[best_match_size];
      stack->match_buffer_[best_match_size] = '\0';
      stack->token_size_ = best_match_size;
      stack->best_match_action_ = best_match_action;
      stack->best_match_size_ = best_match_size;
      stack->best_match_offset_ = best_match_offset;
      stack->best_match_line_ = best_match_line;
      stack->best_match_col_ = best_match_col;
      
      stack->input_index_ = input_index;
      stack->input_offset_ = input_offset;
      stack->input_line_ = input_line;
      stack->input_col_ = input_col;
      return _RXG_MATCH;
    }
  }
  
  r = rxg_append_match_buffer(stack, input + stack->input_index_, input_index - stack->input_index_);
  if (r) return r;
  
  if (!is_final_input) {
    /* Need more input */
    stack->scan_state_ = scan_state;
    stack->token_size_ = 0; /* no match yet */
    stack->input_index_ = input_index;
    stack->input_offset_ = input_offset;
    stack->input_line_ = input_line;
    stack->input_col_ = input_col;
    
    stack->best_match_action_ = best_match_action;
    stack->best_match_size_ = best_match_size;
    stack->best_match_offset_ = best_match_offset;
    stack->best_match_line_ = best_match_line;
    stack->best_match_col_ = best_match_col;
    
    stack->match_index_ = match_index;
    
    return _RXG_FEED_ME;
  }
  for (;;) {
    /* Check for start of input */
    if ((transition_table[row_size * scan_state + 256] != scan_state) && (!input_offset)) {
      scan_state = transition_table[row_size * scan_state + 256];
    }
    /* Check for start of line */
    else if ((transition_table[row_size * scan_state + 257] != scan_state) && (input_col == 1)) {
      scan_state = transition_table[row_size * scan_state + 257];
    }
    /* Check for end of line (always true at end of input) */
    else if (transition_table[row_size * scan_state + 258] != scan_state) {
      scan_state = transition_table[row_size * scan_state + 258];
    }
    /* Check for end of input (always true) */
    else if (transition_table[row_size * scan_state + 259] != scan_state) {
      scan_state = transition_table[row_size * scan_state + 259];
    }
    /* (No need to check for end of input; we have at least 1 character ahead) */
    else {
      break;
    }
  }
  size_t state_action = actions[scan_state];
  if (state_action != default_action) /* replace with actual */ {
    best_match_action = state_action;
    best_match_size = stack->match_buffer_size_;
    best_match_offset = input_offset;
    best_match_col = input_col;
    best_match_line = input_line;
  }
  
  if (!stack->match_buffer_size_ && (stack->input_index_ == input_size)) {
    /* Exhausted all input - leave stack in a state where we can
    * immediately re-use it in its initial state */
    stack->match_index_ = 0;
    stack->best_match_action_ = best_match_action = start_action;
    stack->best_match_size_ = best_match_size;
    stack->best_match_offset_ = best_match_offset;
    stack->best_match_line_ = best_match_line;
    stack->best_match_col_ = best_match_col;
    stack->scan_state_ = scan_state = start_state;
    
    stack->token_size_ = 0;
    stack->input_index_ = 0;
    stack->input_offset_ = input_offset;
    stack->input_line_ = input_line;
    stack->input_col_ = input_col;
    return _RXG_END_OF_INPUT;
  }
  
  if (best_match_action == default_action) {
    goto syntax_error;
  }
  
  /* Ensure token match is null terminated */
  stack->terminator_repair_ = stack->match_buffer_[best_match_size];
  stack->match_buffer_[best_match_size] = '\0';
  stack->token_size_ = best_match_size;
  stack->best_match_action_ = best_match_action;
  stack->best_match_size_ = best_match_size;
  stack->best_match_offset_ = best_match_offset;
  stack->best_match_line_ = best_match_line;
  stack->best_match_col_ = best_match_col;
  
  stack->input_index_ = input_index;
  stack->input_offset_ = input_offset;
  stack->input_line_ = input_line;
  stack->input_col_ = input_col;
  return _RXG_MATCH;
  syntax_error:
  if (stack->match_buffer_size_) {
    stack->best_match_offset_ = stack->match_offset_ + 1;
    if (stack->match_buffer_[0] != '\n') {
      stack->best_match_line_ = stack->match_line_;
      stack->best_match_col_ = stack->match_col_ + 1;
    }
    else {
      stack->best_match_line_ = stack->match_line_ + 1;
      stack->best_match_col_ = 1;
    }
  }
  else {
    /* Append the single character causing the syntax error */
    r = rxg_append_match_buffer(stack, input + stack->input_index_, 1);
    if (r) return r;
    
    input_offset++;
    if (input[stack->input_index_] != '\n') {
      input_col++;
    }
    else {
      input_col = 1;
      input_line++;
    }
    input_index = stack->input_index_ + 1;
    stack->best_match_offset_ = input_offset;
    stack->best_match_line_ = input_line;
    stack->best_match_col_ = input_col;
  }
  
  /* Reset scanner to get ready for next token */
  stack->token_size_ = 1;
  stack->terminator_repair_ = stack->match_buffer_[1];
  stack->match_buffer_[1] = '\0';
  
  stack->input_index_ = input_index;
  stack->input_offset_ = input_offset;
  stack->input_line_ = input_line;
  stack->input_col_ = input_col;
  
  return _RXG_LEXICAL_ERROR;
}

int rxg_scan(struct rxg_stack *stack, struct prd_grammar *g, struct tkr_tokenizer *tkr, struct symbol_table *st, char char_value) {
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
  } /* continuation switch */
for (;;) {
    stack->continue_at_ = 0;
    if (stack->need_sym_) {
      switch (rxg_lex(stack)) {
        case _RXG_MATCH:
        stack->need_sym_ = 0;
        stack->discard_remaining_actions_ = 0;
        stack->current_sym_ = RXG_INPUT_END; /* no sym specific type associated, changed when we successfully construct it below */
        if (stack->mute_error_turns_) stack->mute_error_turns_--;
        stack->slot_0_has_common_data_ = 1;
        stack->continue_at_ = 1;
        {
           snippet_init(&(stack->stack_[0].common_));
        }
        C1:;
        switch (stack->best_match_action_) {
          case 1: /* /\*([^\*]|[\r\n]|(\*+([^\* //]|[\r\n])))*\* // */
          /* Pattern does not have a symbol */
          stack->current_sym_ = RXG_INPUT_END;
          stack->need_sym_ = 1; /* keep scanning */
          stack->continue_at_ = 2;
          {
            
  fprintf(stderr, "Imagine a sensible action here.\n");

          }
          C2:;
          if (stack->need_sym_) {
            stack->continue_at_ = 3;
            {
               snippet_cleanup(&(stack->stack_[0].common_));
            }
            C3:;
          }
          break;
        } /* switch */
        break;
        case _RXG_OVERFLOW:
        stack->continue_at_ = 0;
        {
          re_error_tkr(tkr, "Error: internal error\n"); return PRD_INTERNAL_ERROR;
          
        }
        case _RXG_NO_MEMORY:
        stack->continue_at_ = 0;
        {
          re_error_tkr(tkr, "Error: no memory"); return PRD_INTERNAL_ERROR;
          
        }
        case _RXG_FEED_ME:
        stack->continue_at_ = 0;
        /* Need more input */
        return _RXG_FEED_ME;
        case _RXG_END_OF_INPUT:
        stack->current_sym_ = RXG_INPUT_END;
        stack->need_sym_ = 0;
        if (stack->mute_error_turns_) stack->mute_error_turns_--;
        break;
        case _RXG_LEXICAL_ERROR:
        stack->continue_at_ = 0;
        /* Lexical error */
        return _RXG_LEXICAL_ERROR;
      } /* switch */
    } /* if (need_sym_) */
    else {
      int sym = stack->current_sym_;
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
          
          /* Fill in the sym from the tokenizer */
          stack->need_sym_ = 1;
          stack->stack_[stack->pos_ - 1].common_ = stack->stack_[0].common_;
          stack->slot_0_has_current_sym_data_ = 0;
          stack->slot_0_has_common_data_ = 0;
          stack->top_of_stack_has_sym_data_ = 1;
          stack->top_of_stack_has_common_data_ = 1;
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
                stack->continue_at_ = 4;
                {
                   snippet_init(&(stack->stack_[1].common_));
                }
                C4:;
              }
              break;
              /* grammar: grammar pattern */
              case 2: {
                stack->slot_1_has_common_data_ = 1;
                stack->continue_at_ = 5;
                {
                   snippet_init(&(stack->stack_[1].common_));
                }
                C5:;
              }
              break;
              /* pattern: IDENT start-regex COLON exp end-regex SEMICOLON */
              case 3: {
                stack->slot_1_has_common_data_ = 1;
                stack->continue_at_ = 6;
                {
                   snippet_init(&(stack->stack_[1].common_));
                }
                C6:;
                stack->continue_at_ = 7;
                {
                  {
  r = prd_grammar_check_pattern_reserve(g);
  if (r) return r;
  char *regex = snippet_dup_xlt(&(stack->sym_data_[3].common_));
  if (!regex) {
    return PRD_INTERNAL_ERROR;
  }
  pat = g->patterns_ + g->num_patterns_++;
  prd_pattern_init(pat);
  pat->regex_ = regex;
  r = snippet_append_to_xlts(&pat->term_.id_, &(stack->sym_data_[0].common_));
  if (r) return r;
  r = snippet_append_snippet(&pat->common_action_sequence_, &g->current_common_pattern_action_sequence_);
  if (r) return r;
}
                }
                C7:;
              }
              break;
              /* pattern: IDENT start-regex COLON exp end-regex start-c-tokenizer accept-whitespace CUR_OPEN action-sequence end-c-tokenizer CUR_CLOSE */
              case 4: {
                stack->slot_1_has_common_data_ = 1;
                stack->continue_at_ = 8;
                {
                   snippet_init(&(stack->stack_[1].common_));
                }
                C8:;
                stack->continue_at_ = 9;
                {
                  {
  r = prd_grammar_check_pattern_reserve(g);
  if (r) return r;
  char *regex = snippet_dup_xlt(&(stack->sym_data_[3].common_));
  if (!regex) {
    return PRD_INTERNAL_ERROR;
  }
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
                C9:;
              }
              break;
              /* pattern: IDENT start-regex COLON exp end-regex start-c-tokenizer stmt-action end-c-tokenizer SEMICOLON */
              case 5: {
                stack->slot_1_has_common_data_ = 1;
                stack->continue_at_ = 10;
                {
                   snippet_init(&(stack->stack_[1].common_));
                }
                C10:;
                stack->continue_at_ = 11;
                {
                  {
  r = prd_grammar_check_pattern_reserve(g);
  if (r) return r;
  char *regex = snippet_dup_xlt(&(stack->sym_data_[3].common_));
  if (!regex) {
    return PRD_INTERNAL_ERROR;
  }
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
                C11:;
              }
              break;
              /* pattern: start-regex COLON exp end-regex SEMICOLON */
              case 6: {
                stack->slot_1_has_common_data_ = 1;
                stack->continue_at_ = 12;
                {
                   snippet_init(&(stack->stack_[1].common_));
                }
                C12:;
                stack->continue_at_ = 13;
                {
                  {
  r = prd_grammar_check_pattern_reserve(g);
  if (r) return r;
  char *regex = snippet_dup_xlt(&(stack->sym_data_[2].common_));
  if (!regex) {
    return PRD_INTERNAL_ERROR;
  }
  pat = g->patterns_ + g->num_patterns_++;
  prd_pattern_init(pat);
  pat->regex_ = regex;
  r = snippet_append_snippet(&pat->common_action_sequence_, &g->current_common_pattern_action_sequence_);
  if (r) return r;
}
                }
                C13:;
              }
              break;
              /* pattern: start-regex COLON exp end-regex start-c-tokenizer accept-whitespace CUR_OPEN action-sequence end-c-tokenizer CUR_CLOSE */
              case 7: {
                stack->slot_1_has_common_data_ = 1;
                stack->continue_at_ = 14;
                {
                   snippet_init(&(stack->stack_[1].common_));
                }
                C14:;
                stack->continue_at_ = 15;
                {
                  {
  r = prd_grammar_check_pattern_reserve(g);
  if (r) return r;
  char *regex = snippet_dup_xlt(&(stack->sym_data_[2].common_));
  if (!regex) {
    return PRD_INTERNAL_ERROR;
  }
  pat = g->patterns_ + g->num_patterns_++;
  prd_pattern_init(pat);
  pat->regex_ = regex;
  r = snippet_append_snippet(&pat->action_sequence_, &(stack->sym_data_[7].common_));
  if (r) return r;
  r = snippet_append_snippet(&pat->common_action_sequence_, &g->current_common_pattern_action_sequence_);
  if (r) return r;
}
                }
                C15:;
              }
              break;
              /* pattern: start-regex COLON exp end-regex start-c-tokenizer stmt-action end-c-tokenizer SEMICOLON */
              case 8: {
                stack->slot_1_has_common_data_ = 1;
                stack->continue_at_ = 16;
                {
                   snippet_init(&(stack->stack_[1].common_));
                }
                C16:;
                stack->continue_at_ = 17;
                {
                  {
  r = prd_grammar_check_pattern_reserve(g);
  if (r) return r;
  char *regex = snippet_dup_xlt(&(stack->sym_data_[2].common_));
  if (!regex) {
    return PRD_INTERNAL_ERROR;
  }
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
                C17:;
              }
              break;
              /* pattern: DOLLAR COLON start-c-tokenizer accept-whitespace CUR_OPEN action-sequence end-c-tokenizer CUR_CLOSE */
              case 9: {
                stack->slot_1_has_common_data_ = 1;
                stack->continue_at_ = 18;
                {
                   snippet_init(&(stack->stack_[1].common_));
                }
                C18:;
                stack->continue_at_ = 19;
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
                C19:;
              }
              break;
              /* exp: term */
              case 10: {
                stack->slot_1_has_common_data_ = 1;
                stack->continue_at_ = 20;
                {
                   snippet_init(&(stack->stack_[1].common_));
                }
                C20:;
                stack->continue_at_ = 21;
                {
                  {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
                }
                C21:;
              }
              break;
              /* exp: exp BAR term */
              case 11: {
                stack->slot_1_has_common_data_ = 1;
                stack->continue_at_ = 22;
                {
                   snippet_init(&(stack->stack_[1].common_));
                }
                C22:;
                stack->continue_at_ = 23;
                {
                  {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
                }
                C23:;
              }
              break;
              /* term: */
              case 12: {
                stack->slot_1_has_common_data_ = 1;
                stack->continue_at_ = 24;
                {
                   snippet_init(&(stack->stack_[1].common_));
                }
                C24:;
                stack->continue_at_ = 25;
                {
                  {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
                }
                C25:;
              }
              break;
              /* term: term elm */
              case 13: {
                stack->slot_1_has_common_data_ = 1;
                stack->continue_at_ = 26;
                {
                   snippet_init(&(stack->stack_[1].common_));
                }
                C26:;
                stack->continue_at_ = 27;
                {
                  {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
                }
                C27:;
              }
              break;
              /* elm: sym */
              case 14: {
                stack->slot_1_has_common_data_ = 1;
                stack->continue_at_ = 28;
                {
                   snippet_init(&(stack->stack_[1].common_));
                }
                C28:;
                stack->continue_at_ = 29;
                {
                  {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
                }
                C29:;
              }
              break;
              /* elm: sym ASTERISK */
              case 15: {
                stack->slot_1_has_common_data_ = 1;
                stack->continue_at_ = 30;
                {
                   snippet_init(&(stack->stack_[1].common_));
                }
                C30:;
                stack->continue_at_ = 31;
                {
                  {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
                }
                C31:;
              }
              break;
              /* elm: sym PLUS */
              case 16: {
                stack->slot_1_has_common_data_ = 1;
                stack->continue_at_ = 32;
                {
                   snippet_init(&(stack->stack_[1].common_));
                }
                C32:;
                stack->continue_at_ = 33;
                {
                  {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
                }
                C33:;
              }
              break;
              /* elm: sym QUESTION_MARK */
              case 17: {
                stack->slot_1_has_common_data_ = 1;
                stack->continue_at_ = 34;
                {
                   snippet_init(&(stack->stack_[1].common_));
                }
                C34:;
                stack->continue_at_ = 35;
                {
                  {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
                }
                C35:;
              }
              break;
              /* sym: CHAR */
              case 18: {
                stack->slot_1_has_common_data_ = 1;
                stack->continue_at_ = 36;
                {
                   snippet_init(&(stack->stack_[1].common_));
                }
                C36:;
                stack->continue_at_ = 37;
                {
                  {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
                }
                C37:;
              }
              break;
              /* sym: DOT */
              case 19: {
                stack->slot_1_has_common_data_ = 1;
                stack->continue_at_ = 38;
                {
                   snippet_init(&(stack->stack_[1].common_));
                }
                C38:;
                stack->continue_at_ = 39;
                {
                  {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
                }
                C39:;
              }
              break;
              /* sym: CARET */
              case 20: {
                stack->slot_1_has_common_data_ = 1;
                stack->continue_at_ = 40;
                {
                   snippet_init(&(stack->stack_[1].common_));
                }
                C40:;
                stack->continue_at_ = 41;
                {
                  {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
                }
                C41:;
              }
              break;
              /* sym: DOLLAR */
              case 21: {
                stack->slot_1_has_common_data_ = 1;
                stack->continue_at_ = 42;
                {
                   snippet_init(&(stack->stack_[1].common_));
                }
                C42:;
                stack->continue_at_ = 43;
                {
                  {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
                }
                C43:;
              }
              break;
              /* sym: PAR_OPEN exp PAR_CLOSE */
              case 22: {
                stack->slot_1_has_common_data_ = 1;
                stack->continue_at_ = 44;
                {
                   snippet_init(&(stack->stack_[1].common_));
                }
                C44:;
                stack->continue_at_ = 45;
                {
                  {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
                }
                C45:;
              }
              break;
              /* sym: SQ_OPEN range SQ_CLOSE */
              case 23: {
                stack->slot_1_has_common_data_ = 1;
                stack->continue_at_ = 46;
                {
                   snippet_init(&(stack->stack_[1].common_));
                }
                C46:;
                stack->continue_at_ = 47;
                {
                  {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
                }
                C47:;
              }
              break;
              /* sym: SQ_OPEN CARET range SQ_CLOSE */
              case 24: {
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
              }
              break;
              /* range: range range-elm */
              case 25: {
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
              }
              break;
              /* range: range-elm */
              case 26: {
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
              }
              break;
              /* range-elm: CHAR */
              case 27: {
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
              }
              break;
              /* range-elm: CHAR DASH CHAR */
              case 28: {
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
              }
              break;
              /* action-sequence: */
              case 29: {
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
              }
              break;
              /* action-sequence: action-sequence CHAR */
              case 30: {
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
              }
              break;
              /* action-sequence: action-sequence IDENT */
              case 31: {
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
              }
              break;
              /* action-sequence: action-sequence COLON */
              case 32: {
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
              }
              break;
              /* action-sequence: action-sequence SEMICOLON */
              case 33: {
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
              }
              break;
              /* action-sequence: action-sequence SQ_OPEN */
              case 34: {
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
              }
              break;
              /* action-sequence: action-sequence SQ_CLOSE */
              case 35: {
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
              }
              break;
              /* action-sequence: action-sequence DOT */
              case 36: {
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
              }
              break;
              /* action-sequence: action-sequence DOLLAR */
              case 37: {
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
              }
              break;
              /* action-sequence: action-sequence CARET */
              case 38: {
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
              }
              break;
              /* action-sequence: action-sequence DASH */
              case 39: {
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
              }
              break;
              /* action-sequence: action-sequence BAR */
              case 40: {
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
              }
              break;
              /* action-sequence: action-sequence ASTERISK */
              case 41: {
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
              }
              break;
              /* action-sequence: action-sequence PLUS */
              case 42: {
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
              }
              break;
              /* action-sequence: action-sequence QUESTION_MARK */
              case 43: {
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
              }
              break;
              /* action-sequence: action-sequence TOKEN */
              case 44: {
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
              }
              break;
              /* action-sequence: action-sequence WHITESPACE */
              case 45: {
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
              }
              break;
              /* action-sequence: action-sequence PAR_OPEN action-sequence PAR_CLOSE */
              case 46: {
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
              }
              break;
              /* action-sequence: action-sequence CUR_OPEN action-sequence CUR_CLOSE */
              case 47: {
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
              }
              break;
              /* start-regex: */
              case 48: {
                stack->slot_1_has_common_data_ = 1;
                stack->continue_at_ = 96;
                {
                   snippet_init(&(stack->stack_[1].common_));
                }
                C96:;
                stack->continue_at_ = 97;
                {
                  {
  tok_switch_to_regex(tkr);
}
                }
                C97:;
              }
              break;
              /* end-regex: */
              case 49: {
                stack->slot_1_has_common_data_ = 1;
                stack->continue_at_ = 98;
                {
                   snippet_init(&(stack->stack_[1].common_));
                }
                C98:;
                stack->continue_at_ = 99;
                {
                  {
  tok_switch_to_nonterminal_idents(tkr);
}
                }
                C99:;
              }
              break;
              /* start-c-tokenizer: */
              case 50: {
                stack->slot_1_has_common_data_ = 1;
                stack->continue_at_ = 100;
                {
                   snippet_init(&(stack->stack_[1].common_));
                }
                C100:;
                stack->continue_at_ = 101;
                {
                  {
  tok_switch_to_c_idents(tkr);
}
                }
                C101:;
              }
              break;
              /* end-c-tokenizer: */
              case 51: {
                stack->slot_1_has_common_data_ = 1;
                stack->continue_at_ = 102;
                {
                   snippet_init(&(stack->stack_[1].common_));
                }
                C102:;
                stack->continue_at_ = 103;
                {
                  {
  g->accept_whitespace_ = 0; /* ignore whitespace */
  tok_switch_to_nonterminal_idents(tkr);
}
                }
                C103:;
              }
              break;
              /* accept-whitespace: */
              case 52: {
                stack->slot_1_has_common_data_ = 1;
                stack->continue_at_ = 104;
                {
                   snippet_init(&(stack->stack_[1].common_));
                }
                C104:;
                stack->continue_at_ = 105;
                {
                  {
  g->accept_whitespace_ = 1;
}
                }
                C105:;
              }
              break;
            } /* switch */
          } /* scope guard */
          
          /* Free symdata for every symbol in the production, including the first slot where we will soon
          * push nonterminal_data_reduced_to */
          size_t rxg_sym_idx;
          for (rxg_sym_idx = stack->pos_ - stack->current_production_length_; rxg_sym_idx < stack->pos_; ++rxg_sym_idx) {
            switch (stack->stack_[rxg_sym_idx].state_) {
            } /* switch */
            stack->continue_at_ = 106;
            {
               snippet_cleanup(&((stack->stack_ + rxg_sym_idx)->common_));
            }
            C106:;
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
          stack->top_of_stack_has_sym_data_ = 1;
          stack->top_of_stack_has_common_data_ = 1;
        } /* action < 0 */
        else /* action == 0 */ {
          /* check if we can recover using an error token. */
          size_t n;
          for (n = 0; n < stack->pos_; ++n) {
            int err_action = rxg_parse_table[rxg_num_columns * stack->stack_[n].state_ + (23 /* error token */ - rxg_minimum_sym)];
            if (err_action > 0) {
              /* we can transition on the error token somewhere on the stack */
              break;
            }
          }
          if (n != stack->pos_) {
            /* Enter error-token recovery mode given that such a recovery is possible */
            stack->error_recovery_ = (n != stack->pos_);
          }
          else {
            if (sym != RXG_INPUT_END) {
              /* Retain EOF but discard any other sym so we make progress */
              stack->need_sym_ = 1;
              /* Deconstruct placeholder location for discarded symbol */
              switch (stack->current_sym_) {
              } /* switch */
              stack->continue_at_ = 107;
              {
                 snippet_cleanup(&(stack->stack_[0].common_));
              }
              C107:;
            }
          }
          /* Issue the error here */
          if (!stack->mute_error_turns_) {
            stack->mute_error_turns_ = 3;
            stack->continue_at_ = 0;
            {
              /* Syntax error */ \
              if (sym != PRD_INPUT_END) {\
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
            int err_action = rxg_parse_table[rxg_num_columns * stack->stack_[n].state_ + (23 /* error token */ - rxg_minimum_sym)];
            if (err_action > 0) {
              /* Does the resulting state accept the current symbol? */
              int err_sym_action = rxg_parse_table[rxg_num_columns * err_action + (sym - rxg_minimum_sym)];
              if (err_sym_action) {
                /* Current symbol is accepted, recover error condition by shifting the error token and then process the symbol as usual */
                /* Free symdata for every symbol up to the state where we will shift the error token */
                size_t rxg_sym_idx;
                for (rxg_sym_idx = n + 1; rxg_sym_idx < stack->pos_; ++rxg_sym_idx) {
                  switch (stack->stack_[rxg_sym_idx].state_) {
                  } /* switch */
                  stack->continue_at_ = 108;
                  {
                     snippet_cleanup(&((stack->stack_ + rxg_sym_idx)->common_));
                  }
                  C108:;
                } /* for */
                stack->pos_ = n + 1;
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
          if (stack->current_sym_ == RXG_INPUT_END) {
            /* EOF means we cannot shift to recover, so return completion */
            stack->pending_reset_ = 1;
            return PRD_SUCCESS;
          }
          stack->need_sym_ = 1;
          return PRD_NEXT;
          /* Deconstruct placeholder location for discarded symbol */
          switch (stack->current_sym_) {
          } /* switch */
          stack->continue_at_ = 109;
          {
             snippet_cleanup(&(stack->stack_[0].common_));
          }
          C109:;
        }
      } /* stack->error_recovery_ */
    }
  } /* for (;;) */
}

int rxg_parse(struct rxg_stack *stack, int sym, struct prd_grammar *g, struct tkr_tokenizer *tkr, struct symbol_table *st, char char_value) {
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
            /* pattern: IDENT start-regex COLON exp end-regex SEMICOLON */
            case 3: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 5;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C5:;
              stack->continue_at_ = 6;
              {
                {
  r = prd_grammar_check_pattern_reserve(g);
  if (r) return r;
  char *regex = snippet_dup_xlt(&(stack->sym_data_[3].common_));
  if (!regex) {
    return PRD_INTERNAL_ERROR;
  }
  pat = g->patterns_ + g->num_patterns_++;
  prd_pattern_init(pat);
  pat->regex_ = regex;
  r = snippet_append_to_xlts(&pat->term_.id_, &(stack->sym_data_[0].common_));
  if (r) return r;
  r = snippet_append_snippet(&pat->common_action_sequence_, &g->current_common_pattern_action_sequence_);
  if (r) return r;
}
              }
              C6:;
            }
            break;
            /* pattern: IDENT start-regex COLON exp end-regex start-c-tokenizer accept-whitespace CUR_OPEN action-sequence end-c-tokenizer CUR_CLOSE */
            case 4: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 7;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C7:;
              stack->continue_at_ = 8;
              {
                {
  r = prd_grammar_check_pattern_reserve(g);
  if (r) return r;
  char *regex = snippet_dup_xlt(&(stack->sym_data_[3].common_));
  if (!regex) {
    return PRD_INTERNAL_ERROR;
  }
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
              C8:;
            }
            break;
            /* pattern: IDENT start-regex COLON exp end-regex start-c-tokenizer stmt-action end-c-tokenizer SEMICOLON */
            case 5: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 9;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C9:;
              stack->continue_at_ = 10;
              {
                {
  r = prd_grammar_check_pattern_reserve(g);
  if (r) return r;
  char *regex = snippet_dup_xlt(&(stack->sym_data_[3].common_));
  if (!regex) {
    return PRD_INTERNAL_ERROR;
  }
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
              C10:;
            }
            break;
            /* pattern: start-regex COLON exp end-regex SEMICOLON */
            case 6: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 11;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C11:;
              stack->continue_at_ = 12;
              {
                {
  r = prd_grammar_check_pattern_reserve(g);
  if (r) return r;
  char *regex = snippet_dup_xlt(&(stack->sym_data_[2].common_));
  if (!regex) {
    return PRD_INTERNAL_ERROR;
  }
  pat = g->patterns_ + g->num_patterns_++;
  prd_pattern_init(pat);
  pat->regex_ = regex;
  r = snippet_append_snippet(&pat->common_action_sequence_, &g->current_common_pattern_action_sequence_);
  if (r) return r;
}
              }
              C12:;
            }
            break;
            /* pattern: start-regex COLON exp end-regex start-c-tokenizer accept-whitespace CUR_OPEN action-sequence end-c-tokenizer CUR_CLOSE */
            case 7: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 13;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C13:;
              stack->continue_at_ = 14;
              {
                {
  r = prd_grammar_check_pattern_reserve(g);
  if (r) return r;
  char *regex = snippet_dup_xlt(&(stack->sym_data_[2].common_));
  if (!regex) {
    return PRD_INTERNAL_ERROR;
  }
  pat = g->patterns_ + g->num_patterns_++;
  prd_pattern_init(pat);
  pat->regex_ = regex;
  r = snippet_append_snippet(&pat->action_sequence_, &(stack->sym_data_[7].common_));
  if (r) return r;
  r = snippet_append_snippet(&pat->common_action_sequence_, &g->current_common_pattern_action_sequence_);
  if (r) return r;
}
              }
              C14:;
            }
            break;
            /* pattern: start-regex COLON exp end-regex start-c-tokenizer stmt-action end-c-tokenizer SEMICOLON */
            case 8: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 15;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C15:;
              stack->continue_at_ = 16;
              {
                {
  r = prd_grammar_check_pattern_reserve(g);
  if (r) return r;
  char *regex = snippet_dup_xlt(&(stack->sym_data_[2].common_));
  if (!regex) {
    return PRD_INTERNAL_ERROR;
  }
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
              C16:;
            }
            break;
            /* pattern: DOLLAR COLON start-c-tokenizer accept-whitespace CUR_OPEN action-sequence end-c-tokenizer CUR_CLOSE */
            case 9: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 17;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C17:;
              stack->continue_at_ = 18;
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
              C18:;
            }
            break;
            /* exp: term */
            case 10: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 19;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C19:;
              stack->continue_at_ = 20;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C20:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* exp: exp BAR term */
            case 11: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 21;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C21:;
              stack->continue_at_ = 22;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C22:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* term: */
            case 12: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 23;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C23:;
              stack->continue_at_ = 24;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C24:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* term: term elm */
            case 13: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 25;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C25:;
              stack->continue_at_ = 26;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C26:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* elm: sym */
            case 14: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 27;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C27:;
              stack->continue_at_ = 28;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C28:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* elm: sym ASTERISK */
            case 15: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 29;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C29:;
              stack->continue_at_ = 30;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C30:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* elm: sym PLUS */
            case 16: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 31;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C31:;
              stack->continue_at_ = 32;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C32:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* elm: sym QUESTION_MARK */
            case 17: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 33;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C33:;
              stack->continue_at_ = 34;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C34:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* sym: CHAR */
            case 18: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 35;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C35:;
              stack->continue_at_ = 36;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C36:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* sym: DOT */
            case 19: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 37;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C37:;
              stack->continue_at_ = 38;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C38:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* sym: CARET */
            case 20: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 39;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C39:;
              stack->continue_at_ = 40;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C40:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* sym: DOLLAR */
            case 21: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 41;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C41:;
              stack->continue_at_ = 42;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C42:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* sym: PAR_OPEN exp PAR_CLOSE */
            case 22: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 43;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C43:;
              stack->continue_at_ = 44;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C44:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* sym: SQ_OPEN range SQ_CLOSE */
            case 23: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 45;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C45:;
              stack->continue_at_ = 46;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C46:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* sym: SQ_OPEN CARET range SQ_CLOSE */
            case 24: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 47;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C47:;
              stack->continue_at_ = 48;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C48:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* range: range range-elm */
            case 25: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 49;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C49:;
              stack->continue_at_ = 50;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C50:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* range: range-elm */
            case 26: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 51;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C51:;
              stack->continue_at_ = 52;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C52:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* range-elm: CHAR */
            case 27: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 53;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C53:;
              stack->continue_at_ = 54;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C54:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* range-elm: CHAR DASH CHAR */
            case 28: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 55;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C55:;
              stack->continue_at_ = 56;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C56:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* action-sequence: */
            case 29: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 57;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C57:;
              stack->continue_at_ = 58;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C58:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* action-sequence: action-sequence CHAR */
            case 30: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 59;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C59:;
              stack->continue_at_ = 60;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C60:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* action-sequence: action-sequence IDENT */
            case 31: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 61;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C61:;
              stack->continue_at_ = 62;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C62:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* action-sequence: action-sequence COLON */
            case 32: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 63;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C63:;
              stack->continue_at_ = 64;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C64:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* action-sequence: action-sequence SEMICOLON */
            case 33: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 65;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C65:;
              stack->continue_at_ = 66;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C66:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* action-sequence: action-sequence SQ_OPEN */
            case 34: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 67;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C67:;
              stack->continue_at_ = 68;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C68:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* action-sequence: action-sequence SQ_CLOSE */
            case 35: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 69;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C69:;
              stack->continue_at_ = 70;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C70:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* action-sequence: action-sequence DOT */
            case 36: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 71;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C71:;
              stack->continue_at_ = 72;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C72:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* action-sequence: action-sequence DOLLAR */
            case 37: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 73;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C73:;
              stack->continue_at_ = 74;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C74:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* action-sequence: action-sequence CARET */
            case 38: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 75;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C75:;
              stack->continue_at_ = 76;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C76:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* action-sequence: action-sequence DASH */
            case 39: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 77;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C77:;
              stack->continue_at_ = 78;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C78:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* action-sequence: action-sequence BAR */
            case 40: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 79;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C79:;
              stack->continue_at_ = 80;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C80:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* action-sequence: action-sequence ASTERISK */
            case 41: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 81;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C81:;
              stack->continue_at_ = 82;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C82:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* action-sequence: action-sequence PLUS */
            case 42: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 83;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C83:;
              stack->continue_at_ = 84;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C84:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* action-sequence: action-sequence QUESTION_MARK */
            case 43: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 85;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C85:;
              stack->continue_at_ = 86;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C86:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* action-sequence: action-sequence TOKEN */
            case 44: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 87;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C87:;
              stack->continue_at_ = 88;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C88:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* action-sequence: action-sequence WHITESPACE */
            case 45: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 89;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C89:;
              stack->continue_at_ = 90;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C90:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* action-sequence: action-sequence PAR_OPEN action-sequence PAR_CLOSE */
            case 46: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 91;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C91:;
              stack->continue_at_ = 92;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C92:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* action-sequence: action-sequence CUR_OPEN action-sequence CUR_CLOSE */
            case 47: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 93;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C93:;
              stack->continue_at_ = 94;
              {
                {
  size_t n;
  for (n = 0; n < (stack->current_production_length_); ++n) {
    snippet_append_snippet(&(stack->stack_[1].common_), &(stack->sym_data_[n].common_));
  }
}
              }
              C94:;
              if (!stack->discard_remaining_actions_) {
                            }
            }
            break;
            /* start-regex: */
            case 48: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 95;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C95:;
              stack->continue_at_ = 96;
              {
                {
  tok_switch_to_regex(tkr);
}
              }
              C96:;
            }
            break;
            /* end-regex: */
            case 49: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 97;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C97:;
              stack->continue_at_ = 98;
              {
                {
  tok_switch_to_nonterminal_idents(tkr);
}
              }
              C98:;
            }
            break;
            /* start-c-tokenizer: */
            case 50: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 99;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C99:;
              stack->continue_at_ = 100;
              {
                {
  tok_switch_to_c_idents(tkr);
}
              }
              C100:;
            }
            break;
            /* end-c-tokenizer: */
            case 51: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 101;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C101:;
              stack->continue_at_ = 102;
              {
                {
  g->accept_whitespace_ = 0; /* ignore whitespace */
  tok_switch_to_nonterminal_idents(tkr);
}
              }
              C102:;
            }
            break;
            /* accept-whitespace: */
            case 52: {
              stack->slot_1_has_common_data_ = 1;
              stack->continue_at_ = 103;
              {
                 snippet_init(&(stack->stack_[1].common_));
              }
              C103:;
              stack->continue_at_ = 104;
              {
                {
  g->accept_whitespace_ = 1;
}
              }
              C104:;
            }
            break;
          } /* switch */
        } /* scope guard */
        
        /* Free symdata for every symbol in the production, including the first slot where we will soon
        * push nonterminal_data_reduced_to */
        size_t rxg_sym_idx;
        for (rxg_sym_idx = stack->pos_ - stack->current_production_length_; rxg_sym_idx < stack->pos_; ++rxg_sym_idx) {
          switch (stack->stack_[rxg_sym_idx].state_) {
          } /* switch */
          stack->continue_at_ = 105;
          {
             snippet_cleanup(&((stack->stack_ + rxg_sym_idx)->common_));
          }
          C105:;
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
          int err_action = rxg_parse_table[rxg_num_columns * stack->stack_[n].state_ + (23 /* error token */ - rxg_minimum_sym)];
          if (err_action > 0) {
            /* we can transition on the error token somewhere on the stack */
            break;
          }
        }
        if (n != stack->pos_) {
          /* Enter error-token recovery mode given that such a recovery is possible */
          stack->error_recovery_ = (n != stack->pos_);
        }
        else {
          if (sym != RXG_INPUT_END) {
            /* Retain EOF but discard any other sym so we make progress */
            stack->continue_at_ = 0;
            {
              return PRD_NEXT;
              
            }
          }
        }
        /* Issue the error here */
        if (!stack->mute_error_turns_) {
          stack->mute_error_turns_ = 3;
          stack->continue_at_ = 0;
          {
            /* Syntax error */ \
            if (sym != PRD_INPUT_END) {\
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
          int err_action = rxg_parse_table[rxg_num_columns * stack->stack_[n].state_ + (23 /* error token */ - rxg_minimum_sym)];
          if (err_action > 0) {
            /* Does the resulting state accept the current symbol? */
            int err_sym_action = rxg_parse_table[rxg_num_columns * err_action + (sym - rxg_minimum_sym)];
            if (err_sym_action) {
              /* Current symbol is accepted, recover error condition by shifting the error token and then process the symbol as usual */
              /* Free symdata for every symbol up to the state where we will shift the error token */
              size_t rxg_sym_idx;
              for (rxg_sym_idx = n + 1; rxg_sym_idx < stack->pos_; ++rxg_sym_idx) {
                switch (stack->stack_[rxg_sym_idx].state_) {
                } /* switch */
                stack->continue_at_ = 106;
                {
                   snippet_cleanup(&((stack->stack_ + rxg_sym_idx)->common_));
                }
                C106:;
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

