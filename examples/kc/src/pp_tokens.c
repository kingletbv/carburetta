/* Copyright 2023-2025 Kinglet B.V.
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

#ifndef STDINT_H_INCLUDED
#define STDINT_H_INCLUDED
#include <stdint.h>
#endif

#ifndef STRING_H_INCLUDED
#define STRING_H_INCLUDED
#include <string.h>
#endif

#ifndef ASSERT_H_INCLUDED
#define ASSERT_H_INCLUDED
#include <assert.h>
#endif

#ifndef C_COMPILER_H_INCLUDED
#define C_COMPILER_H_INCLUDED
#include "c_compiler.h"
#endif

#ifndef STMT_H_INCLUDED
#define STMT_H_INCLUDED 
#include "stmt.h"
#endif

#ifndef PP_TOKENS_H_INCLUDED
#define PP_TOKENS_H_INCLUDED
#include "pp_tokens.h"
#endif

#ifndef PP_LINE_DIRECTIVES_H_INCLUDED
#define PP_LINE_DIRECTIVES_H_INCLUDED
#include "pp_line_directives.h"
#endif

#ifndef PP_CONST_EXPR_H_INCLUDED
#define PP_CONST_EXPR_H_INCLUDED
#include "pp_const_expr.h"
#endif

#ifndef PP_MACRO_EXPANDER_H_INCLUDED
#define PP_MACRO_EXPANDER_H_INCLUDED
#include "pp_macro_expander.h"
#endif

#ifndef PP_TOKENIZER_H_INCLUDED
#define PP_TOKENIZER_H_INCLUDED
#include "pp_tokenizer.h"
#endif

#ifndef C_PARSER_H_INCLUDED
#define C_PARSER_H_INCLUDED
#include "c_parser.h"
#endif

int g_pptk_to_ppld_[] = {
#define xx(pptk_id, ppld_id, ppce_id, ppme_id, cp_id) [pptk_id] = ppld_id,
  PPTK_ENUM_PPTOKENS
#undef xx
};

int g_pptk_to_ppce_[] = {
#define xx(pptk_id, ppld_id, ppce_id, ppme_id, cp_id) [pptk_id] = ppce_id,
  PPTK_ENUM_PPTOKENS
#undef xx
};

int g_pptk_to_ppme_[] = {
#define xx(pptk_id, ppld_id, ppce_id, ppme_id, cp_id) [pptk_id] = ppme_id,
  PPTK_ENUM_PPTOKENS
#undef xx
};

int g_pptk_to_cp_[] = {
#define xx(pptk_id, ppld_id, ppce_id, ppme_id, cp_id) [pptk_id] = cp_id,
  PPTK_ENUM_PPTOKENS
#undef xx
};

static int pptk_is_whitespace(struct pptk *tk) {
  return (tk->tok_ == PPTK_WHITESPACE) || (tk->tok_ == PPTK_NEWLINE_WHITESPACE);
}

struct pptk *pptk_alloc_len(struct c_compiler *cc, struct pptk **pp_chain, const char *text, size_t text_len, int sym, struct situs *psit) {
  struct pptk *tk = (struct pptk *)malloc(sizeof(struct pptk));
  if (tk) {
    tk->next_ = tk->prev_ = tk;
    tk->tok_ = sym;
    tk->text_len_ = text_len;
    tk->text_ = malloc(text_len + 1);
    if (tk->text_) {
      memcpy(tk->text_, text, text_len);
      tk->text_[text_len] = '\0';
    }
    situs_init(&tk->situs_);
    situs_clone(&tk->situs_, psit);
    tk->v_.expr_ = NULL;
  }
  if (!tk || !tk->text_) {
    if (tk) free(tk);
    cc_no_memory(cc);
    return NULL;
  }

  if (pp_chain) {
    *pp_chain = pptk_join(*pp_chain, tk);
  }

  return tk;
}

struct pptk *pptk_alloc(struct c_compiler *cc, struct pptk **pp_chain, const char *text, int sym, struct situs *psit) {
  return pptk_alloc_len(cc, pp_chain, text, strlen(text), sym, psit);
}

void pptk_free(struct pptk *tk_chain) {
  struct pptk *tk, *next;
  tk = tk_chain;
  if (tk) {
    next = tk->next_;
    do {
      tk = next;
      next = tk->next_;

      if (tk->text_) free(tk->text_);
      if (tk->tok_ == PPTK_STRING_LIT) {
        if (tk->v_.string_.data_) free(tk->v_.string_.data_);
      }
      else if ((tk->tok_ != PPTK_TYPEDEF_NAME) && tk->v_.expr_) {
        expr_free(tk->v_.expr_);
      }
      situs_cleanup(&tk->situs_);
      free(tk);

    } while (tk != tk_chain);
  }
}

struct pptk *pptk_join(struct pptk *front, struct pptk *back) {
  if (!back) return front;
  if (!front) return back;
  struct pptk *front_head = front;
  struct pptk *front_tail = front->prev_;
  struct pptk *back_head = back;
  struct pptk *back_tail = back->prev_;

  back_tail->next_ = front_head;
  front_head->prev_ = back_tail;

  front_tail->next_ = back_head;
  back_head->prev_ = front_tail;

  return front_head;
}

struct pptk *pptk_pop_front(struct pptk **pp_chain) {
  struct pptk *front;
  if (!*pp_chain) return NULL;
  front = *pp_chain;
  if (front->next_ != front) {
    *pp_chain = front->next_;
    front->next_->prev_ = front->prev_;
    front->prev_->next_ = front->next_;
  }
  else {
    *pp_chain = NULL;
  }
  front->next_ = front->prev_ = front;

  return front;
}

struct pptk *pptk_pop_back(struct pptk **pp_chain) {
  struct pptk *back;
  if (!*pp_chain) return NULL;
  back = (*pp_chain)->prev_;
  if (back == *pp_chain) {
    *pp_chain = NULL;
  }
  else {
    back->next_->prev_ = back->prev_;
    back->prev_->next_ = back->next_;
  }
  back->next_ = back->prev_ = back;

  return back;
}

struct pptk *pptk_trim_front(struct pptk *chain) {
  while (chain && ((chain->tok_ == PPTK_WHITESPACE) ||
                   (chain->tok_ == PPTK_NEWLINE_WHITESPACE))) {
    pptk_free(pptk_pop_front(&chain));
  }
  return chain;
}

struct pptk *pptk_trim_back(struct pptk *chain) {
  while (chain && ((chain->prev_->tok_ == PPTK_WHITESPACE) ||
                   (chain->prev_->tok_ == PPTK_NEWLINE_WHITESPACE))) {
    pptk_free(pptk_pop_back(&chain));
  }
  return chain;
}

struct pptk *pptk_trim(struct pptk *chain) {
  chain = pptk_trim_front(chain);
  chain = pptk_trim_back(chain);
  return chain;
}

struct pptk *pptk_clone_single(struct c_compiler *cc, struct pptk *one) {
  if (!one) return NULL;
  struct pptk *clone = pptk_alloc(cc, NULL, one->text_, one->tok_, &one->situs_);
  if (!clone) {
    return NULL;
  }
  if ((one->tok_ != PPTK_TYPEDEF_NAME) && one->v_.expr_) {
    clone->v_.expr_  = one->v_.expr_;
    one->v_.expr_->refs_++;
  }
  else if (one->tok_ != PPTK_TYPEDEF_NAME) {
    clone->v_.type_ = one->v_.type_;
  }
  return clone;
}

struct pptk *pptk_clone(struct c_compiler *cc, struct pptk *chain) {
  struct pptk *clone_chain = NULL;
  struct pptk *tk = chain;
  if (tk) {
    do {
      struct pptk *clone = pptk_clone_single(cc, tk);
      if (!clone) {
        pptk_free(clone_chain);
        return NULL;
      }

      clone_chain = pptk_join(clone_chain, clone);

      tk = tk->next_;
    } while (tk != chain);
  }

  return clone_chain;
}

struct pptk *pptk_clone_trimmed(struct c_compiler *cc, struct pptk *chain) {
  struct pptk *clone_chain = NULL;
  struct pptk *tk = chain;
  int beyond_leading_ws = 0;
  struct pptk *end;
  /* find the end of the chain when trimmed of whitespace */
  end = chain;
  while ((end->prev_ != chain) && (pptk_is_whitespace(end->prev_))) {
    end = end->prev_;
  }
  if (tk) {
    do {
      if (!beyond_leading_ws) {
        if (pptk_is_whitespace(tk)) {
          tk = tk->next_;
          continue;
        }
        beyond_leading_ws = 1;
      }
      struct pptk *clone = pptk_clone_single(cc, tk);
      if (!clone) {
        pptk_free(clone_chain);
        return NULL;
      }

      clone_chain = pptk_join(clone_chain, clone);

      tk = tk->next_;
    } while (tk != end);
  }

  return clone_chain;

}

