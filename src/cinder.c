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

#ifndef HELPERS_H_INCLUDED
#define HELPERS_H_INCLUDED
#include "helpers.h"
#endif

#ifndef KLT_LOGGER_H_INCLUDED
#define KLT_LOGGER_H_INCLUDED
#define KLT_LOG_MODULE "cinder"
#include "klt_logger.h"
#endif

#ifndef LOG_FUNCTION_H_INCLUDED
#define LOG_FUNCTION_H_INCLUDED
#include "log_function.h"
#endif

#ifndef SCANNER_H_INCLUDED
#define SCANNER_H_INCLUDED
#include "scanner.h"
#endif

#ifndef TOKENIZER_H_INCLUDED
#define TOKENIZER_H_INCLUDED
#include "tokenizer.h"
#endif

#define LDL_END_OF_INPUT 0
#define LDL_MATCH 1
#define LDL_SYNTAX_ERROR 2
#define LDL_INTERNAL_ERROR 3
#define LDL_FEED_ME 4

#define ENUM_LINE_DEFS \
xz(LD_UNKNOWN) \
xx("[ \\x9\\xb\\xc\\xd]*#([^\\\\\\n]|\\\\+[^\\\\])*\\n?", LD_C_PREPROCESSOR) \
xx("[ \\x9\\xb\\xc\\xd]*%%[ \\x9\\xb\\xc\\xd]*\\n?", LD_CINDER_SECTION_DELIMITER) \
xx("[ \\x9\\xb\\xc\\xd]*%([^\\\\\\n]|\\\\+[^\\\\])*\\n?", LD_CINDER_DIRECTIVE) \
xx("([^\\\\\\n]|\\\\+[^\\\\])*\\n?", LD_REGULAR) \

typedef enum ld_line_type {
#define xx(regex, type_of_line) type_of_line,
#define xz(type_of_line) type_of_line,
  ENUM_LINE_DEFS
#undef xx
#undef xz
} ld_line_type_t;

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

struct ld_line {
  ld_line_type_t type_of_line_;
  
  int free_chars_on_cleanup_ : 1;

  size_t num_chars_;
  size_t num_chars_allocated_;
  char *chars_;

  const char *filename_;
  size_t start_offset_;
  int start_line_;
  int start_col_;
  size_t end_offset_;
  int end_line_;
  int end_col_;

};

struct ldl_scanner {
  struct sc_scanner scanner_;
  size_t state_;

  size_t input_index_;

  char *match_;
  size_t match_size_;
  size_t match_size_allocated_;
  size_t match_index_;

  char null_terminator_repair_;
  size_t token_size_;

  const char *filename_;
  size_t start_offset_;
  int start_line_, start_col_;

  size_t offset_;
  int line_, col_;

  uintptr_t best_match_;
  size_t best_match_offset_;
  int best_match_line_, best_match_col_;
};

void ld_line_init(struct ld_line *ldl);
void ld_line_cleanup(struct ld_line *ldl);

int ldl_scanner_init(struct ldl_scanner *sc);
void ldl_scanner_cleanup(struct ldl_scanner *sc);
int ldl_scanner_input(struct ldl_scanner *sc, struct ld_line *ldl, const char *input, size_t input_size, int is_final_input);

static int ldl_fill_token(struct ld_line *ldl, ld_line_type_t minor, const char *begin, const char *end);


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

static const struct sc_scan_rule g_scanner_rules_[] = {
#define xx(regex, line_type) { regex, line_type, line_type },
#define xz(line_type)
ENUM_LINE_DEFS
#undef xz
#undef xx
};

