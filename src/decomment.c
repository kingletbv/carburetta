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

#ifndef ASSERT_H_INCLUDED
#define ASSERT_H_INCLUDED
#include <assert.h>
#endif

#ifndef SCANNER_H_INCLUDED
#define SCANNER_H_INCLUDED
#include "scanner.h"
#endif

#ifndef KLT_LOGGER_H_INCLUDED
#define KLT_LOGGER_H_INCLUDED
#define KLT_LOG_MODULE "las"
#include "klt_logger.h"
#endif

#ifndef TOKENIZER_H_INCLUDED
#define TOKENIZER_H_INCLUDED
#include "tokenizer.h"
#endif

#ifndef DECOMMENT_H_INCLUDED
#define DECOMMENT_H_INCLUDED
#include "decomment.h"
#endif

#define ENUM_DCT_TOKENS \
xz(DCT_UNKNOWN) \
xx("/\\*([^\\*]|[\\r\\n]|(\\*+([^\\*/]|[\\r\\n])))*\\*/", DCT_C_STYLE_COMMENT) \
xx("//.*", DCT_CPP_STYLE_COMMENT)


typedef enum dct_token {
#define xx(regex, type_of_line) type_of_line,
#define xz(type_of_line) type_of_line,
  ENUM_DCT_TOKENS
#undef xx
#undef xz
} dct_token_t;

static struct sc_scanner g_dct_scanner_;

static const struct sc_scan_rule g_dct_scanner_rules_[] = {
#define xx(regex, line_type) { regex, line_type, line_type },
#define xz(line_type)
ENUM_DCT_TOKENS
#undef xz
#undef xx
};


int dct_init() {
  int r;
  sc_scanner_init(&g_dct_scanner_);

  r = sc_scanner_compile(&g_dct_scanner_, DCT_UNKNOWN, sizeof(g_dct_scanner_rules_) / sizeof(*g_dct_scanner_rules_), g_dct_scanner_rules_);

  return r;
}

void dct_cleanup() {
  sc_scanner_cleanup(&g_dct_scanner_);
}

int dct_decomment(struct xlts *val) {
  int r;
  int has_comments = 0;
  struct tkr_tokenizer tkr;
  tkr_tokenizer_init(&tkr, &g_dct_scanner_);

  struct xlts result;
  xlts_init(&result);

  size_t preamble = 0;
  for (;;) {
    r = tkr_tokenizer_inputx(&tkr, val, 1);
    switch (r) {
    case TKR_END_OF_INPUT:
      if (!has_comments) {
        /* There were no comments, no allocation needed, return val as we found it */
        r = 0;
        goto done;
      }
      else {
        xlts_swap(val, &result);
        r = 0;
        goto done;
      }
    case TKR_MATCH:
      if ((tkr.best_match_variant_ == DCT_C_STYLE_COMMENT) ||
          (tkr.best_match_variant_ == DCT_CPP_STYLE_COMMENT)) {
        if (!has_comments) {
          has_comments = 1;
          /* XXX: What about any leading originals?? */

          r = xlts_append_left_translated(&result, val, preamble);
          if (r) return TKR_INTERNAL_ERROR;
        }
        /* Replace whitespace with a single byte */
        r = xlts_append_as_original(&result, &tkr.xmatch_);
        if (r) return TKR_INTERNAL_ERROR;
        r = xlts_append_xlat(&result, 1, " ");
        if (r) return TKR_INTERNAL_ERROR;
      }
      else {
        assert(0 && "Unknown match\n");
      }
      break;
    case TKR_SYNTAX_ERROR:
      if (!has_comments) {
        /* Still no comment found, keep progressing on the more efficient idea there might not be one. */
        preamble++;
        break;
      }
      /* Copy over a single byte */
      r = xlts_append(&result, &tkr.xmatch_);
      if (r) {
        r = TKR_INTERNAL_ERROR;
        goto done;
      }
      break;
    case TKR_INTERNAL_ERROR:
      return TKR_INTERNAL_ERROR;
    }
  }

done:
  xlts_cleanup(&result);
  tkr_tokenizer_cleanup(&tkr);
  return r;
}

int dct_decomment2(struct xlts *val) {
  int has_comments = 0;
  struct tkr_tokenizer tkr;
  tkr_tokenizer_init(&tkr, &g_dct_scanner_);

  struct xlts result, input_buffer;
  xlts_init(&result);
  xlts_init(&input_buffer);

  size_t preamble = 0;

  int r;
  //     r = tkr_tokenizer_input(&las->mlc_tkr_, las->lc_buf_.translated_, las->lc_buf_.num_translated_, is_final_input);
  for (;;) {
    r = tkr_tokenizer_input(&tkr, val->translated_, val->num_translated_, 1);
    switch (r) {
    case TKR_END_OF_INPUT:
      if (!has_comments) {
        /* There were no comments, no allocation needed, return val as we found it */
        r = 0;
        goto done;
      }
      else {
        xlts_swap(val, &result);
        r = 0;
        goto done;
      }
    case TKR_MATCH:
      /* TWO Problems:
       * 1) Should *replace* a comment with a single whitespace (eg. a space)
       * 2) Should *preserve* the original completely, right now we're discarding the original comments.
       *    >> This requires an xlts_append_left_translated_original() (i.e. a skip of the original, and possibly a change of nomeclature.)
       */
      if ((tkr.best_match_variant_ == DCT_C_STYLE_COMMENT) || (tkr.best_match_variant_ == DCT_CPP_STYLE_COMMENT)) {
        if (!has_comments) {
          has_comments = 1;
          r = xlts_append(&input_buffer, val);
          if (r) return TKR_INTERNAL_ERROR;
          r = xlts_append_left_translated(&result, &input_buffer, preamble);
          if (r) return TKR_INTERNAL_ERROR;
          xlts_strip_left_translated(&input_buffer, preamble + tkr.token_size_);
        }
        else {
          xlts_strip_left_translated(&input_buffer, tkr.token_size_);
        }
        /* Replace whitespace with a single byte. */
        r = xlts_append_xlat(&result, 1, " ");
        if (r) return TKR_INTERNAL_ERROR;
      }
      else {
        assert(0 && "Unknown match\n");
      }
      break;
    case TKR_SYNTAX_ERROR:
      if (!has_comments) {
        /* Still no comment found, keep progressing on the more efficient idea there might not be one. */
        preamble++;
        break;
      }
      /* Copy over a single byte */
      r = xlts_append_left_translated(&result, &input_buffer, 1);
      if (r) {
        r = TKR_INTERNAL_ERROR;
        goto done;
      }
      xlts_strip_left_translated(&input_buffer, 1);
      break;
    case TKR_INTERNAL_ERROR:
      return TKR_INTERNAL_ERROR;
    }
  }

done:
  xlts_cleanup(&input_buffer);
  xlts_cleanup(&result);
  tkr_tokenizer_cleanup(&tkr);
  return r;
}