void macro_free(struct macro *m) {
  if (!m) return;
  if (--m->refcount_) return; /* still have references, keep alive.. */
  pptk_free(m->args_);
  pptk_free(m->replacement_list_);
  free(m);
}

struct macro_arg_inst *macro_arg_inst_alloc(struct pptk *tokens) {
  struct macro_arg_inst *mai = (struct macro_arg_inst *)malloc(sizeof(struct macro_arg_inst));
  if (!mai) return NULL;
  mai->tokens_ = tokens;
  mai->next_ = mai->prev_ = mai;
  return mai;
}

void macro_arg_inst_free(struct macro_arg_inst *mai) {
  if (!mai) return;
  pptk_free(mai->tokens_);
  free(mai);
}

struct macro_arg_inst *macro_arg_inst_join(struct macro_arg_inst *front, struct macro_arg_inst *back) {
  if (!back) return front;
  if (!front) return back;
  struct macro_arg_inst *front_head = front;
  struct macro_arg_inst *front_tail = front->prev_;
  struct macro_arg_inst *back_head = back;
  struct macro_arg_inst *back_tail = back->prev_;

  back_tail->next_ = front_head;
  front_head->prev_ = back_tail;

  front_tail->next_ = back_head;
  back_head->prev_ = front_tail;

  return front_head;
}

