
#ifndef STDIO_H_INCLUDED
#define STDIO_H_INCLUDED
#include <stdio.h>
#endif

#ifndef STDLIB_H_INCLUDED
#define STDLIB_H_INCLUDED
#include <stdlib.h>
#endif

#ifndef STDARG_H_INCLUDED
#define STDARG_H_INCLUDED
#include <stdarg.h>
#endif

#ifndef STRING_H_INCLUDED
#define STRING_H_INCLUDED
#include <string.h>
#endif

#ifndef SCANNER_H_INCLUDED
#define SCANNER_H_INCLUDED
#include "scanner.h"
#endif

#ifndef TOKENIZER_H_INCLUDED
#define TOKENIZER_H_INCLUDED
#include "tokenizer.h"
#endif

void tkr_tokenizer_init(struct tkr_tokenizer *tkr, const struct sc_scanner *scanner) {
  tkr->scanner_ = scanner;
  tkr->state_ = tkr->scanner_->start_state;

  tkr->input_index_ = 0;
  tkr->input_offset_ = 0;
  tkr->input_line_ = 1;
  tkr->input_col_ = 1;

  tkr->match_index_ = 0;

  xlts_init(&tkr->xmatch_);

  tkr->null_terminator_repair_ = '\0';
  tkr->token_size_ = 0;

  tkr->filename_ = "";
  tkr->start_offset_ = 0;
  tkr->start_line_ = 1;
  tkr->start_col_ = 1;

  tkr->offset_ = 0;
  tkr->line_ = 1;
  tkr->col_ = 1;

  tkr->best_match_action_ = tkr->scanner_->actions[tkr->scanner_->start_state].action;
  tkr->best_match_variant_ = tkr->scanner_->actions[tkr->scanner_->start_state].variant;
  tkr->best_match_size_ = 0;
  tkr->best_match_offset_ = 0;
  tkr->best_match_line_ = 1;
  tkr->best_match_col_ = 1;
}


void tkr_tokenizer_cleanup(struct tkr_tokenizer *tkr) {
  xlts_cleanup(&tkr->xmatch_);
}

void tkr_tokenizer_switch(struct tkr_tokenizer *tkr, const struct sc_scanner *scanner) {
  tkr->scanner_ = scanner;
  tkr->state_ = tkr->scanner_->start_state;
  tkr->best_match_action_ = tkr->scanner_->default_action;
}

void tkr_tokenizer_reset(struct tkr_tokenizer *tkr) {
  tkr->token_size_ = 0;
  xlts_reset(&tkr->xmatch_);
  tkr->state_ = tkr->scanner_->start_state;
  tkr->best_match_action_ = tkr->scanner_->actions[tkr->scanner_->start_state].action;
  tkr->best_match_variant_ = tkr->scanner_->actions[tkr->scanner_->start_state].variant;
  tkr->match_index_ = 0;
  tkr->input_index_ = 0;
}

