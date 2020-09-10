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

#ifndef CTYPE_H_INCLUDED
#define CTYPE_H_INCLUDED
#include <ctype.h>
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

#ifndef LINE_ASSEMBLY_H_INCLUDED
#define LINE_ASSEMBLY_H_INCLUDED
#include "line_assembly.h"
#endif

#ifndef LINE_SPLITTER_H_INCLUDED
#define LINE_SPLITTER_H_INCLUDED
#include "line_splitter.h"
#endif

#ifndef LINE_DEFS_H_INCLUDED
#define LINE_DEFS_H_INCLUDED
#include "line_defs.h"
#endif

#ifndef DECOMMENT_H_INCLUDED
#define DECOMMENT_H_INCLUDED
#include "decomment.h"
#endif

#ifndef TOKENS_H_INCLUDED
#define TOKENS_H_INCLUDED
#include "tokens.h"
#endif

#ifndef PRD_GRAM_H_INCLUDED
#define PRD_GRAM_H_INCLUDED
#include "prd_gram.h"
#endif

struct part {
  struct part *next_;
  size_t num_chars_;
  char *chars_;
};

static struct part *append_part(struct part **tailptr, size_t num_chars, char *chars) {
  struct part *p = (struct part *)malloc(sizeof(struct part));
  if (!p) return NULL;
  p->chars_ = (char *)malloc(num_chars + 1);
  if (!p->chars_) {
    free(p);
    return NULL;
  }
  memcpy(p->chars_, chars, num_chars);
  p->chars_[num_chars] = '\0';
  p->num_chars_ = num_chars;
  if (*tailptr) {
    p->next_ = (*tailptr)->next_;
    (*tailptr)->next_ = p;
    *tailptr = p;
  }
  else {
    *tailptr = p;
    p->next_ = p;
  }

  return p;
}