int pptk_perform_macro_expansion(struct c_compiler *cc, struct pptk **pp_chain) {
  struct pptk *output_chain = NULL;
  struct pptk *token_chain = *pp_chain;

  if (!token_chain) return 0;

  *pp_chain = NULL;

  struct ppme_stack macro_expander;
  ppme_stack_init(&macro_expander);

  while (token_chain) {
    int next_sym = g_pptk_to_ppme_[token_chain->tok_];
    if (next_sym == PPME_IDENT) {
      struct macro *m = (struct macro *)st_find(&cc->macro_table_, token_chain->text_);
      if (m) {
        if (m->is_function_style_) {
          next_sym = PPME_FUNCTION;
        }
        else {
          next_sym = PPME_OBJECT;
        }
      }
    }

    switch (ppme_parse(&macro_expander, next_sym, cc, &token_chain, 1 /* token_chain is entire and final bit of input */, &output_chain)) {
      case _PPME_FINISH:
        if (token_chain) {
          cc_printf(cc, "Unexpected macro-expander finish: input has not ended.\n");
          goto fail;
        }
        break;
      case _PPME_MATCH:
      case _PPME_END_OF_INPUT:
        /* Should not reach here - lexer internal code */
        assert(0);
        goto fail;
      case _PPME_FEED_ME:
        assert(0);
        cc_printf(cc, "Internal error, macro expander not adhering to end of stream");
        goto fail;
      case _PPME_LEXICAL_ERROR:
        assert(0);
        cc_printf(cc, "Internal error, lexical error on macro expander\n");
        goto fail;
      case _PPME_OVERFLOW:
        cc_printf(cc, "Overflow while parsing macro expansions\n");
        goto fail;
      case _PPME_NO_MEMORY:
        cc_printf(cc, "No memory while parsing macro expansions\n");
        goto fail;
      case _PPME_SYNTAX_ERROR:
        cc_printf(cc, "Syntax error while parsing macro expansions\n");
        break;
      case _PPME_INTERNAL_ERROR:
        cc_printf(cc, "Internal error while parsing macro expansions\n");
        goto fail;
      default:
        assert(0);
        cc_printf(cc, "Unexpected returncode while parsing macro expansions\n");
        goto fail;
    }
  }

  ppme_stack_cleanup(&macro_expander);

  *pp_chain = output_chain;

  return 0;

fail:
  pptk_free(output_chain);
  pptk_free(token_chain);
  return -1;
}

static int pptk_is_only_whitespace(struct pptk *chain) {
  struct pptk *tk;
  tk = chain;
  if (tk) {
    do {
      if (!pptk_is_whitespace(tk)) {
        return 0;
      }

      tk = tk->next_;
    } while (tk != chain);
  }
  return 1;
}

int pptk_rescan(struct c_compiler *cc, struct pptk *input_chain, struct pptk **pp_output_chain, int allow_header_name) {
  struct pptk *output_chain = NULL;
  struct pptk_stack tokenizer;
  struct situs input_chain_situs;
  situs_init(&input_chain_situs);
  struct pptk *tk = input_chain;
  if (tk) {
    do {
      if (situs_concat(&input_chain_situs, &tk->situs_)) {
        situs_cleanup(&input_chain_situs);
        return -1;
      }

      tk = tk->next_;
    } while (tk != input_chain);
  }
  pptk_stack_init(&tokenizer);
  pptk_set_input(&tokenizer, input_chain->text_, strlen(input_chain->text_), (input_chain->next_ == input_chain));
  pptk_set_mode(&tokenizer, allow_header_name ? M_PPTK_HEADER_NAME_POSSIBLE : M_PPTK_DEFAULT);
  int r;
  for (;;) {
    r = pptk_scan(&tokenizer, cc, &output_chain, &input_chain_situs);
    if (r == _PPTK_SYNTAX_ERROR) {
      cc_printf(cc, "(%d): syntax error in rescan during macro expansion at column %d: \"%s\"\n", pptk_line(&tokenizer), pptk_column(&tokenizer), pptk_text(&tokenizer));
    }
    else if (r == PPTK_TOKENIZER_HEADERNAME_CHECK) {
      pptk_set_mode(&tokenizer, allow_header_name ? M_PPTK_HEADER_NAME_POSSIBLE : M_PPTK_DEFAULT);
    }
    else if (r == _PPTK_LEXICAL_ERROR) {
      cc_printf(cc, "(%d): lexical error in rescan during macro expansion at column %d: \"%s\"\n", pptk_line(&tokenizer), pptk_column(&tokenizer), pptk_text(&tokenizer));
    }
    else if (r == _PPTK_FEED_ME) {
      if (input_chain->next_ == input_chain) {
        cc_printf(cc, "(%d): Internal error, input requested beyond end in rescan during macro expansion at column %d: \"%s\"\n", pptk_line(&tokenizer), pptk_column(&tokenizer), pptk_text(&tokenizer));
        pptk_free(output_chain);
        pptk_free(input_chain);
        return -1;
      }
      /* Needs more input; push in next token */
      pptk_free(pptk_pop_front(&input_chain));
      pptk_set_input(&tokenizer, input_chain->text_, strlen(input_chain->text_), (input_chain->next_ == input_chain));
    }
    else if (r == _PPTK_FINISH) {
      break;
    }
    else {
      cc_printf(cc, "(%d): Internal error in rescan during macro expansion at column %d: \"%s\"\n", pptk_line(&tokenizer), pptk_column(&tokenizer), pptk_text(&tokenizer));
      pptk_free(output_chain);
      pptk_free(input_chain);
      return -1;
    }
  }

  /* Pop the last token of input.. then it should be free */
  pptk_free(pptk_pop_front(&input_chain));

  assert(input_chain == NULL);
  pptk_stack_cleanup(&tokenizer);

  *pp_output_chain = pptk_join(*pp_output_chain, output_chain);

  return 0;
}