int ldl_scanner_init(struct ldl_scanner *sc) {
  sc_scanner_init(&sc->scanner_);
  int r;
  r = sc_scanner_compile(&sc->scanner_, LD_UNKNOWN, sizeof(g_scanner_rules_) / sizeof(*g_scanner_rules_), g_scanner_rules_);
  if (r) {
    LOGERROR("Failed to compile line scanner\n");
    return LDL_INTERNAL_ERROR;
  }

  sc->state_ = sc->scanner_.start_state;

  sc->input_index_ = 0;

  sc->match_ = NULL;
  sc->match_size_ = 0;
  sc->match_size_allocated_ = 0;
  sc->match_index_ = 0;

  sc->null_terminator_repair_ = '\0';
  sc->token_size_ = 0;

  sc->filename_ = "";
  sc->start_offset_ = 0;
  sc->start_line_ = 1;
  sc->start_col_ = 1;

  sc->offset_ = 0;
  sc->line_ = 1;
  sc->col_ = 1;

  sc->best_match_ = sc->scanner_.default_action;
  sc->best_match_offset_ = 0;
  sc->best_match_line_ = 1;
  sc->best_match_col_ = 1;

  return 0;
}

void ldl_scanner_cleanup(struct ldl_scanner *sc) {
  if (sc->match_) free(sc->match_);
  sc_scanner_cleanup(&sc->scanner_);
}

static int ldl_scanner_reserve_match(struct ldl_scanner *sc) {
  if (sc->match_size_ == sc->match_size_allocated_) {
    size_t new_size = sc->match_size_allocated_ + sc->match_size_allocated_ + 1;
    if (new_size <= sc->match_size_allocated_) {
      LOGERROR("Overflow of buffer allocation size\n");
      return LDL_INTERNAL_ERROR;
    }
    void *buf = realloc(sc->match_, new_size);
    if (!buf) {
      LOGERROR("Failed to allocate %zu bytes\n", new_size);
      return LDL_INTERNAL_ERROR;
    }
    sc->match_ = (char *)buf;
    sc->match_size_allocated_ = new_size;
  }
  return 0;
}

static int ldl_scanner_append_match(struct ldl_scanner *sc, uint8_t c) {
  int r;
  r = ldl_scanner_reserve_match(sc);
  if (r) return r;
  sc->match_[sc->match_size_++] = (char)c;
  return 0;
}

