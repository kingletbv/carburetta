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

#ifndef ASSERT_H_INCLUDED
#define ASSERT_H_INCLUDED
#include <assert.h>
#endif

#ifndef SCANNER_H_INCLUDED
#define SCANNER_H_INCLUDED
#include "scanner.h"
#endif

#ifndef TOKENIZER_H_INCLUDED
#define TOKENIZER_H_INCLUDED
#include "tokenizer.h"
#endif

#ifndef LINE_ASSEMBLY_H_INCLUDED
#define LINE_ASSEMBLY_H_INCLUDED
#include "line_assembly.h"
#endif

/* Anything other than LS_NEW_LINE and LS_LINE_CONTUATION is line data */
#define ENUM_LC_TOKENS \
xz(LAS_LC_UNKNOWN) \
xx("\\r\\n|\\n", LAS_LC_NEW_LINE) \
xx("\\\\\\r\\n|\\\\\\n", LAS_LC_LINE_CONTINUATION)

#define ENUM_MLC_TOKENS \
xz(LAS_MLC_UNKNOWN) \
xx("/\\*([^\\*]|[\\r\\n]|(\\*+([^\\*/]|[\\r\\n])))*\\*/", LAS_MLC_C_STYLE_COMMENT) \
xx("//.*", LAS_MLC_CPP_STYLE_COMMENT) \
xx("\\r\\n|\\n", LAS_MLC_NEW_LINE)

typedef enum las_lc_token {
#define xx(regex, type_of_line) type_of_line,
#define xz(type_of_line) type_of_line,
  ENUM_LC_TOKENS
#undef xx
#undef xz
} las_lc_token_t;

typedef enum las_mlc_token {
#define xx(regex, type_of_line) type_of_line,
#define xz(type_of_line) type_of_line,
  ENUM_MLC_TOKENS
#undef xx
#undef xz
} las_mlc_token_t;


static struct sc_scanner g_las_lc_scanner_;
static struct sc_scanner g_las_mlc_scanner_;

static const struct sc_scan_rule g_las_lc_scanner_rules_[] = {
#define xx(regex, line_type) { regex, line_type, line_type },
#define xz(line_type)
ENUM_LC_TOKENS
#undef xz
#undef xx
};

static const struct sc_scan_rule g_las_mlc_scanner_rules_[] = {
#define xx(regex, line_type) { regex, line_type, line_type },
#define xz(line_type)
ENUM_MLC_TOKENS
#undef xz
#undef xx
};


int las_init(void) {
  int r;
  sc_scanner_init(&g_las_lc_scanner_);
  sc_scanner_init(&g_las_mlc_scanner_);

  r = sc_scanner_compile(&g_las_lc_scanner_, LAS_LC_UNKNOWN, sizeof(g_las_lc_scanner_rules_) / sizeof(*g_las_lc_scanner_rules_), g_las_lc_scanner_rules_);
  if (r) return r;

  r = sc_scanner_compile(&g_las_mlc_scanner_, LAS_MLC_UNKNOWN, sizeof(g_las_mlc_scanner_rules_) / sizeof(*g_las_mlc_scanner_rules_), g_las_mlc_scanner_rules_);
  return r;
}

void las_cleanup(void) {
  sc_scanner_cleanup(&g_las_lc_scanner_);
  sc_scanner_cleanup(&g_las_mlc_scanner_);
}

void las_init_line_assembly(struct las_line_assembly *las) {
  tkr_tokenizer_init(&las->lc_tkr_, &g_las_lc_scanner_);

  las->lc_clear_buffers_on_entry_ = 0;
  las->lc_last_line_emitted_ = 0;

  xlts_init(&las->lc_buf_);

  tkr_tokenizer_init(&las->mlc_tkr_, &g_las_mlc_scanner_);

  xlts_init(&las->mlc_buf_);

  las->mlc_clear_buffers_on_entry_ = 0;
  las->mlc_last_line_emitted_ = 0;

  las->mlc_cumulative_line_size_ = 0;

  las->las_input_reentry_from_match_ = 0;
  las->mlc_has_final_input_ = 0;
}

void las_cleanup_line_assembly(struct las_line_assembly *las) {
  tkr_tokenizer_cleanup(&las->lc_tkr_);
  xlts_cleanup(&las->lc_buf_);

  tkr_tokenizer_cleanup(&las->mlc_tkr_);
  xlts_cleanup(&las->mlc_buf_);
}

void las_set_filename(struct las_line_assembly *las, const char *filename) {
  las->lc_tkr_.filename_ = filename;
}