int pptk_scan_str(struct c_compiler *cc, const char *text, struct pptk **pp_output_chain) {
  struct pptk *output_chain = NULL;
  struct pptk_stack tokenizer;
  struct situs input_chain_situs;
  situs_init(&input_chain_situs);
  input_chain_situs.num_spans_ = 1;
  input_chain_situs.u_.one_.filename_ = "";
  input_chain_situs.u_.one_.start_ = 0;
  input_chain_situs.u_.one_.start_line_ = 1;
  input_chain_situs.u_.one_.start_col_= 1;
  input_chain_situs.u_.one_.end_ = strlen(text);
  input_chain_situs.u_.one_.end_line_ = 1;
  input_chain_situs.u_.one_.end_col_ = (int)strlen(text); // this is not exact, but may be bigger (UTF-8).

  pptk_stack_init(&tokenizer);
  pptk_set_input(&tokenizer, text, strlen(text), 1);
  pptk_set_mode(&tokenizer, M_PPTK_DEFAULT);
  int r;
  for (;;) {
    r = pptk_scan(&tokenizer, cc, &output_chain, &input_chain_situs);
    if (r == _PPTK_SYNTAX_ERROR) {
      cc_printf(cc, "(%d): syntax error in scan at column %d: \"%s\"\n", pptk_line(&tokenizer), pptk_column(&tokenizer), pptk_text(&tokenizer));
    }
    else if (r == PPTK_TOKENIZER_HEADERNAME_CHECK) {
      pptk_set_mode(&tokenizer, M_PPTK_DEFAULT);
    }
    else if (r == _PPTK_LEXICAL_ERROR) {
      cc_printf(cc, "(%d): lexical error in scan at column %d: \"%s\"\n", pptk_line(&tokenizer), pptk_column(&tokenizer), pptk_text(&tokenizer));
    }
    else if (r == _PPTK_FEED_ME) {
      cc_printf(cc, "(%d): Internal error, input requested beyond end in scan at column %d: \"%s\"\n", pptk_line(&tokenizer), pptk_column(&tokenizer), pptk_text(&tokenizer));
      pptk_free(output_chain);
      return -1;
    }
    else if (r == _PPTK_FINISH) {
      break;
    }
    else {
      cc_printf(cc, "(%d): Internal error in scan at column %d: \"%s\"\n", pptk_line(&tokenizer), pptk_column(&tokenizer), pptk_text(&tokenizer));
      pptk_free(output_chain);
      return -1;
    }
  }

  pptk_stack_cleanup(&tokenizer);

  *pp_output_chain = pptk_join(*pp_output_chain, output_chain);

  return 0;
}

static int pptk_concat(struct c_compiler *cc, struct pptk *left, struct pptk *right, struct pptk **pp_output_chain) {
  struct pptk *input_chain = pptk_join(left, right);
  struct pptk *output_chain = NULL;

  if (pptk_rescan(cc, input_chain, &output_chain, 0)) {
    return -1;
  }

  if (output_chain && (output_chain->next_ != output_chain)) {
    cc_printf(cc, "(%d): Warning, token concatenation yields multiple distinct tokens\n", situs_line(&output_chain->situs_));
  }

  *pp_output_chain = pptk_join(*pp_output_chain, output_chain);

  return 0;
}

static size_t pptk_stringify_buf(char *dst, const char *src, size_t src_size) {
  size_t offset = 0;
  size_t pos;
  for (pos = 0; pos < src_size; ++pos) {
#define LIT(x) do { if (dst) strcpy(dst+offset, x); offset += strlen(x); } while (0)
    switch (src[pos]) {
      case '\0': LIT("\\0"); break;
      case '\'': LIT("\\\'"); break;
      case '\"': LIT("\\\""); break;
      case '\\': LIT("\\\\"); break;
      case '\a': LIT("\\a"); break;
      case '\b': LIT("\\b"); break;
      case '\f': LIT("\\f"); break;
      case '\n': LIT("\\n"); break;
      case '\r': LIT("\\r"); break;
      case '\t': LIT("\\t"); break;
      case '\v': LIT("\\v"); break;
      default:
        if ((src[pos] >= 0x20) && (src[pos] <= 0x7E)) {
          if (dst) dst[offset] = src[pos];
          offset++;
        }
        else {
          if (dst) {
            dst[offset++] = '\\';
            dst[offset++] = "01234567"[(src[pos] >> 6) & 0x7];
            dst[offset++] = "01234567"[(src[pos] >> 3) & 0x7];
            dst[offset++] = "01234567"[src[pos] & 0x7];
          }
          else {
            offset += 4;
          }
        }
        break;
    }
#undef LIT
  }
  return offset;
}

