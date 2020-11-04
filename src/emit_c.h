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

#ifndef EMIT_C_H
#define EMIT_C_H

#ifndef STDIO_H_INCLUDED
#define STDIO_H_INCLUDED
#include <stdio.h>
#endif

#ifndef SCANNER_H_INCLUDED
#define SCANNER_H_INCLUDED
#include "scanner.h"
#endif

#ifndef LALR_H_INCLUDED
#define LALR_H_INCLUDED
#include "lalr.h"
#endif

#ifndef REPORT_ERROR_H_INCLUDED
#define REPORT_ERROR_H_INCLUDED
#include "report_error.h"
#endif

#ifndef CARBURETTA_CONTEXT_H_INCLUDED
#define CARBURETTA_CONTEXT_H_INCLUDED
#include "carburetta_context.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

const char *cc_prefix(struct carburetta_context *cc);
const char *cc_PREFIX(struct carburetta_context *cc);
static const char *cc_TOKEN_PREFIX(struct carburetta_context *cc);

int emit_c_file(FILE *outfp, struct carburetta_context *cc, struct prd_grammar *prdg, struct sc_scanner *scantable, struct lr_generator *lalr);
int emit_h_file(FILE *outfp, struct carburetta_context *cc, struct prd_grammar *prdg);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* EMIT_C_H */