static int las_lc_input(struct las_line_assembly *las, const char *input, size_t input_size, int is_final_input) {
  int r;
  if (las->lc_clear_buffers_on_entry_) {
    las->lc_clear_buffers_on_entry_ = 0;
    xlts_reset(&las->lc_buf_);
  }
  for (;;) {
    r = tkr_tokenizer_inputs(&las->lc_tkr_, input, input_size, is_final_input);
    switch (r) {
    case TKR_END_OF_INPUT:
      if (!las->lc_last_line_emitted_) {
        las->lc_last_line_emitted_ = 1;
        las->lc_clear_buffers_on_entry_ = 1;
        return LAS_MATCH;
      }
      return LAS_END_OF_INPUT;
    case TKR_MATCH:
      if (las->lc_tkr_.best_match_variant_ == LAS_LC_NEW_LINE) {
        r = xlts_append(&las->lc_buf_, &las->lc_tkr_.xmatch_);
        if (r) return r;
        las->lc_clear_buffers_on_entry_ = 1;
        return LAS_MATCH;
      }
      else if (las->lc_tkr_.best_match_variant_ == LAS_LC_LINE_CONTINUATION) {
        r = xlts_append_as_original(&las->lc_buf_, &las->lc_tkr_.xmatch_);
        if (r) return r;
      }
      else {
        /* No more LAS LC token types */
        assert(0);
      }
      break;
    case TKR_SYNTAX_ERROR:
      r = xlts_append(&las->lc_buf_, &las->lc_tkr_.xmatch_);
      if (r) return r;
      break;
    case TKR_INTERNAL_ERROR:
      return LAS_INTERNAL_ERROR;
    case TKR_FEED_ME:
      return LAS_FEED_ME;
    }
  }
}

static int las_mlc_input(struct las_line_assembly *las, int is_final_input) {
  int r;
  if (las->mlc_clear_buffers_on_entry_) {
    las->mlc_clear_buffers_on_entry_ = 0;
    xlts_clamp_remove(&las->mlc_buf_, &las->mlc_buf_early_termination_undo_);

    xlts_strip_left_translated(&las->mlc_buf_, las->mlc_cumulative_line_size_);

    las->mlc_cumulative_line_size_ = 0;
  }
  
  for (;;) {
    r = tkr_tokenizer_inputx(&las->mlc_tkr_, &las->lc_buf_, is_final_input);
    switch (r) {
    case TKR_END_OF_INPUT:
      if (!las->mlc_last_line_emitted_) {
        las->mlc_last_line_emitted_ = 1;
        las->mlc_clear_buffers_on_entry_ = 1;
        xlts_clamp_left_translated(&las->mlc_buf_, las->mlc_cumulative_line_size_, &las->mlc_buf_early_termination_undo_);
        return LAS_MATCH;
      }
      return LAS_END_OF_INPUT;
    case TKR_MATCH:
      if (las->mlc_tkr_.best_match_variant_ == LAS_MLC_C_STYLE_COMMENT) {
        las->mlc_cumulative_line_size_ += las->mlc_tkr_.token_size_;
      }
      else if (las->mlc_tkr_.best_match_variant_ == LAS_MLC_CPP_STYLE_COMMENT) {
        las->mlc_cumulative_line_size_ += las->mlc_tkr_.token_size_;
      }
      else if (las->mlc_tkr_.best_match_variant_ == LAS_MLC_NEW_LINE) {
        las->mlc_cumulative_line_size_ += las->mlc_tkr_.token_size_;
        las->mlc_clear_buffers_on_entry_ = 1;
        r = xlts_clamp_left_translated(&las->mlc_buf_, las->mlc_cumulative_line_size_, &las->mlc_buf_early_termination_undo_);
        if (r) return LAS_INTERNAL_ERROR;
        return LAS_MATCH;
      }
      else {
        /* No more LAS MLC token types */
        assert(0);
      }
      break;
    case TKR_SYNTAX_ERROR:
      /* Non-match = regular character, pass it through */
      las->mlc_cumulative_line_size_ += las->mlc_tkr_.token_size_;
      break;
    case TKR_INTERNAL_ERROR:
      return LAS_INTERNAL_ERROR;
    case TKR_FEED_ME:
      return LAS_FEED_ME;
    }
  }
}

int las_input(struct las_line_assembly *las, const char *input, size_t input_size, int is_final_input) {
  /* for now... */
  int r;
  if (las->las_input_reentry_from_match_) {
    las->las_input_reentry_from_match_ = 0;
    goto back_for_more;
  }

  r = las_lc_input(las, input, input_size, is_final_input);
  while (r != LAS_FEED_ME) {
    if (r == LAS_INTERNAL_ERROR) {
      return r;
    }
    else if (r == LAS_END_OF_INPUT) {
      /* Note we cannot pass is_final_input to las_mlc_input as there
       * may be multiple matches (LAS_MATCH) from las_lc_input() before
       * it emits no more input for las_mlc_input(). */
      las->mlc_has_final_input_ = 1;
    }
    else {
      assert(r == LAS_MATCH);

      xlts_append(&las->mlc_buf_, &las->lc_buf_);
    }

    r = las_mlc_input(las, las->mlc_has_final_input_);
    while (r != LAS_FEED_ME) {
      if (r == LAS_INTERNAL_ERROR) {
        return r;
      }
      else if (r == LAS_END_OF_INPUT) {
        las->las_input_reentry_from_match_ = 0;
        las->mlc_has_final_input_ = 0;
        return LAS_END_OF_INPUT;
      }
      else {
        assert(r == LAS_MATCH);
      }

      /* mlc_buf_ holds match for a single line */
      las->las_input_reentry_from_match_ = 1; /* next invocation of las_input returns us at back_for_more */
      return LAS_MATCH;
back_for_more:

      r = las_mlc_input(las, las->mlc_has_final_input_);
    } /* while (r != LAS_FEED_ME) on mlc */

    r = las_lc_input(las, input, input_size, is_final_input);
  } /* while (r != LAS_FEED_ME) on lc */

  /* Can only get here when las_lc_input() returns LAS_FEED_ME */
  assert(r == LAS_FEED_ME);

  return LAS_FEED_ME;
}