static size_t pptk_stringize_to_buf(char *dst, struct pptk *chain) {
  size_t offset = 0;
  struct pptk *tk = chain;
  if (tk) {
    do {
      int have_whitespace = 0;
      while (pptk_is_whitespace(tk)) {
        tk = tk->next_;
        have_whitespace = 1;
        if (tk == chain) {
          /* Break out */
          break;
        }
      }

      if (have_whitespace) {
        if (dst) {
          dst[offset] = ' ';
        }
        offset++;
      }

      if (have_whitespace && (tk == chain)) {
        /* Tail was whitespace, iteration has ended.. */
        break;
      }

      /* Process current (non-whitespace) item */
      offset += pptk_stringify_buf(dst ? dst + offset : NULL, tk->text_, strlen(tk->text_));

      tk = tk->next_;
    } while (tk != chain);
  }

  return offset;
}


static struct pptk *pptk_stringize(struct c_compiler *cc, struct pptk *chain) {
  /* Note: this function and ptpk_make_string_lit (below) are very similair, if you discover a problem in one, you likely have to fix the other.
   * pptk_make_string_lit() - create a string literal from binary data.
   * pptk_stringize() - create a string literal from the source representation of a set of tokens. */
  size_t size_needed = 2 /* open and closing doublequotes */ + 1 /* terminator */ + pptk_stringize_to_buf(NULL, chain);
  char *string_lit = (char *)malloc(size_needed);
  if (!string_lit) {
    cc_no_memory(cc);
    return NULL;
  }
  pptk_stringize_to_buf(string_lit + 1 /* skip place for opening doublequote */, chain);
  string_lit[0] = '\"';
  string_lit[size_needed - 2] = '\"';
  string_lit[size_needed - 1] = '\0';

  /* Form the situs - will be one grand substitution for the original string. */
  struct pptk *tk = chain;
  struct situs situs;
  situs_init(&situs);
  if (tk) {
    do {
      if (situs_concat(&situs, &tk->situs_)) {
        cc_no_memory(cc);
        situs_cleanup(&situs);
        free(string_lit);
        return NULL;
      }

      tk = tk->next_;
    } while (tk != chain);
  }
  situs_convert_to_substitution(&situs, size_needed - 1);
  tk = pptk_alloc(cc, NULL, string_lit, PPTK_STRING_LIT, &situs);
  situs_cleanup(&situs);
  free(string_lit);
  if (!tk) {
    cc_no_memory(cc);
    return NULL;
  }
  /* PPTK_STRING_LIT depends on expr with datasection portion existing with the actual data; this logic
   * is non-trivial and implemented in pptokenizer. Let's abuse pptk_rescan() to invoke the pptokenizer
   * for us now that we have a beautiful string of data. */
  int r;
  struct pptk *output = NULL;
  r = pptk_rescan(cc, tk, &output, 0);
  if (r) {
    return NULL;
  }
  return output;
}

static struct pptk *pptk_make_string_lit(struct c_compiler *cc, const char *data, size_t len, const struct situs *subs_loc) {
  /* Note: this function and pptk_stringize (above) are very similair, if you discover a problem in one, you likely have to fix the other.
   * pptk_make_string_lit() - create a string literal from binary data.
   * pptk_stringize() - create a string literal from the source representation of a set of tokens. */
  size_t size_needed = 2 /* open and closing double quotes */ + 1 /* null terminator */ + pptk_stringify_buf(NULL, data, len);
  char *string_lit = (char *)malloc(size_needed);
  if (!string_lit) {
    cc_no_memory(cc);
    return NULL;
  }
  pptk_stringify_buf(string_lit + 1 /* skip place for opening doublequote */, data, len);
  string_lit[0] = '\"';
  string_lit[size_needed - 2] = '\"';
  string_lit[size_needed - 1] = '\0';

  struct situs situs;
  situs_init(&situs);
  situs_clone(&situs, subs_loc);
  situs_convert_to_substitution(&situs, size_needed - 1);
  struct pptk *tk = pptk_alloc(cc, NULL, string_lit, PPTK_STRING_LIT, &situs);
  situs_cleanup(&situs);
  free(string_lit);
  if (!tk) {
    cc_no_memory(cc);
    return NULL;
  }
  /* PPTK_STRING_LIT depends on expr with datasection portion existing with the actual data; this logic
   * is non-trivial and implemented in pptokenizer. Let's abuse pptk_rescan() to invoke the pptokenizer
   * for us now that we have a beautiful string of data. */
  int r;
  struct pptk *output = NULL;
  r = pptk_rescan(cc, tk, &output, 0);
  if (r) {
    return NULL;
  }
  return output;

}

