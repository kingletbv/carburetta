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

#ifndef CARBURETTA_CONTEXT_H_INCLUDED
#define CARBURETTA_CONTEXT_H_INCLUDED
#include "carburetta_context.h"
#endif

#ifndef PARSE_INPUT_H_INCLUDED
#define PARSE_INPUT_H_INCLUDED
#include "parse_input.h"
#endif

static int pi_process_carburetta_directive(struct tkr_tokenizer *tkr_tokens, struct xlts *directive_line_match, struct carburetta_context *cc) {
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
    PCD_RAII_CONSTRUCTOR_DIRECTIVE,
    PCD_DESTRUCTOR_DIRECTIVE,
    PCD_TOKEN_ACTION_DIRECTIVE,
    PCD_PREFIX_DIRECTIVE,
    PCD_TOKEN_PREFIX_DIRECTIVE,
    PCD_PARAMS_DIRECTIVE,
    PCD_LOCALS_DIRECTIVE,
    PCD_ON_FINISH_DIRECTIVE,
    PCD_ON_SYNTAX_ERROR_DIRECTIVE,
    PCD_ON_LEXICAL_ERROR_DIRECTIVE,
    PCD_ON_ALLOC_ERROR_DIRECTIVE,
    PCD_ON_INTERNAL_ERROR_DIRECTIVE,
    PCD_ON_NEXT_TOKEN_DIRECTIVE,
    PCD_ON_FEED_ME_DIRECTIVE,
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
            (directive == PCD_RAII_CONSTRUCTOR_DIRECTIVE) ||
            (directive == PCD_DESTRUCTOR_DIRECTIVE) ||
            (directive == PCD_PARAMS_DIRECTIVE) ||
            (directive == PCD_LOCALS_DIRECTIVE) ||
            (directive == PCD_ON_FINISH_DIRECTIVE) ||
            (directive == PCD_ON_LEXICAL_ERROR_DIRECTIVE) ||
            (directive == PCD_ON_SYNTAX_ERROR_DIRECTIVE) ||
            (directive == PCD_ON_ALLOC_ERROR_DIRECTIVE) ||
            (directive == PCD_ON_INTERNAL_ERROR_DIRECTIVE) ||
            (directive == PCD_ON_NEXT_TOKEN_DIRECTIVE) ||
            (directive == PCD_ON_FEED_ME_DIRECTIVE)) {
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
            else if (!strcmp("raii_constructor", tkr_str(tkr_tokens))) {
              directive = PCD_RAII_CONSTRUCTOR_DIRECTIVE;
              if (!cc->most_recent_typestr_) {
                re_error_tkr(tkr_tokens, "%%raii_constructor must follow %%token_type or %%type directive");
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
                re_error_tkr(tkr_tokens, "%%token_action must follow %%token_type or %%type directive");
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
            else if (!strcmp("on_finish", tkr_str(tkr_tokens))) {
              directive = PCD_ON_FINISH_DIRECTIVE;
            }
            else if (!strcmp("on_syntax_error", tkr_str(tkr_tokens))) {
              directive = PCD_ON_SYNTAX_ERROR_DIRECTIVE;
            }
            else if (!strcmp("on_lexical_error", tkr_str(tkr_tokens))) {
              directive = PCD_ON_LEXICAL_ERROR_DIRECTIVE;
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
            else if (!strcmp("on_feed_me", tkr_str(tkr_tokens))) {
              directive = PCD_ON_FEED_ME_DIRECTIVE;
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
          else if ((directive == PCD_CONSTRUCTOR_DIRECTIVE) ||
                   (directive == PCD_RAII_CONSTRUCTOR_DIRECTIVE) ||
                   (directive == PCD_DESTRUCTOR_DIRECTIVE) ||
                   (directive == PCD_TOKEN_ACTION_DIRECTIVE)) {
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
                   (directive == PCD_ON_FINISH_DIRECTIVE) || 
                   (directive == PCD_ON_SYNTAX_ERROR_DIRECTIVE) || 
                   (directive == PCD_ON_LEXICAL_ERROR_DIRECTIVE) ||
                   (directive == PCD_ON_ALLOC_ERROR_DIRECTIVE) || 
                   (directive == PCD_ON_INTERNAL_ERROR_DIRECTIVE) || 
                   (directive == PCD_ON_NEXT_TOKEN_DIRECTIVE) ||
                   (directive == PCD_ON_FEED_ME_DIRECTIVE)) {
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
      (directive == PCD_RAII_CONSTRUCTOR_DIRECTIVE) ||
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
    struct typestr *nt_ts = typestr_add(&cc->tstab_, &dir_snippet);
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
    nt_ts->is_symbol_type_ = 1;
  }

  if (directive == PCD_TOKEN_TYPE_DIRECTIVE) {
    struct typestr *token_ts = typestr_add(&cc->tstab_, &cc->token_type_);
    if (!token_ts) {
      r = TKR_INTERNAL_ERROR;
      goto cleanup_exit;
    }
    cc->token_assigned_type_ = token_ts;
    cc->most_recent_typestr_ = token_ts;
    cc->have_typed_symbols_ = 1;
    token_ts->is_symbol_type_ = 1;
  }

  if (directive == PCD_COMMON_TYPE_DIRECTIVE) {
    struct typestr *common_ts = typestr_add(&cc->tstab_, &cc->common_data_type_);
    if (!common_ts) {
      r = TKR_INTERNAL_ERROR;
      goto cleanup_exit;
    }
    cc->common_data_assigned_type_ = common_ts;
    cc->most_recent_typestr_ = common_ts;
  }

  if (directive == PCD_CONSTRUCTOR_DIRECTIVE) {
    cc->most_recent_typestr_->is_raii_constructor_ = 0;
    snippet_clear(&cc->most_recent_typestr_->constructor_snippet_);
    r = snippet_append_snippet(&cc->most_recent_typestr_->constructor_snippet_, &dir_snippet);
    if (r) goto cleanup_exit;
  }

  if (directive == PCD_RAII_CONSTRUCTOR_DIRECTIVE) {
    cc->most_recent_typestr_->is_raii_constructor_ = 1;
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

  if (directive == PCD_ON_FINISH_DIRECTIVE) {
    snippet_clear(&cc->on_finish_snippet_);
    r = snippet_append_snippet(&cc->on_finish_snippet_, &dir_snippet);
    if (r) goto cleanup_exit;
  }

  if (directive == PCD_ON_SYNTAX_ERROR_DIRECTIVE) {
    snippet_clear(&cc->on_syntax_error_snippet_);
    r = snippet_append_snippet(&cc->on_syntax_error_snippet_, &dir_snippet);
    if (r) goto cleanup_exit;
  }

  if (directive == PCD_ON_LEXICAL_ERROR_DIRECTIVE) {
    snippet_clear(&cc->on_syntax_error_snippet_);
    r = snippet_append_snippet(&cc->on_lexical_error_snippet_, &dir_snippet);
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

  if (directive == PCD_ON_FEED_ME_DIRECTIVE) {
    snippet_clear(&cc->on_feed_me_snippet_);
    r = snippet_append_snippet(&cc->on_feed_me_snippet_, &dir_snippet);
    if (r) goto cleanup_exit;
  }
  
  r = 0;

cleanup_exit:
  tkr_tokenizer_reset(tkr_tokens);
  snippet_cleanup(&dir_snippet);
  if (typed_symbols) free(typed_symbols);

  return r;
}


static int pi_process_grammar_tokens(struct tkr_tokenizer *tkr_tokens, struct xlts *input_line, int is_final, struct prd_stack *prds, struct prd_grammar *g, struct symbol_table *st) {
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

static int pi_process_scanner_tokens(struct tkr_tokenizer *tkr_tokens, struct xlts *input_line, int is_final, struct rxg_stack *rxgs, struct prd_grammar *g, struct symbol_table *st) {
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


int pi_parse_input(FILE *fp, const char *input_filename, struct carburetta_context *cc, struct prd_grammar *prdg) {
  int r;

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

  struct xlts token_buf;
  xlts_init(&token_buf);

  struct xlts comment_free_line;
  xlts_init(&comment_free_line);

  r = prd_stack_reset(&prds);
  if (r) {
    re_error_nowhere("Internal error, failed to reset grammar parsing stack");
    r = 1;
    goto cleanup_exit;
  }

  r = rxg_stack_reset(&rxgs);
  if (r) {
    re_error_nowhere("Internal error, failed to reset scanner parsing stack");
    r = 1;
    goto cleanup_exit;
  }

  las_set_filename(&line_assembly, input_filename ? input_filename : "(stdin)");

  enum {
    UNDEFINED,
    PROLOGUE,
    GRAMMAR,
    SCANNER,
    EPILOGUE
  } where_are_we, default_mode;
  where_are_we = PROLOGUE;
  default_mode = UNDEFINED;

  size_t num_bytes_read;
  static char buf[2400];

  int have_error;
  have_error = 0;

  do {
    num_bytes_read = fread(buf, sizeof(*buf), sizeof(buf) / sizeof(*buf), fp);

    r = las_input(&line_assembly, buf, num_bytes_read, !num_bytes_read);
    while ((r != LAS_END_OF_INPUT) && (r != LAS_FEED_ME)) {
      /* Cannot modify (shift) the line_assembly's mlc_buf line buffer, so copy it over to a work area (token_buf) */
      xlts_reset(&token_buf);
      r = xlts_append(&token_buf, &line_assembly.mlc_buf_);
      if (r) {
        r = 1;
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
        r = 1;
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
            parts_append(&cc->prologue_, token_buf.num_original_, token_buf.original_);
            break;
          case LD_CARBURETTA_SCANNER_SECTION_DELIMETER:
            where_are_we = default_mode = SCANNER;
            break;
          case LD_CARBURETTA_GRAMMAR_SECTION_DELIMETER:
            where_are_we = default_mode = GRAMMAR;
            break;
          case LD_CARBURETTA_SECTION_DELIMITER:
            if (default_mode == UNDEFINED) {
              re_error_tkr(&tkr_lines, "Error, no section type to return to");
              r = 1;
              goto cleanup_exit;
            }
            where_are_we = default_mode;
            break;
          case LD_REGULAR:
            /* Preserve line continuations */
            parts_append(&cc->prologue_, token_buf.num_original_, token_buf.original_);
            break;
          case LD_CARBURETTA_DIRECTIVE:
            r = pi_process_carburetta_directive(&tkr_tokens, &token_buf, cc);
            if (r == TKR_INTERNAL_ERROR) {
              r = 1;
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
            r = pi_process_grammar_tokens(&tkr_tokens, &token_buf, 1, &prds, prdg, &cc->symtab_);

            where_are_we = default_mode = SCANNER;
            break;
          case LD_CARBURETTA_SECTION_DELIMITER:
            /* Finish up */
            r = pi_process_grammar_tokens(&tkr_tokens, &token_buf, 1, &prds, prdg, &cc->symtab_);

            where_are_we = EPILOGUE;
            break;
          case LD_REGULAR:
          {
            r = pi_process_grammar_tokens(&tkr_tokens, &token_buf, 0, &prds, prdg, &cc->symtab_);
            break;
          }
          case LD_CARBURETTA_DIRECTIVE:
            r = pi_process_carburetta_directive(&tkr_tokens, &token_buf, cc);
            if (r == TKR_INTERNAL_ERROR) {
              r = 1;
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
            r = pi_process_scanner_tokens(&tkr_tokens, &token_buf, 1, &rxgs, prdg, &cc->symtab_);

            where_are_we = default_mode = GRAMMAR;
            break;
          case LD_CARBURETTA_SECTION_DELIMITER:
            /* Finish up */
            r = pi_process_scanner_tokens(&tkr_tokens, &token_buf, 1, &rxgs, prdg, &cc->symtab_);

            where_are_we = EPILOGUE;
            break;
          case LD_REGULAR:
            r = pi_process_scanner_tokens(&tkr_tokens, &token_buf, 0, &rxgs, prdg, &cc->symtab_);
            break;
          case LD_CARBURETTA_DIRECTIVE:
            r = pi_process_carburetta_directive(&tkr_tokens, &token_buf, cc);
            if (r == TKR_INTERNAL_ERROR) {
              r = 1;
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
            parts_append(&cc->epilogue_, token_buf.num_original_, token_buf.original_);
            break;
          case LD_CARBURETTA_SCANNER_SECTION_DELIMETER:
          case LD_CARBURETTA_GRAMMAR_SECTION_DELIMETER:
          case LD_CARBURETTA_SECTION_DELIMITER:
            /* Going back to the grammar or scanner, append the epilogue we gathered to the prologue as it is
             * actually inline code. */
          {
            struct part *prologue_head, *prologue_tail, *epilogue_head, *epilogue_tail;
            prologue_tail = cc->prologue_;
            prologue_head = cc->prologue_ ? cc->prologue_->next_ : NULL;
            epilogue_tail = cc->epilogue_;
            epilogue_head = cc->epilogue_ ? cc->epilogue_->next_ : NULL;
            if (epilogue_head) {
              if (!prologue_head) {
                cc->prologue_ = epilogue_head;
                cc->epilogue_ = NULL;
              }
              else {
                epilogue_tail->next_ = prologue_head;
                prologue_tail->next_ = epilogue_head;
                cc->prologue_ = epilogue_tail;
                cc->epilogue_ = NULL;
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
            parts_append(&cc->epilogue_, token_buf.num_original_, token_buf.original_);
            break;
          case LD_CARBURETTA_DIRECTIVE:
            r = pi_process_carburetta_directive(&tkr_tokens, &token_buf, cc);
            if (r == TKR_INTERNAL_ERROR) {
              r = 1;
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
        r = 1;
        goto cleanup_exit;
      }

      r = las_input(&line_assembly, buf, num_bytes_read, !num_bytes_read);
    }

  } while (num_bytes_read);

  /* Finished parsing */
  if (prdg->have_errors_) {
    r = 1;
    goto cleanup_exit;
  }

  r = 0;
cleanup_exit:
  xlts_cleanup(&comment_free_line);

  xlts_cleanup(&token_buf);

  rxg_stack_cleanup(&rxgs);

  prd_stack_cleanup(&prds);

  tkr_tokenizer_cleanup(&tkr_tokens);

  tkr_tokenizer_cleanup(&tkr_lines);

  las_cleanup_line_assembly(&line_assembly);

  return r;
}
