#ifndef LINE_ASSEMBLY_H
#define LINE_ASSEMBLY_H

/* Line assembly, assembles lines of input.
 * Lines are sequences of input that end with either a newline, or the end of input.
 * Lines may be extended by using a line-continuation (a backslash followed by a newline, or a backslash followed by a 
 * carriage return and a newline.)
 * Lines may also be extended using a C-style multi-line comment such as the one you're reading.
 * These two may be combined, eg. the opening "/*" may be encoded as "/\\\r\n*" and this should
 * still work.
 * Downstream from line assembly we still need to generate accurate line and column numbers; so these need to be preserved.
 * Downstream parsing should proceed as if the line-continuations were never part of the input and as if the C-style 
 * comments were left untouched.
 * Downstream from line assembly, when we generate segments of code, we need to reproduce the line-continuations and 
 * comments as they appeared in the original input. So while the line-continuations and multi-line comments affect the 
 * input processing as if they've been removed from the input, on the code generating output, they must re-appear. They
 * must therefore be removed and kept at the same time.
 * Line assembly is surprisingly complicated. */

#ifndef STDINT_H_INCLUDED
#define STDINT_H_INCLUDED
#include <stdint.h>
#endif

#ifndef TOKENIZER_H_INCLUDED
#define TOKENIZER_H_INCLUDED
#include "tokenizer.h"
#endif

#ifndef XLTS_H_INCLUDED
#define XLTS_H_INCLUDED
#include "xlts.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define LAS_END_OF_INPUT 0
#define LAS_MATCH 1
#define LAS_INTERNAL_ERROR 3
#define LAS_FEED_ME 4
  
struct las_line_assembly {
  /* Line continuation translation phase */
  struct tkr_tokenizer lc_tkr_;

  int lc_clear_buffers_on_entry_ : 1,
      lc_last_line_emitted_ : 1,
      mlc_clear_buffers_on_entry_ : 1,
      mlc_last_line_emitted_ : 1,
      las_input_reentry_from_match_ : 1,
      mlc_has_final_input_ : 1;

  size_t mlc_cumulative_line_size_;

  struct xlts lc_buf_;

  /* Multi-line comment translation phase */
  struct tkr_tokenizer mlc_tkr_;
  
  struct xlts mlc_buf_;

  struct xlts_clamp mlc_buf_early_termination_undo_;
};

int las_init();
void las_cleanup();

void las_init_line_assembly(struct las_line_assembly *las);
void las_cleanup_line_assembly(struct las_line_assembly *las);
void las_set_filename(struct las_line_assembly *las, const char *filename);
int las_input(struct las_line_assembly *las, const char *input, size_t input_size, int is_final_input);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LINE_ASSEMBLY_H */