int macro_expand(struct c_compiler *cc, struct pptk *macro_ident, struct macro *m, struct macro_arg_inst *args, struct pptk **pp_output_chain) {
  /* Check for builtin macros */
  if (!strcmp(macro_ident->text_, "__FILE__")) {
    /* builtin */
    struct pptk *stringlit = pptk_make_string_lit(cc, cc->ppme_input_file_, strlen(cc->ppme_input_file_), &macro_ident->situs_);
    if (!stringlit) return -1;
    *pp_output_chain = pptk_join(*pp_output_chain, stringlit);
    return 0;
  }
  if (!strcmp(macro_ident->text_, "__LINE__")) {
    char line_num_str[32] = {0};
    sprintf(line_num_str, "%d", cc->ppme_input_line_);
    struct pptk *intlit = pptk_alloc(cc, NULL, line_num_str, PPTK_INTEGER_LIT, &macro_ident->situs_);
    if (!intlit) {
      cc_no_memory(cc);
      return -1;
    }
    intlit->v_.expr_ = expr_alloc(ET_C_INT);
    if (intlit->v_.expr_) {
      intlit->v_.expr_->v_.i64_ = (int64_t)cc->ppme_input_line_;
    }
    else {
      cc_no_memory(cc);
    }

    *pp_output_chain = pptk_join(*pp_output_chain, intlit);
    return 0;
  }

  if (!m->replacement_list_) return 0;
  struct pptk *instanced = pptk_clone(cc, m->replacement_list_);
  if (!instanced) return -1;

  size_t num_args = 0, num_inst_args = 0;
  struct pptk *arg = m->args_;
  if (arg) {
    do {
      num_args++;

      arg = arg->next_;
    } while (arg != m->args_);
  }

  struct macro_arg_inst *inst_arg = args;
  struct macro_arg_inst *variadic_tail = args;
  if (inst_arg) {
    do {
      if (num_inst_args == num_args) {
        variadic_tail = inst_arg;
      }

      num_inst_args++;

      inst_arg = inst_arg->next_;
    } while (inst_arg != args);
  }

  if ((num_inst_args > num_args) && !m->is_variadic_) {
    cc_printf(cc, "too many arguments to macro %s: %zu found, but only %zu expected\n", m->sym_.ident_, num_inst_args, num_args);
    *pp_output_chain = pptk_join(*pp_output_chain, instanced);
    return 0; /* not fatal */
  }
  else if (num_inst_args < num_args) {
    cc_printf(cc, "insufficient number of arguments to macro %s: %zu found, but %s%zu expected\n", m->sym_.ident_, num_inst_args, m->is_variadic_ ? "at least " : "", num_args);
    *pp_output_chain = pptk_join(*pp_output_chain, instanced);
    return 0; /* not fatal */
  }

  /* - Keep track of preceeding # stringizing
   * - Sweep arguments for macro expansion 
   * - Check if macro has following ## (and perform the tokenization if so.) 
   * - Note that we want to split this up, as according to technique spec, so e.g. placemarkers and the like. 
   * - Validate macro replacement list at time of its #define parsing
   * - Once the final list has been made, do another sweep for macros over it before returning it.
   */
  struct pptk *tk = instanced;
  struct pptk *arg_expanded = NULL;
  int preceeded_by_hash_hash = 0;
  int followed_by_hash_hash;
  do {
    int token_replaced = 0;

    followed_by_hash_hash = (tk->next_ != instanced) && (tk->next_->tok_ == PPTK_HASH_HASH_MARK);

    int preceeded_by_hash = 0;

    if ((g_pptk_to_ppme_[tk->tok_] == PPME_IDENT) ||
        ((tk->tok_ == PPTK_HASH_MARK) && (tk->next_ != instanced) && (g_pptk_to_ppme_[tk->next_->tok_] == PPME_IDENT))) {
      struct pptk *id = NULL;
      if (tk->tok_ != PPTK_HASH_MARK) {
        preceeded_by_hash = 0;
        id = tk;
      }
      else {
        preceeded_by_hash = 1;
        id = tk->next_;
      }
      arg = m->args_;
      if (arg) {
        inst_arg = args;
        do {
          struct pptk *expansion = NULL;
          if (!strcmp(id->text_, arg->text_)) {
            /* Replacement */
            if (pptk_is_only_whitespace(inst_arg->tokens_) && (followed_by_hash_hash || preceeded_by_hash_hash)) {
              /* Empty argument, and we're adjacent to a ## token, insert placemarker if this is not stringified (preceeded by #) */
              if (!preceeded_by_hash) {
                struct situs insertion;
                situs_init_from_after(&insertion, &id->situs_, 0);
                struct pptk *placemarker = pptk_alloc(cc, &expansion, NULL, PPTK_PLACEMARKER, &insertion);
                situs_cleanup(&insertion);
                if (!placemarker) {
                  cc_no_memory(cc);
                  pptk_free(expansion);
                  pptk_free(instanced);
                  pptk_free(arg_expanded);
                  return -1;
                }
              }
            }
            else {
              /* Macro expansion of argument required *prior* to insertion */
              struct pptk *arg_clone = pptk_clone_trimmed(cc, inst_arg->tokens_);
              if (inst_arg->tokens_ && !arg_clone) {
                cc_no_memory(cc);
                pptk_free(expansion);
                pptk_free(instanced);
                pptk_free(arg_expanded);
                return -1;
              }

              if (!preceeded_by_hash && !preceeded_by_hash_hash && !followed_by_hash_hash) {
                int r = pptk_perform_macro_expansion(cc, &arg_clone);
                if (r) {
                  pptk_free(expansion);
                  pptk_free(instanced);
                  pptk_free(arg_expanded);
                  return r;
                }
              }

              expansion = arg_clone;
            }
            if (preceeded_by_hash) {
              struct pptk *stringified = pptk_stringize(cc, expansion);
              if (!stringified) {
                cc_no_memory(cc);
                pptk_free(expansion);
                pptk_free(instanced);
                pptk_free(arg_expanded);
                return -1;
              }
              pptk_free(expansion);
              expansion = stringified;
            }
            arg_expanded = pptk_join(arg_expanded, expansion);

            token_replaced = 1;
            break;
          }
          else if (m->is_variadic_ && !strcmp(id->text_, "__VA_ARGS__")) {
            struct macro_arg_inst *tail = variadic_tail;
            if (tail && tail != args) {
              do {
                if (pptk_is_only_whitespace(tail->tokens_) && (tail->next_ == args) && (followed_by_hash_hash || preceeded_by_hash_hash)) {
                  if (!preceeded_by_hash) {
                    /* Empty __VA_ARGS__ argument, and we're adjacent to a ## token, insert placemarker
                     * Note that if __VA_ARGS__ expands to multiple empty arguments then it expands to one
                     * or more commas; which would not cause a placemarker insertion. */
                    struct situs insertion;
                    situs_init_from_after(&insertion, &id->situs_, 0);
                    struct pptk *placemarker = pptk_alloc(cc, &expansion, NULL, PPTK_PLACEMARKER, &insertion);
                    situs_cleanup(&insertion);
                    if (!placemarker) {
                      cc_no_memory(cc);
                      pptk_free(expansion);
                      pptk_free(instanced);
                      pptk_free(arg_expanded);
                      return -1;
                    }
                  }
                }
                else {
                  struct pptk *arg_clone = pptk_clone(cc, tail->tokens_);
                  if (tail->tokens_ && !arg_clone) {
                    cc_no_memory(cc);
                    pptk_free(expansion);
                    pptk_free(instanced);
                    pptk_free(arg_expanded);
                    return -1;
                  }

                  if (!preceeded_by_hash && !preceeded_by_hash_hash && !followed_by_hash_hash) {
                    /* Expand macros for each __VA_ARGS__ argument individually, and concatenate them together.
                     * (Don't expand as a whole or macro invocations can straddle multiple macro arguments.) */
                    int r = pptk_perform_macro_expansion(cc, &arg_clone);
                    if (r) {
                      pptk_free(arg_clone);
                      pptk_free(expansion);
                      pptk_free(instanced);
                      pptk_free(arg_expanded);
                      return r;
                    }
                  }

                  expansion = pptk_join(expansion, arg_clone);

                  /* Arguments have dropped the comma between the args, recreate them. */
                  if (tail->next_ != args) {
                    struct situs insertion;
                    situs_init_from_after(&insertion, &(*pp_output_chain)->prev_->situs_, 1 /* comma "," byte length 1 */);
                    if (!pptk_alloc(cc, &expansion, ",", PPTK_COMMA, &insertion)) {
                      situs_cleanup(&insertion);
                      cc_no_memory(cc);
                      pptk_free(expansion);
                      pptk_free(instanced);
                      pptk_free(arg_expanded);
                      return -1;
                    }
                    situs_cleanup(&insertion);
                  }
                }

                tail = tail->next_;
              } while (tail != args);
            }
            else if (followed_by_hash_hash || preceeded_by_hash_hash) {
              if (!preceeded_by_hash) {
                /* No __VA_ARGS__ "argument" but adjacent to ## token. Whether this is permitted is dubious (and should
                 * be checked when counting arguments) -- here we defensively handle it as an empty argument.. */
                struct situs insertion;
                situs_init_from_after(&insertion, &id->situs_, 0);
                struct pptk *placemarker = pptk_alloc(cc, &expansion, NULL, PPTK_PLACEMARKER, &insertion);
                if (!placemarker) {
                  cc_no_memory(cc);
                  pptk_free(expansion);
                  pptk_free(instanced);
                  pptk_free(arg_expanded);
                  return -1;
                }
              }
            }
            if (preceeded_by_hash) {
              struct pptk *stringified = pptk_stringize(cc, expansion);
              if (!stringified) {
                cc_no_memory(cc);
                pptk_free(expansion);
                pptk_free(instanced);
                pptk_free(arg_expanded);
                return -1;
              }
              pptk_free(expansion);
              expansion = stringified;
            }
            arg_expanded = pptk_join(arg_expanded, expansion);
            token_replaced = 1;
            break;
          }

          if (inst_arg) {
            if (inst_arg->next_ != args) {
              inst_arg = inst_arg->next_;
            }
            else {
              inst_arg = NULL;
            }
          }
          arg = arg->next_;
        } while (arg != m->args_);
      }
    }

    preceeded_by_hash_hash = (tk->tok_ == PPTK_HASH_HASH_MARK);

    if (!token_replaced) {
      arg_expanded = pptk_join(arg_expanded, pptk_pop_front(&instanced));
    }
    else {
      if (preceeded_by_hash) {
        /* Pop hash first.. */
        pptk_free(pptk_pop_front(&instanced));
      }
      pptk_free(pptk_pop_front(&instanced));
    }

    tk = instanced;
  } while (tk);

  /* Macro replacement list is now in arg_expanded; process ## occurrances (if any) */
  struct pptk *repl_list = NULL;
  if (arg_expanded && (arg_expanded->tok_ == PPTK_HASH_HASH_MARK)) {
    cc_printf(cc, "Error, ## token at start of macro replacement list\n");
    pptk_free(pptk_pop_front(&arg_expanded));
  }
  while (arg_expanded) {
    if ((arg_expanded->next_ != arg_expanded) && (arg_expanded->next_->tok_ == PPTK_HASH_HASH_MARK)) {
      if (arg_expanded->next_->next_ == arg_expanded) {
        cc_printf(cc, "Error, ## token at end of macro replacement list\n");
        repl_list = pptk_join(repl_list, pptk_pop_front(&arg_expanded));
        pptk_free(pptk_pop_front(&arg_expanded)); /* pop the ## */
      }
      else {
        struct pptk *left, *right;
        left = pptk_pop_front(&arg_expanded);
        pptk_free(pptk_pop_front(&arg_expanded)); /* pop the ## */
        right = pptk_pop_front(&arg_expanded);

        if ((left->tok_ == PPTK_PLACEMARKER) && (right->tok_ == PPTK_PLACEMARKER)) {
          /* Push back a placemarker, delete the other one */
          arg_expanded = pptk_join(right, arg_expanded);
          pptk_free(left);
        }
        else if (left->tok_ == PPTK_PLACEMARKER) {
          /* Push back the valid (right) token, delete the other one */
          arg_expanded = pptk_join(right, arg_expanded);
          pptk_free(left);
        }
        else if (right->tok_ == PPTK_PLACEMARKER) {
          /* Push back the valid (left) token, delete the other one */
          arg_expanded = pptk_join(left, arg_expanded);
          pptk_free(right);
        }
        else {
          /* Concatenate the two tokens and re-evaluate their tokenization; then push the result back */
          struct pptk *concatenated_token = NULL;
          int r = pptk_concat(cc, left, right, &concatenated_token);

          if (r || !concatenated_token) {
            /* Fatal error, note that pptk_concat has cleaned up left and right in this case also */
            pptk_free(concatenated_token);
            pptk_free(repl_list);
            return -1;
          }
          arg_expanded = pptk_join(concatenated_token, arg_expanded);
        }
      }
    }
    else {
      /* No concatenation (##) operator, pop the top and copy it over */
      repl_list = pptk_join(repl_list, pptk_pop_front(&arg_expanded));
    }
  }

  /* Run repl_list through pp_macro_expander.cbrt as a final expansion of all macros;
   * note that nested invocations of the macro are explicitly ignored, but that this
   * only the case for the full replacement list, not for argument expansions and the
   * like. */
  m->nested_invocation_ = 1;
  int r = pptk_perform_macro_expansion(cc, &repl_list);
  m->nested_invocation_ = 0;
  if (r) {
    pptk_free(repl_list);
    return r;
  }

  *pp_output_chain = pptk_join(*pp_output_chain, repl_list);

  return 0;
}

