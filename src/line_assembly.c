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

#ifndef LINE_ASSEMBLY_H_INCLUDED
#define LINE_ASSEMBLY_H_INCLUDED
#include "line_assembly.h"
#endif

/* Anything other than LS_NEW_LINE and LS_LINE_CONTUATION is line data */
#define ENUM_LC_TOKENS \
xz(LAS_LC_UNKNOWN) \
xx("\\r\\n|\\n", LAS_LC_NEW_LINE) \
xx("\\\\\\r\\n|\\n", LAS_LC_LINE_CONTINUATION)

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


int las_init() {
  int r;
  sc_scanner_init(&g_las_lc_scanner_);
  sc_scanner_init(&g_las_mlc_scanner_);

  r = sc_scanner_compile(&g_las_lc_scanner_, LAS_LC_UNKNOWN, sizeof(g_las_lc_scanner_rules_) / sizeof(*g_las_lc_scanner_rules_), g_las_lc_scanner_rules_);
  if (r) return r;

  r = sc_scanner_compile(&g_las_mlc_scanner_, LAS_MLC_UNKNOWN, sizeof(g_las_mlc_scanner_rules_) / sizeof(*g_las_mlc_scanner_rules_), g_las_mlc_scanner_rules_);
  return r;
}

void las_cleanup() {
  sc_scanner_cleanup(&g_las_lc_scanner_);
  sc_scanner_cleanup(&g_las_mlc_scanner_);
}

void las_init_line_assembly(struct las_line_assembly *las) {
  tkr_tokenizer_init(&las->lc_tkr_, &g_las_lc_scanner_);

  las->lc_clear_buffers_on_entry_ = 0;
  las->lc_last_line_emitted_ = 0;

  las->lc_num_original_ = las->lc_num_original_allocated_ = 0;
  las->lc_original_ = NULL;

  las->lc_num_translated_ = las->lc_num_translated_allocated_ = 0;
  las->lc_translated_ = NULL;

  las->lc_num_chunks_ = las->lc_num_chunks_allocated_ = 0;
  las->lc_chunks_ = NULL;

  tkr_tokenizer_init(&las->mlc_tkr_, &g_las_mlc_scanner_);

  las->mlc_num_original_ = las->mlc_num_original_allocated_ = 0;
  las->mlc_original_ = NULL;

  las->mlc_num_translated_ = las->mlc_num_translated_allocated_ = 0;
  las->mlc_translated_ = NULL;

  las->mlc_num_chunks_ = las->mlc_num_chunks_allocated_ = 0;
  las->mlc_chunks_ = NULL;

}

void las_cleanup_line_assembly(struct las_line_assembly *las) {
  tkr_tokenizer_cleanup(&las->lc_tkr_);
  if (las->lc_original_) free(las->lc_original_);
  if (las->lc_translated_) free(las->lc_translated_);
  if (las->lc_chunks_) free(las->lc_chunks_);

  tkr_tokenizer_cleanup(&las->mlc_tkr_);
  if (las->mlc_original_) free(las->mlc_original_);
  if (las->mlc_translated_) free(las->mlc_translated_);
  if (las->mlc_chunks_) free(las->mlc_chunks_);
}

static struct las_chunk *las_lc_new_typed_tail_chunk(struct las_line_assembly *las, las_chunk_type_t lasct) {
  if (las->lc_num_chunks_ == las->lc_num_chunks_allocated_) {
    size_t num_to_alloc = las->lc_num_chunks_allocated_ * 2 + 1;
    if (num_to_alloc <= las->lc_num_chunks_allocated_) {
      LOGERROR("Error: overflow on reallocation\n");
      return NULL;
    }
    if (num_to_alloc > (SIZE_MAX / sizeof(struct las_chunk))) {
      LOGERROR("Error: overflow on reallocation\n");
      return NULL;
    }
    if (num_to_alloc < 16) {
      num_to_alloc = 16;
    }
    size_t size_to_allocate = num_to_alloc * sizeof(struct las_chunk);
    void *p = realloc(las->lc_chunks_, size_to_allocate);
    if (!p) {
      LOGERROR("Error: no memory\n");
      return NULL;
    }
    las->lc_chunks_ = (struct las_chunk *)p;
    las->lc_num_chunks_allocated_ = num_to_alloc;
  }
  struct las_chunk *lasc = las->lc_chunks_ + las->lc_num_chunks_++;
  lasc->ct_ = lasct;
  lasc->start_line_ = 0;
  lasc->start_col_ = 0;
  lasc->start_offset_ = 0;
  lasc->end_line_ = 0;
  lasc->end_col_ = 0;
  lasc->end_offset_ = 0;