static int process_cinder_directive(struct tkr_tokenizer *tkr_tokens, struct tkr_tokenizer *directive_line_match) {
  int r;
  int ate_percent = 0;
  int ate_directive = 0;
  enum {
    PCD_NT_DIRECTIVE,
    PCD_TOKEN_DIRECTIVE
  } directive;
  tok_switch_to_nonterminal_idents(tkr_tokens);
  tkr_tokens->filename_ = directive_line_match->filename_;
  tkr_tokens->line_ = directive_line_match->start_line_;
  tkr_tokens->col_ = directive_line_match->start_col_;
  r = tkr_tokenizer_input(tkr_tokens, directive_line_match->match_, directive_line_match->token_size_, 1);
  while ((r != TKR_END_OF_INPUT) && (r != TKR_FEED_ME)) {
    if (r == TKR_SYNTAX_ERROR) {
      if (isprint(tkr_tokens->match_[0])) {
        LOGERROR("%s(%d): Character \"%s\" not expected at column %d\n", tkr_tokens->filename_, tkr_tokens->start_line_, tkr_tokens->match_, tkr_tokens->start_col_);
      }
      else {
        LOGERROR("%s(%d): Character 0x%02x not expected at column %d\n", tkr_tokens->filename_, tkr_tokens->start_line_, tkr_tokens->match_[0], tkr_tokens->start_col_);
      }
    }
    else if (r == TKR_INTERNAL_ERROR) {
      return r;
    }
    else if (r == TKR_MATCH) {
      if (tkr_tokens->best_match_variant_ == TOK_WHITESPACE) {
        /* eat silently */
      }
      else {
        if (!ate_percent) {
          if (tkr_tokens->best_match_variant_ == TOK_PERCENT) {
            /* leading percent sign indicates cinder directive, eat it */
            ate_percent = 1;
          }
          else {
            LOGERROR("%s(%d): Syntax error, \"%%\" expected at column %d\n", tkr_tokens->filename_, tkr_tokens->start_line_, tkr_tokens->start_col_);
            return TKR_SYNTAX_ERROR;
          }
        }
        else if (!ate_directive) {
          if (tkr_tokens->best_match_variant_ == TOK_IDENT) {
            ate_directive = 1;
            if (!strcmp("nt", tkr_tokens->match_)) {
              directive = PCD_NT_DIRECTIVE;
            }
            else if (!strcmp("token", tkr_tokens->match_)) {
              directive = PCD_TOKEN_DIRECTIVE;
            }
            else if (!strcmp("debug_end", tkr_tokens->match_)) {
              LOGERROR("%s(%d): %%debug_end encountered, terminating early\n", directive_line_match->filename_, directive_line_match->start_line_);
              return TKR_INTERNAL_ERROR;
            }
            else {
              LOGERROR("%s(%d): Syntax error, invalid directive at column %d\n", tkr_tokens->filename_, tkr_tokens->start_line_, tkr_tokens->start_col_);
              return TKR_SYNTAX_ERROR;
            }
          }
        }
        else {
          if (directive == PCD_NT_DIRECTIVE) {
            if (tkr_tokens->best_match_variant_ == TOK_IDENT) {
              TRACE("%s(%d): Non-terminal \"%s\" declared at column %d\n", tkr_tokens->filename_, tkr_tokens->start_line_, tkr_tokens->match_, tkr_tokens->start_col_);
            }
            else {
              LOGERROR("%s(%d): Syntax error, identifier expected at column %d\n", tkr_tokens->filename_, tkr_tokens->start_line_, tkr_tokens->start_col_);
              return TKR_SYNTAX_ERROR;
            }
          }
          else if (directive == PCD_TOKEN_DIRECTIVE) {
            if (tkr_tokens->best_match_variant_ == TOK_IDENT) {
              TRACE("%s(%d): Token \"%s\" declared at column %d\n", tkr_tokens->filename_, tkr_tokens->start_line_, tkr_tokens->match_, tkr_tokens->start_col_);
            }
            else {
              LOGERROR("%s(%d): Syntax error, identifier expected at column %d\n", tkr_tokens->filename_, tkr_tokens->start_line_, tkr_tokens->start_col_);
              return TKR_SYNTAX_ERROR;
            }
          }
        }
      }
    }

    r = tkr_tokenizer_input(tkr_tokens, directive_line_match->match_, directive_line_match->token_size_, 1);
  }

  return r;
}


