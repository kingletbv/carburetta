/* Copyright 2020 Kinglet B.V.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http ://www.apache.org/licenses/LICENSE-2.0
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

#ifndef CTYPE_H_INCLUDED
#define CTYPE_H_INCLUDED
#include <ctype.h>
#endif

#ifndef ASSERT_H_INCLUDED
#define ASSERT_H_INCLUDED
#include <assert.h>
#endif

#ifndef REPORT_ERROR_H_INCLUDED
#define REPORT_ERROR_H_INCLUDED
#include "report_error.h"
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

#ifndef REGEX_GRAMMAR_H_INCLUDED
#define REGEX_GRAMMAR_H_INCLUDED
#include "regex_grammar.h"
#endif

#ifndef GRAMMAR_TABLE_H_INCLUDED
#define GRAMMAR_TABLE_H_INCLUDED
#include "grammar_table.h"
#endif

#ifndef LALR_H_INCLUDED
#define LALR_H_INCLUDED
#include "lalr.h"
#endif

#ifndef SYMBOL_H_INCLUDED
#define SYMBOL_H_INCLUDED
#include "symbol.h"
#endif

#ifndef TYPESTR_H_INCLUDED
#define TYPESTR_H_INCLUDED
#include "typestr.h"
#endif

#ifndef CARBURETTA_CONTEXT_H_INCLUDED
#define CARBURETTA_CONTEXT_H_INCLUDED
#include "carburetta_context.h"
#endif

#ifndef EMIT_C_H_INCLUDED
#define EMIT_C_H_INCLUDED
#include "emit_c.h"
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

static int process_carburetta_directive(struct tkr_tokenizer *tkr_tokens, struct xlts *directive_line_match, struct carburetta_context *cc) {
  int r;
  int ate_percent = 0;
  int ate_directive = 0;
  int ate_colon_seperator = 0;
  int found_a_placeholder = 0;
  int found_prefix = 0;
  struct symbol_table *st = &cc->symtab_;
  struct symbol **typed_symbols = NULL;
  size_t num_typed_symbols = 0;
  size_t num_typed_symbols_allocated = 0;
  struct snippet dir_snippet;
  int prefer_over_valid = 0;
  int prefer_over_has_rule = 0;
  snippet_init(&dir_snippet);
  enum {
    PCD_NT_DIRECTIVE,
    PCD_TOKEN_DIRECTIVE,
    PCD_TOKEN_TYPE_DIRECTIVE,
    PCD_TYPE_DIRECTIVE,
    PCD_COMMON_TYPE_DIRECTIVE,
    PCD_CONSTRUCTOR_DIRECTIVE,
    PCD_DESTRUCTOR_DIRECTIVE,
    PCD_TOKEN_ACTION_DIRECTIVE,
    PCD_PREFIX_DIRECTIVE,
    PCD_TOKEN_PREFIX_DIRECTIVE,
    PCD_PARAMS_DIRECTIVE,
    PCD_LOCALS_DIRECTIVE,
    PCD_ON_SUCCESS_DIRECTIVE,
    PCD_ON_SYNTAX_ERROR_DIRECTIVE,
    PCD_ON_ALLOC_ERROR_DIRECTIVE,
    PCD_ON_INTERNAL_ERROR_DIRECTIVE,
    PCD_ON_NEXT_TOKEN_DIRECTIVE,
    PCD_END_TOKEN,
    PCD_ERROR_TOKEN,
    PCD_PREFER,
    PCD_OVER
  } directive;
  tok_switch_to_nonterminal_idents(tkr_tokens);

  r = tkr_tokenizer_inputx(tkr_tokens, directive_line_match, 1);
  while ((r != TKR_END_OF_INPUT) && (r != TKR_FEED_ME)) {
    if (r == TKR_SYNTAX_ERROR) {
      if (isprint(tkr_str(tkr_tokens)[0])) {
        re_error_tkr(tkr_tokens, "Syntax error character \"%s\" not expected", tkr_str(tkr_tokens));
      }
      else {
        re_error_tkr(tkr_tokens, "Syntax error character 0x%02x not expected", tkr_str(tkr_tokens)[0]);
      }
    }
    else if (r == TKR_INTERNAL_ERROR) {
      goto cleanup_exit;
    }
    else if (r == TKR_MATCH) {
      int eat_whitespace = 0;

      if (ate_directive) {
        if ((directive == PCD_TOKEN_ACTION_DIRECTIVE) ||
            (directive == PCD_CONSTRUCTOR_DIRECTIVE) ||
            (directive == PCD_DESTRUCTOR_DIRECTIVE) ||
            (directive == PCD_PARAMS_DIRECTIVE) ||
            (directive == PCD_LOCALS_DIRECTIVE) ||
            (directive == PCD_ON_SUCCESS_DIRECTIVE) ||
            (directive == PCD_ON_SYNTAX_ERROR_DIRECTIVE) ||
            (directive == PCD_ON_ALLOC_ERROR_DIRECTIVE) ||
            (directive == PCD_ON_INTERNAL_ERROR_DIRECTIVE) ||
            (directive == PCD_ON_NEXT_TOKEN_DIRECTIVE)) {
          eat_whitespace = 0;
          /* Keep whitespace for all code directives */
        }
        else {
          /* Consume whitespace for all type directives */
          eat_whitespace = 1;
        }
      }
      if (eat_whitespace && (tkr_tokens->best_match_variant_ == TOK_WHITESPACE)) {
        /* eat silently */
      }
      else {
        if (!ate_percent) {
          if (tkr_tokens->best_match_variant_ == TOK_PERCENT) {
            /* leading percent sign indicates carburetta directive, eat it */
            ate_percent = 1;
          }
          else {
            re_error_tkr(tkr_tokens, "Syntax error, \"%%\" expected");
            r = TKR_SYNTAX_ERROR;
            goto cleanup_exit;
          }
        }
        else if (!ate_directive) {
          if (tkr_tokens->best_match_variant_ == TOK_IDENT) {
            ate_directive = 1;
            if (!strcmp("nt", tkr_str(tkr_tokens))) {
              directive = PCD_NT_DIRECTIVE;
            }
            else if (!strcmp("token", tkr_str(tkr_tokens))) {
              directive = PCD_TOKEN_DIRECTIVE;
            }
            else if (!strcmp("token_type", tkr_str(tkr_tokens))) {
              directive = PCD_TOKEN_TYPE_DIRECTIVE;
              snippet_clear(&cc->token_type_);
            }
            else if (!strcmp("nt_type", tkr_str(tkr_tokens))) {
              re_error_tkr(tkr_tokens, "%%nt_type is deprecated, please use %%type instead");
              directive = PCD_TYPE_DIRECTIVE;
            }
            else if (!strcmp("type", tkr_str(tkr_tokens))) {
              directive = PCD_TYPE_DIRECTIVE;
            }
            else if (!strcmp("common_type", tkr_str(tkr_tokens))) {
              directive = PCD_COMMON_TYPE_DIRECTIVE;
            }
            else if (!strcmp("constructor", tkr_str(tkr_tokens))) {
              directive = PCD_CONSTRUCTOR_DIRECTIVE;
              if (!cc->most_recent_typestr_) {
                re_error_tkr(tkr_tokens, "%%constructor must follow %%token_type or %%type directive");
              }
            }
            else if (!strcmp("destructor", tkr_str(tkr_tokens))) {
              directive = PCD_DESTRUCTOR_DIRECTIVE;
              if (!cc->most_recent_typestr_) {
                re_error_tkr(tkr_tokens, "%%destructor must follow %%token_type or %%type directive");
              }
            }
            else if (!strcmp("token_action", tkr_str(tkr_tokens))) {
              directive = PCD_TOKEN_ACTION_DIRECTIVE;
              if (!cc->most_recent_typestr_) {
                re_error_tkr(tkr_tokens, "%%constructor must follow %%token_type or %%type directive");
              }
            }
            else if (!strcmp("prefix", tkr_str(tkr_tokens))) {
              directive = PCD_PREFIX_DIRECTIVE;
            }
            else if (!strcmp("token_prefix", tkr_str(tkr_tokens))) {
              directive = PCD_TOKEN_PREFIX_DIRECTIVE;
            }
            else if (!strcmp("params", tkr_str(tkr_tokens))) {
              directive = PCD_PARAMS_DIRECTIVE;
            }
            else if (!strcmp("locals", tkr_str(tkr_tokens))) {
              directive = PCD_LOCALS_DIRECTIVE;
            }
            else if (!strcmp("on_success", tkr_str(tkr_tokens))) {
              directive = PCD_ON_SUCCESS_DIRECTIVE;
            }
            else if (!strcmp("on_syntax_error", tkr_str(tkr_tokens))) {
              directive = PCD_ON_SYNTAX_ERROR_DIRECTIVE;
            }
            else if (!strcmp("on_alloc_error", tkr_str(tkr_tokens))) {
              directive = PCD_ON_ALLOC_ERROR_DIRECTIVE;
            }
            else if (!strcmp("on_internal_error", tkr_str(tkr_tokens))) {
              directive = PCD_ON_INTERNAL_ERROR_DIRECTIVE;
            }
            else if (!strcmp("on_next_token", tkr_str(tkr_tokens))) {
              directive = PCD_ON_NEXT_TOKEN_DIRECTIVE;
            }
            else if (!strcmp("end_token", tkr_str(tkr_tokens))) {
              directive = PCD_END_TOKEN;
            }
            else if (!strcmp("error_token", tkr_str(tkr_tokens))) {
              directive = PCD_ERROR_TOKEN;
            }
            else if (!strcmp("prefer", tkr_str(tkr_tokens))) {
              directive = PCD_PREFER;
              prd_prod_reset(&cc->prefer_prod_);
              cc->prefer_prod_place_ = -1;
            }
            else if (!strcmp("over", tkr_str(tkr_tokens))) {
              directive = PCD_OVER;
              prd_prod_reset(&cc->over_prod_);
              cc->over_prod_place_ = -1;
            }
            else if (!strcmp("debug_end", tkr_str(tkr_tokens))) {
              re_error(directive_line_match, "%%debug_end encountered, terminating early");
              r = TKR_INTERNAL_ERROR;
              goto cleanup_exit;
            }
            else {
              re_error_tkr(tkr_tokens, "Syntax error invalid directive");
              r = TKR_SYNTAX_ERROR;
              goto cleanup_exit;
            }
            if (directive != PCD_OVER) {
              /* If there is a valid %prefer directive pending, it should immediately be followed
               * by a %over directive. */
              if (cc->prefer_prod_.nt_.id_.num_translated_) {
                re_error(&cc->prefer_prod_.nt_.id_, "Warning: dud %%prefer directive. %%prefix should be immediately followed by %%over directive");
                prd_prod_reset(&cc->prefer_prod_);
                cc->prefer_prod_place_ = -1;
              }
            }
          }
        }
        else {
          if (directive == PCD_NT_DIRECTIVE) {
            if (tkr_tokens->best_match_variant_ == TOK_IDENT) {
              int is_new = -1;
              struct symbol *sym = symbol_find_or_add(st, SYM_NONTERMINAL, &tkr_tokens->xmatch_, &is_new);
              if (!sym) {
                re_error_tkr(tkr_tokens, "Error, no memory");
                r = TKR_INTERNAL_ERROR;
                goto cleanup_exit;
              }
              if (!is_new) {
                re_error_tkr(tkr_tokens, "Non-terminal \"%s\" already declared at line %d", tkr_str(tkr_tokens), xlts_line(&sym->def_));
              }
            }
            else {
              re_error_tkr(tkr_tokens, "Syntax error identifier expected");
              r = TKR_SYNTAX_ERROR;
              goto cleanup_exit;
            }
          }
          else if (directive == PCD_TOKEN_DIRECTIVE) {
            if (tkr_tokens->best_match_variant_ == TOK_IDENT) {
              int is_new = -1;
              struct symbol *sym = symbol_find_or_add(st, SYM_TERMINAL, &tkr_tokens->xmatch_, &is_new);
              if (!sym) {
                re_error_tkr(tkr_tokens, "Error, no memory");
                r = TKR_INTERNAL_ERROR;
                goto cleanup_exit;
              }
              if (!is_new) {
                re_error_tkr(tkr_tokens, "Token \"%s\" already declared at line %d", tkr_str(tkr_tokens), xlts_line(&sym->def_));
              }
            }
            else {
              re_error_tkr(tkr_tokens, "Syntax error identifier expected");
              r = TKR_SYNTAX_ERROR;
              goto cleanup_exit;
            }
          }
          else if (directive == PCD_END_TOKEN) {
            if (tkr_tokens->best_match_variant_ == TOK_IDENT) {
              int is_new = -1;
              struct symbol *sym = symbol_find_or_add(st, SYM_TERMINAL, &tkr_tokens->xmatch_, &is_new);
              if (!sym) {
                re_error_tkr(tkr_tokens, "Error, no memory");
                r = TKR_INTERNAL_ERROR;
                goto cleanup_exit;
              }
              if (!is_new) {
                if (sym->st_ == SYM_TERMINAL) {
                  re_error_tkr(tkr_tokens, "Warning: End token \"%s\" identifier already used at line %d", tkr_str(tkr_tokens), xlts_line(&sym->def_));
                }
                else {
                  re_error_tkr(tkr_tokens, "Error: End token \"%s\" identifier already used as non-terminal at line %d", tkr_str(tkr_tokens), xlts_line(&sym->def_));
                  r = TKR_SYNTAX_ERROR;
                  goto cleanup_exit;
                }
              }
              cc->input_end_sym_ = sym;
            }
            else {
              re_error_tkr(tkr_tokens, "Syntax error identifier expected");
              r = TKR_SYNTAX_ERROR;
              goto cleanup_exit;
            }
          }
          else if (directive == PCD_ERROR_TOKEN) {
            if (tkr_tokens->best_match_variant_ == TOK_IDENT) {
              int is_new = -1;
              struct symbol *sym = symbol_find_or_add(st, SYM_TERMINAL, &tkr_tokens->xmatch_, &is_new);
              if (!sym) {
                re_error_tkr(tkr_tokens, "Error, no memory");
                r = TKR_INTERNAL_ERROR;
                goto cleanup_exit;
              }
              if (!is_new) {
                if (sym->st_ == SYM_TERMINAL) {
                  re_error_tkr(tkr_tokens, "Warning: Error token \"%s\" identifier already used at line %d", tkr_str(tkr_tokens), xlts_line(&sym->def_));
                }
                else {
                  re_error_tkr(tkr_tokens, "Error: Error token \"%s\" identifier already used as non-terminal at line %d", tkr_str(tkr_tokens), xlts_line(&sym->def_));
                  r = TKR_SYNTAX_ERROR;
                  goto cleanup_exit;
                }
              }
              cc->error_sym_ = sym;
            }
            else {
              re_error_tkr(tkr_tokens, "Syntax error identifier expected");
              r = TKR_SYNTAX_ERROR;
              goto cleanup_exit;
            }
          }
          else if (directive == PCD_TOKEN_TYPE_DIRECTIVE) {
            /* Trim simple whitespace off the head end, otherwise simply copy. */
            if (cc->token_type_.num_tokens_ || (tkr_tokens->best_match_action_ != TOK_WHITESPACE)) {
              if (tkr_tokens->best_match_variant_ == TOK_SPECIAL_IDENT) {
                if (strcmp("$", tkr_str(tkr_tokens))) {
                  re_error_tkr(tkr_tokens, "Error: \"%s\" not allowed, a type string may only have a single \"$\" special identifier as a declarator identifier placeholder", tkr_str(tkr_tokens));
                  r = TKR_SYNTAX_ERROR;
                  goto cleanup_exit;
                }
                if (found_a_placeholder) {
                  re_error_tkr(tkr_tokens, "Error: a type string may have at most 1 declarator identifier placeholder");
                  r = TKR_SYNTAX_ERROR;
                  goto cleanup_exit;
                }
                found_a_placeholder = 1;
              }
              r = snippet_append_tkr(&cc->token_type_, tkr_tokens);
              if (r) {
                r = TKR_INTERNAL_ERROR;
                goto cleanup_exit;
              }
            }
          }
          else if (directive == PCD_TYPE_DIRECTIVE) {
            if (!ate_colon_seperator) {
              if (tkr_tokens->best_match_action_ != TOK_COLON) {
                if (tkr_tokens->best_match_action_ == TOK_IDENT) {
                  struct symbol *sym = symbol_find(st, tkr_str(tkr_tokens));
                  if (!sym) {
                    re_error_tkr(tkr_tokens, "Error: \"%s\" is not a symbol", tkr_str(tkr_tokens));
                    r = TKR_SYNTAX_ERROR;
                    goto cleanup_exit;
                  }
                  else {
                    size_t n;
                    for (n = 0; n < num_typed_symbols; ++n) {
                      if (sym == typed_symbols[n]) {
                        re_error_tkr(tkr_tokens, "Error: \"%s\" appears more than once in type directive", tkr_str(tkr_tokens));
                        r = TKR_SYNTAX_ERROR;
                        goto cleanup_exit;
                      }
                    }
                    if (num_typed_symbols == num_typed_symbols_allocated) {
                      size_t new_num_alloc = num_typed_symbols_allocated * 2 + 1;
                      if (new_num_alloc < num_typed_symbols_allocated) {
                        re_error_tkr(tkr_tokens, "Error: overflow on allocation");
                        r = TKR_INTERNAL_ERROR;
                        goto cleanup_exit;
                      }
                      if (new_num_alloc > (SIZE_MAX / sizeof(struct symbol *))) {
                        re_error_tkr(tkr_tokens, "Error: overflow on allocation");
                        r = TKR_INTERNAL_ERROR;
                        goto cleanup_exit;
                      }
                      size_t alloc_size = new_num_alloc * sizeof(struct symbol *);
                      void *p = realloc(typed_symbols, alloc_size);
                      if (!p) {
                        re_error_tkr(tkr_tokens, "Error: no memory");
                        r = TKR_INTERNAL_ERROR;
                        goto cleanup_exit;
                      }
                      typed_symbols = (struct symbol **)p;
                      num_typed_symbols_allocated = new_num_alloc;
                    }
                    typed_symbols[num_typed_symbols++] = sym;
                  }
                }
                else {
                  re_error_tkr(tkr_tokens, "Error: \"%s\" is not a non-terminal", tkr_str(tkr_tokens));
                  r = TKR_SYNTAX_ERROR;
                  goto cleanup_exit;
                }
              }
              else {
                ate_colon_seperator = 1;
              }
            }
            else {
              /* ate_colon_seperator -- everything else is part of the type, use the same logic
               * as for token_type */
              if (dir_snippet.num_tokens_ || (tkr_tokens->best_match_action_ != TOK_WHITESPACE)) {
                if (tkr_tokens->best_match_variant_ == TOK_SPECIAL_IDENT) {
                  if (strcmp("$", tkr_str(tkr_tokens))) {
                    re_error_tkr(tkr_tokens, "Error: \"%s\" not allowed, a type string may only have a single \"$\" special identifier as a declarator identifier placeholder", tkr_str(tkr_tokens));
                    r = TKR_SYNTAX_ERROR;
                    goto cleanup_exit;
                  }
                  if (found_a_placeholder) {
                    re_error_tkr(tkr_tokens, "Error: a type string may have at most 1 declarator identifier placeholder");
                    r = TKR_SYNTAX_ERROR;
                    goto cleanup_exit;
                  }
                  found_a_placeholder = 1;
                }
                r = snippet_append_tkr(&dir_snippet, tkr_tokens);
                if (r) {
                  r = TKR_INTERNAL_ERROR;
                  goto cleanup_exit;
                }
              }
            }
          }
          else if (directive == PCD_COMMON_TYPE_DIRECTIVE) {
            /* Trim simple whitespace off the head end, otherwise simply copy. */
            if (cc->common_data_type_.num_tokens_ || (tkr_tokens->best_match_action_ != TOK_WHITESPACE)) {
              if (tkr_tokens->best_match_variant_ == TOK_SPECIAL_IDENT) {
                if (strcmp("$", tkr_str(tkr_tokens))) {
                  re_error_tkr(tkr_tokens, "Error: \"%s\" not allowed, a type string may only have a single \"$\" special identifier as a declarator identifier placeholder", tkr_str(tkr_tokens));
                  r = TKR_SYNTAX_ERROR;
                  goto cleanup_exit;
                }
                if (found_a_placeholder) {
                  re_error_tkr(tkr_tokens, "Error: a type string may have at most 1 declarator identifier placeholder");
                  r = TKR_SYNTAX_ERROR;
                  goto cleanup_exit;
                }
                found_a_placeholder = 1;
              }
              r = snippet_append_tkr(&cc->common_data_type_, tkr_tokens);
              if (r) {
                r = TKR_INTERNAL_ERROR;
                goto cleanup_exit;
              }
            }
          }
          else if (directive == PCD_CONSTRUCTOR_DIRECTIVE) {
            if (dir_snippet.num_tokens_ || (tkr_tokens->best_match_action_ != TOK_WHITESPACE)) {
              r = snippet_append_tkr(&dir_snippet, tkr_tokens);
              if (r) {
                r = TKR_INTERNAL_ERROR;
                goto cleanup_exit;
              }
            }
          }
          else if (directive == PCD_DESTRUCTOR_DIRECTIVE) {
            if (dir_snippet.num_tokens_ || (tkr_tokens->best_match_action_ != TOK_WHITESPACE)) {
              r = snippet_append_tkr(&dir_snippet, tkr_tokens);
              if (r) {
                r = TKR_INTERNAL_ERROR;
                goto cleanup_exit;
              }
            }
          }
          else if (directive == PCD_TOKEN_ACTION_DIRECTIVE) {
            if (dir_snippet.num_tokens_ || (tkr_tokens->best_match_action_ != TOK_WHITESPACE)) {
              r = snippet_append_tkr(&dir_snippet, tkr_tokens);
              if (r) {
                r = TKR_INTERNAL_ERROR;
                goto cleanup_exit;
              }
            }
          }
          else if (directive == PCD_PREFIX_DIRECTIVE) {
            if (found_prefix) {
              re_error_tkr(tkr_tokens, "Error: Only a single identifier expected following %%prefix", tkr_str(tkr_tokens));
              r = TKR_SYNTAX_ERROR;
              goto cleanup_exit;
            }
            if (tkr_tokens->best_match_action_ != TOK_IDENT) {
              re_error_tkr(tkr_tokens, "Error: \"%s\" not allowed, expected an identifier", tkr_str(tkr_tokens));
              r = TKR_SYNTAX_ERROR;
              goto cleanup_exit;
            }
            xlts_reset(&cc->prefix_);
            r = xlts_append(&cc->prefix_, &tkr_tokens->xmatch_);
            if (r) {
              r = TKR_INTERNAL_ERROR;
              goto cleanup_exit;
            }
            if (cc->prefix_.num_translated_) {
              if (cc->prefix_uppercase_) free(cc->prefix_uppercase_);
              cc->prefix_uppercase_ = (char *)malloc(cc->prefix_.num_translated_ + 1);
              memcpy(cc->prefix_uppercase_, cc->prefix_.translated_, cc->prefix_.num_translated_);
              cc->prefix_uppercase_[cc->prefix_.num_translated_] = 0;
              size_t n;
              for (n = 0; n < cc->prefix_.num_translated_; ++n) {
                char c;
                c = cc->prefix_uppercase_[n];
                if ((c >= 'a') && (c <= 'z')) {
                  c = c - 'a' + 'A';
                }
                cc->prefix_uppercase_[n] = c;
              }
            }

            found_prefix = 1;
          }
          else if (directive == PCD_TOKEN_PREFIX_DIRECTIVE) {
            if (found_prefix) {
              re_error_tkr(tkr_tokens, "Error: Only a single identifier expected following %%token_prefix", tkr_str(tkr_tokens));
              r = TKR_SYNTAX_ERROR;
              goto cleanup_exit;
            }
            if (tkr_tokens->best_match_action_ != TOK_IDENT) {
              re_error_tkr(tkr_tokens, "Error: \"%s\" not allowed, expected an identifier", tkr_str(tkr_tokens));
              r = TKR_SYNTAX_ERROR;
              goto cleanup_exit;
            }
            xlts_reset(&cc->token_prefix_);
            r = xlts_append(&cc->token_prefix_, &tkr_tokens->xmatch_);
            if (r) {
              r = TKR_INTERNAL_ERROR;
              goto cleanup_exit;
            }
            if (cc->token_prefix_.num_translated_) {
              if (cc->token_prefix_uppercase_) free(cc->token_prefix_uppercase_);
              cc->token_prefix_uppercase_ = (char *)malloc(cc->token_prefix_.num_translated_ + 1);
              memcpy(cc->token_prefix_uppercase_, cc->token_prefix_.translated_, cc->token_prefix_.num_translated_);
              cc->token_prefix_uppercase_[cc->token_prefix_.num_translated_] = 0;
              size_t n;
              for (n = 0; n < cc->token_prefix_.num_translated_; ++n) {
                char c;
                c = cc->token_prefix_uppercase_[n];
                if ((c >= 'a') && (c <= 'z')) {
                  c = c - 'a' + 'A';
                }
                cc->token_prefix_uppercase_[n] = c;
              }
            }

            found_prefix = 1;
          }
          else if ((directive == PCD_PARAMS_DIRECTIVE) || 
                   (directive == PCD_LOCALS_DIRECTIVE) ||
                   (directive == PCD_ON_SUCCESS_DIRECTIVE) || 
                   (directive == PCD_ON_SYNTAX_ERROR_DIRECTIVE) || 
                   (directive == PCD_ON_ALLOC_ERROR_DIRECTIVE) || 
                   (directive == PCD_ON_INTERNAL_ERROR_DIRECTIVE) || 
                   (directive == PCD_ON_NEXT_TOKEN_DIRECTIVE)) {
            if (dir_snippet.num_tokens_ || (tkr_tokens->best_match_action_ != TOK_WHITESPACE_CHAR)) {
              r = snippet_append_tkr(&dir_snippet, tkr_tokens);
              if (r) {
                r = TKR_INTERNAL_ERROR;
                goto cleanup_exit;
              }
            }
          }
          else if ((directive == PCD_PREFER) || (directive == PCD_OVER)) {
          struct prd_production *prod = (directive == PCD_PREFER) ? &cc->prefer_prod_ : &cc->over_prod_;
          int *pplace = (directive == PCD_PREFER) ? &cc->prefer_prod_place_ : &cc->over_prod_place_;
            if (!ate_colon_seperator) {
              /* Grab production non-terminal */
              if (prod->nt_.id_.num_translated_) {
                if (tkr_tokens->best_match_action_ == TOK_COLON) {
                  ate_colon_seperator = 1;
                }
                else {
                  re_error_tkr(tkr_tokens, "Error: \"%s\" not allowed, expected a colon ':'", tkr_str(tkr_tokens));
                  r = TKR_SYNTAX_ERROR;
                  goto cleanup_exit;
                }
              }
              else {
                if (tkr_tokens->best_match_action_ == TOK_IDENT) {
                  xlts_reset(&prod->nt_.id_);
                  r = xlts_append(&prod->nt_.id_, &tkr_tokens->xmatch_);
                  if (r) {
                    r = TKR_INTERNAL_ERROR;
                    goto cleanup_exit;
                  }
                }
                else {
                  re_error_tkr(tkr_tokens, "Error: \"%s\" not allowed, expected an identifier for the non-terminal", tkr_str(tkr_tokens));
                  r = TKR_SYNTAX_ERROR;
                  goto cleanup_exit;
                }
              }
            }
            else {
              /* Grab rules and position placement. */
              prefer_over_has_rule = 1;
              if (tkr_tokens->best_match_action_ == TOK_IDENT) {
                r = prd_prod_check_sym_reserve(prod, &tkr_tokens->xmatch_);
                if (r) {
                  r = TKR_INTERNAL_ERROR;
                  goto cleanup_exit;
                }
                struct prd_production_sym *prod_sym = prod->syms_ + prod->num_syms_++;
                prd_production_sym_init(prod_sym);
                r = xlts_append(&prod_sym->id_, &tkr_tokens->xmatch_);
                if (r) {
                  r = TKR_INTERNAL_ERROR;
                  goto cleanup_exit;
                }
              }
              else if ((tkr_tokens->best_match_action_ == TOK_ASTERISK) &&
                       ((*pplace) == -1)) {
                *pplace = (int)prod->num_syms_;
                prefer_over_valid = 1;
              }
              else {
                if (tkr_tokens->best_match_action_ == TOK_ASTERISK) {
                  re_error_tkr(tkr_tokens, "Error: '*' placement marker should appear only once");
                  r = TKR_SYNTAX_ERROR;
                  goto cleanup_exit;
                }
                else {
                  re_error_tkr(tkr_tokens, "Error: \"%s\" not allowed, expected an identifier as part of the production rule", tkr_str(tkr_tokens));
                  r = TKR_SYNTAX_ERROR;
                  goto cleanup_exit;
                }
              }
            }
          }
        }
      }
    }

    r = tkr_tokenizer_inputx(tkr_tokens, directive_line_match, 1);
  }

  assert(r != TKR_FEED_ME); /* should not ask to get fed on final input */

  if (directive == PCD_PREFIX_DIRECTIVE) {
    if (!found_prefix) {
      re_error(directive_line_match, "Error: incomplete %%prefix directive, token expected");
      r = TKR_SYNTAX_ERROR;
      goto cleanup_exit;
    }
  }

  if (directive == PCD_TOKEN_PREFIX_DIRECTIVE) {
    if (!found_prefix) {
      re_error(directive_line_match, "Error: incomplete %%token_prefix directive, token expected");
      r = TKR_SYNTAX_ERROR;
      goto cleanup_exit;
    }
  }

  if ((directive == PCD_PREFER) || (directive == PCD_OVER)) {
    if (!prefer_over_valid) {
      if (prefer_over_has_rule) {
        re_error(directive_line_match, "Error: incomplete %s directive, missing asterisk", (directive == PCD_PREFER) ? "%prefer" : "%over");
      }
      else {
        re_error(directive_line_match, "Error: incomplete %s directive", (directive == PCD_PREFER) ? "%prefer" : "%over");
      }
      r = TKR_SYNTAX_ERROR;
      goto cleanup_exit;
    }
    if ((directive == PCD_OVER) && cc->prefer_prod_.nt_.id_.num_translated_) {
      /* Have both the %prefer and %over productions ready to go. */
      struct conflict_resolution *cr = (struct conflict_resolution *)malloc(sizeof(struct conflict_resolution));
      if (!cr) {
        conflict_resolution_cleanup(cr);
        free(cr);
        re_error_tkr(tkr_tokens, "Error: no memory");
        r = TKR_INTERNAL_ERROR;
        goto cleanup_exit;
      }
      conflict_resolution_init(cr);
      prd_prod_swap(&cr->prefer_prod_, &cc->prefer_prod_);
      cr->prefer_prod_place_ = cc->prefer_prod_place_;
      cr->over_prod_place_ = cc->over_prod_place_;
      prd_prod_swap(&cr->over_prod_, &cc->over_prod_);
      if (cc->conflict_resolutions_) {
        cr->next_ = cc->conflict_resolutions_->next_;
        cc->conflict_resolutions_->next_ = cr;
      }
      else {
        cr->next_ = cr;
      }
      cc->conflict_resolutions_ = cr;

      prd_prod_reset(&cc->prefer_prod_);
      cc->prefer_prod_place_ = -1;
      prd_prod_reset(&cc->over_prod_);
      cc->over_prod_place_ = -1;
    }
  }

  if ((directive == PCD_TYPE_DIRECTIVE) ||
      (directive == PCD_TOKEN_TYPE_DIRECTIVE) ||
      (directive == PCD_COMMON_TYPE_DIRECTIVE)) {
    /* Trim all whitespace off, pop any "$$" special identifiers at the tail end */
    struct snippet *snip;
    if (directive == PCD_TYPE_DIRECTIVE) {
      snip = &dir_snippet;
    }
    else if (directive == PCD_TOKEN_TYPE_DIRECTIVE) {
      snip = &cc->token_type_;
    }
    else /* (directive == PCD_COMMON_TYPE_DIRECTIVE) */ {
      assert(directive == PCD_COMMON_TYPE_DIRECTIVE);
      snip = &cc->common_data_type_;
    }
    while (snip->num_tokens_ &&
           ((snip->tokens_[snip->num_tokens_ - 1].variant_ == TOK_WHITESPACE) ||
           (snip->tokens_[snip->num_tokens_ - 1].variant_ == TOK_SPECIAL_IDENT))) {
      snippet_pop_last_token(snip);
    }
  }

  if ((directive == PCD_LOCALS_DIRECTIVE) ||
      (directive == PCD_PARAMS_DIRECTIVE) ||
      (directive == PCD_CONSTRUCTOR_DIRECTIVE) ||
      (directive == PCD_DESTRUCTOR_DIRECTIVE) ||
      (directive == PCD_TOKEN_ACTION_DIRECTIVE)) {
    /* Trim simple whitespace off the tail end, preserve comments, remove newlines and spaces */
    while (dir_snippet.num_tokens_ && (
      (dir_snippet.tokens_[dir_snippet.num_tokens_ - 1].match_ == TOK_WHITESPACE_CHAR) ||
      (dir_snippet.tokens_[dir_snippet.num_tokens_ - 1].match_ == TOK_NEWLINE))) {
      snippet_pop_last_token(&dir_snippet);
    }
  }

  if (directive == PCD_TYPE_DIRECTIVE) {
    size_t n;
    int is_new;
    struct typestr *nt_ts = typestr_find_or_add(&cc->tstab_, &dir_snippet, &is_new);
    if (!nt_ts) {
      r = TKR_INTERNAL_ERROR;
      goto cleanup_exit;
    }
    cc->most_recent_typestr_ = nt_ts;
    for (n = 0; n < num_typed_symbols; ++n) {
      struct symbol *sym = typed_symbols[n];
      snippet_clear(&sym->type_snippet_);
      r = snippet_append_snippet(&sym->type_snippet_, &dir_snippet);
      if (r) goto cleanup_exit;
      sym->assigned_type_ = nt_ts;
    }
    cc->have_typed_symbols_ = 1;
  }

  if (directive == PCD_TOKEN_TYPE_DIRECTIVE) {
    int is_new;
    struct typestr *token_ts = typestr_find_or_add(&cc->tstab_, &cc->token_type_, &is_new);
    if (!token_ts) {
      r = TKR_INTERNAL_ERROR;
      goto cleanup_exit;
    }
    cc->token_assigned_type_ = token_ts;
    cc->most_recent_typestr_ = token_ts;
    cc->have_typed_symbols_ = 1;
  }

  if (directive == PCD_COMMON_TYPE_DIRECTIVE) {
    int is_new;
    struct typestr *common_ts = typestr_find_or_add(&cc->tstab_, &cc->common_data_type_, &is_new);
    if (!common_ts) {
      r = TKR_INTERNAL_ERROR;
      goto cleanup_exit;
    }
    cc->common_data_assigned_type_ = common_ts;
    cc->most_recent_typestr_ = common_ts;
  }

  if (directive == PCD_CONSTRUCTOR_DIRECTIVE) {
    snippet_clear(&cc->most_recent_typestr_->constructor_snippet_);
    r = snippet_append_snippet(&cc->most_recent_typestr_->constructor_snippet_, &dir_snippet);
    if (r) goto cleanup_exit;
  }

  if (directive == PCD_DESTRUCTOR_DIRECTIVE) {
    snippet_clear(&cc->most_recent_typestr_->destructor_snippet_);
    r = snippet_append_snippet(&cc->most_recent_typestr_->destructor_snippet_, &dir_snippet);
    if (r) goto cleanup_exit;
  }

  if (directive == PCD_TOKEN_ACTION_DIRECTIVE) {
    snippet_clear(&cc->most_recent_typestr_->token_action_snippet_);
    r = snippet_append_snippet(&cc->most_recent_typestr_->token_action_snippet_, &dir_snippet);
    if (r) goto cleanup_exit;
  }

  if (directive == PCD_PARAMS_DIRECTIVE) {
    snippet_clear(&cc->params_snippet_);
    r = snippet_append_snippet(&cc->params_snippet_, &dir_snippet);
    if (r) goto cleanup_exit;
  }
  
  if (directive == PCD_LOCALS_DIRECTIVE) {
    snippet_clear(&cc->locals_snippet_);
    r = snippet_append_snippet(&cc->locals_snippet_, &dir_snippet);
    if (r) goto cleanup_exit;
  }

  if (directive == PCD_ON_SUCCESS_DIRECTIVE) {
    snippet_clear(&cc->on_success_snippet_);
    r = snippet_append_snippet(&cc->on_success_snippet_, &dir_snippet);
    if (r) goto cleanup_exit;
  }

  if (directive == PCD_ON_SYNTAX_ERROR_DIRECTIVE) {
    snippet_clear(&cc->on_syntax_error_snippet_);
    r = snippet_append_snippet(&cc->on_syntax_error_snippet_, &dir_snippet);
    if (r) goto cleanup_exit;
  }

  if (directive == PCD_ON_ALLOC_ERROR_DIRECTIVE) {
    snippet_clear(&cc->on_alloc_error_snippet_);
    r = snippet_append_snippet(&cc->on_alloc_error_snippet_, &dir_snippet);
    if (r) goto cleanup_exit;
  }

  if (directive == PCD_ON_INTERNAL_ERROR_DIRECTIVE) {
    snippet_clear(&cc->on_internal_error_snippet_);
    r = snippet_append_snippet(&cc->on_internal_error_snippet_, &dir_snippet);
    if (r) goto cleanup_exit;
  }

  if (directive == PCD_ON_NEXT_TOKEN_DIRECTIVE) {
    snippet_clear(&cc->on_next_token_snippet_);
    r = snippet_append_snippet(&cc->on_next_token_snippet_, &dir_snippet);
    if (r) goto cleanup_exit;
  }
  
  r = 0;