int ppce_eval_const_expr(struct c_compiler *cc, struct pptk *input_chain, int64_t *result, int *is_unsigned) {
  struct ppce_stack stack;
  ppce_stack_init(&stack);
  int next_sym;
  struct pptk *tk = input_chain;

  if (tk) {
    do {
      next_sym = g_pptk_to_ppce_[tk->tok_];
      /* Skip whitespace, process everything else.. */
      if (next_sym != PPCE_WS) {
        int r = ppce_parse(&stack, next_sym, cc, &tk, input_chain, 1, result, is_unsigned);
        switch (r) {
          case _PPCE_FINISH:
            if (tk != input_chain) {
              cc_printf(cc, "Unexpected macro-expander finish: input has not ended.\n");
              goto fail;
            }
            ppce_stack_cleanup(&stack);
            return 0;
          case _PPCE_MATCH:
          case _PPCE_END_OF_INPUT:
            /* Should not reach here - lexer internal code */
            assert(0);
            goto fail;
          case _PPCE_FEED_ME:
            assert(0);
            cc_printf(cc, "Internal error, pp const expr evaluator not adhering to end of stream");
            goto fail;
          case _PPCE_LEXICAL_ERROR:
            assert(0);
            cc_printf(cc, "Internal error, lexical error on pp const expr evaluator \n");
            goto fail;
          case _PPCE_OVERFLOW:
            cc_printf(cc, "Overflow while parsing pp const expr\n");
            goto fail;
          case _PPCE_NO_MEMORY:
            goto fail;
          case _PPCE_SYNTAX_ERROR:
            cc_printf(cc, "Syntax error while parsing pp const expr\n");
            break;
          case _PPCE_INTERNAL_ERROR:
            cc_printf(cc, "Internal error while parsing pp const expr\n");
            goto fail;
          default:
            assert(0);
            cc_printf(cc, "Unexpected returncode while pp const expr\n");
            goto fail;
        }
      }

      tk = tk->next_;
    } while (tk != input_chain);
  }

fail:
  ppce_stack_cleanup(&stack);
  return -1;
}