int tkr_tokenizer_inputx(struct tkr_tokenizer *tkr, struct xlts *input, int is_final_input) {
  int r;
  uint8_t c;
  size_t state = tkr->state_;
  const size_t *transition_table = tkr->scanner_->transition_table;;
  const sc_action_t *actions = tkr->scanner_->actions;
  size_t default_action = tkr->scanner_->default_action;
  size_t start_state = tkr->scanner_->start_state;
  size_t start_action = actions[start_state].action;
  size_t start_variant = actions[start_state].variant;

  size_t match_index = tkr->match_index_;

  uintptr_t best_match_action = tkr->best_match_action_;
  uintptr_t best_match_variant = tkr->best_match_variant_;
  size_t best_match_size = tkr->best_match_size_;

  /* Move any prior token out of the way */
  if (tkr->token_size_) {
    /* Discard current token out of the way, repair buffer, get ready for next token */
    xlts_clamp_remove(&tkr->xmatch_, &tkr->xmatch_token_clamp_);
    xlts_strip_left_translated(&tkr->xmatch_, tkr->token_size_);

    /* Reset scanner to get ready for next token */
    tkr->match_index_ = 0;
    tkr->best_match_action_ = best_match_action = start_action;
    tkr->best_match_variant_ = best_match_variant = start_variant;
    tkr->best_match_size_ = best_match_size = 0;
    tkr->state_ = state = start_state;

    tkr->token_size_ = 0;
  }

  while (match_index < tkr->xmatch_.num_translated_) {
    c = ((const uint8_t *)tkr->xmatch_.translated_)[match_index];
    state = transition_table[256 * state + c];
    if (state) {

      uintptr_t state_action = actions[state].action;
      uintptr_t state_variant = actions[state].variant;
      if (state_action != tkr->scanner_->default_action) {
        best_match_action = state_action;
        best_match_variant = state_variant;
        best_match_size = match_index + 1;
      }
      match_index++;
    }
    else {
      if (best_match_action == default_action) {
        goto syntax_error;
      }

      /* Ensure token match is null terminated */
      r = xlts_clamp_left_translated(&tkr->xmatch_, best_match_size, &tkr->xmatch_token_clamp_);
      if (r) {
        return TKR_INTERNAL_ERROR;
      }
      tkr->token_size_ = best_match_size;

      tkr->best_match_action_ = best_match_action;
      tkr->best_match_variant_ = best_match_variant;
      tkr->best_match_size_ = best_match_size;

      return TKR_MATCH;
    }
  }

  size_t input_index = tkr->input_index_;

  while (input_index < input->num_translated_) {
    c = ((const uint8_t *)(input->translated_))[input_index];
    state = transition_table[256 * state + c];
    if (state) {

      uintptr_t state_action = actions[state].action;
      uintptr_t state_variant = actions[state].variant;
      if (state_action != default_action) {
        best_match_action = state_action;
        best_match_variant = state_variant;
        best_match_size = tkr->xmatch_.num_translated_ + input_index - tkr->input_index_ + 1;
      }

      input_index++;
    }
    else {
      /* Append From tkr->input_index To input_index, excluding input_index, but including any trailing "original" segments. */
      r = xlts_append_mid_translated(&tkr->xmatch_, input, tkr->input_index_, input_index);
      if (r) {
        return TKR_INTERNAL_ERROR;
      }
            
      if (best_match_action == default_action) {
        goto syntax_error;
      }

      /* Ensure token match is null terminated */
      r = xlts_clamp_left_translated(&tkr->xmatch_, best_match_size, &tkr->xmatch_token_clamp_);
      if (r) {
        return TKR_INTERNAL_ERROR;
      }
      tkr->token_size_ = best_match_size;

      tkr->best_match_action_ = best_match_action;
      tkr->best_match_variant_ = best_match_variant;
      tkr->best_match_size_ = best_match_size;

      tkr->input_index_ = input_index;

      return TKR_MATCH;
    }
  }
  r = xlts_append_mid_translated(&tkr->xmatch_, input, tkr->input_index_, input_index);
  if (r) {
    return TKR_INTERNAL_ERROR;
  }
  match_index = tkr->xmatch_.num_translated_;

  if (!is_final_input) {
    /* Need more input */
    tkr->state_ = state;

    tkr->token_size_ = 0; /* no match yet */

    tkr->input_index_ = 0;

    tkr->best_match_action_ = best_match_action;
    tkr->best_match_variant_ = best_match_variant;
    tkr->best_match_size_ = best_match_size;

    tkr->match_index_ = match_index;

    return TKR_FEED_ME;
  }

  /* There will be no more input, so don't ask for more, instead
   * match what we have; if we can't because we don't have any more,
   * return success, otherwise return a syntax error. */
  if (!tkr->xmatch_.num_translated_ && (tkr->input_index_ == input->num_translated_)) {
    /* Exhausted input, therefore no match and no error, just end of input.
     * Make sure we leave the tokenizer in a state where we can immediately re-use
     * it in its initial state. */
    tkr->match_index_ = 0;
    tkr->best_match_action_ = best_match_action = start_action;
    tkr->best_match_variant_ = best_match_variant = start_variant;
    tkr->best_match_size_ = best_match_size;
    tkr->state_ = state = start_state;

    tkr->token_size_ = 0;
    tkr->input_index_ = 0;

    return TKR_END_OF_INPUT;
  }

  if (best_match_action == default_action) {
    goto syntax_error;
  }

  /* Ensure token match is null terminated */
  r = xlts_clamp_left_translated(&tkr->xmatch_, best_match_size, &tkr->xmatch_token_clamp_);
  if (r) {
    return TKR_INTERNAL_ERROR;
  }
  tkr->token_size_ = best_match_size;

  tkr->best_match_action_ = best_match_action;
  tkr->best_match_variant_ = best_match_variant;
  tkr->best_match_size_ = best_match_size;

  tkr->input_index_ = input_index;

  return TKR_MATCH;

syntax_error:
  if (!tkr->xmatch_.num_translated_) {
    /* Note that if we have nothing in xmatch_, we therefore *must* have something in input at
     * tkr->input_index_ -- otherwise, without any input, there is no way we could have a syntax error */
    /* Move 1 character for syntax error into xmatch_ */
    r = xlts_append_mid_translated(&tkr->xmatch_, input, tkr->input_index_, tkr->input_index_ + 1);
    if (r) {
      return TKR_INTERNAL_ERROR;
    }
    /* Move input index one beyond */
    input_index = tkr->input_index_ + 1;
  }

  /* Reset scanner to get ready for next token prior to returning */
  tkr->token_size_ = 1;
  r = xlts_clamp_left_translated(&tkr->xmatch_, 1, &tkr->xmatch_token_clamp_);
  if (r) {
    return TKR_INTERNAL_ERROR;
  }

  tkr->input_index_ = input_index;

  return TKR_SYNTAX_ERROR;
}

