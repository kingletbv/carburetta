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

#ifndef KLT_LOGGER_H_INCLUDED
#define KLT_LOGGER_H_INCLUDED
#define KLT_LOG_MODULE "line_defs"
#include "klt_logger.h"
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

void ld_line_init(struct ld_line *ldl) {
  ldl->type_of_line_ = LD_UNKNOWN;
  ldl->free_chars_on_cleanup_ = 1;
  ldl->num_chars_ = ldl->num_chars_allocated_ = 0;
  ldl->chars_ = NULL;
}

void ld_line_cleanup(struct ld_line *ldl) {
  if (ldl->free_chars_on_cleanup_ && ldl->chars_) {
    free(ldl->chars_);
  }
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

int ldl_fill_token(struct ld_line *ldl, struct tkr_tokenizer *tkr) {
  ldl->start_offset_ = tkr->start_offset_;
  ldl->start_line_ = tkr->start_line_;
  ldl->start_col_ = tkr->start_col_;
  ldl->end_offset_ = tkr->best_match_offset_;
  ldl->end_line_ = tkr->best_match_line_;
  ldl->end_col_ = tkr->best_match_col_;
  ldl->type_of_line_ = (ld_line_type_t)tkr->best_match_action_;
  if (!ldl->free_chars_on_cleanup_ || (ldl->num_chars_allocated_ <= tkr->token_size_)) {
    size_t new_alloc_size = tkr->token_size_ + 1;
    if (new_alloc_size <= tkr->token_size_) {
      /* Overflow */
      LOGERROR("Match size overflow\n");
      return TKR_INTERNAL_ERROR;
    }
    void *buf = realloc(ldl->chars_, new_alloc_size);
    if (!buf) {
      /* No mem */
      return TKR_INTERNAL_ERROR;
    }
    ldl->chars_ = (char *)buf;
    ldl->num_chars_allocated_ = new_alloc_size;
    ldl->free_chars_on_cleanup_ = 1;
  }
  ldl->num_chars_ = tkr->token_size_;
  memcpy(ldl->chars_, tkr->match_, ldl->num_chars_);
  ldl->chars_[ldl->num_chars_] = '\0';

  return 0;
}