static int process_cinder_directive2(struct tkr_tokenizer *tkr_tokens, struct xlts *directive_line_match) {
  int r;
  int ate_percent = 0;
  int ate_directive = 0;
  enum {
    PCD_NT_DIRECTIVE,
    PCD_TOKEN_DIRECTIVE
  } directive;
  tok_switch_to_nonterminal_idents(tkr_tokens);

  r = tkr_tokenizer_input(tkr_tokens, directive_line_match->translated_, directive_line_match->num_translated_, 1);
  while ((r != TKR_END_OF_INPUT) && (r != TKR_FEED_ME)) {
    if (r == TKR_SYNTAX_ERROR) {
      if (isprint(tkr_tokens->match_[0])) {
        LOGERROR("%s(%d): Character \"%s\" not expected at column %d\n", tkr_tokens->filename_, directive_line_match->chunks_[0].line_, tkr_tokens->match_, directive_line_match->chunks_[0].col_);
      }
      else {
        LOGERROR("%s(%d): Character 0x%02x not expected at column %d\n", tkr_tokens->filename_, directive_line_match->chunks_[0].line_, tkr_tokens->match_[0], directive_line_match->chunks_[0].col_);
      }
    }
    else if (r == TKR_INTERNAL_ERROR) {
      return r;
    }
    else if (r == TKR_MATCH) {
      if (tkr_tokens->best_match_variant_ == TOK_WHITESPACE) {
        /* eat silently */
      }
      else {
        if (!ate_percent) {
          if (tkr_tokens->best_match_variant_ == TOK_PERCENT) {
            /* leading percent sign indicates cinder directive, eat it */
            ate_percent = 1;
          }
          else {
            LOGERROR("%s(%d): Syntax error, \"%%\" expected at column %d\n", tkr_tokens->filename_, directive_line_match->chunks_[0].line_, directive_line_match->chunks_[0].col_);
            return TKR_SYNTAX_ERROR;
          }
        }
        else if (!ate_directive) {
          if (tkr_tokens->best_match_variant_ == TOK_IDENT) {
            ate_directive = 1;
            if (!strcmp("nt", tkr_tokens->match_)) {
              directive = PCD_NT_DIRECTIVE;
            }
            else if (!strcmp("token", tkr_tokens->match_)) {
              directive = PCD_TOKEN_DIRECTIVE;
            }
            else if (!strcmp("debug_end", tkr_tokens->match_)) {
              LOGERROR("%s(%d): %%debug_end encountered, terminating early\n", tkr_tokens->filename_, directive_line_match->chunks_[0].line_);
              return TKR_INTERNAL_ERROR;
            }
            else {
              LOGERROR("%s(%d): Syntax error, invalid directive at column %d\n", tkr_tokens->filename_, directive_line_match->chunks_[0].line_, directive_line_match->chunks_[0].col_);
              return TKR_SYNTAX_ERROR;
            }
          }
        }
        else {
          if (directive == PCD_NT_DIRECTIVE) {
            if (tkr_tokens->best_match_variant_ == TOK_IDENT) {
              TRACE("%s(%d): Non-terminal \"%s\" declared at column %d\n", tkr_tokens->filename_, directive_line_match->chunks_[0].line_, tkr_tokens->match_, directive_line_match->chunks_[0].col_);
            }
            else {
              LOGERROR("%s(%d): Syntax error, identifier expected at column %d\n", tkr_tokens->filename_, directive_line_match->chunks_[0].line_, directive_line_match->chunks_[0].col_);
              return TKR_SYNTAX_ERROR;
            }
          }
          else if (directive == PCD_TOKEN_DIRECTIVE) {
            if (tkr_tokens->best_match_variant_ == TOK_IDENT) {
              TRACE("%s(%d): Token \"%s\" declared at column %d\n", tkr_tokens->filename_, directive_line_match->chunks_[0].line_, tkr_tokens->match_, directive_line_match->chunks_[0].col_);
            }
            else {
              LOGERROR("%s(%d): Syntax error, identifier expected at column %d\n", tkr_tokens->filename_, directive_line_match->chunks_[0].line_, directive_line_match->chunks_[0].col_);
              return TKR_SYNTAX_ERROR;
            }
          }
        }
      }
    }

    r = tkr_tokenizer_input(tkr_tokens, directive_line_match->translated_, directive_line_match->num_translated_, 1);
  }

  return r;
}