cleanup_exit:
  tkr_tokenizer_reset(tkr_tokens);
  snippet_cleanup(&dir_snippet);
  if (typed_symbols) free(typed_symbols);

  return r;
}


static int process_grammar_tokens(struct tkr_tokenizer *tkr_tokens, struct xlts *input_line, int is_final, struct prd_stack *prds, struct prd_grammar *g, struct symbol_table *st) {
  int r;
  struct xlts empty;
  xlts_init(&empty);
  if (is_final) input_line = &empty;
  r = tkr_tokenizer_inputx(tkr_tokens, input_line, is_final);
  while ((r != TKR_END_OF_INPUT) && (r != TKR_FEED_ME)) {
    if (r == TKR_SYNTAX_ERROR) {
      if (isprint(tkr_str(tkr_tokens)[0])) {
        re_error_tkr(tkr_tokens, "Syntax error character \"%s\" not expected", tkr_str(tkr_tokens));
      }
      else {
        re_error_tkr(tkr_tokens, "Syntax error character 0x%02x not expected", tkr_str(tkr_tokens)[0]);
      }
    }
    else if (r == TKR_INTERNAL_ERROR) {
      return r;
    }
    else if (r == TKR_MATCH) {
      r = prd_parse_tkr(prds, g, tkr_tokens, 0, st);
      switch (r) {
      case PRD_SUCCESS:
        /* This should not be possible without is_final==1 */
        re_error_tkr(tkr_tokens, "Internal error, premature end of grammar");
        return TKR_INTERNAL_ERROR;
        break;
      case PRD_SYNTAX_ERROR:
        /* XXX: Need to recover, but how! (Transition on all parent Error syms and attempt to shift input) */
        g->have_errors_ = 1;
        break;
      case PRD_NEXT:
        break;
      case PRD_INTERNAL_ERROR:
        return TKR_INTERNAL_ERROR;
      }
    }

    r = tkr_tokenizer_inputx(tkr_tokens, input_line, is_final);
  }

  if (r == TKR_END_OF_INPUT) {
    r = prd_parse_tkr(prds, g, tkr_tokens, 1, st);
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
      if (!g->have_errors_) {
        re_error_tkr(tkr_tokens, "Internal error, grammar expected to end");
      }
      return TKR_INTERNAL_ERROR;
    case PRD_INTERNAL_ERROR:
      return TKR_INTERNAL_ERROR;
    }
  }

  return r;
}

