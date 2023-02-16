/* Copyright 2020-2023 Kinglet B.V.
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

#ifndef STRING_H_INCLUDED
#define STRING_H_INCLUDED
#include <string.h>
#endif

#ifndef SCANNER_H_INCLUDED
#define SCANNER_H_INCLUDED
#include "scanner.h"
#endif

#ifndef TOKENIZER_H_INCLUDED
#define TOKENIZER_H_INCLUDED
#include "tokenizer.h"
#endif

#ifndef LINE_DEFS_H_INCLUDED
#define LINE_DEFS_H_INCLUDED
#include "line_defs.h"
#endif

static const struct sc_scan_rule g_scanner_rules_[] = {
#define xx(regex, line_type) { regex, line_type, line_type },
#define xz(line_type)
ENUM_LINE_DEFS
#undef xz
#undef xx
};

static struct sc_scanner g_ldl_scanner_;

const char *ld_line_type_to_str(ld_line_type_t ldlt) {
#define xx(regex, type_of_line) case type_of_line: return #type_of_line;
#define xz(type_of_line) case type_of_line: return #type_of_line;
  switch (ldlt) {
    ENUM_LINE_DEFS
  }
#undef xx
#undef xz
  return "?";
}


int ldl_init(void) {
  sc_scanner_init(&g_ldl_scanner_);
  int r;
  r = sc_scanner_compile(&g_ldl_scanner_, LD_UNKNOWN, sizeof(g_scanner_rules_) / sizeof(*g_scanner_rules_), g_scanner_rules_);
  return r;
}

void ldl_cleanup(void) {
  sc_scanner_cleanup(&g_ldl_scanner_);
}

void ldl_init_tokenizer(struct tkr_tokenizer *tkr) {
  tkr_tokenizer_init(tkr, &g_ldl_scanner_);
}