static int process_tokens(struct tkr_tokenizer *tkr_tokens, struct tkr_tokenizer *tkr_lines, int is_final, struct prd_stack *prds) {
  int r;
  if (!is_final) {
    tkr_tokens->filename_ = tkr_lines->filename_;
    tkr_tokens->line_ = tkr_lines->best_match_line_;
    tkr_tokens->col_ = tkr_lines->best_match_col_;
  }
  r = tkr_tokenizer_input(tkr_tokens, tkr_lines->match_, is_final ? 0 : tkr_lines->token_size_, is_final);
  while ((r != TKR_END_OF_INPUT) && (r != TKR_FEED_ME)) {
    if (r == TKR_SYNTAX_ERROR) {
      if (isprint(tkr_tokens->match_[0])) {
        LOGERROR("%s(%d): Character \"%s\" not expected at column %d\n", tkr_tokens->filename_, tkr_tokens->best_match_line_, tkr_tokens->match_, tkr_tokens->best_match_col_);
      }
      else {
        LOGERROR("%s(%d): Character 0x%02x not expected at column %d\n", tkr_tokens->filename_, tkr_tokens->best_match_line_, tkr_tokens->match_[0], tkr_tokens->best_match_col_);
      }
    }
    else if (r == TKR_INTERNAL_ERROR) {
      return r;
    }
    else if (r == TKR_MATCH) {
      printf("Match %s: \"%s\"\n", tok_token_type_to_str(tkr_tokens->best_match_action_), tkr_tokens->match_);
      r = prd_parse(prds, tkr_tokens, 0);
      switch (r) {
      case PRD_SUCCESS:
        /* This should not be possible without is_final==1 */
        LOGERROR("%s(%d): Internal error, premature end of grammar at column %d\n", tkr_tokens->filename_, tkr_tokens->best_match_line_, tkr_tokens->best_match_col_);
        return TKR_INTERNAL_ERROR;
        break;
      case PRD_SYNTAX_ERROR:
        /* XXX: Need to recover, but how! (Transition on all parent Error syms and attempt to shift input) */
        break;
      case PRD_NEXT:
        break;
      case PRD_INTERNAL_ERROR:
        return TKR_INTERNAL_ERROR;
      }
    }

    r = tkr_tokenizer_input(tkr_tokens, tkr_lines->match_, is_final ? 0 : tkr_lines->token_size_, is_final);
  }

  if (r == TKR_END_OF_INPUT) {
    r = prd_parse(prds, tkr_tokens, 1);
    switch (r) {
    case PRD_SUCCESS:
      return TKR_END_OF_INPUT;
      break;
    case PRD_SYNTAX_ERROR:
      /* Not attempting to recover because there is no more input to synchronize with, end here. */
      return TKR_END_OF_INPUT;
    case PRD_NEXT:
      /* Grammar should not exit regularly awaiting next token when the next token is INPUT_END.
       * (INPUT_END, i.e. end_of_input==1, never shifts) */
      LOGERROR("%s(%d): Internal error, grammar did not end at column %d\n", tkr_tokens->filename_, tkr_tokens->best_match_line_, tkr_tokens->best_match_col_);
      return TKR_INTERNAL_ERROR;
    case PRD_INTERNAL_ERROR:
      return TKR_INTERNAL_ERROR;
    }
  }

  return r;
}