static int process_scanner_tokens(struct tkr_tokenizer *tkr_tokens, struct xlts *input_line, int is_final, struct rxg_stack *rxgs, struct prd_grammar *g, struct symbol_table *st) {
  int r;
  struct xlts empty;
  xlts_init(&empty);
  if (is_final) input_line = &empty;
  r = tkr_tokenizer_inputx(tkr_tokens, input_line, is_final);
  while ((r != TKR_END_OF_INPUT) && (r != TKR_FEED_ME)) {
    if (r == TKR_SYNTAX_ERROR) {
      if (isprint(tkr_str(tkr_tokens)[0])) {
        re_error_tkr(tkr_tokens, "Syntax error character \"%s\" not expected", tkr_str(tkr_tokens));
      }
      else {
        re_error_tkr(tkr_tokens, "Syntax error character 0x%02x not expected", tkr_str(tkr_tokens)[0]);
      }
    }
    else if (r == TKR_INTERNAL_ERROR) {
      return r;
    }
    else if (r == TKR_MATCH) {
      r = rxg_parse_tkr(rxgs, g, tkr_tokens, 0, st);
      switch (r) {
      case PRD_SUCCESS:
        /* This should not be possible without is_final==1 */
        re_error_tkr(tkr_tokens, "Internal error, premature end of scanner");
        return TKR_INTERNAL_ERROR;
        break;
      case PRD_SYNTAX_ERROR:
        /* XXX: Need to recover, but how! (Transition on all parent Error syms and attempt to shift input) */
        g->have_errors_ = 1;
        break;
      case PRD_NEXT:
        break;
      case PRD_INTERNAL_ERROR:
        return TKR_INTERNAL_ERROR;
      }
    }

    r = tkr_tokenizer_inputx(tkr_tokens, input_line, is_final);
  }

  if (r == TKR_END_OF_INPUT) {
    r = rxg_parse_tkr(rxgs, g, tkr_tokens, 1, st);
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
      if (!g->have_errors_) {
        re_error_tkr(tkr_tokens, "Internal error, scanner expected to end");
      }
      return TKR_INTERNAL_ERROR;
    case PRD_INTERNAL_ERROR:
      return TKR_INTERNAL_ERROR;
    }
  }

  return r;
}

