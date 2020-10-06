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

#ifndef STDINT_H_INCLUDED
#define STDINT_H_INCLUDED
#include <stdint.h>
#endif

#ifndef SCANNER_H_INCLUDED
#define SCANNER_H_INCLUDED
#include "scanner.h"
#endif

#ifndef TOKENS_H_INCLUDED
#define TOKENS_H_INCLUDED
#include "tokens.h"
#endif

static const struct sc_scan_rule g_scanner_rules_[] = {
#define xx(regex, minor) { regex, minor, minor },
#define xy(regex, minor, major) { regex, minor, major},
#define xz(major)
  ENUM_SCANNER_DEFS
#undef xx
#undef xy
#undef xz
};

static const struct sc_scan_rule g_scanner_production_rules_[] = {
#define xx(regex, minor) { regex, minor, minor },
#define xy(regex, minor, major) { regex, minor, major},
#define xz(major)
  ENUM_SCANNER_PRODUCTION_DEFS
#undef xx
#undef xy
#undef xz
};

static struct sc_scanner g_tok_c_ident_scanner_;
static struct sc_scanner g_tok_nonterminal_ident_scanner_;

int tok_init(void) {
  int r;
  sc_scanner_init(&g_tok_c_ident_scanner_);
  sc_scanner_init(&g_tok_nonterminal_ident_scanner_);

  r = sc_scanner_compile(&g_tok_c_ident_scanner_, TOK_NO_MATCH, sizeof(g_scanner_rules_) / sizeof(*g_scanner_rules_), g_scanner_rules_);
  if (r) return r;

  r = sc_scanner_compile(&g_tok_nonterminal_ident_scanner_, TOK_NO_MATCH, sizeof(g_scanner_production_rules_) / sizeof(*g_scanner_production_rules_), g_scanner_production_rules_);
  if (r) return r;
  return 0;
}

void tok_cleanup(void) {
  sc_scanner_cleanup(&g_tok_c_ident_scanner_);
  sc_scanner_cleanup(&g_tok_nonterminal_ident_scanner_);
}

void tok_init_tkr_tokenizer(struct tkr_tokenizer *tkr) {
  //tkr_tokenizer_init(tkr, &g_tok_c_ident_scanner_);
  tkr_tokenizer_init(tkr, &g_tok_nonterminal_ident_scanner_);
}

void tok_switch_to_c_idents(struct tkr_tokenizer *tkr) {
  tkr_tokenizer_switch(tkr, &g_tok_c_ident_scanner_);
}

void tok_switch_to_nonterminal_idents(struct tkr_tokenizer *tkr) {
  tkr_tokenizer_switch(tkr, &g_tok_nonterminal_ident_scanner_);
}

const char *tok_token_type_to_str(token_type_t tkt) {
  switch (tkt) {
#define xx(regex, minor) case minor: return #minor;
#define xy(regex, minor, major) case minor: return #minor;
#define xz(major) case major: return #major;
    ENUM_SCANNER_DEFS
#undef xx
#undef xy
#undef xz
  }
  return "???";
}
