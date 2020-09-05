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
#define KLT_LOG_MODULE "line_splitter"
#include "klt_logger.h"
#endif

#ifndef TOKENIZER_H_INCLUDED
#define TOKENIZER_H_INCLUDED
#include "tokenizer.h"
#endif

#ifndef LINE_SPLITTER_H_INCLUDED
#define LINE_SPLITTER_H_INCLUDED
#include "line_splitter.h"
#endif

static struct tkr_tokenizer tkr_line_splitter_;

/* Anything other than LS_NEW_LINE and LS_LINE_CONTUATION is line data */
#define ENUM_LINE_SPLITS \
xz(LS_UNKNOWN) \
xx("\\r\\n|\\n", LS_NEW_LINE) \
xx("\\\\\\r\\n|\\n", LS_LINE_CONTINUATION)

typedef enum ls_split_type {
#define xx(regex, type_of_line) type_of_line,
#define xz(type_of_line) type_of_line,
  ENUM_LINE_SPLITS
#undef xx
#undef xz
} ls_split_type_t;

static const struct sc_scan_rule g_scanner_rules_[] = {
#define xx(regex, line_type) { regex, line_type, line_type },
#define xz(line_type)
ENUM_LINE_SPLITS
#undef xz
#undef xx
};

const char *ls_line_split_to_str(ls_split_type_t lsst) {
#define xx(regex, type_of_split) case type_of_split: return #type_of_split;
#define xz(type_of_split) case type_of_split: return #type_of_split;
  switch (lsst) {
    ENUM_LINE_SPLITS
  }
#undef xx
#undef xz
  return "?";
}

struct sc_scanner g_ls_scanner_;

void lscs_init_chunk_seq(struct ls_chunk_seq *lscs) {
  lscs->num_chunks_ = lscs->num_chunks_allocated_ = 0;
  lscs->chunks_ = NULL;
}

void lscs_cleanup_chunk_seq(struct ls_chunk_seq *lscs) {
  if (lscs->chunks_) free(lscs->chunks_);
}

int ls_init(void) {
  sc_scanner_init(&g_ls_scanner_);
  return sc_scanner_compile(&g_ls_scanner_, LS_UNKNOWN, sizeof(g_scanner_rules_) / sizeof(*g_scanner_rules_), g_scanner_rules_);
}

void ls_cleanup(void) {
  sc_scanner_cleanup(&g_ls_scanner_);
}

void ls_init_line_splitter(struct ls_line_splitter *ls) {
  ls->clear_buffers_on_entry_ = 0; /* this value is sensitive at construction and may not be 1, see comment at ls_input(). */
  ls->last_line_emitted_ = 0;

  ls->num_original_ = ls->num_original_allocated_ = 0;
  ls->original_ = NULL;
  ls->num_stripped_ = ls->num_stripped_allocated_ = 0;
  ls->stripped_ = NULL;
  lscs_init_chunk_seq(&ls->chunks_);

  ls->line_ = 1;
  ls->col_ = 1;
  ls->offset_ = 0;

  tkr_tokenizer_init(&ls->tkr_, &g_ls_scanner_);
}

void ls_cleanup_line_splitter(struct ls_line_splitter *ls) {
  if (ls->original_) free(ls->original_);
  if (ls->stripped_) free(ls->stripped_);
  tkr_tokenizer_cleanup(&ls->tkr_);
  lscs_cleanup_chunk_seq(&ls->chunks_);
}


static int ls_append_match_to_original(struct ls_line_splitter *ls) {
  size_t size_needed = ls->num_original_ + ls->tkr_.token_size_;
  if (size_needed < ls->num_original_) {
    LOGERROR("Error: overflow on reallocation\n");
    return LSSL_INTERNAL_ERROR;
  }
  /* add in null terminator */
  if ((size_needed + 1) < size_needed) {
    LOGERROR("Error: overflow on reallocation\n");
    return LSSL_INTERNAL_ERROR;
  }
  size_needed++;
  if (size_needed < 128) {
    /* start with a decent minimum */
    size_needed = 128;
  }
  if (size_needed > ls->num_original_allocated_) {
    size_t size_to_allocate = ls->num_original_allocated_ + ls->num_original_allocated_ + 1;
    if (size_to_allocate <= ls->num_original_allocated_) {
      LOGERROR("Error: overflow on reallocation\n");
      return LSSL_INTERNAL_ERROR;
    }
    if (size_to_allocate < size_needed) {
      size_to_allocate = size_needed;
    }
    void *p = realloc(ls->original_, size_to_allocate);
    if (!p) {
      LOGERROR("Error: no memory\n");
      return LSSL_INTERNAL_ERROR;
    }
    ls->original_ = p;
    ls->num_original_allocated_ = size_to_allocate;
  }
  memcpy(ls->original_ + ls->num_original_, ls->tkr_.match_, ls->tkr_.token_size_);
  ls->num_original_ += ls->tkr_.token_size_;
  ls->original_[ls->num_original_] = '\0';
  return 0;
}

