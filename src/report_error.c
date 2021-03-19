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

#ifndef STDLIB_H_INCULDED
#define STDLIB_H_INCLUDED
#include <stdlib.h>
#endif

#ifndef STDIO_H_INCLUDED
#define STDIO_H_INCLUDED
#include <stdio.h>
#endif

#ifndef STDARG_H_INCLUDED
#define STDARG_H_INCLUDED
#include <stdarg.h>
#endif

#ifndef XLTS_H_INCLUDED
#define XLTS_H_INCLUDED
#include "xlts.h"
#endif

#ifndef TOKENIZER_H_INCLUDED
#define TOKENIZER_H_INCLUDED
#include "tokenizer.h"
#endif

#ifndef REPORT_ERROR_H_INCLUDED
#define REPORT_ERROR_H_INCLUDED
#include "report_error.h"
#endif

static void re_error_nowhere_impl(const char *fmt, va_list args) {
  vfprintf(stderr, fmt, args);
  fprintf(stderr, "\n");
}

static void re_error_impl(const char *filename, int line_nr, int col_nr, const char *fmt, va_list args) {
  if (line_nr) {
    fprintf(stderr, "%s(%d): ", filename ? filename : "", line_nr);
  }
  else {
    fprintf(stderr, "%s(?): ", filename ? filename : "");
  }
  re_error_nowhere_impl(fmt, args);
}

static int re_error_x_marks_the_spot(struct xlts *x) {
  struct xlts_chunk *ori_chunk;
  for (ori_chunk = x->chunks_; ori_chunk < (x->chunks_ + x->num_chunks_); ori_chunk++) {
    if (ori_chunk->ct_ != XLTS_XLAT) {
      /* Found chunk that has source location we can use. */
      return 1;
    }
  }
  return 0;
}

static void re_error_x_impl(struct xlts *loc, const char *fmt, va_list args) {
  struct xlts_chunk *ori_chunk;
  for (ori_chunk = loc->chunks_; ori_chunk < (loc->chunks_ + loc->num_chunks_); ori_chunk++) {
    if (ori_chunk->ct_ != XLTS_XLAT) {
      /* Found chunk that has source location we can use. */
      break;
    }
  }
  if (ori_chunk == (loc->chunks_ + loc->num_chunks_)) {
    ori_chunk = NULL;
  }

  if (ori_chunk) {
    re_error_impl(ori_chunk->filename_, ori_chunk->line_, ori_chunk->col_, fmt, args);
  }
  else {
    re_error_impl(NULL, 0, 0, fmt, args);
  }
}

void re_error_flc(const char *filename, int line, int col, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);

  re_error_impl(filename, line, col, fmt, args);

  va_end(args);
}

void re_error(struct xlts *loc, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);

  re_error_x_impl(loc, fmt, args);

  va_end(args);
}

void re_error_tkr(struct tkr_tokenizer *tkr, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);

  if (re_error_x_marks_the_spot(&tkr->xmatch_)) {
    re_error_x_impl(&tkr->xmatch_, fmt, args);
  }
  else {
    re_error_impl(tkr->filename_, tkr->start_line_, tkr->start_col_, fmt, args);
  }

  va_end(args);
}

void re_error_nowhere(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  re_error_nowhere_impl(fmt, args);
  va_end(args);
}
