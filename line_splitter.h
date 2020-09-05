#ifndef LINE_SPLITTER_H
#define LINE_SPLITTER_H

#ifndef STDINT_H_INCLUDED
#define STDINT_H_INCLUDED
#include <stdint.h>
#endif

#ifndef TOKENIZER_H_INCLUDED
#define TOKENIZER_H_INCLUDED
#include "tokenizer.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define LSSL_END_OF_INPUT 0
#define LSSL_MATCH 1
#define LSSL_INTERNAL_ERROR 3
#define LSSL_FEED_ME 4

typedef enum ls_chunk_type_enum {
  LS_CT_SKIP,
  LS_CT_SAME,
} ls_chunk_type_t;

struct ls_chunk {
  ls_chunk_type_t ct_;
  int start_line_, start_col_;
  size_t start_offset_;
  int end_line_, end_col_;
  size_t end_offset_;
  
  size_t num_original_bytes_;
  size_t num_stripped_bytes_;
};

struct ls_chunk_seq {
  size_t num_chunks_;
  size_t num_chunks_allocated_;
  struct ls_chunk *chunks_;
};

struct ls_line_splitter {
  struct tkr_tokenizer tkr_;

  /* Line number, column number, and offset, at the start of the current line in original_ and
   * stripped_, relative to the input. */
  int line_, col_;
  size_t offset_;

  int clear_buffers_on_entry_ : 1,
      last_line_emitted_ : 1;

  size_t num_original_;
  size_t num_original_allocated_;
  char *original_;

  size_t num_stripped_;
  size_t num_stripped_allocated_;
  char *stripped_;

  struct ls_chunk_seq chunks_;
};

void lscs_init_chunk_seq(struct ls_chunk_seq *lscs);
void lscs_cleanup_chunk_seq(struct ls_chunk_seq *lscs);

int ls_init(void);
void ls_cleanup(void);

void ls_init_line_splitter(struct ls_line_splitter *ls);
void ls_cleanup_line_splitter(struct ls_line_splitter *ls);

int ls_input(struct ls_line_splitter *ls, const char *input, size_t input_size, int is_final_input);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LINE_SPLITTER_H */

