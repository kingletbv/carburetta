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

#ifndef SNIPPET_H_INCLUDED
#define SNIPPET_H_INCLUDED
#include "snippet.h"
#endif








/* --------- START OF GENERATED CODE ------------ */
#include <stdlib.h> /* realloc(), free(), NULL, size_t */
#include <string.h> /* memcpy() */
struct rxg_sym_data {
  int state_;
  struct snippet common_;
};
static const size_t rxg_scan_table[] = {
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 4, 4, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 6, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
   4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 4, 4, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 6, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
   4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 4, 4, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 6, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
   7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 8, 7, 7, 8, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 9, 7, 7, 7, 7,10, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
   4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 4, 4, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 6, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
   4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 4, 4, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 6, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
   7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 8, 7, 7, 8, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 9, 7, 7, 7, 7, 0, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
static const size_t rxg_scan_actions[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 };
static const int rxg_minimum_sym = 3;
static const size_t rxg_num_columns = 38;
static const size_t rxg_num_rows = 80;
static const size_t rxg_num_productions = 52;
static const int rxg_parse_table[] = {
  0, -2,  0,  0,  0,  0, -2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,-2, 0, 0, 0, 0, 0, 0,79, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, -3,  0,  0,  0,  0, -3,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,-3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, -4,  0,  0,  0,  0, -4,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,-4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, -5,  0,  0,  0,  0, -5,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,-5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0,  0,  0,  0,  0,  3,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 20, 21, 38,  0, 41,-51, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0,
-29,-29,-29,  0,-29,-29,-29,-29,-29,-29,-29,-29,-29,-29,-29,-29,-29,-29,-29,-29, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0,
  0,  0,  0,  0,  6,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, -6,  0,  0,  0,  0, -6,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,-6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0,  0,  0,  0,  0,  0,  0,  8,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0,  0,  0,  0,  0,  0,  0,-51,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 0, 0,
  0,  0,  0,  0,-52,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,10, 0, 0, 0, 0, 7, 0,
  0,  0,  0,  0,-50,  0,  0,  2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,11, 0, 0, 0,
  0,  0,  0,  0,-49,  0,  0,-49,  0,  0,  0,  0,  0,  0, 61,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,12, 0, 0, 0, 0,
-12,  0,-12,  0,-12,  0,  0,-12,-12,  0,-12,-12,-12,  0,-12,  0,  0,  0,  0,  0, 0, 0,13,76, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0,  0,  0,  0,  0,  0, 14,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0,  0,  0,  0,  0,  0,-48,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,15, 0, 0, 0, 0, 0,
  0, -7,  0,  0,  0,  0, -7,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,-7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, -8,  0,  0,  0,  0, -8,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,-8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0,  0,  0,  0,  0, 18,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
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
-44,-44,-44,-44,-44,-44,-44,-44,-44,-44,-44,-44,-44,-44,-44,-44,-44,-44,-44,-44, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
-45,-45,-45,-45,-45,-45,-45,-45,-45,-45,-45,-45,-45,-45,-45,-45,-45,-45,-45,-45, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
-46,-46,-46,-46,-46,-46,-46,-46,-46,-46,-46,-46,-46,-46,-46,-46,-46,-46,-46,-46, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 20, 21, 38, 36, 41,  0, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
-29,-29,-29,-29,-29,  0,-29,-29,-29,-29,-29,-29,-29,-29,-29,-29,-29,-29,-29,-29, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,37, 0, 0, 0, 0, 0, 0, 0,
-47,-47,-47,-47,-47,-47,-47,-47,-47,-47,-47,-47,-47,-47,-47,-47,-47,-47,-47,-47, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 20, 21, 38,  0, 41, 39, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
-29,-29,-29,  0,-29,-29,-29,-29,-29,-29,-29,-29,-29,-29,-29,-29,-29,-29,-29,-29, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,40, 0, 0, 0, 0, 0, 0, 0,
 20, 21, 38,  0, 41,-51, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,19, 0, 0,
-29,-29,-29,  0,-29,-29,-29,-29,-29,-29,-29,-29,-29,-29,-29,-29,-29,-29,-29,-29, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,42, 0, 0, 0, 0, 0, 0, 0,
  0,  0,  0,  0, 43,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, -9,  0,  0,  0,  0, -9,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,-9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0,  0,  0,  0,  0,  0,  0, 45,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0,  0,  0,  0,  0,  0,  0,-51,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,46, 0, 0,
  0,  0,  0,  0,-52,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,47, 0, 0, 0, 0,44, 0,
  0,  0,  0,  0,-50,  0,  0, 17,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,48, 0, 0, 0,
  0,  0,  0,  0,-49,  0,  0,-49,  0,  0,  0,  0,  0,  0, 61,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,49, 0, 0, 0, 0,
-13,  0,-13,-13,-13,  0,  0,-13,-13,  0,-13,-13,-13,  0,-13,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
-15,  0,-15,-15,-15,  0,  0,-15,-15,  0,-15,-15,-15,  0,-15,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
-16,  0,-16,-16,-16,  0,  0,-16,-16,  0,-16,-16,-16,  0,-16,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
-17,  0,-17,-17,-17,  0,  0,-17,-17,  0,-17,-17,-17,  0,-17,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
-14,  0,-14,-14,-14,  0,  0,-14,-14,  0,-14,-14,-14,  0,-14, 52, 53, 54,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
-18,  0,-18,-18,-18,  0,  0,-18,-18,  0,-18,-18,-18,  0,-18,-18,-18,-18,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
-19,  0,-19,-19,-19,  0,  0,-19,-19,  0,-19,-19,-19,  0,-19,-19,-19,-19,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
-20,  0,-20,-20,-20,  0,  0,-20,-20,  0,-20,-20,-20,  0,-20,-20,-20,-20,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
-21,  0,-21,-21,-21,  0,  0,-21,-21,  0,-21,-21,-21,  0,-21,-21,-21,-21,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 56,  0, 64,-11,-11,  0,  0,-11, 75,  0, 57, 59, 58,  0,-11,  0,  0,  0,  0,  0, 0, 0, 0, 0,51,55, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
-12,  0,-12,-12,-12,  0,  0,-12,-12,  0,-12,-12,-12,  0,-12,  0,  0,  0,  0,  0, 0, 0, 0,60, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
-22,  0,-22,-22,-22,  0,  0,-22,-22,  0,-22,-22,-22,  0,-22,-22,-22,-22,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0,  0,  0, 62,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 61,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
-12,  0,-12,-12,  0,  0,  0,  0,-12,  0,-12,-12,-12,  0,-12,  0,  0,  0,  0,  0, 0, 0,63,76, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
-23,  0,-23,-23,-23,  0,  0,-23,-23,  0,-23,-23,-23,  0,-23,-23,-23,-23,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 73,  0,  0,  0,  0,  0,  0,  0,  0, 65,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0,68, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
-24,  0,-24,-24,-24,  0,  0,-24,-24,  0,-24,-24,-24,  0,-24,-24,-24,-24,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
-25,  0,  0,  0,  0,  0,  0,  0,  0,-25,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 73,  0,  0,  0,  0,  0,  0,  0,  0, 67,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0,68, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
-26,  0,  0,  0,  0,  0,  0,  0,  0,-26,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
-28,  0,  0,  0,  0,  0,  0,  0,  0,-28,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 71,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
-27,  0,  0,  0,  0,  0,  0,  0,  0,-27,  0,  0,  0, 72,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 73,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0,69,70, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 73,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 74,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0,66,70, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 56,  0, 64,-10,-10,  0,  0,-10, 75,  0, 57, 59, 58,  0,-10,  0,  0,  0,  0,  0, 0, 0, 0, 0,51,55, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
-12,  0,-12,  0,-12,  0,  0,-12,-12,  0,-12,-12,-12,  0,-12,  0,  0,  0,  0,  0, 0, 0,50,76, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0,  0,  0,  0,  0,  0, 77,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 16,  0,  0,  0,  0,-48,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0,-1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,78, 0, 0, 0, 0, 0,
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
 33,
 7,
 39,
 10,
 38,
 34,
 37,
 36,
 25,
 9,
 35,
 4,
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
 31
};

#ifndef CARB_RXG_SRCREGEX_GRAMMAR_H_INCLUDED
struct rxg_stack {
  int error_recovery_:1;
  int report_error_:1;
  int need_sym_:1;
  int current_sym_;
  int mute_error_turns_;
  size_t pos_, num_stack_allocated_;
  struct rxg_sym_data *stack_;
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
#define _RXG_MATCH 1
#define _RXG_OVERFLOW 2
#define _RXG_NO_MEMORY 3
#define _RXG_FEED_ME 4
#define _RXG_END_OF_INPUT 5
#define _RXG_SYNTAX_ERROR 6
#define _RXG_INTERNAL_ERROR 7

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
  stack->report_error_ = 0;
  stack->need_sym_ = 1;
  stack->current_sym_ = 0;
  stack->mute_error_turns_ = 0;
  stack->pos_ = 0;
  stack->num_stack_allocated_ = 0;
  stack->stack_ = NULL;
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
  for (n = 1; n < stack->pos_; ++n) {
    switch (stack->stack_[n].state_) {
    } /* switch */
    {
       snippet_cleanup(&((stack->stack_ + n)->common_));

    }
  }
  if ((!stack->need_sym_) && stack->pos_) {
    /* Deconstruct placeholder location for terminal not yet shifted */
    switch (stack->current_sym_) {
    } /* switch */
    {
       snippet_cleanup(&(stack->stack_[0].common_));

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
  for (n = 1; n < stack->pos_; ++n) {
    switch (stack->stack_[n].state_) {
    } /* switch */
    {
       snippet_cleanup(&((stack->stack_ + n)->common_));

    }
  }
  if ((!stack->need_sym_) && stack->pos_) {
    /* Deconstruct placeholder location for terminal not yet shifted */
    switch (stack->current_sym_) {
    } /* switch */
    {
       snippet_cleanup(&(stack->stack_[0].common_));

    }
  }
  stack->pos_ = 0;
  stack->error_recovery_ = 0;
  stack->report_error_ = 0;
  stack->need_sym_ = 1;
  stack->current_sym_ = 0;
  stack->mute_error_turns_ = 0;
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
  stack->input_index_ = 0;
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
      return _RXG_OVERFLOW;
    }
    stack->match_buffer_ = (char *)buf;
    stack->match_buffer_size_allocated_ = size_to_allocate;
  }

  memcpy(stack->match_buffer_ + stack->match_buffer_size_, s, len);
  stack->match_buffer_size_ += len;
  stack->match_buffer_[stack->match_buffer_size_] = '\0';
  return 0;
}

int rxg_lex(struct rxg_stack *stack, const char *input, size_t input_size, int is_final_input) {
  int r;
  unsigned char c;
  size_t scan_state = stack->scan_state_;
  const size_t *transition_table = rxg_scan_table;
  const size_t *actions = rxg_scan_actions;
  const size_t default_action = 0;
  const size_t start_state = 1;
  const size_t start_action = rxg_scan_actions[start_state];

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
    scan_state = transition_table[256 * scan_state + c];
    if (scan_state) {
      at_match_index_offset++;
      if (c != '\n') {
        at_match_index_col++;
      }
      else {
        at_match_index_col = 1;
        at_match_index_line++;
      }

      size_t state_action = actions[scan_state];
      if (state_action != default_action) /* replace with actual */ {
        best_match_action = state_action;
        best_match_size = match_index + 1;
        best_match_offset = at_match_index_offset;
        best_match_line = at_match_index_line;
        best_match_col = at_match_index_col;
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
    scan_state = transition_table[256 * scan_state + c];
    if (scan_state) {
      input_offset++;
      if (c != '\n') {
        input_col++;
      }
      else {
        input_col = 1;
        input_line++;
      }
      size_t state_action = actions[scan_state];
      if (state_action != default_action) /* replace with actual */ {
        best_match_action = state_action;
        best_match_size = stack->match_buffer_size_ + input_index - stack->input_index_ + 1;
        best_match_col = input_col;
        best_match_line = input_line;
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
    stack->input_index_ = 0;
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

  return _RXG_SYNTAX_ERROR;
}
int rxg_scan(struct rxg_stack *stack, const char *input, size_t input_size, int is_final_input, struct prd_grammar *g, struct tkr_tokenizer *tkr, struct symbol_table *st, char char_value) {
  for (;;) {
    if (stack->need_sym_) {
      switch (rxg_lex(stack, input, input_size, is_final_input)) {
        case _RXG_MATCH:
          stack->need_sym_ = 0;
          stack->current_sym_ = RXG_INPUT_END; /* no sym specific type associated, changed when we successfully construct it below */
          if (stack->mute_error_turns_) stack->mute_error_turns_--;
          {
             snippet_init(&(stack->stack_[0].common_));
          }
          {
             snippet_append_tkr(&(stack->stack_[0].common_), tkr);
          }
          switch (stack->best_match_action_) {
            case 1: /* /\*([^\*]|[\r\n]|(\*+([^\* //]|[\r\n])))*\* // */
              /* Pattern does not have a symbol */
              stack->current_sym_ = RXG_INPUT_END;
              stack->need_sym_ = 1; /* keep scanning */
              {
                
  fprintf(stderr, "Imagine a sensible action here.\n");

              }
              if (stack->need_sym_) {
                 snippet_cleanup(&(stack->stack_[0].common_));
              }
              break;
          } /* switch */
          break;
        case _RXG_OVERFLOW:
          return _RXG_OVERFLOW;
        case _RXG_NO_MEMORY:
          return _RXG_NO_MEMORY;
        case _RXG_FEED_ME:
          return _RXG_FEED_ME;
        case _RXG_END_OF_INPUT:
          stack->current_sym_ = RXG_INPUT_END;
          stack->need_sym_ = 0;
          if (stack->mute_error_turns_) stack->mute_error_turns_--;
          break;
        case _RXG_SYNTAX_ERROR:
          return _RXG_SYNTAX_ERROR;
      } /* switch */
    } /* if (need_sym_) */
    else {
      int sym = stack->current_sym_;
      if (!stack->error_recovery_) {
        int action = rxg_parse_table[rxg_num_columns * stack->stack_[stack->pos_ - 1].state_ + (sym - rxg_minimum_sym)];
        if (action > 0) {
          switch (rxg_push_state(stack, action /* action for a shift is the ordinal */)) {
            case _RXG_OVERFLOW: {
              re_error_tkr(tkr, "Error: internal error\n"); return PRD_INTERNAL_ERROR;
            }
            break;
            case _RXG_NO_MEMORY: {
              re_error_tkr(tkr, "Error: no memory"); return PRD_INTERNAL_ERROR;
            }
            break;
          } /* switch */

          /* Fill in the sym from the tokenizer */
          stack->need_sym_ = 1;
          stack->stack_[stack->pos_ - 1].common_ = stack->stack_[0].common_;
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
          int r; \
        struct prd_pattern *pat;
            switch (production) {
            /* grammar: */
              case 1: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

              }
              {
                
              }
              break;
            /* grammar: grammar pattern */
              case 2: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

              }
              {
                
              }
              break;
            /* pattern: IDENT start-regex COLON exp end-regex SEMICOLON */
              case 3: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

              }
              {
                {
  r = prd_grammar_check_pattern_reserve(g);
  if (r) return r;
  char *regex = snippet_dup_xlt(&(sym_data[3].common_));
  if (!regex) {
    return PRD_INTERNAL_ERROR;
  }
  pat = g->patterns_ + g->num_patterns_++;
  prd_pattern_init(pat);
  pat->regex_ = regex;
  r = snippet_append_to_xlts(&pat->term_.id_, &(sym_data[0].common_));
  if (r) return r;
}
              }
              break;
            /* pattern: IDENT start-regex COLON exp end-regex start-c-tokenizer accept-whitespace CUR_OPEN action-sequence end-c-tokenizer CUR_CLOSE */
              case 4: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

              }
              {
                {
  r = prd_grammar_check_pattern_reserve(g);
  if (r) return r;
  char *regex = snippet_dup_xlt(&(sym_data[3].common_));
  if (!regex) {
    return PRD_INTERNAL_ERROR;
  }
  pat = g->patterns_ + g->num_patterns_++;
  prd_pattern_init(pat);
  pat->regex_ = regex;
  r = snippet_append_to_xlts(&pat->term_.id_, &(sym_data[0].common_));
  if (r) return r;
  r = snippet_append_snippet(&pat->action_sequence_, &(sym_data[8].common_));
  if (r) return r;
}
              }
              break;
            /* pattern: IDENT start-regex COLON exp end-regex start-c-tokenizer stmt-action end-c-tokenizer SEMICOLON */
              case 5: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

              }
              {
                {
  r = prd_grammar_check_pattern_reserve(g);
  if (r) return r;
  char *regex = snippet_dup_xlt(&(sym_data[3].common_));
  if (!regex) {
    return PRD_INTERNAL_ERROR;
  }
  pat = g->patterns_ + g->num_patterns_++;
  prd_pattern_init(pat);
  pat->regex_ = regex;
  r = snippet_append_to_xlts(&pat->term_.id_, &(sym_data[0].common_));
  if (r) return r;
  r = snippet_append_snippet(&pat->action_sequence_, &(sym_data[6].common_));
  if (r) return r;
  /* Pop the '=' token from stmt-action as it is not part of the action sequence itself. */
  if (pat->action_sequence_.num_tokens_ && (pat->action_sequence_.tokens_[0].match_ == TOK_EQUALS)) {
    snippet_pop_first_token(&pat->action_sequence_);
  }
}
              }
              break;
            /* pattern: start-regex COLON exp end-regex SEMICOLON */
              case 6: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

              }
              {
                {
  r = prd_grammar_check_pattern_reserve(g);
  if (r) return r;
  char *regex = snippet_dup_xlt(&(sym_data[2].common_));
  if (!regex) {
    return PRD_INTERNAL_ERROR;
  }
  pat = g->patterns_ + g->num_patterns_++;
  prd_pattern_init(pat);
  pat->regex_ = regex;
}
              }
              break;
            /* pattern: start-regex COLON exp end-regex start-c-tokenizer accept-whitespace CUR_OPEN action-sequence end-c-tokenizer CUR_CLOSE */
              case 7: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

              }
              {
                {
  r = prd_grammar_check_pattern_reserve(g);
  if (r) return r;
  char *regex = snippet_dup_xlt(&(sym_data[2].common_));
  if (!regex) {
    return PRD_INTERNAL_ERROR;
  }
  pat = g->patterns_ + g->num_patterns_++;
  prd_pattern_init(pat);
  pat->regex_ = regex;
  r = snippet_append_snippet(&pat->action_sequence_, &(sym_data[7].common_));
  if (r) return r;
}
              }
              break;
            /* pattern: start-regex COLON exp end-regex start-c-tokenizer stmt-action end-c-tokenizer SEMICOLON */
              case 8: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

              }
              {
                {
  r = prd_grammar_check_pattern_reserve(g);
  if (r) return r;
  char *regex = snippet_dup_xlt(&(sym_data[2].common_));
  if (!regex) {
    return PRD_INTERNAL_ERROR;
  }
  pat = g->patterns_ + g->num_patterns_++;
  prd_pattern_init(pat);
  pat->regex_ = regex;
  r = snippet_append_snippet(&pat->action_sequence_, &(sym_data[5].common_));
  if (r) return r;
  /* Pop the '=' token from stmt-action as it is not part of the action sequence itself. */
  if (pat->action_sequence_.num_tokens_ && (pat->action_sequence_.tokens_[0].match_ == TOK_EQUALS)) {
    snippet_pop_first_token(&pat->action_sequence_);
  }
}
              }
              break;
            /* exp: term */
              case 9: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

              }
              {
                {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

              }
              if (!discard_action) {
                
              }
              break;
            /* exp: exp BAR term */
              case 10: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

              }
              {
                {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

              }
              if (!discard_action) {
                
              }
              break;
            /* term: */
              case 11: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

              }
              {
                {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

              }
              if (!discard_action) {
                
              }
              break;
            /* term: term elm */
              case 12: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

              }
              {
                {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

              }
              if (!discard_action) {
                
              }
              break;
            /* elm: sym */
              case 13: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

              }
              {
                {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

              }
              if (!discard_action) {
                
              }
              break;
            /* elm: sym ASTERISK */
              case 14: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

              }
              {
                {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

              }
              if (!discard_action) {
                
              }
              break;
            /* elm: sym PLUS */
              case 15: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

              }
              {
                {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

              }
              if (!discard_action) {
                
              }
              break;
            /* elm: sym QUESTION_MARK */
              case 16: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

              }
              {
                {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

              }
              if (!discard_action) {
                
              }
              break;
            /* sym: CHAR */
              case 17: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

              }
              {
                {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

              }
              if (!discard_action) {
                
              }
              break;
            /* sym: DOT */
              case 18: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

              }
              {
                {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

              }
              if (!discard_action) {
                
              }
              break;
            /* sym: CARET */
              case 19: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

              }
              {
                {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

              }
              if (!discard_action) {
                
              }
              break;
            /* sym: DOLLAR */
              case 20: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

              }
              {
                {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

              }
              if (!discard_action) {
                
              }
              break;
            /* sym: PAR_OPEN exp PAR_CLOSE */
              case 21: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

              }
              {
                {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

              }
              if (!discard_action) {
                
              }
              break;
            /* sym: SQ_OPEN range SQ_CLOSE */
              case 22: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

              }
              {
                {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

              }
              if (!discard_action) {
                
              }
              break;
            /* sym: SQ_OPEN CARET range SQ_CLOSE */
              case 23: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

              }
              {
                {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

              }
              if (!discard_action) {
                
              }
              break;
            /* range: range range-elm */
              case 24: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

              }
              {
                {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

              }
              if (!discard_action) {
                
              }
              break;
            /* range: range-elm */
              case 25: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

              }
              {
                {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

              }
              if (!discard_action) {
                
              }
              break;
            /* range-elm: CHAR */
              case 26: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

              }
              {
                {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

              }
              if (!discard_action) {
                
              }
              break;
            /* range-elm: CHAR DASH CHAR */
              case 27: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

              }
              {
                {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

              }
              if (!discard_action) {
                
              }
              break;
            /* action-sequence: */
              case 28: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

              }
              {
                {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

              }
              if (!discard_action) {
                
              }
              break;
            /* action-sequence: action-sequence CHAR */
              case 29: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

              }
              {
                {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

              }
              if (!discard_action) {
                
              }
              break;
            /* action-sequence: action-sequence IDENT */
              case 30: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

              }
              {
                {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

              }
              if (!discard_action) {
                
              }
              break;
            /* action-sequence: action-sequence COLON */
              case 31: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

              }
              {
                {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

              }
              if (!discard_action) {
                
              }
              break;
            /* action-sequence: action-sequence SEMICOLON */
              case 32: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

              }
              {
                {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

              }
              if (!discard_action) {
                
              }
              break;
            /* action-sequence: action-sequence SQ_OPEN */
              case 33: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

              }
              {
                {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

              }
              if (!discard_action) {
                
              }
              break;
            /* action-sequence: action-sequence SQ_CLOSE */
              case 34: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

              }
              {
                {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

              }
              if (!discard_action) {
                
              }
              break;
            /* action-sequence: action-sequence DOT */
              case 35: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

              }
              {
                {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

              }
              if (!discard_action) {
                
              }
              break;
            /* action-sequence: action-sequence DOLLAR */
              case 36: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

              }
              {
                {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

              }
              if (!discard_action) {
                
              }
              break;
            /* action-sequence: action-sequence CARET */
              case 37: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

              }
              {
                {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

              }
              if (!discard_action) {
                
              }
              break;
            /* action-sequence: action-sequence DASH */
              case 38: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

              }
              {
                {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

              }
              if (!discard_action) {
                
              }
              break;
            /* action-sequence: action-sequence BAR */
              case 39: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

              }
              {
                {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

              }
              if (!discard_action) {
                
              }
              break;
            /* action-sequence: action-sequence ASTERISK */
              case 40: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

              }
              {
                {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

              }
              if (!discard_action) {
                
              }
              break;
            /* action-sequence: action-sequence PLUS */
              case 41: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

              }
              {
                {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

              }
              if (!discard_action) {
                
              }
              break;
            /* action-sequence: action-sequence QUESTION_MARK */
              case 42: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

              }
              {
                {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

              }
              if (!discard_action) {
                
              }
              break;
            /* action-sequence: action-sequence TOKEN */
              case 43: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

              }
              {
                {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

              }
              if (!discard_action) {
                
              }
              break;
            /* action-sequence: action-sequence WHITESPACE */
              case 44: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

              }
              {
                {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

              }
              if (!discard_action) {
                
              }
              break;
            /* action-sequence: action-sequence PAR_OPEN action-sequence PAR_CLOSE */
              case 45: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

              }
              {
                {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

              }
              if (!discard_action) {
                
              }
              break;
            /* action-sequence: action-sequence CUR_OPEN action-sequence CUR_CLOSE */
              case 46: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

              }
              {
                {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

              }
              if (!discard_action) {
                
              }
              break;
            /* start-regex: */
              case 47: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

              }
              {
                {
  tok_switch_to_regex(tkr);
}
              }
              break;
            /* end-regex: */
              case 48: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

              }
              {
                {
  tok_switch_to_nonterminal_idents(tkr);
}
              }
              break;
            /* start-c-tokenizer: */
              case 49: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

              }
              {
                {
  tok_switch_to_c_idents(tkr);
}
              }
              break;
            /* end-c-tokenizer: */
              case 50: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

              }
              {
                {
  g->accept_whitespace_ = 0; /* ignore whitespace */
  tok_switch_to_nonterminal_idents(tkr);
}
              }
              break;
            /* accept-whitespace: */
              case 51: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

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
             snippet_cleanup(&((stack->stack_ + rxg_sym_idx)->common_));

          }
          } /* for */
          stack->pos_ -= production_length;
          action = rxg_parse_table[rxg_num_columns * stack->stack_[stack->pos_ - 1].state_ + (nonterminal - rxg_minimum_sym)];
          if (action <= 0) {
            re_error_tkr(tkr, "Error: internal error\n"); return PRD_INTERNAL_ERROR;
          }
          switch (rxg_push_state(stack, action /* action for a "goto" shift is the ordinal */)) {
            case _RXG_OVERFLOW: {
              re_error_tkr(tkr, "Error: internal error\n"); return PRD_INTERNAL_ERROR;
            }
            break;
            case _RXG_NO_MEMORY: {
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
                     snippet_cleanup(&((stack->stack_ + rxg_sym_idx)->common_));

                  }
                } /* for */
                stack->pos_ = n + 1;
                /* Push the state of the error transition */
                switch (rxg_push_state(stack, err_action /* action for a shift is the state */)) {
                  case _RXG_OVERFLOW: {
                    re_error_tkr(tkr, "Error: internal error\n"); return PRD_INTERNAL_ERROR;
                  }
                  break;
                  case _RXG_NO_MEMORY: {
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
        if (stack->error_recovery_) {
          /* Did not yet recover, discard current sym and get next */
          stack->need_sym_ = 1;
return PRD_NEXT;
        }
      } /* stack->error_recovery_ */
    } /* for (;;) */
  } /* for (;;) lexing loop */
}
int rxg_parse(struct rxg_stack *stack, int sym, struct prd_grammar *g, struct tkr_tokenizer *tkr, struct symbol_table *st, char char_value) {
  if (stack->mute_error_turns_) stack->mute_error_turns_--;
  for (;;) {
    if (!stack->error_recovery_) {
      int action = rxg_parse_table[rxg_num_columns * stack->stack_[stack->pos_ - 1].state_ + (sym - rxg_minimum_sym)];
      if (action > 0) {
        switch (rxg_push_state(stack, action /* action for a shift is the ordinal */)) {
          case _RXG_OVERFLOW: {
            re_error_tkr(tkr, "Error: internal error\n"); return PRD_INTERNAL_ERROR;
          }
          break;
          case _RXG_NO_MEMORY: {
            re_error_tkr(tkr, "Error: no memory"); return PRD_INTERNAL_ERROR;
          }
          break;
        } /* switch */

        struct rxg_sym_data *sym_data = stack->stack_ + stack->pos_ - 1;
        {
           snippet_init(&(sym_data->common_));
        }
        {
           snippet_append_tkr(&(sym_data->common_), tkr);
        }
        switch (sym) {
            {
               snippet_init(&(sym_data->common_));
            }
            {
               snippet_append_tkr(&(sym_data->common_), tkr);
            }
            break;
        } /* switch */
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
          int r; \
        struct prd_pattern *pat;
          switch (production) {
            /* grammar: */
            case 1: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              
            }
            break;
            /* grammar: grammar pattern */
            case 2: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              
            }
            break;
            /* pattern: IDENT start-regex COLON exp end-regex SEMICOLON */
            case 3: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  r = prd_grammar_check_pattern_reserve(g);
  if (r) return r;
  char *regex = snippet_dup_xlt(&(sym_data[3].common_));
  if (!regex) {
    return PRD_INTERNAL_ERROR;
  }
  pat = g->patterns_ + g->num_patterns_++;
  prd_pattern_init(pat);
  pat->regex_ = regex;
  r = snippet_append_to_xlts(&pat->term_.id_, &(sym_data[0].common_));
  if (r) return r;
}
            }
            break;
            /* pattern: IDENT start-regex COLON exp end-regex start-c-tokenizer accept-whitespace CUR_OPEN action-sequence end-c-tokenizer CUR_CLOSE */
            case 4: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  r = prd_grammar_check_pattern_reserve(g);
  if (r) return r;
  char *regex = snippet_dup_xlt(&(sym_data[3].common_));
  if (!regex) {
    return PRD_INTERNAL_ERROR;
  }
  pat = g->patterns_ + g->num_patterns_++;
  prd_pattern_init(pat);
  pat->regex_ = regex;
  r = snippet_append_to_xlts(&pat->term_.id_, &(sym_data[0].common_));
  if (r) return r;
  r = snippet_append_snippet(&pat->action_sequence_, &(sym_data[8].common_));
  if (r) return r;
}
            }
            break;
            /* pattern: IDENT start-regex COLON exp end-regex start-c-tokenizer stmt-action end-c-tokenizer SEMICOLON */
            case 5: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  r = prd_grammar_check_pattern_reserve(g);
  if (r) return r;
  char *regex = snippet_dup_xlt(&(sym_data[3].common_));
  if (!regex) {
    return PRD_INTERNAL_ERROR;
  }
  pat = g->patterns_ + g->num_patterns_++;
  prd_pattern_init(pat);
  pat->regex_ = regex;
  r = snippet_append_to_xlts(&pat->term_.id_, &(sym_data[0].common_));
  if (r) return r;
  r = snippet_append_snippet(&pat->action_sequence_, &(sym_data[6].common_));
  if (r) return r;
  /* Pop the '=' token from stmt-action as it is not part of the action sequence itself. */
  if (pat->action_sequence_.num_tokens_ && (pat->action_sequence_.tokens_[0].match_ == TOK_EQUALS)) {
    snippet_pop_first_token(&pat->action_sequence_);
  }
}
            }
            break;
            /* pattern: start-regex COLON exp end-regex SEMICOLON */
            case 6: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  r = prd_grammar_check_pattern_reserve(g);
  if (r) return r;
  char *regex = snippet_dup_xlt(&(sym_data[2].common_));
  if (!regex) {
    return PRD_INTERNAL_ERROR;
  }
  pat = g->patterns_ + g->num_patterns_++;
  prd_pattern_init(pat);
  pat->regex_ = regex;
}
            }
            break;
            /* pattern: start-regex COLON exp end-regex start-c-tokenizer accept-whitespace CUR_OPEN action-sequence end-c-tokenizer CUR_CLOSE */
            case 7: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  r = prd_grammar_check_pattern_reserve(g);
  if (r) return r;
  char *regex = snippet_dup_xlt(&(sym_data[2].common_));
  if (!regex) {
    return PRD_INTERNAL_ERROR;
  }
  pat = g->patterns_ + g->num_patterns_++;
  prd_pattern_init(pat);
  pat->regex_ = regex;
  r = snippet_append_snippet(&pat->action_sequence_, &(sym_data[7].common_));
  if (r) return r;
}
            }
            break;
            /* pattern: start-regex COLON exp end-regex start-c-tokenizer stmt-action end-c-tokenizer SEMICOLON */
            case 8: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  r = prd_grammar_check_pattern_reserve(g);
  if (r) return r;
  char *regex = snippet_dup_xlt(&(sym_data[2].common_));
  if (!regex) {
    return PRD_INTERNAL_ERROR;
  }
  pat = g->patterns_ + g->num_patterns_++;
  prd_pattern_init(pat);
  pat->regex_ = regex;
  r = snippet_append_snippet(&pat->action_sequence_, &(sym_data[5].common_));
  if (r) return r;
  /* Pop the '=' token from stmt-action as it is not part of the action sequence itself. */
  if (pat->action_sequence_.num_tokens_ && (pat->action_sequence_.tokens_[0].match_ == TOK_EQUALS)) {
    snippet_pop_first_token(&pat->action_sequence_);
  }
}
            }
            break;
            /* exp: term */
            case 9: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* exp: exp BAR term */
            case 10: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* term: */
            case 11: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* term: term elm */
            case 12: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* elm: sym */
            case 13: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* elm: sym ASTERISK */
            case 14: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* elm: sym PLUS */
            case 15: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* elm: sym QUESTION_MARK */
            case 16: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* sym: CHAR */
            case 17: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* sym: DOT */
            case 18: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* sym: CARET */
            case 19: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* sym: DOLLAR */
            case 20: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* sym: PAR_OPEN exp PAR_CLOSE */
            case 21: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* sym: SQ_OPEN range SQ_CLOSE */
            case 22: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* sym: SQ_OPEN CARET range SQ_CLOSE */
            case 23: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* range: range range-elm */
            case 24: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* range: range-elm */
            case 25: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* range-elm: CHAR */
            case 26: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* range-elm: CHAR DASH CHAR */
            case 27: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* action-sequence: */
            case 28: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* action-sequence: action-sequence CHAR */
            case 29: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* action-sequence: action-sequence IDENT */
            case 30: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* action-sequence: action-sequence COLON */
            case 31: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* action-sequence: action-sequence SEMICOLON */
            case 32: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* action-sequence: action-sequence SQ_OPEN */
            case 33: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* action-sequence: action-sequence SQ_CLOSE */
            case 34: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* action-sequence: action-sequence DOT */
            case 35: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* action-sequence: action-sequence DOLLAR */
            case 36: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* action-sequence: action-sequence CARET */
            case 37: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* action-sequence: action-sequence DASH */
            case 38: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* action-sequence: action-sequence BAR */
            case 39: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* action-sequence: action-sequence ASTERISK */
            case 40: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* action-sequence: action-sequence PLUS */
            case 41: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* action-sequence: action-sequence QUESTION_MARK */
            case 42: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* action-sequence: action-sequence TOKEN */
            case 43: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* action-sequence: action-sequence WHITESPACE */
            case 44: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* action-sequence: action-sequence PAR_OPEN action-sequence PAR_CLOSE */
            case 45: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* action-sequence: action-sequence CUR_OPEN action-sequence CUR_CLOSE */
            case 46: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  size_t n;
  for (n = 0; n < ((size_t)production_length); ++n) {
    snippet_append_snippet(&(nonterminal_sym_data_reduced_to.common_), &(sym_data[n].common_));
  }
}

            }
            if (!discard_action) {
              
            }
            break;
            /* start-regex: */
            case 47: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  tok_switch_to_regex(tkr);
}
            }
            break;
            /* end-regex: */
            case 48: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  tok_switch_to_nonterminal_idents(tkr);
}
            }
            break;
            /* start-c-tokenizer: */
            case 49: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  tok_switch_to_c_idents(tkr);
}
            }
            break;
            /* end-c-tokenizer: */
            case 50: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

            }
            {
              {
  g->accept_whitespace_ = 0; /* ignore whitespace */
  tok_switch_to_nonterminal_idents(tkr);
}
            }
            break;
            /* accept-whitespace: */
            case 51: {
     snippet_init(&(nonterminal_sym_data_reduced_to.common_));

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
           snippet_cleanup(&((stack->stack_ + rxg_sym_idx)->common_));

        }
        } /* for */
        stack->pos_ -= production_length;
        action = rxg_parse_table[rxg_num_columns * stack->stack_[stack->pos_ - 1].state_ + (nonterminal - rxg_minimum_sym)];
        if (action <= 0) {
          re_error_tkr(tkr, "Error: internal error\n"); return PRD_INTERNAL_ERROR;
        }
        switch (rxg_push_state(stack, action /* action for a "goto" shift is the ordinal */)) {
          case _RXG_OVERFLOW: {
            re_error_tkr(tkr, "Error: internal error\n"); return PRD_INTERNAL_ERROR;
          }
          break;
          case _RXG_NO_MEMORY: /* out of memory */ {
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
                   snippet_cleanup(&((stack->stack_ + rxg_sym_idx)->common_));

                }
              } /* for */
              stack->pos_ = n + 1;
              /* Push the state of the error transition */
              switch (rxg_push_state(stack, err_action /* action for a shift is the state */)) {
                case _RXG_OVERFLOW: {
                  re_error_tkr(tkr, "Error: internal error\n"); return PRD_INTERNAL_ERROR;
                }
                break;
                case _RXG_NO_MEMORY: {
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
      if (stack->error_recovery_) {
        /* Did not yet recover, discard current sym and get next */
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