int tkr_tokenizer_inputs(struct tkr_tokenizer *tkr, const char *input, size_t input_size, int is_final_input) {
  int r;
  uint8_t c;
  size_t state = tkr->state_;
  const size_t *transition_table = tkr->scanner_->transition_table;;
  const sc_action_t *actions = tkr->scanner_->actions;
  size_t default_action = tkr->scanner_->default_action;
  size_t start_state = tkr->scanner_->start_state;
  size_t start_action = actions[start_state].action;
  size_t start_variant = actions[start_state].variant;

  size_t match_index = tkr->match_index_;

  uintptr_t best_match_action = tkr->best_match_action_;
  uintptr_t best_match_variant = tkr->best_match_variant_;
  size_t best_match_size = tkr->best_match_size_;

  /* Move any prior token out of the way */
  if (tkr->token_size_) {
    /* Discard current token out of the way, repair buffer, get ready for next token */
    xlts_clamp_remove(&tkr->xmatch_, &tkr->xmatch_token_clamp_);
    xlts_strip_left_translated(&tkr->xmatch_, tkr->token_size_);

    /* Reset scanner to get ready for next token */
    tkr->match_index_ = 0;
    tkr->best_match_action_ = best_match_action = start_action;
    tkr->best_match_variant_ = best_match_variant = start_variant;
    tkr->best_match_size_ = best_match_size = 0;
    tkr->state_ = state = start_state;

    tkr->token_size_ = 0;
  }

  while (match_index < tkr->xmatch_.num_translated_) {
    c = ((const uint8_t *)tkr->xmatch_.translated_)[match_index];
    state = transition_table[256 * state + c];
    if (state) {

      uintptr_t state_action = actions[state].action;
      uintptr_t state_variant = actions[state].variant;
      if (state_action != tkr->scanner_->default_action) {
        best_match_action = state_action;
        best_match_variant = state_variant;
        best_match_size = match_index + 1;
      }
      match_index++;
    }
    else {
      if (best_match_action == default_action) {
        goto syntax_error;
      }

      /* Ensure token match is null terminated */
      r = xlts_clamp_left_translated(&tkr->xmatch_, best_match_size, &tkr->xmatch_token_clamp_);
      if (r) {
        return TKR_INTERNAL_ERROR;
      }
      tkr->token_size_ = best_match_size;

      tkr->best_match_action_ = best_match_action;
      tkr->best_match_variant_ = best_match_variant;
      tkr->best_match_size_ = best_match_size;

      return TKR_MATCH;
    }
  }

  size_t input_index = tkr->input_index_;
  size_t input_offset = tkr->input_offset_;
  int input_line = tkr->input_line_;
  int input_col = tkr->input_col_;

  while (input_index < input_size) {
    c = ((const uint8_t *)input)[input_index];
    state = transition_table[256 * state + c];
    if (state) {

      uintptr_t state_action = actions[state].action;
      uintptr_t state_variant = actions[state].variant;
      if (state_action != default_action) {
        best_match_action = state_action;
        best_match_variant = state_variant;
        best_match_size = tkr->xmatch_.num_translated_ + input_index - tkr->input_index_ + 1;
      }

      input_index++;
    }
    else {
      /* Append From tkr->input_index To input_index, excluding input_index, but including any trailing "original" segments. */
      size_t offs, base;
      size_t base_offset;
      int base_line, base_col;
      base_offset = input_offset;
      base_line = input_line;
      base_col = input_col;
      base = tkr->input_index_;
      for (offs = tkr->input_index_; offs < input_index; ++offs) {
        input_offset++;
        if (input[offs] != '\n') {
          input_col++;
        }
        else /* input[offs] == '\n' */ {
          r = xlts_append_equal(&tkr->xmatch_, tkr->filename_, base_line, base_col, base_offset, offs - base, input + base);
          if (r) {
            return TKR_INTERNAL_ERROR;
          }
          input_line++;
          input_col = 1;
          base_line = input_line;
          base_col = input_col;
          base_offset = input_offset;
          base = offs;
        }
      }
      r = xlts_append_equal(&tkr->xmatch_, tkr->filename_, base_line, base_col, base_offset, offs - base, input + base);
      if (r) {
        return TKR_INTERNAL_ERROR;
      }

      if (best_match_action == default_action) {
        goto syntax_error;
      }

      /* Ensure token match is null terminated */
      r = xlts_clamp_left_translated(&tkr->xmatch_, best_match_size, &tkr->xmatch_token_clamp_);
      if (r) {
        return TKR_INTERNAL_ERROR;
      }
      tkr->token_size_ = best_match_size;

      tkr->best_match_action_ = best_match_action;
      tkr->best_match_variant_ = best_match_variant;
      tkr->best_match_size_ = best_match_size;

      tkr->input_index_ = input_index;
      tkr->input_offset_ = input_offset;
      tkr->input_line_ = input_line;
      tkr->input_col_ = input_col;


      return TKR_MATCH;
    }
  }
  /* Append From tkr->input_index To input_index, excluding input_index, but including any trailing "original" segments. */
  size_t offs, base;
  int base_line, base_col;
  size_t base_offset;
  base_offset = input_offset;
  base_line = input_line;
  base_col = input_col;
  base = tkr->input_index_;
  for (offs = tkr->input_index_; offs < input_index; ++offs) {
    input_offset++;
    if (input[offs] != '\n') {
      input_col++;
    }
    else /* input[offs] == '\n' */ {
      r = xlts_append_equal(&tkr->xmatch_, tkr->filename_, base_line, base_col, base_offset, offs - base, input + base);
      if (r) {
        return TKR_INTERNAL_ERROR;
      }
      input_line++;
      input_col = 1;
      base_line = input_line;
      base_col = input_col;
      base_offset = input_offset;
      base = offs;
    }
  }
  match_index = tkr->xmatch_.num_translated_;
  if (offs != base) {
    r = xlts_append_equal(&tkr->xmatch_, tkr->filename_, base_line, base_col, base_offset, offs - base, input + base);
    if (r) {
      return TKR_INTERNAL_ERROR;
    }
  }

  if (!is_final_input) {
    /* Need more input */
    tkr->state_ = state;

    tkr->token_size_ = 0; /* no match yet */

    tkr->input_index_ = 0;
    tkr->input_offset_ = input_offset;
    tkr->input_line_ = input_line;
    tkr->input_col_ = input_col;


    tkr->best_match_action_ = best_match_action;
    tkr->best_match_variant_ = best_match_variant;
    tkr->best_match_size_ = best_match_size;

    tkr->match_index_ = match_index;

    return TKR_FEED_ME;
  }

  /* There will be no more input, so don't ask for more, instead
   * match what we have; if we can't because we don't have any more,
   * return success, otherwise return a syntax error. */
  if (!tkr->xmatch_.num_translated_ && (tkr->input_index_ == input_size)) {
    /* Exhausted input, therefore no match and no error, just end of input.
     * Make sure we leave the tokenizer in a state where we can immediately re-use
     * it in its initial state. */
    tkr->match_index_ = 0;
    tkr->best_match_action_ = best_match_action = start_action;
    tkr->best_match_variant_ = best_match_variant = start_variant;
    tkr->best_match_size_ = best_match_size;
    tkr->state_ = state = start_state;

    tkr->token_size_ = 0;
    tkr->input_index_ = 0;
    tkr->input_offset_ = input_offset;
    tkr->input_line_ = input_line;
    tkr->input_col_ = input_col;


    return TKR_END_OF_INPUT;
  }

  if (best_match_action == default_action) {
    goto syntax_error;
  }

  /* Ensure token match is null terminated */
  r = xlts_clamp_left_translated(&tkr->xmatch_, best_match_size, &tkr->xmatch_token_clamp_);
  if (r) {
    return TKR_INTERNAL_ERROR;
  }
  tkr->token_size_ = best_match_size;

  tkr->best_match_action_ = best_match_action;
  tkr->best_match_variant_ = best_match_variant;
  tkr->best_match_size_ = best_match_size;

  tkr->input_index_ = input_index;
  tkr->input_offset_ = input_offset;
  tkr->input_line_ = input_line;
  tkr->input_col_ = input_col;


  return TKR_MATCH;

syntax_error:
  if (!tkr->xmatch_.num_translated_) {
    /* Note that if we have nothing in xmatch_, we therefore *must* have something in input at
     * tkr->input_index_ -- otherwise, without any input, there is no way we could have a syntax error */
     /* Move 1 character for syntax error into xmatch_ */
    r = xlts_append_equal(&tkr->xmatch_, tkr->filename_, input_line, input_col, input_offset, 1, input + tkr->input_index_);
    if (r) {
      return TKR_INTERNAL_ERROR;
    }
    /* Move input index one beyond */
    input_offset++;
    if (input[tkr->input_index_] != '\n') {
      input_col++;
    }
    else {
      input_line++;
      input_col = 1;
    }
    input_index = tkr->input_index_ + 1;
  }

  /* Reset scanner to get ready for next token prior to returning */
  tkr->token_size_ = 1;
  r = xlts_clamp_left_translated(&tkr->xmatch_, 1, &tkr->xmatch_token_clamp_);
  if (r) {
    return TKR_INTERNAL_ERROR;
  }

  tkr->input_index_ = input_index;
  tkr->input_offset_ = input_offset;
  tkr->input_line_ = input_line;
  tkr->input_col_ = input_col;


  return TKR_SYNTAX_ERROR;
}


const char *tkr_str(const struct tkr_tokenizer *tkr) {
  return tkr->xmatch_.translated_ ? tkr->xmatch_.translated_ : "";
}

