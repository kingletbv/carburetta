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

/* Uncommenting this will dynamically generate the scan tables and write these into files (see tok_init()). */
/* #define GENERATE_AND_DUMP_SCANNER_TABLES */

#ifdef GENERATE_AND_DUMP_SCANNER_TABLES 
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

static const struct sc_scan_rule g_scanner_regex_rules_[] = {
#define xx(regex, minor) { regex, minor, minor },
#define xy(regex, minor, major) { regex, minor, major},
#define xz(major)
  ENUM_SCANNER_REGEX_DEFS
#undef xx
#undef xy
#undef xz
};

#endif

#ifdef GENERATE_AND_DUMP_SCANNER_TABLES
static struct sc_scanner g_tok_c_ident_scanner_;
static struct sc_scanner g_tok_nonterminal_ident_scanner_;
static struct sc_scanner g_tok_regex_scanner_;
#else
#include "tokens_generated_scanners.c"
#endif

int tok_init(void) {
#ifdef GENERATE_AND_DUMP_SCANNER_TABLES
  int r;
  sc_scanner_init(&g_tok_c_ident_scanner_);
  sc_scanner_init(&g_tok_nonterminal_ident_scanner_);
  sc_scanner_init(&g_tok_regex_scanner_);

  r = sc_scanner_compile(&g_tok_c_ident_scanner_, TOK_NO_MATCH, sizeof(g_scanner_rules_) / sizeof(*g_scanner_rules_), g_scanner_rules_);
  if (r) return r;

  r = sc_scanner_compile(&g_tok_nonterminal_ident_scanner_, TOK_NO_MATCH, sizeof(g_scanner_production_rules_) / sizeof(*g_scanner_production_rules_), g_scanner_production_rules_);
  if (r) return r;

  r = sc_scanner_compile(&g_tok_regex_scanner_, TOK_NO_MATCH, sizeof(g_scanner_regex_rules_) / sizeof(*g_scanner_regex_rules_), g_scanner_regex_rules_);
  if (r) return r;

  FILE *fp = fopen("tokens_generated_scanners.c", "wb");
  fprintf(fp, "/* Copyright 2020 Kinglet B.V.\n"
              " *\n"
              " * Licensed under the Apache License, Version 2.0 (the \"License\");\n"
              " * you may not use this file except in compliance with the License.\n"
              " * You may obtain a copy of the License at\n"
              " *\n"
              " * http ://www.apache.org/licenses/LICENSE-2.0\n"
              " *\n"
              " * Unless required by applicable law or agreed to in writing, software\n"
              " * distributed under the License is distributed on an \"AS IS\" BASIS,\n"
              " * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.\n"
              " * See the License for the specific language governing permissions and\n"
              " * limitations under the License.\n"
              " */\n"
              " /** NOTE: This file is generated at tok_init() in tokens.c by defining GENERATE_AND_DUMP_SCANNER_TABLES **/\n");
  sc_scanner_write_to_c_file(&g_tok_c_ident_scanner_, fp, "g_tok_c_ident_scanner_transition_table_", "g_tok_c_ident_scanner_state_actions_", "g_tok_c_ident_scanner_");
  sc_scanner_write_to_c_file(&g_tok_nonterminal_ident_scanner_, fp, "g_tok_nonterminal_ident_scanner_transition_table_", "g_tok_nonterminal_ident_scanner_state_actions_", "g_tok_nonterminal_ident_scanner_");
  sc_scanner_write_to_c_file(&g_tok_regex_scanner_, fp, "g_tok_regex_scanner_transition_table_", "g_tok_regex_scanner_state_actions_", "g_tok_regex_scanner_");
  fclose(fp);
#endif
  return 0;
}

void tok_cleanup(void) {
#ifdef GENERATE_AND_DUMP_SCANNER_TABLES
  sc_scanner_cleanup(&g_tok_c_ident_scanner_);
  sc_scanner_cleanup(&g_tok_nonterminal_ident_scanner_);
  sc_scanner_cleanup(&g_tok_regex_scanner_);
#endif
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

void tok_switch_to_regex(struct tkr_tokenizer *tkr) {
  tkr_tokenizer_switch(tkr, &g_tok_regex_scanner_);
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