static int ls_append_match_to_stripped(struct ls_line_splitter *ls) {
  size_t size_needed = ls->num_stripped_ + ls->tkr_.token_size_;
  if (size_needed < ls->num_stripped_) {
    LOGERROR("Error: overflow on reallocation\n");
    return LSSL_INTERNAL_ERROR;
  }
  /* add in null terminator */
  if ((size_needed + 1) < size_needed) {
    LOGERROR("Error: overflow on reallocation\n");
    return LSSL_INTERNAL_ERROR;
  }
  size_needed++;
  if (size_needed < 128) {
    /* start with a decent minimum */
    size_needed = 128;
  }
  if (size_needed > ls->num_stripped_allocated_) {
    size_t size_to_allocate = ls->num_stripped_allocated_ + ls->num_stripped_allocated_ + 1;
    if (size_to_allocate <= ls->num_stripped_allocated_) {
      LOGERROR("Error: overflow on reallocation\n");
      return LSSL_INTERNAL_ERROR;
    }
    if (size_to_allocate < size_needed) {
      size_to_allocate = size_needed;
    }
    void *p = realloc(ls->stripped_, size_to_allocate);
    if (!p) {
      LOGERROR("Error: no memory\n");
      return LSSL_INTERNAL_ERROR;
    }
    ls->stripped_ = p;
    ls->num_stripped_allocated_ = size_to_allocate;
  }
  memcpy(ls->stripped_ + ls->num_stripped_, ls->tkr_.match_, ls->tkr_.token_size_);
  ls->num_stripped_ += ls->tkr_.token_size_;
  ls->stripped_[ls->num_stripped_] = '\0';
  return 0;
}

static struct ls_chunk *lscs_new_typed_tail_chunk(struct ls_chunk_seq *lscs, ls_chunk_type_t lsct) {
  if (lscs->num_chunks_ == lscs->num_chunks_allocated_) {
    size_t num_to_alloc = lscs->num_chunks_allocated_ * 2 + 1;
    if (num_to_alloc <= lscs->num_chunks_allocated_) {
      LOGERROR("Error: overflow on reallocation\n");
      return NULL;
    }
    if (num_to_alloc > (SIZE_MAX / sizeof(struct ls_chunk))) {
      LOGERROR("Error: overflow on reallocation\n");
      return NULL;
    }
    if (num_to_alloc < 16) {
      num_to_alloc = 16;
    }
    size_t size_to_allocate = num_to_alloc * sizeof(struct ls_chunk);
    void *p = realloc(lscs->chunks_, size_to_allocate);
    if (!p) {
      LOGERROR("Error: no memory\n");
      return NULL;
    }
    lscs->chunks_ = (struct ls_chunk *)p;
    lscs->num_chunks_allocated_ = num_to_alloc;
  }
  struct ls_chunk *lsc = lscs->chunks_ + lscs->num_chunks_++;
  lsc->ct_ = lsct;
  lsc->start_line_ = 0;
  lsc->start_col_ = 0;
  lsc->start_offset_ = 0;
  lsc->end_line_ = 0;
  lsc->end_col_ = 0;
  lsc->end_offset_ = 0;

  lsc->num_original_bytes_ = 0;
  return lsc;
}

static int lscs_chunks_are_adjacent(struct ls_chunk *a, struct ls_chunk *b) {
  if (a->ct_ != b->ct_) return 0;
  if ((a->end_col_ == b->start_col_) &&
    (a->end_line_ == b->start_line_) &&
    (a->end_offset_ == b->start_offset_)) {
    return 1;
  }
  return 0;
}


