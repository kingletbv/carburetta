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

#ifndef KLT_LOGGER_H_INCLUDED
#define KLT_LOGGER_H_INCLUDED
#define KLT_LOG_MODULE "tokens"
#include "klt_logger.h"
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

int tok_init_tkr_tokenizer(struct tkr_tokenizer *tkr) {
  int r;
  r = tkr_tokenizer_init(tkr, TOK_NO_MATCH, sizeof(g_scanner_rules_) / sizeof(*g_scanner_rules_), g_scanner_rules_);
  if (r) {
    LOGERROR("Failed to compile scanner\n");
    return TKR_INTERNAL_ERROR;
  }
  return 0;
}

