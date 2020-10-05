#ifndef TOKENIZER_H
#define TOKENIZER_H

#ifndef STDINT_H_INCLUDED
#define STDINT_H_INCLUDED
#include <stdint.h>
#endif

#ifndef SCANNER_H_INCLUDED
#define SCANNER_H_INCLUDED
#include "scanner.h"
#endif

#ifndef XLTS_H_INCLUDED
#define XLTS_H_INCLUDED
#include "xlts.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define TKR_END_OF_INPUT 0
#define TKR_MATCH 1
#define TKR_SYNTAX_ERROR 2
#define TKR_INTERNAL_ERROR 3
#define TKR_FEED_ME 4
  
struct tkr_tokenizer {
  /* Scanner that implements the lexical rules */
  const struct sc_scanner *scanner_;

  int have_private_scanner_ : 1;

  /* State of scanner during lexical scanning */
  size_t state_;

  /* Current index in the input buffer passed to tkr_tokenizer_input */
  size_t input_index_;

  /* Stream offset, line and column of next byte of input */
  size_t input_offset_;
  int input_line_, input_col_;

  struct xlts xmatch_;
  struct xlts_clamp xmatch_token_clamp_;

  /* Current index in the match_ buffer during processing */
  size_t match_index_;

  /* The value originally at the null terminator at index token_size_ of
   * the match_ buffer. */
  char null_terminator_repair_;

  /* Size of the token in match_, valid only after TKR_MATCH is returned
   * from tkr_tokenizer_input(). */
  size_t token_size_;

  /* Filename of the input. May be set directly by callers; does not
   * influence lexical scanning directly. */
  const char *filename_;

  /* Offset, in bytes, to the first character of the match within the input 
   * stream. */
  size_t start_offset_;

  /* Line and column of the first character of the match, may be used
   * for error reporting or highlighting. */
  int start_line_, start_col_;

  /* Current offset, line and column in the input stream. */
  size_t offset_;
  int line_, col_;

  /* Best matched action, or scanner_.default_action if no match was (yet)
   * found. best_match_variant_ contains the variant of the best matched 
   * action (see scanner.h sc_action comments for a discussion of action and
   * variant.) */
  uintptr_t best_match_action_;
  uintptr_t best_match_variant_;

  /* Size of the current best match */
  size_t best_match_size_;

  /* Offset, line and column of the first character *after* the current
   * best match */
  size_t best_match_offset_;
  int best_match_line_, best_match_col_;
};


void tkr_tokenizer_init(struct tkr_tokenizer *tkr, const struct sc_scanner *scanner);

void tkr_tokenizer_cleanup(struct tkr_tokenizer *tkr);

void tkr_tokenizer_switch(struct tkr_tokenizer *tkr, const struct sc_scanner *scanner);

/* Clear any prior partial buffered state from tokenizer */
void tkr_tokenizer_reset(struct tkr_tokenizer *tkr);

int tkr_tokenizer_inputs(struct tkr_tokenizer *tkr, const char *input, size_t input_size, int is_final_input);
int tkr_tokenizer_inputx(struct tkr_tokenizer *tkr, struct xlts *input, int is_final_input);

const char *tkr_str(const struct tkr_tokenizer *tkr);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* TOKENIZER_H */