int ldl_scanner_input(struct ldl_scanner *sc, struct ld_line *ldl, const char *input, size_t input_size, int is_final_input) {
  int r;
  uint8_t c;
  size_t state = sc->state_;

  size_t match_index = sc->match_index_;

  size_t start_offset = sc->start_offset_;
  int start_line = sc->start_line_;
  int start_col = sc->start_col_;

  size_t offset = sc->offset_;
  int line = sc->line_;
  int col = sc->col_;

  uintptr_t best_match = sc->best_match_;
  size_t best_match_offset = sc->best_match_offset_;
  int best_match_line = sc->best_match_line_;
  int best_match_col = sc->best_match_col_;

  /* Move any prior token out of the way */
  if (sc->token_size_) {
    memcpy(sc->match_, sc->match_ + sc->token_size_, sc->match_size_);
    sc->token_size_ = 0;
  }
  /* Recover character originally at null terminator */
  if (sc->match_) sc->match_[0] = sc->null_terminator_repair_;

  while (match_index < sc->match_size_) {
    c = ((const uint8_t *)sc->match_)[match_index];
    state = sc->scanner_.transition_table[256 * state + c];
    if (state) {
      if (c != '\n') {
        col++;
      }
      else {
        col = 1;
        line++;
      }
      offset++;

      uintptr_t state_action = sc->scanner_.actions[state].action;
      if (state_action != sc->scanner_.default_action) {
        best_match = state_action;
        best_match_offset = offset;
        best_match_line = line;
        best_match_col = col;
      }
      match_index++;
    }
    else {
      if (best_match == sc->scanner_.default_action) {
        goto syntax_error;
      }

      ldl->type_of_line_ = best_match;
      ldl->filename_ = sc->filename_;
      ldl->start_offset_ = start_offset;
      ldl->start_line_ = start_line;
      ldl->start_col_ = start_col;
      ldl->end_offset_ = best_match_offset;
      ldl->end_line_ = best_match_line;
      ldl->end_col_ = best_match_col;

      size_t token_size = best_match_offset - start_offset;

      /* Ensure token match is null terminated */
      ldl_scanner_reserve_match(sc);
      sc->null_terminator_repair_ = sc->match_[token_size];
      sc->match_[token_size] = '\0';
      sc->token_size_ = token_size;

      ldl_fill_token(ldl, (ld_line_type_t)best_match, sc->match_, sc->match_ + token_size);

      /* Reset scanner to get ready for next token prior to return to caller; the match_ buffer 
       * will be reset upon entry */
      sc->match_size_ = sc->match_size_ - token_size;
      sc->match_index_ = 0;
      sc->best_match_ = sc->scanner_.default_action;
      sc->state_ = sc->scanner_.start_state;

      sc->start_offset_ = best_match_offset;
      sc->start_line_ = best_match_line;
      sc->start_col_ = best_match_col;

      sc->offset_ = best_match_offset;
      sc->line_ = best_match_line;
      sc->col_ = best_match_col;

      sc->best_match_offset_ = best_match_offset;
      sc->best_match_line_ = best_match_line;
      sc->best_match_col_ = best_match_col;

      return LDL_MATCH;
    }
  }

  size_t input_index = sc->input_index_;

  while (input_index < input_size) {
    c = ((const uint8_t *)input)[input_index];
    state = sc->scanner_.transition_table[256 * state + c];
    if (state) {
      if (c != '\n') {
        col++;
      }
      else {
        col = 1;
        line++;
      }
      offset++;

      uintptr_t state_action = sc->scanner_.actions[state].action;
      if (state_action != sc->scanner_.default_action) {
        best_match = state_action;
        best_match_offset = offset;
        best_match_line = line;
        best_match_col = col;
      }

      if ((r = ldl_scanner_append_match(sc, c))) {
        return r;
      }
      input_index++;
    }
    else {
      if (best_match == sc->scanner_.default_action) {
        goto syntax_error;
      }

      ldl->type_of_line_ = best_match;
      ldl->filename_ = sc->filename_;
      ldl->start_offset_ = start_offset;
      ldl->start_line_ = start_line;
      ldl->start_col_ = start_col;
      ldl->end_offset_ = best_match_offset;
      ldl->end_line_ = best_match_line;
      ldl->end_col_ = best_match_col;

      size_t token_size = best_match_offset - start_offset;

      /* Ensure token match is null terminated */
      ldl_scanner_reserve_match(sc);
      sc->null_terminator_repair_ = sc->match_[token_size];
      sc->match_[token_size] = '\0';
      sc->token_size_ = token_size;

      ldl_fill_token(ldl, (ld_line_type_t)best_match, sc->match_, sc->match_ + token_size);

      /* Reset scanner to get ready for next token prior to return to caller; the match_ buffer
       * will be reset upon entry */
      sc->match_size_ = sc->match_size_ - token_size;
      sc->match_index_ = 0;
      sc->best_match_ = sc->scanner_.default_action;
      sc->state_ = sc->scanner_.start_state;

      sc->start_offset_ = best_match_offset;
      sc->start_line_ = best_match_line;
      sc->start_col_ = best_match_col;

      sc->offset_ = best_match_offset;
      sc->line_ = best_match_line;
      sc->col_ = best_match_col;

      sc->best_match_offset_ = best_match_offset;
      sc->best_match_line_ = best_match_line;
      sc->best_match_col_ = best_match_col;

      sc->input_index_ = input_index;

      return LDL_MATCH;
    }
  }

  if (!is_final_input) {
    /* Need more input */
    sc->state_ = state;

    if (sc->match_) sc->null_terminator_repair_ = sc->match_[0];

    sc->input_index_ = 0;

    sc->start_offset_ = start_offset;
    sc->start_line_ = start_line;
    sc->start_col_ = start_col;

    sc->offset_ = offset;
    sc->line_ = line;
    sc->col_ = col;

    sc->best_match_ = best_match;
    sc->best_match_offset_ = best_match_offset;
    sc->best_match_line_ = best_match_line;
    sc->best_match_col_ = best_match_col;

    return LDL_FEED_ME;
  }

  /* There will be no more input, so don't ask for more, instead
   * match what we have; if we can't because we don't have any more,
   * return success, otherwise return a syntax error. */
  if (start_offset == best_match_offset) {
    /* We matched nothing, we can only get here when:
     * 1. no prior input was buffered (match_index == sc->match_size_)
     * 2. There is no more input (input_index == input_size)
     * 3. We matched not a single character.
     * 4. is_final_input is true (there will be nothing more)
     * In other words: we finished. */
    return LDL_END_OF_INPUT;
  }

  if (best_match == sc->scanner_.default_action) {
    goto syntax_error;
  }

  ldl->type_of_line_ = best_match;
  ldl->filename_ = sc->filename_;
  ldl->start_offset_ = start_offset;
  ldl->start_line_ = start_line;
  ldl->start_col_ = start_col;
  ldl->end_offset_ = best_match_offset;
  ldl->end_line_ = best_match_line;
  ldl->end_col_ = best_match_col;

  size_t token_size = best_match_offset - start_offset;

  /* Ensure token match is null terminated */
  ldl_scanner_reserve_match(sc);
  sc->null_terminator_repair_ = sc->match_[token_size];
  sc->match_[token_size] = '\0';
  sc->token_size_ = token_size;

  ldl_fill_token(ldl, (ld_line_type_t)best_match, sc->match_, sc->match_ + token_size);

  /* Reset scanner to get ready for next token prior to return to caller; the match_ buffer
   * will be reset upon entry */
  sc->match_size_ = sc->match_size_ - token_size;
  sc->match_index_ = 0;
  sc->best_match_ = sc->scanner_.default_action;
  sc->state_ = sc->scanner_.start_state;

  sc->start_offset_ = best_match_offset;
  sc->start_line_ = best_match_line;
  sc->start_col_ = best_match_col;

  sc->offset_ = best_match_offset;
  sc->line_ = best_match_line;
  sc->col_ = best_match_col;

  sc->best_match_offset_ = best_match_offset;
  sc->best_match_line_ = best_match_line;
  sc->best_match_col_ = best_match_col;

  sc->input_index_ = input_index;

  return LDL_MATCH;

syntax_error:
  if (!sc->match_size_) {
    /* Will match 1 character of invalid input */
    /* Note that there are only two ways to get here:
     * Either the first character of a match is illegal (and so not matched),
     * Or some prefix already matched ran into a premature end of stream.
     * The latter case will already have a match_size_ (and so not enter this branch),
     * the former case will "goto syntax_error;" from a point where 'c' is initialized
     * to the current character. */
    if ((r = ldl_scanner_append_match(sc, c))) {
      return r;
    }
    input_index = input_index + 1;
  }

  ldl->filename_ = sc->filename_;
  ldl->start_offset_ = start_offset;
  ldl->start_line_ = start_line;
  ldl->start_col_ = start_col;

  c = sc->match_[0];

  if (c != '\n') {
    ldl->end_offset_ = ldl->start_offset_ + 1;
    ldl->end_line_ = ldl->start_line_;
    ldl->end_col_ = ldl->start_col_ + 1;
  }
  else {
    ldl->end_offset_ = ldl->start_offset_ + 1;
    ldl->end_line_ = ldl->start_line_ + 1;
    ldl->end_col_ = 1;
  }
  ldl->type_of_line_ = LD_UNKNOWN;

  /* Reset scanner to get ready for next token prior to returning */
  memcpy(sc->match_, sc->match_ + 1, sc->match_size_ - 1);
  sc->match_size_ = sc->match_size_ - 1;
  sc->match_index_ = 0;
  sc->best_match_ = sc->scanner_.default_action;
  sc->state_ = sc->scanner_.start_state;

  sc->start_offset_ = ldl->end_offset_;
  sc->start_line_ = ldl->end_line_;
  sc->start_col_ = ldl->end_col_;

  sc->offset_ = sc->start_offset_;
  sc->line_ = sc->start_line_;
  sc->col_ = sc->start_col_;

  sc->best_match_offset_ = sc->start_offset_;
  sc->best_match_line_ = sc->start_line_;
  sc->best_match_col_ = sc->start_col_;

  sc->input_index_ = input_index;

  return LDL_SYNTAX_ERROR;
}