  lasc->num_original_bytes_ = 0;
  lasc->num_stripped_bytes_ = 0;
  return lasc;
}

static struct las_chunk *las_lc_typed_tail_chunk(struct las_line_assembly *las, las_chunk_type_t lasct) {
  struct las_chunk *lasc = NULL;
  if (las->lc_num_chunks_) {
    lasc = las->lc_chunks_ + las->lc_num_chunks_ - 1;
  }

  if (!lasc ||
      (lasc->ct_ != lasct) ||
      (lasc->end_line_ != las->lc_tkr_.start_line_) ||
      (lasc->end_col_ != las->lc_tkr_.start_col_) ||
      (lasc->end_offset_ != las->lc_tkr_.start_offset_)) {
    /* Chunk cannot be continguous with predecessor */
    lasc = las_lc_new_typed_tail_chunk(las, lasct);
    lasc->start_line_ = las->lc_tkr_.start_line_;
    lasc->start_col_ = las->lc_tkr_.start_col_;
    lasc->start_offset_ = las->lc_tkr_.start_offset_;
  }
  lasc->end_line_ = las->lc_tkr_.best_match_line_;
  lasc->end_col_ = las->lc_tkr_.best_match_col_;
  lasc->end_offset_ = las->lc_tkr_.best_match_offset_;

  return lasc;
}

static int las_lc_append_same(struct las_line_assembly *las) {
  int r;
  struct las_chunk *lasc = las_lc_typed_tail_chunk(las, LAS_CT_SAME);
  if (!lasc) {
    return LAS_INTERNAL_ERROR;
  }
  lasc->num_original_bytes_ += las->lc_tkr_.token_size_;
  lasc->num_stripped_bytes_ += las->lc_tkr_.token_size_;

  r = las_lc_append_match_to_original(las);
  if (r) return r;
  r = las_lc_append_match_to_stripped(las);
  return r;
}

static int las_lc_append_skip(struct las_line_assembly *las) {
  int r;
  struct las_chunk *lasc = las_lc_typed_tail_chunk(las, LAS_CT_SKIP);
  if (!lasc) {
    return LAS_INTERNAL_ERROR;
  }
  lasc->num_original_bytes_ += las->lc_tkr_.token_size_;

  r = las_lc_append_match_to_original(las);
  return r;
}

static int las_lc_input(struct las_line_assembly *las, const char *input, size_t input_size, int is_final_input) {
  int r;
  if (las->lc_clear_buffers_on_entry_) {
    las->lc_clear_buffers_on_entry_ = 0;
    las->lc_num_original_ = las->lc_num_translated_ = 0;
    las->lc_num_chunks_ = 0;
  }
  for (;;) {
    r = tkr_tokenizer_input(&las->lc_tkr_, input, input_size, is_final_input);
    switch (r) {
    case TKR_END_OF_INPUT:
      if (!las->lc_last_line_emitted_) {
        las->lc_last_line_emitted_ = 1;
        las->lc_clear_buffers_on_entry_ = 1;
        return LAS_MATCH;
      }
    case TKR_MATCH:
      if (las->lc_tkr_.best_match_variant_ == LAS_LC_NEW_LINE) {
        r = las_lc_append_same(las);
        if (r) return r;
        las->lc_clear_buffers_on_entry_ = 1;
        return LAS_MATCH;
      }
      else if (las->lc_tkr_.best_match_variant_ == LAS_LC_LINE_CONTINUATION) {
        r = las_lc_append_skip(las);
        if (r) return r;
      }
      else {
        /* No more LAS LC token types */
        assert(0);
      }
      break;
    case TKR_SYNTAX_ERROR:
      r = las_lc_append_same(las);
      if (r) return r;
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
  return las_lc_input(las, input, input_size, is_final_input);
}