static struct ls_chunk *ls_typed_tail_chunk(struct ls_line_splitter *ls, ls_chunk_type_t lsct) {
  struct ls_chunk *lsc = NULL;
  if (ls->chunks_.num_chunks_) {
    lsc = ls->chunks_.chunks_ + ls->chunks_.num_chunks_ - 1;
  }

  if (!lsc || 
      (lsc->ct_ != lsct) ||
      (lsc->end_line_ != ls->tkr_.start_line_) ||
      (lsc->end_col_ != ls->tkr_.start_col_) ||
      (lsc->end_offset_ != ls->tkr_.start_offset_)) {
    /* Chunk cannot be contiguous with predecessor */
    lsc = lscs_new_typed_tail_chunk(&ls->chunks_, lsct);
    lsc->start_line_ = ls->tkr_.start_line_;
    lsc->start_col_ = ls->tkr_.start_col_;
    lsc->start_offset_ = ls->tkr_.start_offset_;
  }

  lsc->end_line_ = ls->tkr_.best_match_line_;
  lsc->end_col_ = ls->tkr_.best_match_col_;
  lsc->end_offset_ = ls->tkr_.best_match_offset_;

  return lsc;
}

static int ls_append_same(struct ls_line_splitter *ls) {
  int r;
  struct ls_chunk *lsc = ls_typed_tail_chunk(ls, LS_CT_SAME);
  if (!lsc) {
    return LSSL_INTERNAL_ERROR;
  }
  lsc->num_original_bytes_ += ls->tkr_.token_size_;
  lsc->num_stripped_bytes_ += ls->tkr_.token_size_;

  /* Append to buffers and return */
  r = ls_append_match_to_original(ls);
  if (r) return r;
  r = ls_append_match_to_stripped(ls);
  return r;
}

static int ls_append_skip(struct ls_line_splitter *ls) {
  int r;
  struct ls_chunk *lsc = ls_typed_tail_chunk(ls, LS_CT_SKIP);
  if (!lsc) {
    return LSSL_INTERNAL_ERROR;
  }
  lsc->num_original_bytes_ += ls->tkr_.token_size_;

  /* Append to buffers and return */
  r = ls_append_match_to_original(ls);
  return r;
}


int ls_input(struct ls_line_splitter *ls, const char *input, size_t input_size, int is_final_input) {
  int r;
  if (ls->clear_buffers_on_entry_) {
    ls->clear_buffers_on_entry_ = 0;
    ls->num_original_ = ls->num_stripped_ = 0;
    ls->chunks_.num_chunks_ = 0;
    /* starting a new line; copy over tokenizer's current location at the start of our line; the
     * current location is not the start of the last match (in tkr_.start_line_ etc.) but the
     * end of the current match (in tkr_.best_match_line_ etc.) 
     * We are therefore dependant on having matched at least 1 newline prior to clear_buffers_on_entry_
     * being true. */
    ls->line_ = ls->tkr_.best_match_line_;
    ls->col_ = ls->tkr_.best_match_col_;
    ls->offset_ = ls->tkr_.best_match_offset_;
  }
  for (;;) {
    r = tkr_tokenizer_input(&ls->tkr_, input, input_size, is_final_input);
    switch (r) {
    case TKR_END_OF_INPUT:
      if (!ls->last_line_emitted_) {
        ls->last_line_emitted_ = 1;
        ls->clear_buffers_on_entry_ = 1;
        return LSSL_MATCH;
      }
      return LSSL_END_OF_INPUT;
    case TKR_MATCH:
      if (ls->tkr_.best_match_variant_ == LS_NEW_LINE) {
        r = ls_append_same(ls);
        if (r) return r;
        ls->clear_buffers_on_entry_ = 1;
        return LSSL_MATCH;
      }
      else if (ls->tkr_.best_match_variant_ == LS_LINE_CONTINUATION) {
        /* Append to original as-is, but consume for the stripped (line-continued) version */
        r = ls_append_skip(ls);
        if (r) return r;
        /* keep going.. */
      }
      else {
        assert(0 && "LS_ Options exhausted\n");
      }
      break;
    case TKR_SYNTAX_ERROR:
      /* Syntax error, meaning: "this is not a newline type situation" -- append to both buffers */
      r = ls_append_same(ls);
      if (r) return r;
      /* keep going.. */
      break;
    case TKR_INTERNAL_ERROR:
      return LSSL_INTERNAL_ERROR;
    case TKR_FEED_ME:
      return LSSL_FEED_ME;
    }
  }
}
