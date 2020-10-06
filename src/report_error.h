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

#ifndef REPORT_ERROR_H
#define REPORT_ERROR_H

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

/* Report using xlts as location reference */
void re_error(struct xlts *where, const char *fmt, ...);

/* Report using the last matched xlts (tkr->xmatch_) as location reference */
void re_error_tkr(struct tkr_tokenizer *tkr, const char *fmt, ...);

/* Report using explicit file, line, column location reference */
void re_error_flc(const char *filename, int line, int col, const char *fmt, ...);

/* Report error without any specific location associated */
void re_error_nowhere(const char *fmt, ...);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* REPORT_ERROR_H */