static int ldl_fill_token(struct ld_line *ldl, ld_line_type_t minor, const char *begin, const char *end) {
  if (!ldl->free_chars_on_cleanup_ || (ldl->num_chars_allocated_ <= (size_t)(end - begin))) {
    size_t new_alloc_size = (size_t)(end - begin) + 1;
    if (new_alloc_size <= (size_t)(end - begin)) {
      /* Overflow */
      LOGERROR("Match size overflow\n");
      return LDL_INTERNAL_ERROR;
    }
    void *buf = realloc(ldl->chars_, new_alloc_size);
    if (!buf) {
      /* No mem */
      return LDL_INTERNAL_ERROR;
    }
    ldl->chars_ = (char *)buf;
    ldl->num_chars_allocated_ = new_alloc_size;
    ldl->free_chars_on_cleanup_ = 1;
  }
  ldl->num_chars_ = (size_t)(end - begin);
  memcpy(ldl->chars_, begin, ldl->num_chars_);
  ldl->chars_[ldl->num_chars_] = '\0';

  return 0;
}


int main(int argc, char **argv) {
  klt_logger_init();
  klt_log_set_function("/", log_function, NULL);

  LOG("We've started..\n");

  struct ldl_scanner sc;
  ldl_scanner_init(&sc);

  struct tkr_tokenizer tkr;
  tkr_tokenizer_init(&tkr, LD_UNKNOWN, sizeof(g_scanner_rules_) / sizeof(*g_scanner_rules_), g_scanner_rules_);

  struct ld_line ldl;
  ld_line_init(&ldl);

  const char *input_filename = "test_input.cnd";
  FILE *fp = fopen(input_filename, "rb");
  if (!fp) {
    LOGERROR("Failed to open file \"%s\"\n", input_filename);
    return EXIT_FAILURE;
  }

  size_t num_bytes_read;
  static char buf[2400];
  do {
    num_bytes_read = fread(buf, sizeof(*buf), sizeof(buf) / sizeof(*buf), fp);
    int r;
    
    //r = ldl_scanner_input(&sc, &ldl, buf, num_bytes_read, !num_bytes_read);
    r = tkr_tokenizer_input(&tkr, buf, num_bytes_read, !num_bytes_read);
    while ((r != LDL_END_OF_INPUT) && (r != LDL_FEED_ME)) {

      if (r == LDL_INTERNAL_ERROR) {
        num_bytes_read = 0;
        break;
      }

#if 0
      if (r == LDL_MATCH) {
        ldl_fill_token(&ldl, ldl.type_of_line_, sc.match_, sc.match_ + sc.token_size_);
        
        printf("Match %s:\n", ld_line_type_to_str(ldl.type_of_line_));
        printf("%s", ldl.chars_);
        if (ldl.num_chars_ && (ldl.chars_[ldl.num_chars_ - 1] == '\n')) {
          /* Line finishes with a newline, no manual newline required */
        }
        else {
          printf("\n");
        }
      }
#endif
      if (r == LDL_MATCH) {
        printf("Match %s:\n", ld_line_type_to_str(tkr.best_match_action_));
        printf("%s", tkr.match_);
        if (tkr.match_[tkr.token_size_ - 1] != '\n') {
          /* Last line of input has no trailing newline */
          printf("\n");
        }
      }

      //r = ldl_scanner_input(&sc, &ldl, buf, num_bytes_read, !num_bytes_read);
      r = tkr_tokenizer_input(&tkr, buf, num_bytes_read, !num_bytes_read);
    }
  } while (num_bytes_read);

  tkr_tokenizer_cleanup(&tkr);
  LOG("We've finished\n");

  return EXIT_SUCCESS;
}