int main(int argc, char **argv) {
  int r;
  klt_logger_init();
  klt_log_set_function("/", log_function, NULL);

  LOG("We've started..\n");

  prd_init();
  r = ls_init();
  if (r) {
    LOGERROR("Failed to initialize ls\n");
    return EXIT_FAILURE;
  }
  r = ldl_init();
  if (r) {
    LOGERROR("Failed to initialize ldl\n");
    return EXIT_FAILURE;
  }
  r = tok_init();
  if (r) {
    LOGERROR("Failed to initialize tok\n");
    return EXIT_FAILURE;
  }
  r = las_init();
  if (r) {
    LOGERROR("Failed to initialize las\n");
    return EXIT_FAILURE;
  }
  r = dct_init();
  if (r) {
    LOGERROR("Failed to initialize dct\n");
    return EXIT_FAILURE;
  }

  struct las_line_assembly line_assembly;
  las_init_line_assembly(&line_assembly);

  struct ls_line_splitter line_splitter;
  ls_init_line_splitter(&line_splitter);

  struct tkr_tokenizer tkr_lines;
  ldl_init_tokenizer(&tkr_lines);

  struct tkr_tokenizer tkr_tokens;
  tok_init_tkr_tokenizer(&tkr_tokens);

  struct ld_line ldl;
  ld_line_init(&ldl);

  struct prd_stack prds;
  prd_stack_init(&prds);

  r = prd_reset(&prds);
  if (r) {
    LOGERROR("Internal error, failed to reset parsing stack\n");
    return EXIT_FAILURE;
  }

  const char *input_filename = "test_input.cnd";
  FILE *fp = fopen(input_filename, "rb");
  if (!fp) {
    LOGERROR("Failed to open file \"%s\"\n", input_filename);
    return EXIT_FAILURE;
  }

  struct part *prologue = NULL;
  struct part *epilogue = NULL;

  enum {
    PROLOGUE,
    GRAMMAR,
    EPILOGUE
  } where_are_we = PROLOGUE;

  size_t num_bytes_read;
  static char buf[2400];

  struct xlts token_buf;
  xlts_init(&token_buf);

  do {
    num_bytes_read = fread(buf, sizeof(*buf), sizeof(buf) / sizeof(*buf), fp);

#if 1
    r = las_input(&line_assembly, buf, num_bytes_read, !num_bytes_read);
    while ((r != LAS_END_OF_INPUT) && (r != LAS_FEED_ME)) {
      /* Cannot modify (shift) the line_assembly's mlc_buf line buffer, so copy it over to a work area (token_buf) */
      xlts_reset(&token_buf);
      r = xlts_append(&token_buf, &line_assembly.mlc_buf_);
      if (r) {
        return EXIT_FAILURE;
      }

      dct_decomment(&token_buf);

      r = tkr_tokenizer_input(&tkr_lines, token_buf.translated_, token_buf.num_translated_, 1);

      if ((r == TKR_END_OF_INPUT) || (r == TKR_FEED_ME)) {
        LOGERROR("%s(%d): Internal error: all lines are expected to match.\n");
        return EXIT_FAILURE;
      }
      if (r == TKR_SYNTAX_ERROR) {
        if (isprint(tkr_lines.match_[0])) {
          LOGERROR("%s(%d): Character \"%s\" not expected at column %d\n", tkr_lines.filename_, tkr_lines.best_match_line_, tkr_lines.match_, tkr_lines.best_match_col_);
        }
        else {
          LOGERROR("%s(%d): Character 0x%02x not expected at column %d\n", tkr_lines.filename_, tkr_lines.best_match_line_, tkr_lines.match_[0], tkr_lines.best_match_col_);
        }
      }
      else if (r == TKR_INTERNAL_ERROR) {
        num_bytes_read = 0;
        break;
      }

      if (r == TKR_MATCH) {
        printf("Match %s: ", ld_line_type_to_str(tkr_lines.best_match_action_));
        printf("%s", tkr_lines.match_);
        if (tkr_lines.match_[tkr_lines.token_size_ - 1] != '\n') {
          /* Last line of input has no trailing newline */
          printf("\n");
        }
        if (where_are_we == PROLOGUE) {
          switch (tkr_lines.best_match_variant_) {
          case LD_C_PREPROCESSOR:
            /* Preserve line continuations */
            append_part(&prologue, token_buf.num_original_, token_buf.original_);
            break;
          case LD_CINDER_SECTION_DELIMITER:
            where_are_we = GRAMMAR;
            break;
          case LD_REGULAR:
            /* XXX: Process this by individual tokens later ? */
            /* Preserve line continuations */
            append_part(&prologue, token_buf.num_original_, token_buf.original_);
            printf("%s", token_buf.original_);
            break;
          case LD_CINDER_DIRECTIVE:
            r = process_cinder_directive2(&tkr_tokens, &token_buf);
            if (r == TKR_INTERNAL_ERROR) {
              return EXIT_FAILURE;
            }
            break;
          }
        }
        else if (where_are_we == GRAMMAR) {
          switch (tkr_lines.best_match_variant_) {
          case LD_C_PREPROCESSOR:
            LOGERROR("%s(%d): Error, preprocessor directives do not belong in grammar area\n", tkr_lines.filename_, tkr_lines.start_line_);
            return EXIT_SUCCESS;
          case LD_CINDER_SECTION_DELIMITER:
            /* Finish up */
            r = process_tokens(&tkr_tokens, &tkr_lines, 1, &prds);
            where_are_we = EPILOGUE;
            break;
          case LD_REGULAR:
          {
            r = process_tokens(&tkr_tokens, &tkr_lines, 0, &prds);
            break;
          }
          case LD_CINDER_DIRECTIVE:
            r = process_cinder_directive2(&tkr_tokens, &token_buf);
            if (r == TKR_INTERNAL_ERROR) {
              return EXIT_FAILURE;
            }
            break;
          }
        }
        else /* (where_are_we == EPILOGUE) */ {
          switch (tkr_lines.best_match_variant_) {
          case LD_C_PREPROCESSOR:
            append_part(&prologue, line_splitter.num_original_, line_splitter.original_);
            break;
          case LD_CINDER_SECTION_DELIMITER:
            where_are_we = GRAMMAR;
            break;
          case LD_REGULAR:
            /* XXX: Process this by individual tokens later ? */
            /* Preserve line continuations */
            append_part(&prologue, line_splitter.num_original_, line_splitter.original_);
            printf("%s", line_splitter.original_);
            break;
          case LD_CINDER_DIRECTIVE:
            r = process_cinder_directive2(&tkr_tokens, &token_buf);
            if (r == TKR_INTERNAL_ERROR) {
              return EXIT_FAILURE;
            }
            break;
          }
        }
      }

      r = tkr_tokenizer_input(&tkr_lines, token_buf.translated_, token_buf.num_translated_, 1);
      if ((r != TKR_END_OF_INPUT) && (r != TKR_INTERNAL_ERROR)) {
        LOGERROR("%s(%d): Internal error: lines from a single line are expected to match a single time.\n");
        return EXIT_FAILURE;
      }

      r = las_input(&line_assembly, buf, num_bytes_read, !num_bytes_read);
    }

#else
    r = ls_input(&line_splitter, buf, num_bytes_read, !num_bytes_read);
    while ((r != LSSL_END_OF_INPUT) && (r != LSSL_FEED_ME)) {

      /* Because line_splitter consumes line continuations, the actual line and column count may
       * drift from the observed line and column count on tkr_lines. We synchronize line, column
       * and offset at every start of a line-continuation patched line. */
      tkr_lines.start_line_ = line_splitter.line_;
      tkr_lines.start_col_ = line_splitter.col_;
      tkr_lines.start_offset_ = line_splitter.offset_;
      tkr_lines.line_ = line_splitter.line_;
      tkr_lines.col_ = line_splitter.col_;
      tkr_lines.offset_ = line_splitter.offset_;

      /* Lines are tokenizer as final input (1 for last parameter) to ensure the _current_ line in 
       * line_splitter is matched as the current line in tkr_lines. Otherwise, tkr_lines would be
       * running behind line_splitter by 1 line for want of a lookahead character. */

      r = tkr_tokenizer_input(&tkr_lines, line_splitter.stripped_, line_splitter.num_stripped_, 1);
      if ((r == TKR_END_OF_INPUT) || (r == TKR_FEED_ME)) {
        LOGERROR("%s(%d): Internal error: all lines are expected to match.\n");
        return EXIT_FAILURE;
      }
      if (r == TKR_SYNTAX_ERROR) {
        if (isprint(tkr_lines.match_[0])) {
          LOGERROR("%s(%d): Character \"%s\" not expected at column %d\n", tkr_lines.filename_, tkr_lines.best_match_line_, tkr_lines.match_, tkr_lines.best_match_col_);
        }
        else {
          LOGERROR("%s(%d): Character 0x%02x not expected at column %d\n", tkr_lines.filename_, tkr_lines.best_match_line_, tkr_lines.match_[0], tkr_lines.best_match_col_);
        }
      }
      else if (r == TKR_INTERNAL_ERROR) {
        num_bytes_read = 0;
        break;
      }

      if (r == TKR_MATCH) {
        printf("Match %s: ", ld_line_type_to_str(tkr_lines.best_match_action_));
        printf("%s", tkr_lines.match_);
        if (tkr_lines.match_[tkr_lines.token_size_ - 1] != '\n') {
          /* Last line of input has no trailing newline */
          printf("\n");
        }
        if (where_are_we == PROLOGUE) {
          switch (tkr_lines.best_match_variant_) {
          case LD_C_PREPROCESSOR:
            /* Preserve line continuations */
            append_part(&prologue, line_splitter.num_original_, line_splitter.original_);
            break;
          case LD_CINDER_SECTION_DELIMITER:
            where_are_we = GRAMMAR;
            break;
          case LD_REGULAR:
            /* XXX: Process this by individual tokens later ? */
            /* Preserve line continuations */
            append_part(&prologue, line_splitter.num_original_, line_splitter.original_);
            printf("%s", line_splitter.original_);
            break;
          case LD_CINDER_DIRECTIVE:
            r = process_cinder_directive(&tkr_tokens, &tkr_lines);
            if (r == TKR_INTERNAL_ERROR) {
              return EXIT_FAILURE;
            }
            break;
          }
        }
        else if (where_are_we == GRAMMAR) {
          switch (tkr_lines.best_match_variant_) {
          case LD_C_PREPROCESSOR:
            LOGERROR("%s(%d): Error, preprocessor directives do not belong in grammar area\n", tkr_lines.filename_, tkr_lines.start_line_);
            return EXIT_SUCCESS;
          case LD_CINDER_SECTION_DELIMITER:
            /* Finish up */
            r = process_tokens(&tkr_tokens, &tkr_lines, 1, &prds);
            where_are_we = EPILOGUE;
            break;
          case LD_REGULAR:
          {
            r = process_tokens(&tkr_tokens, &tkr_lines, 0, &prds);
            break;
          }
          case LD_CINDER_DIRECTIVE:
            r = process_cinder_directive(&tkr_tokens, &tkr_lines);
            if (r == TKR_INTERNAL_ERROR) {
              return EXIT_FAILURE;
            }
            break;
          }
        }
        else /* (where_are_we == EPILOGUE) */ {
          switch (tkr_lines.best_match_variant_) {
          case LD_C_PREPROCESSOR:
            append_part(&prologue, line_splitter.num_original_, line_splitter.original_);
            break;
          case LD_CINDER_SECTION_DELIMITER:
            where_are_we = GRAMMAR;
            break;
          case LD_REGULAR:
            /* XXX: Process this by individual tokens later ? */
            /* Preserve line continuations */
            append_part(&prologue, line_splitter.num_original_, line_splitter.original_);
            printf("%s", line_splitter.original_);
            break;
          case LD_CINDER_DIRECTIVE:
            r = process_cinder_directive(&tkr_tokens, &tkr_lines);
            if (r == TKR_INTERNAL_ERROR) {
              return EXIT_FAILURE;
            }
            break;
          }
        }
      }

      r = tkr_tokenizer_input(&tkr_lines, line_splitter.stripped_, line_splitter.num_stripped_, 1);
      if ((r != TKR_END_OF_INPUT) && (r != TKR_INTERNAL_ERROR)) {
        LOGERROR("%s(%d): Internal error: lines from a single line are expected to match a single time.\n");
        return EXIT_FAILURE;
      }

      r = ls_input(&line_splitter, buf, num_bytes_read, !num_bytes_read);
    }

#endif

  } while (num_bytes_read);

  /* Finish */
  r = process_tokens(&tkr_tokens, &tkr_lines, 0, &prds);

  xlts_cleanup(&token_buf);

  prd_stack_cleanup(&prds);

  tkr_tokenizer_cleanup(&tkr_tokens);

  tkr_tokenizer_cleanup(&tkr_lines);

  ls_cleanup_line_splitter(&line_splitter);

  las_cleanup_line_assembly(&line_assembly);

  dct_cleanup();
  las_cleanup();
  tok_cleanup();
  ldl_cleanup();
  prd_cleanup();

  LOG("We've finished\n");

  return EXIT_SUCCESS;
}