void print_usage(void) {
  fprintf(stderr, "Carburetta parser generator (C) 2020 Kinglet B.V.\n"
                  "https://carburetta.com/\n"
                  "\n"
                  "carburetta <inputfile.cnd> [ --c [ <c_filename> [--h [h_filename] ] ] ]\n"
                  "\n"
                  "<inputfile.cnd>\n"
                  "         the input file containing the grammar (mandatory)\n"
                  "\n"
                  "--c\n"
                  "         generate a C file (implied if ommitted)\n"
                  "\n"
                  "--c [<c_filename>]\n"
                  "         generate a C file and output it to c_filename. If no filename is\n"
                  "         specified, then output will be to stdout\n"
                  "\n"
                  "--h [<h_filename>]\n"
                  "         generate a C header file and output it to h_filename. If no filename\n"
                  "         is present, a c_filename must be present as it will then be used to\n"
                  "         derive a filename for the header file.\n"
  );
}

int main(int argc, char **argv) {
  int r;

  struct carburetta_context cc;
  carburetta_context_init(&cc);

  char **cpv = argv + 1;
  int cr = argc - 1;
  int expecting_inputfile = 1;
  int expecting_hfile = 0;
  int expecting_cfile = 0;
  int generate_cfile = 1;
  int generate_hfile = 0;
  char *input_filename = NULL;
  while (cr) {
    if (!strcmp("--c", *cpv)) {
      expecting_hfile = 0;
      expecting_cfile = 1;
      cr--; cpv++;
    }
    else if (!strcmp("--h", *cpv)) {
      expecting_hfile = 1;
      expecting_cfile = 0;
      generate_hfile = 1;
      cr--; cpv++;
    }
    else if (expecting_hfile) {
      if (cc.h_output_filename_) {
        re_error_nowhere("Error: only one C header output file permitted");
        print_usage();
        goto exit_arg_eval;
      }
      if (!strcmp("-", *cpv)) {
        /* H output filename derived from C output filename */
      }
      else {
        cc.h_output_filename_ = strdup(*cpv);
        if (!cc.h_output_filename_) {
          re_error_nowhere("Error: no memory");
          goto exit_arg_eval;
        }
      }
      expecting_hfile = 0;
      cr--; cpv++;
    }
    else if (expecting_cfile) {
      if (cc.c_output_filename_) {
        re_error_nowhere("Error: only one C output file permitted");
        print_usage();
        goto exit_arg_eval;
      }
      cc.c_output_filename_ = strdup(*cpv);
      if (!cc.c_output_filename_) {
        re_error_nowhere("Error: no memory");
        goto exit_arg_eval;
      }
      expecting_cfile = 0;
      cr--; cpv++;
    }
    else if (expecting_inputfile) {
      input_filename = strdup(*cpv);
      if (!input_filename) {
        re_error_nowhere("Error: no memory");
        goto exit_arg_eval;
      }
      expecting_inputfile = 0;
      cr--; cpv++;
    }
    else {
      re_error_nowhere("Error: unrecognized commandline argument \"%s\"", *cpv);
      print_usage();
      goto exit_arg_eval;
    }
  }

  if (!input_filename) {
    re_error_nowhere("Error: need an input filename");
    print_usage();
    goto exit_arg_eval;
  }

  if (generate_hfile && !cc.h_output_filename_) {
    if (!cc.c_output_filename_) {
      re_error_nowhere("Error: Need C output filename to derive a C header output filename");
      goto exit_arg_eval;
    }
    const char *ext = strrchr(cc.c_output_filename_, '.');
    if (!ext || (strlen(ext) < 2)) {
      re_error_nowhere("Error: Need C output filename that ends in a filename extension to derive a C header output filename");
      goto exit_arg_eval;
    }
    cc.h_output_filename_ = strdup(cc.c_output_filename_);

    memcpy(cc.h_output_filename_ + (ext - cc.c_output_filename_), ".h", 3 /* inc terminator */);
  }

  if (0) {
  exit_arg_eval:
    carburetta_context_cleanup(&cc);
    if (input_filename) free(input_filename);
    return EXIT_FAILURE;
  }
  r = ldl_init();
  if (r) {
    re_error_nowhere("Failed to initialize ldl");
    return EXIT_FAILURE;
  }
  r = tok_init();
  if (r) {
    re_error_nowhere("Failed to initialize tok");
    return EXIT_FAILURE;
  }
  r = las_init();
  if (r) {
    re_error_nowhere("Failed to initialize las");
    return EXIT_FAILURE;
  }
  r = dct_init();
  if (r) {
    re_error_nowhere("Failed to initialize dct");
    return EXIT_FAILURE;
  }

  struct las_line_assembly line_assembly;
  las_init_line_assembly(&line_assembly);

  struct tkr_tokenizer tkr_lines;
  ldl_init_tokenizer(&tkr_lines);

  struct tkr_tokenizer tkr_tokens;
  tok_init_tkr_tokenizer(&tkr_tokens);

  struct prd_stack prds;
  prd_stack_init(&prds);

  struct rxg_stack rxgs;
  rxg_stack_init(&rxgs);

  struct prd_grammar prdg;
  prd_grammar_init(&prdg);

  struct grammar_table gt;
  gt_grammar_table_init(&gt);

  struct lr_generator lalr;
  lr_init(&lalr);

  struct sc_scanner scantable;
  sc_scanner_init(&scantable);

  struct xlts token_buf;
  xlts_init(&token_buf);

  struct xlts comment_free_line;
  xlts_init(&comment_free_line);

  r = prd_stack_reset(&prds);
  if (r) {
    re_error_nowhere("Internal error, failed to reset grammar parsing stack");
    r = EXIT_FAILURE;
    goto cleanup_exit;
  }

  r = rxg_stack_reset(&rxgs);
  if (r) {
    re_error_nowhere("Internal error, failed to reset scanner parsing stack");
    r = EXIT_FAILURE;
    goto cleanup_exit;
  }

  FILE *fp = NULL;
  if (input_filename) {
    fp = fopen(input_filename, "rb");
    if (!fp) {
      int err = errno;
      re_error_nowhere("Failed to open file \"%s\": %s", input_filename, strerror(err));
      r = EXIT_FAILURE;
      goto cleanup_exit;
    }
    las_set_filename(&line_assembly, input_filename);
    fprintf(stderr, "%s\n", input_filename);
  }
  else {
    fp = stdin;
    las_set_filename(&line_assembly, "(stdin)");
  }

  struct part *prologue = NULL;
  struct part *epilogue = NULL;

  enum {
    PROLOGUE,
    GRAMMAR,
    SCANNER,
    EPILOGUE
  } where_are_we = PROLOGUE, default_mode = GRAMMAR;
  

  size_t num_bytes_read;
  static char buf[2400];

  int have_error = 0;

  do {
    num_bytes_read = fread(buf, sizeof(*buf), sizeof(buf) / sizeof(*buf), fp);

    r = las_input(&line_assembly, buf, num_bytes_read, !num_bytes_read);
    while ((r != LAS_END_OF_INPUT) && (r != LAS_FEED_ME)) {
      /* Cannot modify (shift) the line_assembly's mlc_buf line buffer, so copy it over to a work area (token_buf) */
      xlts_reset(&token_buf);
      r = xlts_append(&token_buf, &line_assembly.mlc_buf_);
      if (r) {
        r = EXIT_FAILURE;
        goto cleanup_exit;
      }

      xlts_reset(&comment_free_line);
      xlts_append(&comment_free_line, &token_buf);
      dct_decomment(&comment_free_line);

      r = tkr_tokenizer_inputx(&tkr_lines, &comment_free_line, 1);

      /* Empty line cannot be matched as a match requires at least a single byte to be processed to force progress;
       * manually override that case to match LD_REGULAR */
      if ((r == TKR_END_OF_INPUT) && (comment_free_line.num_translated_ == 0)) {
        r = TKR_MATCH;
        tkr_lines.best_match_action_ = LD_REGULAR;
        tkr_lines.best_match_variant_ = LD_REGULAR;
      }

      if ((r == TKR_END_OF_INPUT) || (r == TKR_FEED_ME)) {
        re_error_nowhere("%s(%d): Internal error: all lines are expected to match.", line_assembly.lc_tkr_.filename_, line_assembly.lc_tkr_.input_line_);
        r = EXIT_FAILURE;
        goto cleanup_exit;
      }
      if (r == TKR_SYNTAX_ERROR) {
        if (isprint(tkr_str(&tkr_lines)[0])) {
          re_error_tkr(&tkr_lines, "Syntax error character \"%s\" not expected", tkr_str(&tkr_lines));
          have_error = 1;
        }
        else {
          re_error_tkr(&tkr_lines, "Syntax error character 0x%02x not expected", tkr_str(&tkr_lines));
          have_error = 1;
        }
      }
      else if (r == TKR_INTERNAL_ERROR) {
        num_bytes_read = 0;
        break;
      }

      if (r == TKR_MATCH) {
        if (where_are_we == PROLOGUE) {
          switch (tkr_lines.best_match_variant_) {
          case LD_C_PREPROCESSOR:
            /* Preserve line continuations */
            append_part(&prologue, token_buf.num_original_, token_buf.original_);
            break;
          case LD_CARBURETTA_SCANNER_SECTION_DELIMETER:
            where_are_we = default_mode = SCANNER;
            break;
          case LD_CARBURETTA_GRAMMAR_SECTION_DELIMETER:
            where_are_we = default_mode = GRAMMAR;
            break;
          case LD_CARBURETTA_SECTION_DELIMITER:
            where_are_we = default_mode;
            break;
          case LD_REGULAR:
            /* Preserve line continuations */
            append_part(&prologue, token_buf.num_original_, token_buf.original_);
            break;
          case LD_CARBURETTA_DIRECTIVE:
            r = process_carburetta_directive(&tkr_tokens, &token_buf, &cc);
            if (r == TKR_INTERNAL_ERROR) {
              r = EXIT_FAILURE;
              goto cleanup_exit;
            }
            else if (r) {
              have_error = 1;
            }
            break;
          }
        }
        else if (where_are_we == GRAMMAR) {
          switch (tkr_lines.best_match_variant_) {
          case LD_C_PREPROCESSOR:
            re_error_tkr(&tkr_lines, "Error, preprocessor directives do not belong in grammar area");
            have_error = 1;
            break;
          case LD_CARBURETTA_GRAMMAR_SECTION_DELIMETER:
            re_error_tkr(&tkr_lines, "Error, already in grammar area");
            have_error = 1;
            break;
          case LD_CARBURETTA_SCANNER_SECTION_DELIMETER:
            /* Finish up */
            r = process_grammar_tokens(&tkr_tokens, &token_buf, 1, &prds, &prdg, &cc.symtab_);

            where_are_we = default_mode = SCANNER;
            break;
          case LD_CARBURETTA_SECTION_DELIMITER:
            /* Finish up */
            r = process_grammar_tokens(&tkr_tokens, &token_buf, 1, &prds, &prdg, &cc.symtab_);

            where_are_we = EPILOGUE;
            break;
          case LD_REGULAR:
          {
            r = process_grammar_tokens(&tkr_tokens, &token_buf, 0, &prds, &prdg, &cc.symtab_);
            break;
          }
          case LD_CARBURETTA_DIRECTIVE:
            r = process_carburetta_directive(&tkr_tokens, &token_buf, &cc);
            if (r == TKR_INTERNAL_ERROR) {
              r = EXIT_FAILURE;
              goto cleanup_exit;
            }
            else if (r) {
              have_error = 1;
            }
            break;
          }
        }
        else if (where_are_we == SCANNER) {
          switch (tkr_lines.best_match_variant_) {
          case LD_C_PREPROCESSOR:
            re_error_tkr(&tkr_lines, "Error, preprocessor directives do not belong in scanner area");
            have_error = 1;
            break;
          case LD_CARBURETTA_SCANNER_SECTION_DELIMETER:
            re_error_tkr(&tkr_lines, "Error, already in scanner area");
            have_error = 1;
            break;
          case LD_CARBURETTA_GRAMMAR_SECTION_DELIMETER:
            /* Finish up */
            r = process_scanner_tokens(&tkr_tokens, &token_buf, 1, &rxgs, &prdg, &cc.symtab_);

            where_are_we = default_mode = GRAMMAR;
            break;
          case LD_CARBURETTA_SECTION_DELIMITER:
            /* Finish up */
            r = process_scanner_tokens(&tkr_tokens, &token_buf, 1, &rxgs, &prdg, &cc.symtab_);

            where_are_we = EPILOGUE;
            break;
          case LD_REGULAR:
            r = process_scanner_tokens(&tkr_tokens, &token_buf, 0, &rxgs, &prdg, &cc.symtab_);
            break;
          case LD_CARBURETTA_DIRECTIVE:
            r = process_carburetta_directive(&tkr_tokens, &token_buf, &cc);
            if (r == TKR_INTERNAL_ERROR) {
              r = EXIT_FAILURE;
              goto cleanup_exit;
            }
            else if (r) {
              have_error = 1;
            }
            break;
          }
        }
        else /* (where_are_we == EPILOGUE) */ {
          switch (tkr_lines.best_match_variant_) {
          case LD_C_PREPROCESSOR:
            append_part(&epilogue, token_buf.num_original_, token_buf.original_);
            break;
          case LD_CARBURETTA_SCANNER_SECTION_DELIMETER:
          case LD_CARBURETTA_GRAMMAR_SECTION_DELIMETER:
          case LD_CARBURETTA_SECTION_DELIMITER:
            /* Going back to the grammar or scanner, append the epilogue we gathered to the prologue as it is
             * actually inline code. */
          {
            struct part *prologue_head, *prologue_tail, *epilogue_head, *epilogue_tail;
            prologue_tail = prologue;
            prologue_head = prologue ? prologue->next_ : NULL;
            epilogue_tail = epilogue;
            epilogue_head = epilogue ? epilogue->next_ : NULL;
            if (epilogue_head) {
              if (!prologue_head) {
                prologue = epilogue_head;
                epilogue = NULL;
              }
              else {
                epilogue_tail->next_ = prologue_head;
                prologue_tail->next_ = epilogue_head;
                prologue = epilogue_tail;
                epilogue = NULL;
              }
            }
          }
          if (tkr_lines.best_match_variant_ == LD_CARBURETTA_SCANNER_SECTION_DELIMETER) {
            where_are_we = default_mode = SCANNER;
          }
          else if (tkr_lines.best_match_variant_ == LD_CARBURETTA_GRAMMAR_SECTION_DELIMETER) {
            where_are_we = default_mode = GRAMMAR;
          }
          else {
            /* Back to previous or default */
            where_are_we = default_mode;
          }
          break;
          case LD_REGULAR:
            /* Preserve line continuations */
            append_part(&epilogue, token_buf.num_original_, token_buf.original_);
            break;
          case LD_CARBURETTA_DIRECTIVE:
            r = process_carburetta_directive(&tkr_tokens, &token_buf, &cc);
            if (r == TKR_INTERNAL_ERROR) {
              r = EXIT_FAILURE;
              goto cleanup_exit;
            }
            else if (r) {
              have_error = 1;
            }
            break;
          }
        }
      }

      r = tkr_tokenizer_inputx(&tkr_lines, &comment_free_line, 1);
      //r = tkr_tokenizer_input(&tkr_lines, token_buf.translated_, token_buf.num_translated_, 1);
      if ((r != TKR_END_OF_INPUT) && (r != TKR_INTERNAL_ERROR)) {
        re_error(&comment_free_line, "Internal error: lines from a single line are expected to match a single time");
        r = EXIT_FAILURE;
        goto cleanup_exit;
      }

      r = las_input(&line_assembly, buf, num_bytes_read, !num_bytes_read);
    }

  } while (num_bytes_read);

  /* Finished parsing */
  if (prdg.have_errors_) {
    r = EXIT_FAILURE;
    goto cleanup_exit;
  }

  /* Assign types to all symbols */
  struct symbol *sym;

  /* Assign the token type to tokens that don't yet have a type assigned. */
  sym = cc.symtab_.terminals_;
  if (sym) {
    do {
      sym = sym->next_;

      if (!sym->assigned_type_) {
        sym->assigned_type_ = cc.token_assigned_type_;
      }
    } while (sym != cc.symtab_.terminals_);
  }

  /* Ensure we have error and end-of-input tokens */
  if (!cc.error_sym_) {
    struct xlts error_id;
    xlts_init(&error_id);
    r = xlts_append_xlat(&error_id, strlen("error"), "error");
    if (r) {
      re_error_nowhere("Error: no memory");
      r = EXIT_FAILURE;
      goto cleanup_exit;
    }
    int is_new = 0;
    struct symbol *sym = symbol_find_or_add(&cc.symtab_, SYM_TERMINAL, &error_id, &is_new);
    if (!sym) {
      re_error_nowhere("Error: no memory");
      r = EXIT_FAILURE;
      goto cleanup_exit;
    }
    if (!is_new) {
      re_error(&sym->def_, "Error: \"error\" token already in use. Define an error token explicitly using %%error_token or ensure the \"error\" identifier is not used anywhere.");
      r = EXIT_FAILURE;
      goto cleanup_exit;
    }
    cc.error_sym_ = sym;
  }

  if (!cc.input_end_sym_) {
    struct xlts input_end_id;
    xlts_init(&input_end_id);
    r = xlts_append_xlat(&input_end_id, strlen("input-end"), "input-end");
    if (r) {
      re_error_nowhere("Error: no memory");
      r = EXIT_FAILURE;
      goto cleanup_exit;
    }
    int is_new = 0;
    struct symbol *sym = symbol_find_or_add(&cc.symtab_, SYM_TERMINAL, &input_end_id, &is_new);
    if (!sym) {
      re_error_nowhere("Error: no memory");
      r = EXIT_FAILURE;
      goto cleanup_exit;
    }
    if (!is_new) {
      re_error(&sym->def_, "Error: \"error\" token already in use. Define an error token explicitly using %%error_token or ensure the \"error\" identifier is not used anywhere.");
      r = EXIT_FAILURE;
      goto cleanup_exit;
    }
    cc.input_end_sym_ = sym;
  }

  /* Number all symbols */
  int NT_END = 0, RULE_END = 1, GRAMMAR_END = 2;
  int next_ordinal = 3;
  sym = cc.symtab_.terminals_;
  if (sym) {
    do {
      sym = sym->next_;

      if ((sym != cc.error_sym_) && (sym != cc.input_end_sym_)) {
        sym->ordinal_ = next_ordinal++;
      }
    } while (sym != cc.symtab_.terminals_);
  }
  cc.error_sym_->ordinal_ = next_ordinal++;
  cc.input_end_sym_->ordinal_ = next_ordinal++;
  int INPUT_END = cc.input_end_sym_->ordinal_;
  sym = cc.symtab_.non_terminals_;
  if (sym) {
    do {
      sym = sym->next_;

      sym->ordinal_ = next_ordinal++;
    } while (sym != cc.symtab_.non_terminals_);
  }
  int SYNTHETIC_S = next_ordinal++;

  /* Resolve all symbol references in the productions */
  size_t prod_idx;
  for (prod_idx = 0; prod_idx < prdg.num_productions_; ++prod_idx) {
    struct prd_production *prod = prdg.productions_ + prod_idx;
    if (!prod->nt_.id_.num_translated_) {
      prdg.have_errors_ = 1;
      continue;
    }
    struct symbol *sym = symbol_find(&cc.symtab_, prod->nt_.id_.translated_);
    if (!sym || (sym->st_ != SYM_NONTERMINAL)) {
      re_error(&prod->nt_.id_, "Error, symbol \"%s\" not declared as %%nt", prod->nt_.id_.translated_);
      prdg.have_errors_ = 1;
    }
    else {
      prod->nt_.sym_ = sym;
    }
    size_t sym_idx;
    for (sym_idx = 0; sym_idx < prod->num_syms_; ++sym_idx) {
      struct prd_production_sym *prod_sym = prod->syms_ + sym_idx;
      if (!prod_sym->id_.num_translated_) {
        prdg.have_errors_ = 1;
        continue;
      }
      sym = symbol_find(&cc.symtab_, prod_sym->id_.translated_);
      if (!sym) {
        re_error(&prod_sym->id_, "Error, symbol \"%s\" was not declared as %%nt or %%token", prod_sym->id_.translated_);
        prdg.have_errors_ = 1;
        continue;
      }
      prod_sym->sym_ = sym;
    }
  }

  /* Resolve all symbol references in the patterns */
  size_t pat_idx;
  for (pat_idx = 0; pat_idx < prdg.num_patterns_; ++pat_idx) {
    struct prd_pattern *pat = prdg.patterns_ + pat_idx;
    if (!pat->term_.id_.num_translated_) {
      /* Not resolving to a terminal.. but pattern will still match its action so keep it around */
      continue;
    }
    struct symbol *sym = symbol_find(&cc.symtab_, pat->term_.id_.translated_);
    if (!sym) {
      re_error(&pat->term_.id_, "Error, symbol \"%s\" not declared as %%token", pat->term_.id_.translated_);
      prdg.have_errors_ = 1;
    }
    else if (sym->st_ != SYM_TERMINAL) {
      re_error(&pat->term_.id_, "Error, pattern symbol \"%s\" must be declared as %%token, not as %%nt", pat->term_.id_.translated_);
      prdg.have_errors_ = 1;
    }
    else {
      pat->term_.sym_ = sym;
    }
  }

  if (prdg.have_errors_) {
    r = EXIT_FAILURE;
    goto cleanup_exit;
  }

  /* Take the production "syntax tree" and transcribe it into the form used for consumption by lalr.c */
  r = gt_transcribe_grammar(&gt, prdg.num_productions_, prdg.productions_, RULE_END, GRAMMAR_END);
  if (r) {
    r = EXIT_FAILURE;
    goto cleanup_exit;
  }


  /* Resolve all conflict resolutions */
  struct conflict_resolution *confres;
  confres = cc.conflict_resolutions_;
  if (confres) {
    do {
      confres = confres->next_;

      struct prd_production *prods[] = {
        &confres->prefer_prod_,
        &confres->over_prod_
      };

      size_t matches[sizeof(prods) / sizeof(*prods)];
      
      size_t n;
      for (n = 0; n < sizeof(prods) / sizeof(*prods); ++n) {
        int had_failed_lookup = 0;
        struct prd_production *prod = prods[n];

        prod->nt_.sym_ = symbol_find(&cc.symtab_, prod->nt_.id_.translated_);
        if (!prod->nt_.sym_ || (prod->nt_.sym_->st_ != SYM_NONTERMINAL)) {
          re_error(&prod->nt_.id_, "Error, symbol \"%s\" was not declared as %%nt", prod->nt_.id_.translated_);
          prdg.have_errors_ = 1;
          had_failed_lookup = 1;
        }
        size_t sym_idx;
        for (sym_idx = 0; sym_idx < prod->num_syms_; ++sym_idx) {
          struct prd_production_sym *ps = prod->syms_ + sym_idx;
          ps->sym_ = symbol_find(&cc.symtab_, ps->id_.translated_);
          if (!ps->sym_) {
            re_error(&prod->nt_.id_, "Error, symbol \"%s\" was not declared as %%nt or %%token", ps->id_.translated_);
            prdg.have_errors_ = 1;
            had_failed_lookup = 1;
          }
        }

        if (had_failed_lookup) {
          continue;
        }
        /* Find a match; if possible */
        size_t prod_idx;
        for (prod_idx = 0; prod_idx < prdg.num_productions_; ++prod_idx) {
          struct prd_production *gp = prdg.productions_ + prod_idx;
          if (gp->nt_.sym_ != prod->nt_.sym_) {
            continue;
          }
          if (gp->num_syms_ != prod->num_syms_) {
            continue;
          }
          for (sym_idx = 0; sym_idx < prod->num_syms_; ++sym_idx) {
            if (gp->syms_[sym_idx].sym_ != prod->syms_[sym_idx].sym_) {
              break;
            }
          }
          if (sym_idx != prod->num_syms_) {
            continue;
          }
          /* Production matches */
          break;
        }
        if (prod_idx != prdg.num_productions_) {
          /* Production prod_idx matches */
          matches[n] = prod_idx;
        }
        else {
          re_error(&prod->nt_.id_, "Error, no matching production found");
          prdg.have_errors_ = 1;
        }
      }

      /* Productions are 1 based for LALR (as production 0 is the synthetic S reduction) */
      r = lr_add_conflict_resolution(&lalr, 1 + (int)matches[0], confres->prefer_prod_place_, 1 + (int)matches[1], confres->over_prod_place_);
      if (r) {
        re_error_nowhere("Error, no memory");
        r = EXIT_FAILURE;
        goto cleanup_exit;
      }

    } while (confres != cc.conflict_resolutions_);
  }

  if (prdg.have_errors_) {
    r = EXIT_FAILURE;
    goto cleanup_exit;
  }

  r = gt_generate_lalr(&gt, &lalr, RULE_END, GRAMMAR_END, INPUT_END, SYNTHETIC_S);
  if (r == GT_CONFLICTS) {
    struct lr_conflict_pair *cp;
    for (cp = lalr.conflicts_; cp; cp = cp->chain_) {
      /* NOTE: lalr parser inserts a rule 0, so consider all conflicts 1-based. */
      struct {
        int production, position;
      } conflict[2] = {
        {cp->production_a_ - 1, cp->position_a_},
        {cp->production_b_ - 1, cp->position_b_}
      };
      size_t n;
      const char *a;
      const char *b;
      if (conflict[0].position == (int)prdg.productions_[conflict[0].production].num_syms_) {
        /* A = reduce */
        a = "reduce";
      }
      else {
        a = "shift";
      }
      if (conflict[1].position == (int)prdg.productions_[conflict[1].production].num_syms_) {
        /* B = reduce */
        b = "reduce";
      }
      else {
        b = "shift";
      }

      re_error_nowhere("Error, %s/%s conflict found:", a, b);
      for (n = 0; n < sizeof(conflict) / sizeof(*conflict); ++n) {
        int prod_idx, pos_idx;
        prod_idx = conflict[n].production;
        pos_idx = conflict[n].position;
        struct prd_production *gp = prdg.productions_ + prod_idx;
        size_t sym_idx;
        size_t msg_size = 0;
        msg_size = gp->nt_.id_.num_translated_ + 2 /* ": " */;
        for (sym_idx = 0; sym_idx < gp->num_syms_; ++sym_idx) {
          msg_size += gp->syms_[sym_idx].id_.num_translated_ + 1 /* " " */;
        }
        msg_size += 2 /* " *" */;

        if (sym_idx == (size_t)pos_idx) {
          msg_size += strlen(" (reduce)");
        }
        else {
          msg_size += strlen(" (shift)");
        }

        msg_size++ /* '\0' */;

        char *msg = malloc(msg_size);
        if (!msg) {
          re_error_nowhere("Error, no memory");
          r = EXIT_FAILURE;
          goto cleanup_exit;
        }

        msg[0] = '\0';
        strcat(msg, gp->nt_.id_.translated_);
        strcat(msg, ":");

        for (sym_idx = 0; sym_idx < gp->num_syms_; ++sym_idx) {
          if (sym_idx == (size_t)pos_idx) strcat(msg, " *");
          strcat(msg, " ");
          strcat(msg, gp->syms_[sym_idx].id_.translated_);
        }
        if (sym_idx == (size_t)pos_idx) {
          strcat(msg, " * (reduce)");
        }
        else {
          strcat(msg, " (shift)");
        }

        re_error(&gp->nt_.id_, msg);
        free(msg);
      }
    }
    re_error_nowhere("(Use %%prefer %%over directives to force resolution of conflicts)");
  }
  if (r) {
    r = EXIT_FAILURE;
    goto cleanup_exit;
  }

  if (prdg.num_patterns_) {
    /* Build DFA scantable */
    struct sc_scan_rule *scanner_rules = NULL;
    if (prdg.num_patterns_ > (SIZE_MAX / sizeof(struct sc_scan_rule))) {
      re_error_nowhere("Internal error, overflow");
      r = EXIT_FAILURE;
      goto cleanup_exit;
    }
    scanner_rules = (struct sc_scan_rule *)malloc(sizeof(struct sc_scan_rule) * prdg.num_patterns_);
    if (!scanner_rules) {
      re_error_nowhere("Internal error, no memory");
      r = EXIT_FAILURE;
      goto cleanup_exit;
    }
    size_t n;
    for (n = 0; n < prdg.num_patterns_; ++n) {
      struct prd_pattern *pat = prdg.patterns_ + n;
      struct sc_scan_rule *sr = scanner_rules + n;
      /* Set the action to the index of the pattern, set the variant to the ordinal of the symbol..
       * .. if we have such a symbol.
       * This is mostly for debugging, resolution of the symbol occurs on a per-action basis. */
      sr->regexp = pat->regex_;
      sr->action.action = n + 1;
      sr->action.variant = pat->term_.sym_ ? pat->term_.sym_->ordinal_ : 0;
    }
    r = sc_scanner_compile(&scantable, 0, prdg.num_patterns_, scanner_rules);
    free(scanner_rules);
    if (r) {
      /* If this fails, it means the grammar we used for parsing the regular expression
       * in regex_grammar.cnd mismatched the grammar used in nfa.c for *actually* parsing
       * the regular expression and constructing an NFA out of it. */
      fprintf(stderr, "Internal error, failed to compile pattern scantable (%d)\n", r);
      r = EXIT_FAILURE;
      goto cleanup_exit;
    }
  }

  FILE *outfp = NULL;

  if (generate_cfile) {
    if (cc.c_output_filename_) {
      outfp = fopen(cc.c_output_filename_, "wb");
      if (!outfp) {
        int err = errno;
        re_error_nowhere("Failed to open file \"%s\" for writing: %s", cc.c_output_filename_, strerror(err));
        r = EXIT_FAILURE;
        goto cleanup_exit;
      }
    }
    else {
      outfp = stdout;
    }

    struct part *pt;
    pt = prologue;
    if (pt) {
      do {
        pt = pt->next_;

        size_t written = fwrite(pt->chars_, 1, pt->num_chars_, outfp);
        if (written != pt->num_chars_) {
          int err = errno;
          re_error_nowhere("Failed to write to \"%s\": %s", cc.c_output_filename_, strerror(err));
          r = EXIT_FAILURE;
          goto cleanup_exit;
        }

      } while (pt != prologue);
    }

    /* Start generating files.. */

    if (cc.h_output_filename_) {
      /* First we develop the "include_guard" - this is used by the header file as the "#ifndef HEADER_INCLUDED"
       * header duplicate inclusion guard, and used by the C file to detect whether the header has been included
       * (and the declarations already made.) */
      const char *carburetta = "CARB_";
      size_t carburetta_len = strlen(carburetta);
      size_t prefix_len = strlen(cc_PREFIX(&cc));
      size_t header_filename_len = strlen(cc.h_output_filename_);
      const char *included = "_INCLUDED";
      size_t included_len = strlen(included);
      size_t include_guard_size = carburetta_len + prefix_len + header_filename_len + included_len + 1;
      cc.include_guard_ = (char *)malloc(include_guard_size);
      cc.include_guard_[0] = '\0';
      strcat(cc.include_guard_, carburetta);
      strcat(cc.include_guard_, cc_PREFIX(&cc));
      size_t n;
      char *p = cc.include_guard_ + carburetta_len + prefix_len;
      for (n = 0; n < header_filename_len; ++n) {
        char c = cc.h_output_filename_[n];
        if ((c >= 'a') && (c <= 'z')) {
          c = c - 'a' + 'A';
        }
        else if ((c == '.') || (c == '-') || (c == ' ')) {
          c = '_';
        }
        else if (((c >= 'A') && (c <= 'Z')) ||
          ((c >= '0') && (c <= '9')) ||
          (c == '_')) {
          /* accept these... */
        }
        else {
          /* and skip everything else.. */
          continue;
        }
        *p++ = c;
      }
      *p = '\0';
      strcat(p, included);
    }
    else {
      cc.include_guard_ = NULL;
    }

    r = emit_c_file(outfp, &cc, &prdg, &scantable, &lalr);
    if (r) goto cleanup_exit;

    pt = epilogue;
    if (pt) {
      do {
        pt = pt->next_;

        size_t written = fwrite(pt->chars_, 1, pt->num_chars_, outfp);
        if (written != pt->num_chars_) {
          int err = errno;
          re_error_nowhere("Failed to write to \"%s\": %s", cc.c_output_filename_, strerror(err));
          r = EXIT_FAILURE;
          goto cleanup_exit;
        }

      } while (pt != epilogue);
    }

    if (outfp != stdout) {
      fclose(outfp);
    }
  } /* generate_cfile */

  if (generate_hfile) {
    if (cc.h_output_filename_) {
      outfp = fopen(cc.h_output_filename_, "wb");
      if (!outfp) {
        int err = errno;
        re_error_nowhere("Error, failed to open file \"%s\" for writing: %s", cc.h_output_filename_, strerror(err));
        r = EXIT_FAILURE;
        goto cleanup_exit;
      }
    }
    else {
      re_error_nowhere("Error, generating header file requires output filename");
      r = EXIT_FAILURE;
      goto cleanup_exit;
    }

    r = emit_h_file(outfp, &cc, &prdg);
    if (r) goto cleanup_exit;

    if (outfp != stdout) {
      fclose(outfp);
    }
  }


  r = EXIT_SUCCESS;
cleanup_exit:

  xlts_cleanup(&comment_free_line);

  xlts_cleanup(&token_buf);

  lr_cleanup(&lalr);

  sc_scanner_cleanup(&scantable);

  gt_grammar_table_cleanup(&gt);

  carburetta_context_cleanup(&cc);

  prd_grammar_cleanup(&prdg);

  rxg_stack_cleanup(&rxgs);

  prd_stack_cleanup(&prds);

  tkr_tokenizer_cleanup(&tkr_tokens);

  tkr_tokenizer_cleanup(&tkr_lines);

  las_cleanup_line_assembly(&line_assembly);

  dct_cleanup();
  las_cleanup();
  tok_cleanup();
  ldl_cleanup();

  if (input_filename) free(input_filename);

  return r;
}
