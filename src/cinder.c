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

#ifndef MUL_H_INCLUDED
#define MUL_H_INCLUDED
#include "mul.h"
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

struct conflict_resolution {
  struct conflict_resolution *next_;

  struct prd_production prefer_prod_;
  int prefer_prod_place_;
  struct prd_production over_prod_;
  int over_prod_place_;
};

struct cinder_context {
  struct snippet token_type_;
  struct snippet common_data_type_;
  struct symbol_table symtab_;
  struct typestr_table tstab_;
  struct typestr *most_recent_typestr_;
  struct typestr *token_assigned_type_;
  struct typestr *common_data_assigned_type_;
  struct snippet token_action_snippet_;
  struct xlts prefix_;
  char *prefix_uppercase_;
  struct xlts token_prefix_;
  char *token_prefix_uppercase_;
  struct snippet params_snippet_;
  struct snippet locals_snippet_;
  struct snippet on_success_snippet_;
  struct snippet on_syntax_error_snippet_;
  struct snippet on_alloc_error_snippet_;
  struct snippet on_internal_error_snippet_;
  struct snippet on_next_token_snippet_;
  struct symbol *input_end_sym_;
  struct symbol *error_sym_;
  struct prd_production prefer_prod_;
  int prefer_prod_place_;
  struct prd_production over_prod_;
  int over_prod_place_;
  struct conflict_resolution *conflict_resolutions_;
  int have_typed_symbols_ :1;
};

void conflict_resolution_init(struct conflict_resolution *cr) {
  prd_prod_init(&cr->prefer_prod_);
  cr->prefer_prod_place_ = -1;
  prd_prod_init(&cr->over_prod_);
  cr->over_prod_place_ = -1;
}

void conflict_resolution_cleanup(struct conflict_resolution *cr) {
  prd_prod_cleanup(&cr->prefer_prod_);
  prd_prod_cleanup(&cr->over_prod_);
}

void cinder_context_init(struct cinder_context *cc) {
  snippet_init(&cc->token_type_);
  snippet_init(&cc->common_data_type_);
  symbol_table_init(&cc->symtab_);
  typestr_table_init(&cc->tstab_);
  cc->most_recent_typestr_ = NULL;
  cc->token_assigned_type_ = NULL;
  cc->common_data_assigned_type_ = NULL;
  snippet_init(&cc->token_action_snippet_);
  xlts_init(&cc->prefix_);
  cc->prefix_uppercase_ = NULL;
  xlts_init(&cc->token_prefix_);
  cc->token_prefix_uppercase_ = NULL;
  snippet_init(&cc->params_snippet_);
  snippet_init(&cc->locals_snippet_);
  snippet_init(&cc->on_success_snippet_);
  snippet_init(&cc->on_syntax_error_snippet_);
  snippet_init(&cc->on_alloc_error_snippet_);
  snippet_init(&cc->on_internal_error_snippet_);
  snippet_init(&cc->on_next_token_snippet_);
  cc->input_end_sym_ = NULL;
  cc->error_sym_ = NULL;
  prd_prod_init(&cc->prefer_prod_);
  cc->prefer_prod_place_ = -1;
  prd_prod_init(&cc->over_prod_);
  cc->over_prod_place_ = -1;
  cc->conflict_resolutions_ = NULL;
  cc->have_typed_symbols_ = 0;
}

void cinder_context_cleanup(struct cinder_context *cc) {
  snippet_cleanup(&cc->token_type_);
  snippet_cleanup(&cc->common_data_type_);
  symbol_table_cleanup(&cc->symtab_);
  typestr_table_cleanup(&cc->tstab_);
  snippet_cleanup(&cc->token_action_snippet_);
  xlts_cleanup(&cc->prefix_);
  if (cc->prefix_uppercase_) free(cc->prefix_uppercase_);
  xlts_cleanup(&cc->token_prefix_);
  if (cc->token_prefix_uppercase_) {
    free(cc->token_prefix_uppercase_);
  }
  snippet_cleanup(&cc->params_snippet_);
  snippet_cleanup(&cc->locals_snippet_);
  snippet_cleanup(&cc->on_success_snippet_);
  snippet_cleanup(&cc->on_syntax_error_snippet_);
  snippet_cleanup(&cc->on_alloc_error_snippet_);
  snippet_cleanup(&cc->on_internal_error_snippet_);
  snippet_cleanup(&cc->on_next_token_snippet_);
  prd_prod_cleanup(&cc->prefer_prod_);
  prd_prod_cleanup(&cc->over_prod_);
  struct conflict_resolution *cr, *next;
  cr = cc->conflict_resolutions_;
  if (cr) {
    next = cr->next_;
    do {
      cr = next;
      next = cr->next_;

      conflict_resolution_cleanup(cr);
      free(cr);

    } while (cr != cc->conflict_resolutions_);
  }
}

static int process_cinder_directive(struct tkr_tokenizer *tkr_tokens, struct xlts *directive_line_match, struct cinder_context *cc) {
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
    PCD_NT_TYPE_DIRECTIVE,
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
            /* leading percent sign indicates cinder directive, eat it */
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
              directive = PCD_NT_TYPE_DIRECTIVE;
            }
            else if (!strcmp("common_type", tkr_str(tkr_tokens))) {
              directive = PCD_COMMON_TYPE_DIRECTIVE;
            }
            else if (!strcmp("constructor", tkr_str(tkr_tokens))) {
              directive = PCD_CONSTRUCTOR_DIRECTIVE;
              if (!cc->most_recent_typestr_) {
                re_error_tkr(tkr_tokens, "%%constructor must follow %%token_type or %%nt_type directive");
              }
            }
            else if (!strcmp("destructor", tkr_str(tkr_tokens))) {
              directive = PCD_DESTRUCTOR_DIRECTIVE;
              if (!cc->most_recent_typestr_) {
                re_error_tkr(tkr_tokens, "%%destructor must follow %%token_type or %%nt_type directive");
              }
            }
            else if (!strcmp("token_action", tkr_str(tkr_tokens))) {
              directive = PCD_TOKEN_ACTION_DIRECTIVE;
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
          else if (directive == PCD_NT_TYPE_DIRECTIVE) {
            if (!ate_colon_seperator) {
              if (tkr_tokens->best_match_action_ != TOK_COLON) {
                if (tkr_tokens->best_match_action_ == TOK_IDENT) {
                  struct symbol *sym = symbol_find(st, tkr_str(tkr_tokens));
                  if (!sym) {
                    re_error_tkr(tkr_tokens, "Error: \"%s\" is not a non-terminal", tkr_str(tkr_tokens));
                    r = TKR_SYNTAX_ERROR;
                    goto cleanup_exit;
                  }
                  else if (sym->st_ != SYM_NONTERMINAL) {
                    re_error_tkr(tkr_tokens, "Error: \"%s\" is not a non-terminal but a token.", tkr_str(tkr_tokens));
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

  if ((directive == PCD_NT_TYPE_DIRECTIVE) ||
      (directive == PCD_TOKEN_TYPE_DIRECTIVE) ||
      (directive == PCD_COMMON_TYPE_DIRECTIVE)) {
    /* Trim all whitespace off, pop any "$$" special identifiers at the tail end */
    struct snippet *snip;
    if (directive == PCD_NT_TYPE_DIRECTIVE) {
      snip = &dir_snippet;
    }
    else if (directive == PCD_TOKEN_TYPE_DIRECTIVE) {
      snip = &cc->token_type_;
    }
    else if (directive == PCD_COMMON_TYPE_DIRECTIVE) {
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

  if (directive == PCD_NT_TYPE_DIRECTIVE) {
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
    snippet_clear(&cc->token_action_snippet_);
    r = snippet_append_snippet(&cc->token_action_snippet_, &dir_snippet);
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

static const char *cc_prefix(struct cinder_context *cc) {
  if (cc->prefix_.num_translated_) return cc->prefix_.translated_;
  return "cinder_";
}

static const char *cc_PREFIX(struct cinder_context *cc) {
  if (cc->prefix_uppercase_) return cc->prefix_uppercase_;
  return "CINDER_";
}

static const char *cc_TOKEN_PREFIX(struct cinder_context *cc) {
  if (cc->token_prefix_uppercase_) return cc->token_prefix_uppercase_;
  return cc_PREFIX(cc);
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


struct snippet_emission {
  struct snippet *code_;

  enum dest_type {
    SEDT_NONE,
    SEDT_FMT,
    SEDT_FMT_PREFIX,
    SEDT_FMT_DATATYPE_ORDINAL,
    SEDT_FMT_PREFIX_DATATYPE_ORDINAL,
    SEDT_FMT_PREFIX_TYPESTR_ORDINAL,
    SEDT_FMT_TYPESTR_ORDINAL
  } dest_type_;
  const char *dest_fmt_;
  struct prd_production_sym *dest_;
  struct typestr *dest_typestr_;

  enum sym_type {
    SEST_NONE,
    SEST_FMT_INDEX_ORDINAL
  } sym_type_;
  const char *sym_fmt_;
  struct prd_production *prod_;

  enum common_type {
    SECT_NONE,
    SECT_FMT_PREFIX,
    SECT_FMT
  } common_type_;
  const char *common_fmt_prefix_;
  const char *common_fmt_suffix_;

  enum common_dest_type {
    SECDT_NONE,
    SECDT_FMT_PREFIX,
    SECDT_FMT
  } common_dest_type_;
  const char *common_dest_fmt_;

  enum len_type {
    SELT_NONE,
    SELT_FMT
  } len_type_;
  const char *len_fmt_;

  enum discard_type {
    SEDIT_NONE,
    SEDIT_FMT
  } discard_type_;
  const char *discard_fmt_;
};

static int emit_dest(FILE *outfp, struct cinder_context *cc, struct snippet_emission *se, struct snippet_token *st) {
  switch (se->dest_type_) {
  case SEDT_NONE:
    re_error(&st->text_, "$$ cannot resolve to a symbol");
    return TKR_SYNTAX_ERROR;
  case SEDT_FMT:
    fprintf(outfp, se->dest_fmt_);
    break;
  case SEDT_FMT_PREFIX:
    fprintf(outfp, se->dest_fmt_, cc_prefix(cc));
    break;
  case SEDT_FMT_DATATYPE_ORDINAL:
    if (!se->dest_->sym_->assigned_type_) {
      re_error(&st->text_, "$$ cannot resolve to a data type for non-terminal %s", se->dest_->id_.translated_);
      return TKR_SYNTAX_ERROR;
    }
    fprintf(outfp, se->dest_fmt_, se->dest_->sym_->assigned_type_->ordinal_);
    break;
  case SEDT_FMT_PREFIX_DATATYPE_ORDINAL:
    if (!se->dest_->sym_->assigned_type_) {
      re_error(&st->text_, "$$ cannot resolve to a data type for non-terminal %s", se->dest_->id_.translated_);
      return TKR_SYNTAX_ERROR;
    }
    fprintf(outfp, se->dest_fmt_, cc_prefix(cc), se->dest_->sym_->assigned_type_->ordinal_);
    break;
  case SEDT_FMT_PREFIX_TYPESTR_ORDINAL:
    fprintf(outfp, se->dest_fmt_, cc_prefix(cc), se->dest_typestr_->ordinal_);
    break;
  case SEDT_FMT_TYPESTR_ORDINAL:
    fprintf(outfp, se->dest_fmt_, se->dest_typestr_->ordinal_);
    break;
  }

  return 0;
}

static int emit_sym_specific_data(FILE *outfp, struct cinder_context *cc, struct snippet_emission *se, struct snippet_token *st, size_t sym_index) {
  if (se->sym_type_ == SEST_NONE) {
    re_error(&st->text_, "No symbols to reference");
    return TKR_SYNTAX_ERROR;
  }
  if (sym_index >= se->prod_->num_syms_) {
    re_error(&st->text_, "Symbol index exceeds number of symbols in production");
    return TKR_SYNTAX_ERROR;
  }
  struct symbol *sym = se->prod_->syms_[sym_index].sym_;
  if (!sym->assigned_type_) {
    re_error(&st->text_, "%s cannot resolve to a data type for the symbol", st->text_.translated_);
    return TKR_SYNTAX_ERROR;
  }
  switch (se->sym_type_) {
  case SEST_FMT_INDEX_ORDINAL:
    fprintf(outfp, se->sym_fmt_, sym_index, sym->assigned_type_->ordinal_);
    break;
  }
  return 0;
}

static int emit_dest_commondata(FILE *outfp, struct cinder_context *cc, struct snippet_emission *se, struct snippet_token *st) {
  if (se->common_dest_type_ == SECDT_NONE) {
    re_error(&st->text_, "$ cannot resolve to a symbol");
    return TKR_SYNTAX_ERROR;
  }
  if (!cc->common_data_assigned_type_) {
    re_error(&st->text_, "$ cannot resolve, no common data type defined");
  }

  switch (se->common_dest_type_) {
  case SECDT_FMT_PREFIX:
    fprintf(outfp, se->common_dest_fmt_, cc_prefix(cc));
    break;
  case SECDT_FMT:
    fprintf(outfp, se->common_dest_fmt_);
    break;
  }

  return 0;
}

static int emit_len(FILE *outfp, struct cinder_context *cc, struct snippet_emission *se, struct snippet_token *st) {
  if (se->len_type_ == SELT_NONE) {
    re_error(&st->text_, "%len cannot resolve to a length");
    return TKR_SYNTAX_ERROR;
  }
  switch (se->len_type_) {
  case SELT_FMT:
    fprintf(outfp, se->len_fmt_);
    break;
  }
  return 0;
}

static int emit_discard(FILE *outfp, struct cinder_context *cc, struct snippet_emission *se, struct snippet_token *st) {
  if (se->discard_type_ == SEDIT_NONE) {
    re_error(&st->text_, "%discard use limited to common actions");
    return TKR_SYNTAX_ERROR;
  }
  switch (se->discard_type_) {
  case SEDIT_FMT:
    fprintf(outfp, se->discard_fmt_);
    break;
  }
  return 0;
}

static int emit_common(FILE *outfp, struct cinder_context *cc, struct snippet_emission *se, struct snippet_token *special_ident_token, size_t start_of_index, size_t end_of_index) {
  if (se->common_type_ == SECT_NONE) {
    re_error(&special_ident_token->text_, "No symbols to reference");
    return TKR_SYNTAX_ERROR;
  }
  if (!cc->common_data_assigned_type_) {
    re_error(&special_ident_token->text_, "${} cannot resolve, no common data type defined");
    return TKR_SYNTAX_ERROR;
  }
  switch (se->common_type_) {
  case SECT_FMT_PREFIX:
    fprintf(outfp, se->common_fmt_prefix_, cc_prefix(cc));
    break;
  case SECT_FMT:
    fprintf(outfp, se->common_fmt_prefix_);
    break;
  }
  size_t n;
  for (n = start_of_index; n < end_of_index; ++n) {
    fprintf(outfp, "%s", se->code_->tokens_[n].text_.original_);
  }
  fprintf(outfp, se->common_fmt_suffix_);
  return 0;
}

static int emit_snippet_code_emission(FILE *outfp, struct cinder_context *cc, struct snippet_emission *se) {
  int r;
  size_t col;
  for (col = 0; col < se->code_->num_tokens_; ++col) {
    /* Print the original code, to preserve formatting and line continuations */
    if (se->code_->tokens_[col].match_ == TOK_SPECIAL_IDENT_DST) {
      /* Expansion of special destination sym identifier */
      r = emit_dest(outfp, cc, se, se->code_->tokens_ + col);
      if (r) {
        return r;
      }
    }
    else if ((se->code_->tokens_[col].match_ == TOK_SPECIAL_IDENT_STR) && !strcmp("$", se->code_->tokens_[col].text_.translated_)) {
      /* Isolated single $; if this is followed by a { in the immediately adjacent next token, we expand to index to common data */
      if ((se->code_->num_tokens_ > (col + 1)) && (se->code_->tokens_[col + 1].variant_ == TOK_CUBRACE_OPEN)) {
        int num_open_cubraces = 0;
        size_t special_ident_at = col;
        size_t open_cubrace_at = col + 1;
        size_t index_starts_at = col + 2; /* beyond the opening brace */
        do {
          col++;
          
          if (se->code_->tokens_[col].variant_ == TOK_CUBRACE_CLOSE) {
            num_open_cubraces--;
          }
          else if (se->code_->tokens_[col].variant_ == TOK_CUBRACE_OPEN) {
            num_open_cubraces++;
          }

        } while ((col < se->code_->num_tokens_) && num_open_cubraces);

        size_t index_ends_at = col; /* at (but not including) the closing brace */

        if (num_open_cubraces) {
          re_error(&se->code_->tokens_[open_cubrace_at].text_, "Unmatched brace");
        }
        else {
          r = emit_common(outfp, cc, se, se->code_->tokens_ + special_ident_at, index_starts_at, index_ends_at);
          if (r) {
            return r;
          }
        }
      }
      else {
        /* Single $ with no { } following it refers to the destination symbol's common data */
        r = emit_dest_commondata(outfp, cc, se, se->code_->tokens_ + col);
        if (r) {
          return r;
        }
      }
    }
    else if ((se->code_->tokens_[col].match_ == TOK_SPECIAL_IDENT_STR) && !strcmp("$len", se->code_->tokens_[col].text_.translated_)) {
      r = emit_len(outfp, cc, se, se->code_->tokens_ + col);
      if (r) {
        return r;
      }
    }
    else if ((se->code_->tokens_[col].match_ == TOK_SPECIAL_IDENT_STR) && !strcmp("$discard", se->code_->tokens_[col].text_.translated_)) {
      r = emit_discard(outfp, cc, se, se->code_->tokens_ + col);
      if (r) {
        return r;
      }
    }
    else if (se->code_->tokens_[col].match_ == TOK_SPECIAL_IDENT_STR) {
      /* Expansion of another sym identifier */
      size_t n;
      int failed = 0;
      size_t special_index = 0;
      for (n = 1; n < se->code_->tokens_[col].text_.num_translated_; ++n) {
        char c = se->code_->tokens_[col].text_.translated_[n];
        if (!isdigit(c)) {
          re_error(&se->code_->tokens_[col].text_, "Unrecognized special identifier");
          failed = 1;
          break;
        }
        if (multiply_size_t(10, special_index, NULL, &special_index)) {
          re_error(&se->code_->tokens_[col].text_, "Overflow on symbol index");
          failed = 1;
          break;
        }
        special_index += (size_t)(c - '0');
      }
      if (failed) {
        return TKR_SYNTAX_ERROR;
      }
      if (se->sym_type_ == SEST_NONE) {
        re_error(&se->code_->tokens_[col].text_, "Symbol reference cannot resolve");
        return TKR_SYNTAX_ERROR;
      }
      r = emit_sym_specific_data(outfp, cc, se, se->code_->tokens_ + col, special_index);
      if (r) {
        return r;
      }
    }
    else {
      fprintf(outfp, "%s", se->code_->tokens_[col].text_.original_);
    }
  }
  fprintf(outfp, "\n");

  return 0;
}

static int emit_common_action_snippet(FILE *outfp, struct cinder_context *cc, struct prd_production *prd) {
  struct snippet_emission se = { 0 };
  se.code_ = &prd->common_action_sequence_;
  se.dest_type_ = SEDT_NONE;
  se.sym_type_ = SEST_NONE;
  se.common_type_ = SECT_FMT;
  se.common_fmt_prefix_ = "(sym_data[";
  se.common_fmt_suffix_ = "].common_)";
  se.common_dest_type_ = SECDT_FMT;
  se.common_dest_fmt_ = "(nonterminal_sym_data_reduced_to.common_)";
  se.len_type_ = SELT_FMT;
  se.len_fmt_ = "((size_t)production_length)";
  se.discard_type_ = SEDT_FMT;
  se.discard_fmt_ = "discard_action = 1;";
  return emit_snippet_code_emission(outfp, cc, &se);
}

static int emit_action_snippet(FILE *outfp, struct cinder_context *cc, struct prd_production *prd) {
  struct snippet_emission se = { 0 };
  se.code_ = &prd->action_sequence_;
  se.dest_type_ = SEDT_FMT_DATATYPE_ORDINAL;
  se.dest_fmt_ = "(nonterminal_sym_data_reduced_to.v_.uv%d_)";
  se.dest_ = &prd->nt_;
  se.sym_type_ = SEST_FMT_INDEX_ORDINAL;
  se.sym_fmt_ = "(sym_data[%zu].v_.uv%d_)";
  se.prod_ = prd;
  se.common_type_ = SECT_FMT;
  se.common_fmt_prefix_ = "(sym_data[";
  se.common_fmt_suffix_ = "].common_)";
  se.common_dest_type_ = SECDT_FMT;
  se.common_dest_fmt_ = "(nonterminal_sym_data_reduced_to.common_)";
  se.len_type_ = SELT_FMT;
  se.len_fmt_ = "((size_t)production_length)";
  se.discard_type_ = SEDT_NONE;
  return emit_snippet_code_emission(outfp, cc, &se);
}

static int emit_token_action_snippet(FILE *outfp, struct cinder_context *cc, struct snippet *code) {
  struct snippet_emission se = { 0 };
  se.code_ = code;
  se.dest_type_ = SEDT_FMT_TYPESTR_ORDINAL;
  se.dest_typestr_ = cc->token_assigned_type_;
  se.dest_fmt_ = "(sym_data->v_.uv%d_)";
  se.sym_type_ = SEST_NONE;
  se.common_type_ = SECT_NONE;
  se.common_dest_type_ = SECDT_FMT;
  se.common_dest_fmt_ = "(sym_data->common_)";
  se.len_type_ = SELT_NONE;
  se.discard_type_ = SEDT_NONE;
  return emit_snippet_code_emission(outfp, cc, &se);
}

static int emit_constructor_snippet(FILE *outfp, struct cinder_context *cc, struct typestr *ts) {
  struct snippet_emission se = { 0 };
  se.code_ = &ts->constructor_snippet_;
  se.dest_type_ = SEDT_FMT_PREFIX_TYPESTR_ORDINAL;
  se.dest_typestr_ = ts;
  se.dest_fmt_ = "((stack->stack_ + %ssym_idx)->v_.uv%d_)";
  se.sym_type_ = SEST_NONE;
  se.common_type_ = SECT_NONE;
  se.common_dest_type_ = SECDT_FMT_PREFIX;
  se.common_dest_fmt_ = "((stack->stack_ + %ssym_idx)->common_)";
  se.len_type_ = SELT_NONE;
  se.discard_type_ = SEDT_NONE;
  return emit_snippet_code_emission(outfp, cc, &se);
}

static int emit_dst_sym_constructor_snippet(FILE *outfp, struct cinder_context *cc, struct typestr *ts) {
  struct snippet_emission se = { 0 };
  se.code_ = &ts->constructor_snippet_;
  se.dest_type_ = SEDT_FMT_TYPESTR_ORDINAL;
  se.dest_typestr_ = ts;
  se.dest_fmt_ = "(nonterminal_sym_data_reduced_to.v_.uv%d_)";
  se.sym_type_ = SEST_NONE;
  se.common_type_ = SECT_NONE;
  se.common_dest_type_ = SECDT_FMT;
  se.common_dest_fmt_ = "(nonterminal_sym_data_reduced_to.common_)";
  se.len_type_ = SELT_NONE;
  se.discard_type_ = SEDT_NONE;
  return emit_snippet_code_emission(outfp, cc, &se);
}

static int emit_common_constructor_snippet(FILE *outfp, struct cinder_context *cc) {
  struct snippet_emission se = { 0 };
  if (!cc->common_data_assigned_type_) return 0;
  se.code_ = &cc->common_data_assigned_type_->constructor_snippet_;
  se.dest_type_ = SEDT_FMT_PREFIX;
  se.dest_fmt_ = "((stack->stack_ + %ssym_idx)->common_)";
  se.sym_type_ = SEST_NONE;
  se.common_type_ = SECT_NONE;
  se.common_dest_type_ = SECDT_FMT_PREFIX;
  se.common_dest_fmt_ = "((stack->stack_ + %ssym_idx)->common_)";
  se.len_type_ = SELT_NONE;
  se.discard_type_ = SEDT_NONE;
  return emit_snippet_code_emission(outfp, cc, &se);
}

static int emit_dst_common_constructor_snippet(FILE *outfp, struct cinder_context *cc) {
  struct snippet_emission se = { 0 };
  if (!cc->common_data_assigned_type_) return 0;
  se.code_ = &cc->common_data_assigned_type_->constructor_snippet_;
  se.dest_type_ = SEDT_FMT;
  se.dest_fmt_ = "(nonterminal_sym_data_reduced_to.common_)";
  se.sym_type_ = SEST_NONE;
  se.common_type_ = SECT_NONE;
  se.common_dest_type_ = SECDT_FMT;
  se.common_dest_fmt_ = "(nonterminal_sym_data_reduced_to.common_)";
  se.len_type_ = SELT_NONE;
  se.discard_type_ = SEDT_NONE;
  return emit_snippet_code_emission(outfp, cc, &se);
}

static int emit_token_common_constructor_snippet(FILE *outfp, struct cinder_context *cc) {
  struct snippet_emission se = { 0 };
  if (!cc->common_data_assigned_type_) return 0;
  se.code_ = &cc->common_data_assigned_type_->constructor_snippet_;
  se.dest_type_ = SEDT_FMT;
  se.dest_fmt_ = "(sym_data->common_)";
  se.sym_type_ = SEST_NONE;
  se.common_type_ = SECT_NONE;
  se.common_dest_type_ = SECDT_FMT;
  se.common_dest_fmt_ = "(sym_data->common_)";
  se.len_type_ = SELT_NONE;
  se.discard_type_ = SEDT_NONE;
  return emit_snippet_code_emission(outfp, cc, &se);
}

static int emit_token_constructor_snippet(FILE *outfp, struct cinder_context *cc, struct typestr *ts) {
  struct snippet_emission se = { 0 };
  se.code_ = &ts->constructor_snippet_;
  se.dest_type_ = SEDT_FMT_TYPESTR_ORDINAL;
  se.dest_typestr_ = ts;
  se.dest_fmt_ = "(sym_data->v_.uv%d_)";
  se.sym_type_ = SEST_NONE;
  se.common_type_ = SECT_NONE;
  se.common_dest_type_ = SECDT_FMT;
  se.common_dest_fmt_ = "(sym_data->common_)";
  se.len_type_ = SELT_NONE;
  se.discard_type_ = SEDT_NONE;
  return emit_snippet_code_emission(outfp, cc, &se);
}

static int emit_destructor_snippet(FILE *outfp, struct cinder_context *cc, struct typestr *ts) {
  struct snippet_emission se = { 0 };
  se.code_ = &ts->destructor_snippet_;
  se.dest_type_ = SEDT_FMT_PREFIX_TYPESTR_ORDINAL;
  se.dest_typestr_ = ts;
  se.dest_fmt_ = "((stack->stack_ + %ssym_idx)->v_.uv%d_)";
  se.sym_type_ = SEST_NONE;
  se.common_type_ = SECT_NONE;
  se.common_dest_type_ = SECDT_FMT_PREFIX;
  se.common_dest_fmt_ = "((stack->stack_ + %ssym_idx)->common_)";
  se.len_type_ = SELT_NONE;
  se.discard_type_ = SEDT_NONE;
  return emit_snippet_code_emission(outfp, cc, &se);
}

static int emit_destructor_snippet_indexed_by_n(FILE *outfp, struct cinder_context *cc, struct typestr *ts) {
  struct snippet_emission se = { 0 };
  se.code_ = &ts->destructor_snippet_;
  se.dest_type_ = SEDT_FMT_TYPESTR_ORDINAL;
  se.dest_typestr_ = ts;
  se.dest_fmt_ = "((stack->stack_ + n)->v_.uv%d_)";
  se.sym_type_ = SEST_NONE;
  se.common_type_ = SECT_NONE;
  se.common_dest_type_ = SECDT_FMT;
  se.common_dest_fmt_ = "((stack->stack_ + n)->common_)";
  se.len_type_ = SELT_NONE;
  se.discard_type_ = SEDT_NONE;
  return emit_snippet_code_emission(outfp, cc, &se);
}

static int emit_common_destructor_snippet(FILE *outfp, struct cinder_context *cc) {
  struct snippet_emission se = { 0 };
  if (!cc->common_data_assigned_type_) return 0;
  se.code_ = &cc->common_data_assigned_type_->destructor_snippet_;
  se.dest_type_ = SEDT_FMT_PREFIX;
  se.dest_fmt_ = "((stack->stack_ + %ssym_idx)->common_)";
  se.sym_type_ = SEST_NONE;
  se.common_type_ = SECT_NONE;
  se.common_dest_type_ = SECDT_FMT_PREFIX;
  se.common_dest_fmt_ = "((stack->stack_ + %ssym_idx)->common_)";
  se.len_type_ = SELT_NONE;
  se.discard_type_ = SEDT_NONE;
  return emit_snippet_code_emission(outfp, cc, &se);
}

static int emit_common_destructor_snippet_indexed_by_n(FILE *outfp, struct cinder_context *cc) {
  struct snippet_emission se = { 0 };
  if (!cc->common_data_assigned_type_) return 0;
  se.code_ = &cc->common_data_assigned_type_->destructor_snippet_;
  se.dest_type_ = SEDT_FMT;
  se.dest_fmt_ = "((stack->stack_ + n)->common_)";
  se.sym_type_ = SEST_NONE;
  se.common_type_ = SECT_NONE;
  se.common_dest_type_ = SECDT_FMT;
  se.common_dest_fmt_ = "((stack->stack_ + n)->common_)";
  se.len_type_ = SELT_NONE;
  se.discard_type_ = SEDT_NONE;
  return emit_snippet_code_emission(outfp, cc, &se);
}

static int emit_lex_function(FILE *outfp, struct cinder_context *cc, struct prd_grammar *prdg, struct sc_scanner *scantable) {
  /* Emit the scan function, it scans the input for regex matches without actually executing any actions */
  /* (we're obviously in need of a templating language..) */
  fprintf(outfp,  "static int %sappend_match_buffer(struct %sstack *stack, const char *s, size_t len) {\n", cc_prefix(cc), cc_prefix(cc));
  fprintf(outfp,  "  size_t size_needed = len;\n"
                  "  size_needed += stack->match_buffer_size_;\n"
                  "  if (size_needed < stack->match_buffer_size_) {\n");
  fprintf(outfp,  "    return _%sOVERFLOW;\n", cc_PREFIX(cc));
  fprintf(outfp,  "  }\n"
                  "  if (size_needed == SIZE_MAX) {\n"
                  "    /* cannot fit null terminator */\n");
  fprintf(outfp,  "    return _%sOVERFLOW;\n", cc_PREFIX(cc));
  fprintf(outfp,  "  }\n"
                  "  size_needed++; /* null terminator */\n"
                  "  if (size_needed < 128) {\n"
                  "    size_needed = 128;\n"
                  "  }\n"
                  "  if (size_needed > stack->match_buffer_size_allocated_) {\n"
                  "    /* intent of code: grow buffer size by powers of 2-1, unless our needs require more now. */\n"
                  "    size_t size_to_allocate = stack->match_buffer_size_allocated_ * 2 + 1;\n"
                  "    if (size_to_allocate <= stack->match_buffer_size_allocated_) {\n");
  fprintf(outfp,  "      return _%sOVERFLOW;\n", cc_PREFIX(cc));
  fprintf(outfp,  "    }\n"
                  "    if (size_to_allocate < size_needed) {\n"
                  "      size_to_allocate = size_needed;\n"
                  "    }\n"
                  "    void *buf = realloc(stack->match_buffer_, size_to_allocate);\n"
                  "    if (!buf) {\n");
  fprintf(outfp,  "      return _%sOVERFLOW;\n", cc_PREFIX(cc));
  fprintf(outfp,  "    }\n"
                  "    stack->match_buffer_ = (char *)buf;\n"
                  "    stack->match_buffer_size_allocated_ = size_to_allocate;\n"
                  "  }\n"
                  "\n"
                  "  memcpy(stack->match_buffer_ + stack->match_buffer_size_, s, len);\n"
                  "  stack->match_buffer_size_ += len;\n"
                  "  stack->match_buffer_[stack->match_buffer_size_] = '\\0';\n"
                  "  return 0;\n"
                  "}\n"
                  "\n");
  fprintf(outfp,  "int %slex(struct %sstack *stack, const char *input, size_t input_size, int is_final_input) {\n", cc_prefix(cc), cc_prefix(cc));
  fprintf(outfp,  "  int r;\n"
                  "  unsigned char c;\n"
                  "  size_t scan_state = stack->scan_state_;\n");
  fprintf(outfp,  "  const size_t *transition_table = %sscan_table;\n", cc_prefix(cc));
  fprintf(outfp,  "  const size_t *actions = %sscan_actions;\n", cc_prefix(cc));
  fprintf(outfp,  "  const size_t default_action = %zu;\n", scantable->default_action);
  fprintf(outfp,  "  const size_t start_state = %zu;\n", scantable->start_state);
  fprintf(outfp,  "  const size_t start_action = %sscan_actions[start_state];\n", cc_prefix(cc));
  fprintf(outfp,  "\n"
                  "  size_t match_index = stack->match_index_;\n"
                  "\n"
                  "  size_t best_match_action = stack->best_match_action_;\n"
                  "  size_t best_match_size = stack->best_match_size_;\n"
                  "  size_t best_match_offset = stack->best_match_offset_;\n"
                  "  int best_match_line = stack->best_match_line_;\n"
                  "  int best_match_col = stack->best_match_col_;\n"
                  "\n"
                  "  size_t input_index = stack->input_index_;\n"
                  "  size_t input_offset = stack->input_offset_;\n"
                  "  int input_line = stack->input_line_;\n"
                  "  int input_col = stack->input_col_;\n"
                  "\n"
                  "  /* Move any prior token out of the way */\n"
                  "  if (stack->token_size_) {\n"
                  "    stack->match_buffer_[stack->token_size_] = stack->terminator_repair_;\n"
                  "\n"
                  "    memcpy(stack->match_buffer_, stack->match_buffer_ + stack->token_size_, stack->match_buffer_size_ - stack->token_size_);\n"
                  "    stack->match_buffer_size_ -= stack->token_size_;\n"
                  "    stack->match_offset_ = stack->best_match_offset_;\n"
                  "    stack->match_line_ = stack->best_match_line_;\n"
                  "    stack->match_col_ = stack->best_match_col_;\n"
                  "    \n"
                  "    /* Reset scanner to get ready for next token */\n"
                  "    stack->match_index_ = 0;\n"
                  "    stack->best_match_action_ = best_match_action = start_action;\n"
                  "    stack->best_match_size_ = best_match_size = 0;\n"
                  "    stack->scan_state_ = scan_state = start_state;\n"
                  "    stack->token_size_ = 0;\n"
                  "    \n"
                  "  }\n"
                  "\n"
                  "  size_t at_match_index_offset = stack->match_offset_;\n"
                  "  int at_match_index_line = stack->match_line_;\n"
                  "  int at_match_index_col = stack->match_col_;\n"
                  "  while (match_index < stack->match_buffer_size_) {\n"
                  "    c = (unsigned char)stack->match_buffer_[match_index];\n"
                  "    scan_state = transition_table[256 * scan_state + c];\n"
                  "    if (scan_state) {\n"
                  "      at_match_index_offset++;\n"
                  "      if (c != '\\n') {\n"
                  "        at_match_index_col++;\n"
                  "      }\n"
                  "      else {\n"
                  "        at_match_index_col = 1;\n"
                  "        at_match_index_line++;\n"
                  "      }\n"
                  "\n"
                  "      size_t state_action = actions[scan_state];\n"
                  "      if (state_action != default_action) /* replace with actual */ {\n"
                  "        best_match_action = state_action;\n"
                  "        best_match_size = match_index + 1;\n"
                  "        best_match_offset = at_match_index_offset;\n"
                  "        best_match_line = at_match_index_line;\n"
                  "        best_match_col = at_match_index_col;\n"
                  "      }\n"
                  "      match_index++;\n"
                  "    }\n"
                  "    else {\n"
                  "      /* error, or, end of token, depending on whether we have a match before */\n"
                  "      if (best_match_action == default_action) {\n"
                  "        goto syntax_error;\n"
                  "      }\n"
                  "\n"
                  "      /* Ensure token match is null terminated */\n"
                  "      stack->terminator_repair_ = stack->match_buffer_[best_match_size];\n"
                  "      stack->match_buffer_[best_match_size] = '\\0';\n"
                  "      stack->token_size_ = best_match_size;\n"
                  "      stack->best_match_action_ = best_match_action;\n"
                  "      stack->best_match_size_ = best_match_size;\n"
                  "      stack->best_match_offset_ = best_match_offset;\n"
                  "      stack->best_match_line_ = best_match_line;\n"
                  "      stack->best_match_col_ = best_match_col;\n"
                  "\n");
  fprintf(outfp,  "      return _%sMATCH;\n", cc_PREFIX(cc));
  fprintf(outfp,  "    }\n"
                  "  }\n"
                  "\n"
                  "  while (input_index < input_size) {\n"
                  "    c = (unsigned char)input[input_index];\n"
                  "    scan_state = transition_table[256 * scan_state + c];\n"
                  "    if (scan_state) {\n"
                  "      input_offset++;\n"
                  "      if (c != '\\n') {\n"
                  "        input_col++;\n"
                  "      }\n"
                  "      else {\n"
                  "        input_col = 1;\n"
                  "        input_line++;\n"
                  "      }\n"
                  "      size_t state_action = actions[scan_state];\n"
                  "      if (state_action != default_action) /* replace with actual */ {\n"
                  "        best_match_action = state_action;\n"
                  "        best_match_size = stack->match_buffer_size_ + input_index - stack->input_index_ + 1;\n"
                  "        best_match_col = input_col;\n"
                  "        best_match_line = input_line;\n"
                  "      }\n"
                  "      input_index++;\n"
                  "    }\n"
                  "    else {\n"
                  "      /* Append from stack->input_index_ to input_index, excluding input_index itself */\n"
                  "      r = rxg_append_match_buffer(stack, input + stack->input_index_, input_index - stack->input_index_);\n"
                  "      if (r) return r;\n"
                  " \n"
                  "      if (best_match_action == default_action) {\n"
                  "        goto syntax_error;\n"
                  "      }\n"
                  "\n"
                  "      /* Ensure token match is null terminated, note that the size we just appended may\n"
                  "       * (likely) be longer than the last section we matched. */\n"
                  "      stack->terminator_repair_ = stack->match_buffer_[best_match_size];\n"
                  "      stack->match_buffer_[best_match_size] = '\\0';\n"
                  "      stack->token_size_ = best_match_size;\n"
                  "      stack->best_match_action_ = best_match_action;\n"
                  "      stack->best_match_size_ = best_match_size;\n"
                  "      stack->best_match_offset_ = best_match_offset;\n"
                  "      stack->best_match_line_ = best_match_line;\n"
                  "      stack->best_match_col_ = best_match_col;\n"
                  "\n");
  fprintf(outfp,  "      return _%sMATCH;\n", cc_PREFIX(cc));
  fprintf(outfp,  "    }\n"
                  "  }\n"
                  "\n"
                  "  r = rxg_append_match_buffer(stack, input + stack->input_index_, input_index - stack->input_index_);\n"
                  "  if (r) return r;\n"
                  "\n"
                  "  if (!is_final_input) {\n"
                  "    /* Need more input */\n"
                  "    stack->scan_state_ = scan_state;\n"
                  "    stack->token_size_ = 0; /* no match yet */\n"
                  "    stack->input_index_ = 0;\n"
                  "    stack->input_offset_ = input_offset;\n"
                  "    stack->input_line_ = input_line;\n"
                  "    stack->input_col_ = input_col;\n"
                  "\n"
                  "    stack->best_match_action_ = best_match_action;\n"
                  "    stack->best_match_size_ = best_match_size;\n"
                  "    stack->best_match_offset_ = best_match_offset;\n"
                  "    stack->best_match_line_ = best_match_line;\n"
                  "    stack->best_match_col_ = best_match_col;\n"
                  "\n"
                  "    stack->match_index_ = match_index;\n"
                  "\n");
  fprintf(outfp,  "    return _%sFEED_ME;\n", cc_PREFIX(cc));
  fprintf(outfp,  "  }\n"
                  "\n"
                  "  if (!stack->match_buffer_size_ && (stack->input_index_ == input_size)) {\n"
                  "    /* Exhausted all input - leave stack in a state where we can\n"
                  "     * immediately re-use it in its initial state */\n"
                  "    stack->match_index_ = 0;\n"
                  "    stack->best_match_action_ = best_match_action = start_action;\n"
                  "    stack->best_match_size_ = best_match_size;\n"
                  "    stack->best_match_offset_ = best_match_offset;\n"
                  "    stack->best_match_line_ = best_match_line;\n"
                  "    stack->best_match_col_ = best_match_col;\n"
                  "    stack->scan_state_ = scan_state = start_state;\n"
                  "\n"
                  "    stack->token_size_ = 0;\n"
                  "    stack->input_index_ = 0;\n"
                  "    stack->input_offset_ = input_offset;\n"
                  "    stack->input_line_ = input_line;\n"
                  "    stack->input_col_ = input_col;\n"
                  "\n");
  fprintf(outfp,  "    return _%sEND_OF_INPUT;\n", cc_PREFIX(cc));
  fprintf(outfp,  "  }\n"
                  "\n"
                  "  if (best_match_action == default_action) {\n"
                  "    goto syntax_error;\n"
                  "  }\n"
                  "\n"
                  "  /* Ensure token match is null terminated */\n"
                  "  stack->terminator_repair_ = stack->match_buffer_[best_match_size];\n"
                  "  stack->match_buffer_[best_match_size] = '\\0';\n"
                  "  stack->token_size_ = best_match_size;\n"
                  "  stack->best_match_action_ = best_match_action;\n"
                  "  stack->best_match_size_ = best_match_size;\n"
                  "  stack->best_match_offset_ = best_match_offset;\n"
                  "  stack->best_match_line_ = best_match_line;\n"
                  "  stack->best_match_col_ = best_match_col;\n"
                  "\n");
  fprintf(outfp,  "      return _%sMATCH;\n", cc_PREFIX(cc));
  fprintf(outfp,  "syntax_error:\n"
                  "  if (stack->match_buffer_size_) {\n"
                  "    stack->best_match_offset_ = stack->match_offset_ + 1;\n"
                  "    if (stack->match_buffer_[0] != '\\n') {\n"
                  "      stack->best_match_line_ = stack->match_line_;\n"
                  "      stack->best_match_col_ = stack->match_col_ + 1;\n"
                  "    }\n"
                  "    else {\n"
                  "      stack->best_match_line_ = stack->match_line_ + 1;\n"
                  "      stack->best_match_col_ = 1;\n"
                  "    }\n"
                  "  }\n"
                  "  else {\n"
                  "    /* Append the single character causing the syntax error */\n"
                  "    r = rxg_append_match_buffer(stack, input + stack->input_index_, 1);\n"
                  "    if (r) return r;\n"
                  "\n"
                  "    input_offset++;\n"
                  "    if (input[stack->input_index_] != '\\n') {\n"
                  "      input_col++;\n"
                  "    }\n"
                  "    else {\n"
                  "      input_col = 1;\n"
                  "      input_line++;\n"
                  "    }\n"
                  "    input_index = stack->input_index_ + 1;\n"
                  "    stack->best_match_offset_ = input_offset;\n"
                  "    stack->best_match_line_ = input_line;\n"
                  "    stack->best_match_col_ = input_col;\n"
                  "  }\n"
                  "  \n"
                  "  /* Reset scanner to get ready for next token */\n"
                  "  stack->token_size_ = 1;\n"
                  "  stack->terminator_repair_ = stack->match_buffer_[1];\n"
                  "  stack->match_buffer_[1] = '\\0';\n"
                  "\n"
                  "  stack->input_index_ = input_index;\n"
                  "  stack->input_offset_ = input_offset;\n"
                  "  stack->input_line_ = input_line;\n"
                  "  stack->input_col_ = input_col;\n"
                  "\n");
  fprintf(outfp,  "  return _%sSYNTAX_ERROR;\n", cc_PREFIX(cc));
  fprintf(outfp,  "}\n");

  return 0;
}

static int emit_scan_function(FILE *outfp, struct cinder_context *cc, struct prd_grammar *prdg, struct lr_generator *lalr, int *state_syms) {
  int r;

  /* Emit the parse function */
  if (cc->params_snippet_.num_tokens_) {
    fprintf(outfp,
      "int %sscan(struct %sstack *stack, const char *input, size_t input_size, int is_final_input, ", cc_prefix(cc), cc_prefix(cc));
    size_t token_idx;
    for (token_idx = 0; token_idx < cc->params_snippet_.num_tokens_; ++token_idx) {
      fprintf(outfp, "%s", cc->params_snippet_.tokens_[token_idx].text_.original_);
    }
    fprintf(outfp, ") {\n");
  }
  else {
    fprintf(outfp, "int %sscan(struct %sstack *stack, const char *input, size_t input_size, int is_final_input) {\n", cc_prefix(cc), cc_prefix(cc));
  }
#if 0
  fprintf(outfp, "#define _%sMATCH 1\n", cc_PREFIX(&cc));
  fprintf(outfp, "#define _%sOVERFLOW 2\n", cc_PREFIX(&cc));
  fprintf(outfp, "#define _%sNO_MEMORY 3\n", cc_PREFIX(&cc));
  fprintf(outfp, "#define _%sFEED_ME 4\n", cc_PREFIX(&cc));
  fprintf(outfp, "#define _%sEND_OF_INPUT 5\n", cc_PREFIX(&cc));
  fprintf(outfp, "#define _%sSYNTAX_ERROR 6\n", cc_PREFIX(&cc));
#endif

  fprintf(outfp, "  for (;;) {\n");
  fprintf(outfp, "    if (stack->need_sym_) {\n");
  fprintf(outfp, "      switch (%slex(stack, input, input_size, is_final_input)) {\n", cc_prefix(cc));
  fprintf(outfp, "        case _%sMATCH:\n", cc_PREFIX(cc));
  fprintf(outfp, "          stack->current_sym_ = (int)stack->best_match_action_;\n");
  fprintf(outfp, "          stack->need_sym_ = 0;\n");
  fprintf(outfp, "          if (stack->mute_error_turns_) stack->mute_error_turns_--;\n");
  fprintf(outfp, "          break;\n");
  fprintf(outfp, "        case _%sOVERFLOW:\n", cc_PREFIX(cc));
  fprintf(outfp, "          return -2;\n");
  fprintf(outfp, "        case _%sNO_MEMORY:\n", cc_PREFIX(cc));
  fprintf(outfp, "          return -2;\n");
  fprintf(outfp, "        case _%sFEED_ME:\n", cc_PREFIX(cc));
  fprintf(outfp, "          return 1;\n");
  fprintf(outfp, "        case _%sEND_OF_INPUT:\n", cc_PREFIX(cc));
  fprintf(outfp, "          stack->current_sym_ = %d; /* %s */\n", cc->input_end_sym_->ordinal_, cc->input_end_sym_->def_.translated_);
  fprintf(outfp, "          stack->need_sym_ = 0;\n");
  fprintf(outfp, "          if (stack->mute_error_turns_) stack->mute_error_turns_--;\n");
  fprintf(outfp, "          break;\n");
  fprintf(outfp, "        case _%sSYNTAX_ERROR:\n", cc_PREFIX(cc));
  fprintf(outfp, "          return -1;\n");
  fprintf(outfp, "      } /* switch */\n");
  fprintf(outfp, "    } /* if (need_sym_) */\n");
  fprintf(outfp, "    else {\n");
  fprintf(outfp, "      int sym = stack->current_sym_;\n"
                 "      if (!stack->error_recovery_) {\n"
                 "        int action = %sparse_table[%snum_columns * stack->stack_[stack->pos_ - 1].state_ + (sym - %sminimum_sym)];\n", cc_prefix(cc), cc_prefix(cc), cc_prefix(cc));
  /* Shift logic */
  fprintf(outfp, "        if (action > 0) {\n");
  fprintf(outfp, "          switch (%spush_state(stack, action /* action for a shift is the ordinal */)) {\n"
                 "            case -1: /* overflow */ {\n"
                 "              ", cc_prefix(cc));
  if (cc->on_internal_error_snippet_.num_tokens_) {
    size_t token_idx;
    for (token_idx = 0; token_idx < cc->on_internal_error_snippet_.num_tokens_; ++token_idx) {
      fprintf(outfp, "%s", cc->on_internal_error_snippet_.tokens_[token_idx].text_.original_);
    }
  }
  else {
    fprintf(outfp, "return -2;\n");
  }
  fprintf(outfp, "            }\n"
                 "            break;\n"
                 "            case -2: /* out of memory */ {\n"
                 "              ");
  if (cc->on_alloc_error_snippet_.num_tokens_) {
    size_t token_idx;
    for (token_idx = 0; token_idx < cc->on_alloc_error_snippet_.num_tokens_; ++token_idx) {
      fprintf(outfp, "%s", cc->on_alloc_error_snippet_.tokens_[token_idx].text_.original_);
    }
  }
  else {
    fprintf(outfp, "return -2;\n");
  }
  fprintf(outfp, "            }\n"
                 "            break;\n"
                 "          } /* switch */\n");

  fprintf(outfp, "\n"
                 "          /* Fill in the sym from the tokenizer */\n");
  /* We already pushed the new state and so have committed to needing a new sym.
   * In the future, we may want to avoid pushing the state until the action snippets all
   * complete. Doing so would allow those action snippets to return to caller with the
   * expectation of re-entering (which is now not the case..) */
  fprintf(outfp, "          stack->need_sym_ = 1;\n");
  int need_sym_data = 0;
  if (cc->token_assigned_type_ && cc->token_assigned_type_->constructor_snippet_.num_tokens_) {
    need_sym_data = 1;
  }
  if (cc->token_action_snippet_.num_tokens_) {
    need_sym_data = 1;
  }
  if (cc->common_data_assigned_type_ && cc->common_data_assigned_type_->constructor_snippet_.num_tokens_) {
    need_sym_data = 1;
  }
  if (need_sym_data) {
    fprintf(outfp, "          struct %ssym_data *sym_data = stack->stack_ + stack->pos_ - 1;\n", cc_prefix(cc));
  }
  if (cc->common_data_assigned_type_ && cc->common_data_assigned_type_->constructor_snippet_.num_tokens_) {
    fprintf(outfp, "          {\n"
                   "            ");
    if (emit_token_common_constructor_snippet(outfp, cc)) {
      r = EXIT_FAILURE;
      goto cleanup_exit;
    }
    fprintf(outfp, "\n"
                   "          }\n");
  }
  if (cc->token_assigned_type_ && cc->token_assigned_type_->constructor_snippet_.num_tokens_) {
    fprintf(outfp, "          {\n"
                   "            ");
    if (emit_token_constructor_snippet(outfp, cc, cc->token_assigned_type_)) {
      r = EXIT_FAILURE;
      goto cleanup_exit;
    }
    fprintf(outfp, "\n"
                   "          }\n");
  }
  if (cc->token_action_snippet_.num_tokens_) {
    fprintf(outfp, "          {\n"
                   "            ");

    if (emit_token_action_snippet(outfp, cc, &cc->token_action_snippet_)) {
      r = EXIT_FAILURE;
      goto cleanup_exit;
    }

    fprintf(outfp, "\n"
                   "          }\n");
  }
  fprintf(outfp, "          if (stack->report_error_) {\n"
                 "            /* We're shifting this sym following an error recovery on the same sym, report syntax error */\n"
                 "            stack->report_error_ = 0;\n"
                 "            ");
  if (cc->on_syntax_error_snippet_.num_tokens_) {
    size_t token_idx;
    for (token_idx = 0; token_idx < cc->on_syntax_error_snippet_.num_tokens_; ++token_idx) {
      fprintf(outfp, "%s", cc->on_syntax_error_snippet_.tokens_[token_idx].text_.original_);
    }
  }
  else {
    fprintf(outfp, "/* Syntax error */\n"
                   "            return -1;\n");
  }
  fprintf(outfp, "          }\n"
                 "          else {\n"
                 "            ");
  if (cc->on_next_token_snippet_.num_tokens_) {
    size_t token_idx;
    for (token_idx = 0; token_idx < cc->on_next_token_snippet_.num_tokens_; ++token_idx) {
      fprintf(outfp, "%s", cc->on_next_token_snippet_.tokens_[token_idx].text_.original_);
    }
  }
  else {
    fprintf(outfp, "/* Fall through to fetch next token */\n");
  }
  fprintf(outfp, "          }\n");
  fprintf(outfp, "        } /* action > 0 */\n");

  fprintf(outfp, "        else if (action < 0) {\n"
                 "          int production = -action - 1;\n"
                 "          int discard_action = 0;\n"
                 "          size_t production_length = %sproduction_lengths[production];\n", cc_prefix(cc));
  fprintf(outfp, "          int nonterminal = %sproduction_syms[production];\n", cc_prefix(cc));
  fprintf(outfp, "          if (0 == production) {\n"
                 "            ");
  if (cc->on_success_snippet_.num_tokens_) {
    size_t token_idx;
    for (token_idx = 0; token_idx < cc->on_success_snippet_.num_tokens_; ++token_idx) {
      fprintf(outfp, "%s", cc->on_success_snippet_.tokens_[token_idx].text_.original_);
    }
  }
  else {
    fprintf(outfp, "/* Synth S production we're done */\n"
                   "            return 0;\n");
  }

  fprintf(outfp, "          }\n"
                 "\n"
                 "          struct %ssym_data nonterminal_sym_data_reduced_to = { 0 };\n", cc_prefix(cc));

  fprintf(outfp, "          { /* scope guard */\n"
                 "            struct %ssym_data *sym_data = stack->stack_ + stack->pos_ - production_length;\n", cc_prefix(cc));
  /* Emit any requested locals */
  if (cc->locals_snippet_.num_tokens_) {
    fprintf(outfp, "          ");
    size_t token_idx;
    for (token_idx = 0; token_idx < cc->locals_snippet_.num_tokens_; ++token_idx) {
      fprintf(outfp, "%s", cc->locals_snippet_.tokens_[token_idx].text_.original_);
    }
    fprintf(outfp, "\n");
  }

  fprintf(outfp, "            switch (production) {\n");
  size_t row;
  for (row = 0; row < prdg->num_productions_; ++row) {
    struct prd_production *pd = prdg->productions_ + row;
    fprintf(outfp, "            /* %s:", pd->nt_.id_.translated_);
    size_t n;
    for (n = 0; n < pd->num_syms_; ++n) {
      fprintf(outfp, " %s", pd->syms_[n].id_.translated_);
    }
    fprintf(outfp, " */\n");
    fprintf(outfp, "              case %d: {\n    ", (int)row + 1);
    /* Emit dst_sym_data constructor first */
    if (cc->common_data_assigned_type_ && cc->common_data_assigned_type_->constructor_snippet_.num_tokens_) {
      if (emit_dst_common_constructor_snippet(outfp, cc)) {
        r = EXIT_FAILURE;
        goto cleanup_exit;
      }
      fprintf(outfp, "\n"
                     "              }\n"
                     "              {\n"
                     "                ");
    }
    if (pd->nt_.sym_->assigned_type_ && pd->nt_.sym_->assigned_type_->constructor_snippet_.num_tokens_) {
      if (emit_dst_sym_constructor_snippet(outfp, cc, pd->nt_.sym_->assigned_type_)) {
        r = EXIT_FAILURE;
        goto cleanup_exit;
      }
      fprintf(outfp, "\n"
                     "              }\n"
                     "              {\n"
                     "                ");
    }
    if (pd->common_action_sequence_.num_tokens_) {
      if (emit_common_action_snippet(outfp, cc, pd)) {
        r = EXIT_FAILURE;
        goto cleanup_exit;
      }
      fprintf(outfp, "\n"
                     "              }\n"
                     "              if (!discard_action) {\n"
                     "                ");
    }

    if (emit_action_snippet(outfp, cc, pd)) {
      r = EXIT_FAILURE;
      goto cleanup_exit;
    }

    fprintf(outfp, "              }\n"
                   "              break;\n");
  }
  fprintf(outfp, "            } /* switch */\n");
  fprintf(outfp, "          } /* scope guard */\n");
  fprintf(outfp, "\n");
  fprintf(outfp, "          /* Free symdata for every symbol in the production, including the first slot where we will soon\n"
                 "           * push nonterminal_data_reduced_to */\n"
                 "          size_t %ssym_idx;\n", cc_prefix(cc));
  fprintf(outfp, "          for (%ssym_idx = stack->pos_ - production_length; %ssym_idx < stack->pos_; ++%ssym_idx) {\n", cc_prefix(cc), cc_prefix(cc), cc_prefix(cc));

  fprintf(outfp, "            switch (stack->stack_[%ssym_idx].state_) {\n", cc_prefix(cc));
  size_t typestr_idx;
  for (typestr_idx = 0; typestr_idx < cc->tstab_.num_typestrs_; ++typestr_idx) {
    struct typestr *ts = cc->tstab_.typestrs_[typestr_idx];
    if (ts->destructor_snippet_.num_tokens_) {
      int have_cases = 0; /* always true if all types are always used */
      /* Type has a destructor associated.. Find all state for whose corresponding symbol has the associated type */
      size_t state_idx;
      for (state_idx = 0; state_idx < lalr->nr_states_; ++state_idx) {
        struct symbol *sym = symbol_find_by_ordinal(&cc->symtab_, state_syms[state_idx]);
        if (!sym) continue;
        if (sym->assigned_type_ == ts) {
          fprintf(outfp, "              case %d: /* %s */\n", (int)state_idx, sym->def_.translated_);
          have_cases = 1;
        }
      }
      if (have_cases) {
        fprintf(outfp, "              {\n"
                       "                ");
        if (emit_destructor_snippet(outfp, cc, ts)) {
          r = EXIT_FAILURE;
          goto cleanup_exit;
        }

        fprintf(outfp, "\n"
                       "              }\n"
                       "              break;\n");
      }
    }
  }
  /* Enumerate all states, for each state, determine the type corresponding to the state from the symbol corresponding to the state. */
  fprintf(outfp, "            } /* switch */\n");
  if (cc->common_data_assigned_type_ && cc->common_data_assigned_type_->constructor_snippet_.num_tokens_) {
    fprintf(outfp, "          {\n"
                   "            ");
    if (emit_common_destructor_snippet(outfp, cc)) {
      r = EXIT_FAILURE;
      goto cleanup_exit;
    }
    fprintf(outfp, "\n"
                   "          }\n");
  }

  fprintf(outfp, "          } /* for */\n"
                 "          stack->pos_ -= production_length;\n"
                 "          action = %sparse_table[%snum_columns * stack->stack_[stack->pos_ - 1].state_ + (nonterminal - %sminimum_sym)];\n", cc_prefix(cc), cc_prefix(cc), cc_prefix(cc));
  fprintf(outfp, "          if (action <= 0) {\n"
                 "            ");
  if (cc->on_internal_error_snippet_.num_tokens_) {
    size_t token_idx;
    for (token_idx = 0; token_idx < cc->on_internal_error_snippet_.num_tokens_; ++token_idx) {
      fprintf(outfp, "%s", cc->on_internal_error_snippet_.tokens_[token_idx].text_.original_);
    }
  }
  else {
    fprintf(outfp, "/* Internal error: cannot shift an already reduced non-terminal */\n"
                   "            return -2;\n");
  }
  fprintf(outfp, "          }\n");

  fprintf(outfp, "          switch (%spush_state(stack, action /* action for a \"goto\" shift is the ordinal */)) {\n"
                 "            case -1: /* overflow */ {\n"
                 "              ", cc_prefix(cc));
  if (cc->on_internal_error_snippet_.num_tokens_) {
    size_t token_idx;
    for (token_idx = 0; token_idx < cc->on_internal_error_snippet_.num_tokens_; ++token_idx) {
      fprintf(outfp, "%s", cc->on_internal_error_snippet_.tokens_[token_idx].text_.original_);
    }
  }
  else {
    fprintf(outfp, "return -2;\n");
  }
  fprintf(outfp, "            }\n"
                 "            break;\n"
                 "            case -2: /* out of memory */ {\n"
                 "              ");
  if (cc->on_alloc_error_snippet_.num_tokens_) {
    size_t token_idx;
    for (token_idx = 0; token_idx < cc->on_alloc_error_snippet_.num_tokens_; ++token_idx) {
      fprintf(outfp, "%s", cc->on_alloc_error_snippet_.tokens_[token_idx].text_.original_);
    }
  }
  else {
    fprintf(outfp, "return -2;\n");
  }
  fprintf(outfp, "            }\n"
                 "            break;\n"
                 "          } /* switch */\n");
  fprintf(outfp, "          struct %ssym_data *sd = stack->stack_ + stack->pos_ - 1;\n", cc_prefix(cc));
  fprintf(outfp, "          *sd = nonterminal_sym_data_reduced_to;\n"
                 "          sd->state_ = action;\n");
  fprintf(outfp, "        } /* action < 0 */\n"
                 "        else /* action == 0 */ {\n"
                 "          stack->error_recovery_ = 1;\n"
                 "          stack->report_error_ = !stack->mute_error_turns_;\n"
                 "          stack->mute_error_turns_ = 3;\n"
                 "        }\n");
  fprintf(outfp, "      } /* !stack->error_recovery_ */\n"
                 "      if (stack->error_recovery_) {\n");
  fprintf(outfp, "        size_t n;\n"
                 "        n = stack->pos_;\n"
                 "        if (n) {\n"
                 "          do {\n"
                 "            --n;\n"
                 "            /* Can we shift an error token? */\n");
  fprintf(outfp, "            int err_action = %sparse_table[%snum_columns * stack->stack_[n].state_ + (%d /* error token */ - %sminimum_sym)];\n", cc_prefix(cc), cc_prefix(cc), cc->error_sym_->ordinal_, cc_prefix(cc));
  fprintf(outfp, "            if (err_action > 0) {\n");
  fprintf(outfp, "              /* Does the resulting state accept the current symbol? */\n"
                 "              int err_sym_action = %sparse_table[%snum_columns * stack->stack_[n].state_ + (sym - %sminimum_sym)];\n", cc_prefix(cc), cc_prefix(cc), cc_prefix(cc));
  fprintf(outfp, "              if (err_sym_action) {\n"
                 "                /* Current symbol is accepted, recover error condition by shifting the error token and then process the symbol as usual */\n");

  fprintf(outfp, "                /* Free symdata for every symbol up to the state where we will shift the error token */\n"
                 "                size_t %ssym_idx;\n", cc_prefix(cc));
  fprintf(outfp, "                for (%ssym_idx = n + 1; %ssym_idx < stack->pos_; ++%ssym_idx) {\n", cc_prefix(cc), cc_prefix(cc), cc_prefix(cc));

  fprintf(outfp, "                  switch (stack->stack_[%ssym_idx].state_) {\n", cc_prefix(cc));
  for (typestr_idx = 0; typestr_idx < cc->tstab_.num_typestrs_; ++typestr_idx) {
    struct typestr *ts = cc->tstab_.typestrs_[typestr_idx];
    if (ts->destructor_snippet_.num_tokens_) {
      int have_cases = 0; /* always true if all types are always used */
      /* Type has a destructor associated.. Find all state for whose corresponding symbol has the associated type */
      size_t state_idx;
      for (state_idx = 0; state_idx < lalr->nr_states_; ++state_idx) {
        struct symbol *sym = symbol_find_by_ordinal(&cc->symtab_, state_syms[state_idx]);
        if (!sym) continue;
        if (sym->assigned_type_ == ts) {
          fprintf(outfp, "                    case %d: /* %s */\n", (int)state_idx, sym->def_.translated_);
          have_cases = 1;
        }
      }
      if (have_cases) {
        fprintf(outfp, "                    {\n"
                       "                       ");
        if (emit_destructor_snippet(outfp, cc, ts)) {
          r = EXIT_FAILURE;
          goto cleanup_exit;
        }

        /* Close this compound block  */
        fprintf(outfp, "\n"
                       "                    }\n"
                       "                    break;\n");
      }
    }
  }
  fprintf(outfp, "                  } /* switch */\n");
  if (cc->common_data_assigned_type_ && cc->common_data_assigned_type_->destructor_snippet_.num_tokens_) {
    fprintf(outfp, "                  {\n"
                   "                    ");
    if (emit_common_destructor_snippet(outfp, cc)) {
      r = EXIT_FAILURE;
      goto cleanup_exit;
    }
    fprintf(outfp, "\n"
                   "                  }\n");
  }
  /* Enumerate all states, for each state, determine the type corresponding to the state from the symbol corresponding to the state. */
  fprintf(outfp, "                } /* for */\n"
                 "                stack->pos_ = n + 1;\n");

  fprintf(outfp, "                /* Push the state of the error transition */\n"
                 "                switch (%spush_state(stack, err_action /* action for a shift is the state */)) {\n"
                 "                  case -1: /* overflow */ {\n"
                 "                    ", cc_prefix(cc));
  if (cc->on_internal_error_snippet_.num_tokens_) {
    size_t token_idx;
    for (token_idx = 0; token_idx < cc->on_internal_error_snippet_.num_tokens_; ++token_idx) {
      fprintf(outfp, "%s", cc->on_internal_error_snippet_.tokens_[token_idx].text_.original_);
    }
  }
  else {
    fprintf(outfp, "return -2;\n");
  }
  fprintf(outfp, "                  }\n"
                 "                  break;\n"
                 "                  case -2: /* out of memory */ {\n"
                 "                    ");
  if (cc->on_alloc_error_snippet_.num_tokens_) {
    size_t token_idx;
    for (token_idx = 0; token_idx < cc->on_alloc_error_snippet_.num_tokens_; ++token_idx) {
      fprintf(outfp, "%s", cc->on_alloc_error_snippet_.tokens_[token_idx].text_.original_);
    }
  }
  else {
    fprintf(outfp, "return -2;\n");
  }
  fprintf(outfp, "                  }\n"
                 "                  break;\n"
                 "                } /* switch */\n");

  fprintf(outfp, "                stack->error_recovery_ = 0;\n");
  fprintf(outfp, "                /* Break out of do { .. } while loop, we've recovered */\n"
                 "                break;\n");

  fprintf(outfp, "              } /* if (err_sym_action) (if the current sym can continue after an error transition) */\n");
  fprintf(outfp, "            } /* if (err_action) (if the state at position 'n' can accept an error transition) */\n");

  fprintf(outfp, "          } while (n);\n"
                 "        }\n");

  fprintf(outfp, "        if (stack->error_recovery_) {\n"
                 "          /* Did not yet recover, discard current sym and get next */\n"
                 "          stack->need_sym_ = 1;\n");
  if (cc->on_next_token_snippet_.num_tokens_) {
    size_t token_idx;
    for (token_idx = 0; token_idx < cc->on_next_token_snippet_.num_tokens_; ++token_idx) {
      fprintf(outfp, "%s", cc->on_next_token_snippet_.tokens_[token_idx].text_.original_);
    }
  }
  fprintf(outfp, "        }\n");
  fprintf(outfp, "      } /* stack->error_recovery_ */\n");
  fprintf(outfp, "    } /* for (;;) */\n");
  fprintf(outfp, "  } /* for (;;) lexing loop */\n");
  fprintf(outfp, "}\n");

  r = 0;
cleanup_exit:
  return r;
}


static int emit_parse_function(FILE *outfp, struct cinder_context *cc, struct prd_grammar *prdg, struct lr_generator *lalr, int *state_syms) {
  int r;

  /* Emit the parse function */
  if (cc->params_snippet_.num_tokens_) {
    fprintf(outfp, "int %sparse(struct %sstack *stack, int sym, ", cc_prefix(cc), cc_prefix(cc));
    size_t token_idx;
    for (token_idx = 0; token_idx < cc->params_snippet_.num_tokens_; ++token_idx) {
      fprintf(outfp, "%s", cc->params_snippet_.tokens_[token_idx].text_.original_);
    }
    fprintf(outfp, ") {\n");
  }
  else {
    fprintf(outfp, "int %sparse(struct %sstack *stack, int sym) {\n", cc_prefix(cc), cc_prefix(cc));
  }
  fprintf(outfp, "  if (stack->mute_error_turns_) stack->mute_error_turns_--;\n");
  fprintf(outfp, "  for (;;) {\n"
                 "    if (!stack->error_recovery_) {\n"
                 "      int action = %sparse_table[%snum_columns * stack->stack_[stack->pos_ - 1].state_ + (sym - %sminimum_sym)];\n", cc_prefix(cc), cc_prefix(cc), cc_prefix(cc));

  /* Shift logic */
  fprintf(outfp, "      if (action > 0) {\n");
  fprintf(outfp, "        switch (%spush_state(stack, action /* action for a shift is the ordinal */)) {\n"
                 "          case -1: /* overflow */ {\n"
                 "            ", cc_prefix(cc));
  if (cc->on_internal_error_snippet_.num_tokens_) {
    size_t token_idx;
    for (token_idx = 0; token_idx < cc->on_internal_error_snippet_.num_tokens_; ++token_idx) {
      fprintf(outfp, "%s", cc->on_internal_error_snippet_.tokens_[token_idx].text_.original_);
    }
  }
  else {
    fprintf(outfp, "return -2;\n");
  }
  fprintf(outfp, "          }\n"
                 "          break;\n"
                 "          case -2: /* out of memory */ {\n"
                 "            ");
  if (cc->on_alloc_error_snippet_.num_tokens_) {
    size_t token_idx;
    for (token_idx = 0; token_idx < cc->on_alloc_error_snippet_.num_tokens_; ++token_idx) {
      fprintf(outfp, "%s", cc->on_alloc_error_snippet_.tokens_[token_idx].text_.original_);
    }
  }
  else {
    fprintf(outfp, "return -2;\n");
  }
  fprintf(outfp, "          }\n"
                 "          break;\n"
                 "        } /* switch */\n");

  fprintf(outfp, "\n"
                 "        /* Fill in the sym from the tokenizer */\n");
  int need_sym_data = 0;
  if (cc->token_assigned_type_ && cc->token_assigned_type_->constructor_snippet_.num_tokens_) {
    need_sym_data = 1;
  }
  if (cc->token_action_snippet_.num_tokens_) {
    need_sym_data = 1;
  }
  if (cc->common_data_assigned_type_ && cc->common_data_assigned_type_->constructor_snippet_.num_tokens_) {
    need_sym_data = 1;
  }
  if (need_sym_data) {
    fprintf(outfp, "        struct %ssym_data *sym_data = stack->stack_ + stack->pos_ - 1;\n", cc_prefix(cc));
  }
  if (cc->common_data_assigned_type_ && cc->common_data_assigned_type_->constructor_snippet_.num_tokens_) {
    fprintf(outfp, "        {\n"
                   "          ");
    if (emit_token_common_constructor_snippet(outfp, cc)) {
      r = EXIT_FAILURE;
      goto cleanup_exit;
    }
    fprintf(outfp, "\n"
                   "        }\n");
  }
  if (cc->token_assigned_type_ && cc->token_assigned_type_->constructor_snippet_.num_tokens_) {
    fprintf(outfp, "        {\n"
                   "          ");
    if (emit_token_constructor_snippet(outfp, cc, cc->token_assigned_type_)) {
      r = EXIT_FAILURE;
      goto cleanup_exit;
    }
    fprintf(outfp, "\n"
                   "        }\n");
  }
  if (cc->token_action_snippet_.num_tokens_) {
    fprintf(outfp, "        {\n"
                   "          ");

    if (emit_token_action_snippet(outfp, cc, &cc->token_action_snippet_)) {
      r = EXIT_FAILURE;
      goto cleanup_exit;
    }

    fprintf(outfp, "\n"
                   "        }\n");
  }
  fprintf(outfp, "        if (stack->report_error_) {\n"
                 "          /* We're shifting this sym following an error recovery on the same sym, report syntax error */\n"
                 "          stack->report_error_ = 0;\n"
                 "          ");
  if (cc->on_syntax_error_snippet_.num_tokens_) {
    size_t token_idx;
    for (token_idx = 0; token_idx < cc->on_syntax_error_snippet_.num_tokens_; ++token_idx) {
      fprintf(outfp, "%s", cc->on_syntax_error_snippet_.tokens_[token_idx].text_.original_);
    }
  }
  else {
    fprintf(outfp, "/* Syntax error */\n"
                   "          return -1;\n");
  }
  fprintf(outfp, "        }\n"
                 "        else {\n"
                 "          ");
  if (cc->on_next_token_snippet_.num_tokens_) {
    size_t token_idx;
    for (token_idx = 0; token_idx < cc->on_next_token_snippet_.num_tokens_; ++token_idx) {
      fprintf(outfp, "%s", cc->on_next_token_snippet_.tokens_[token_idx].text_.original_);
    }
  }
  else {
    fprintf(outfp, "/* Next token */\n"
                   "          return 1;\n");
  }
  fprintf(outfp, "        }\n");
  fprintf(outfp, "      } /* action > 0 */\n");

  fprintf(outfp, "      else if (action < 0) {\n"
                 "        int production = -action - 1;\n"
                 "        int discard_action = 0;\n"
                 "        size_t production_length = %sproduction_lengths[production];\n", cc_prefix(cc));
  fprintf(outfp, "        int nonterminal = %sproduction_syms[production];\n", cc_prefix(cc));
  fprintf(outfp, "        if (0 == production) {\n"
                 "          ");
  if (cc->on_success_snippet_.num_tokens_) {
    size_t token_idx;
    for (token_idx = 0; token_idx < cc->on_success_snippet_.num_tokens_; ++token_idx) {
      fprintf(outfp, "%s", cc->on_success_snippet_.tokens_[token_idx].text_.original_);
    }
  }
  else {
    fprintf(outfp, "/* Synth S we're done */\n"
                   "          return 0;\n");
  }

  fprintf(outfp, "        }\n"
                 "\n"
                 "        struct %ssym_data nonterminal_sym_data_reduced_to = { 0 };\n", cc_prefix(cc));

  fprintf(outfp, "        { /* scope guard */\n"
                 "          struct %ssym_data *sym_data = stack->stack_ + stack->pos_ - production_length;\n", cc_prefix(cc));
  /* Emit any requested locals */
  if (cc->locals_snippet_.num_tokens_) {
    fprintf(outfp, "          ");
    size_t token_idx;
    for (token_idx = 0; token_idx < cc->locals_snippet_.num_tokens_; ++token_idx) {
      fprintf(outfp, "%s", cc->locals_snippet_.tokens_[token_idx].text_.original_);
    }
    fprintf(outfp, "\n");
  }
   
  fprintf(outfp, "          switch (production) {\n");
  size_t row;
  for (row = 0; row < prdg->num_productions_; ++row) {
    struct prd_production *pd = prdg->productions_ + row;
    fprintf(outfp, "            /* %s:", pd->nt_.id_.translated_);
    size_t n;
    for (n = 0; n < pd->num_syms_; ++n) {
      fprintf(outfp, " %s", pd->syms_[n].id_.translated_);
    }
    fprintf(outfp, " */\n");
    fprintf(outfp, "            case %d: {\n    ", (int)row + 1);
    /* Emit dst_sym_data constructor first */
    if (cc->common_data_assigned_type_ && cc->common_data_assigned_type_->constructor_snippet_.num_tokens_) {
      if (emit_dst_common_constructor_snippet(outfp, cc)) {
        r = EXIT_FAILURE;
        goto cleanup_exit;
      }
      fprintf(outfp, "\n"
                     "            }\n"
                     "            {\n"
                     "              ");
    }
    if (pd->nt_.sym_->assigned_type_ && pd->nt_.sym_->assigned_type_->constructor_snippet_.num_tokens_) {
      if (emit_dst_sym_constructor_snippet(outfp, cc, pd->nt_.sym_->assigned_type_)) {
        r = EXIT_FAILURE;
        goto cleanup_exit;
      }
      fprintf(outfp, "\n"
                     "            }\n"
                     "            {\n"
                     "              ");
    }
    if (pd->common_action_sequence_.num_tokens_) {
      if (emit_common_action_snippet(outfp, cc, pd)) {
        r = EXIT_FAILURE;
        goto cleanup_exit;
      }
      fprintf(outfp, "\n"
                     "            }\n"
                     "            if (!discard_action) {\n"
                     "              ");
    }

    if (emit_action_snippet(outfp, cc, pd)) {
      r = EXIT_FAILURE;
      goto cleanup_exit;
    }

    fprintf(outfp, "            }\n"
                   "            break;\n");
  }
  fprintf(outfp, "          } /* switch */\n");
  fprintf(outfp, "        } /* scope guard */\n");
  fprintf(outfp, "\n");
  fprintf(outfp, "        /* Free symdata for every symbol in the production, including the first slot where we will soon\n"
                 "         * push nonterminal_data_reduced_to */\n"
                 "        size_t %ssym_idx;\n", cc_prefix(cc));
  fprintf(outfp, "        for (%ssym_idx = stack->pos_ - production_length; %ssym_idx < stack->pos_; ++%ssym_idx) {\n", cc_prefix(cc), cc_prefix(cc), cc_prefix(cc));

  fprintf(outfp, "          switch (stack->stack_[%ssym_idx].state_) {\n", cc_prefix(cc));
  size_t typestr_idx;
  for (typestr_idx = 0; typestr_idx < cc->tstab_.num_typestrs_; ++typestr_idx) {
    struct typestr *ts = cc->tstab_.typestrs_[typestr_idx];
    if (ts->destructor_snippet_.num_tokens_) {
      int have_cases = 0; /* always true if all types are always used */
      /* Type has a destructor associated.. Find all state for whose corresponding symbol has the associated type */
      size_t state_idx;
      for (state_idx = 0; state_idx < lalr->nr_states_; ++state_idx) {
        struct symbol *sym = symbol_find_by_ordinal(&cc->symtab_, state_syms[state_idx]);
        if (!sym) continue;
        if (sym->assigned_type_ == ts) {
          fprintf(outfp, "            case %d: /* %s */\n", (int)state_idx, sym->def_.translated_);
          have_cases = 1;
        }
      }
      if (have_cases) {
        fprintf(outfp, "            {\n"
                       "              ");
        if (emit_destructor_snippet(outfp, cc, ts)) {
          r = EXIT_FAILURE;
          goto cleanup_exit;
        }

        fprintf(outfp, "\n"
                       "            }\n"
                       "            break;\n");
      }
    }
  }
  /* Enumerate all states, for each state, determine the type corresponding to the state from the symbol corresponding to the state. */
  fprintf(outfp, "          } /* switch */\n");
  if (cc->common_data_assigned_type_ && cc->common_data_assigned_type_->constructor_snippet_.num_tokens_) {
    fprintf(outfp, "        {\n"
                   "          ");
    if (emit_common_destructor_snippet(outfp, cc)) {
      r = EXIT_FAILURE;
      goto cleanup_exit;
    }
    fprintf(outfp, "\n"
                   "        }\n");
  }

  fprintf(outfp, "        } /* for */\n"
                 "        stack->pos_ -= production_length;\n"
                 "        action = %sparse_table[%snum_columns * stack->stack_[stack->pos_ - 1].state_ + (nonterminal - %sminimum_sym)];\n", cc_prefix(cc), cc_prefix(cc), cc_prefix(cc));
  fprintf(outfp, "        if (action <= 0) {\n"
                 "          ");
  if (cc->on_internal_error_snippet_.num_tokens_) {
    size_t token_idx;
    for (token_idx = 0; token_idx < cc->on_internal_error_snippet_.num_tokens_; ++token_idx) {
      fprintf(outfp, "%s", cc->on_internal_error_snippet_.tokens_[token_idx].text_.original_);
    }
  }
  else {
    fprintf(outfp, "/* Internal error: cannot shift an already reduced non-terminal */\n"
                   "          return -2;\n");
  }
  fprintf(outfp, "        }\n");

  fprintf(outfp, "        switch (%spush_state(stack, action /* action for a \"goto\" shift is the ordinal */)) {\n"
                 "          case -1: /* overflow */ {\n"
                 "            ", cc_prefix(cc));
  if (cc->on_internal_error_snippet_.num_tokens_) {
    size_t token_idx;
    for (token_idx = 0; token_idx < cc->on_internal_error_snippet_.num_tokens_; ++token_idx) {
      fprintf(outfp, "%s", cc->on_internal_error_snippet_.tokens_[token_idx].text_.original_);
    }
  }
  else {
    fprintf(outfp, "return -2;\n");
  }
  fprintf(outfp, "          }\n"
                 "          break;\n"
                 "          case -2: /* out of memory */ {\n"
                 "            ");
  if (cc->on_alloc_error_snippet_.num_tokens_) {
    size_t token_idx;
    for (token_idx = 0; token_idx < cc->on_alloc_error_snippet_.num_tokens_; ++token_idx) {
      fprintf(outfp, "%s", cc->on_alloc_error_snippet_.tokens_[token_idx].text_.original_);
    }
  }
  else {
    fprintf(outfp, "return -2;\n");
  }
  fprintf(outfp, "          }\n"
                 "          break;\n"
                 "        } /* switch */\n");
  fprintf(outfp, "        struct %ssym_data *sd = stack->stack_ + stack->pos_ - 1;\n", cc_prefix(cc));
  fprintf(outfp, "        *sd = nonterminal_sym_data_reduced_to;\n"
                 "        sd->state_ = action;\n");
  fprintf(outfp, "      } /* action < 0 */\n"
                 "      else /* action == 0 */ {\n"
                 "        stack->error_recovery_ = 1;\n"
                 "        stack->report_error_ = !stack->mute_error_turns_;\n"
                 "        stack->mute_error_turns_ = 3;\n"
                 "      }\n");
  fprintf(outfp, "    } /* !stack->error_recovery_ */\n"
                 "    if (stack->error_recovery_) {\n");
  fprintf(outfp, "      size_t n;\n"
                 "      n = stack->pos_;\n"
                 "      if (n) {\n"
                 "        do {\n"
                 "          --n;\n"
                 "          /* Can we shift an error token? */\n");
  fprintf(outfp, "          int err_action = %sparse_table[%snum_columns * stack->stack_[n].state_ + (%d /* error token */ - %sminimum_sym)];\n", cc_prefix(cc), cc_prefix(cc), cc->error_sym_->ordinal_, cc_prefix(cc));
  fprintf(outfp, "          if (err_action > 0) {\n");
  fprintf(outfp, "            /* Does the resulting state accept the current symbol? */\n"
                 "            int err_sym_action = %sparse_table[%snum_columns * stack->stack_[n].state_ + (sym - %sminimum_sym)];\n", cc_prefix(cc), cc_prefix(cc), cc_prefix(cc));
  fprintf(outfp, "            if (err_sym_action) {\n"
                 "              /* Current symbol is accepted, recover error condition by shifting the error token and then process the symbol as usual */\n");

  fprintf(outfp, "              /* Free symdata for every symbol up to the state where we will shift the error token */\n"
                 "              size_t %ssym_idx;\n", cc_prefix(cc));
  fprintf(outfp, "              for (%ssym_idx = n + 1; %ssym_idx < stack->pos_; ++%ssym_idx) {\n", cc_prefix(cc), cc_prefix(cc), cc_prefix(cc));

  fprintf(outfp, "                switch (stack->stack_[%ssym_idx].state_) {\n", cc_prefix(cc));
  for (typestr_idx = 0; typestr_idx < cc->tstab_.num_typestrs_; ++typestr_idx) {
    struct typestr *ts = cc->tstab_.typestrs_[typestr_idx];
    if (ts->destructor_snippet_.num_tokens_) {
      int have_cases = 0; /* always true if all types are always used */
      /* Type has a destructor associated.. Find all state for whose corresponding symbol has the associated type */
      size_t state_idx;
      for (state_idx = 0; state_idx < lalr->nr_states_; ++state_idx) {
        struct symbol *sym = symbol_find_by_ordinal(&cc->symtab_, state_syms[state_idx]);
        if (!sym) continue;
        if (sym->assigned_type_ == ts) {
          fprintf(outfp, "                  case %d: /* %s */\n", (int)state_idx, sym->def_.translated_);
          have_cases = 1;
        }
      }
      if (have_cases) {
        fprintf(outfp, "                  {\n"
                       "                     ");
        if (emit_destructor_snippet(outfp, cc, ts)) {
          r = EXIT_FAILURE;
          goto cleanup_exit;
        }

        /* Close this compound block  */
        fprintf(outfp, "\n"
                       "                  }\n"
                       "                  break;\n");
      }
    }
  }
  fprintf(outfp, "                } /* switch */\n");
  if (cc->common_data_assigned_type_ && cc->common_data_assigned_type_->destructor_snippet_.num_tokens_) {
    fprintf(outfp, "                {\n"
                   "                  ");
    if (emit_common_destructor_snippet(outfp, cc)) {
      r = EXIT_FAILURE;
      goto cleanup_exit;
    }
    fprintf(outfp, "\n"
                   "                }\n");
  }
  /* Enumerate all states, for each state, determine the type corresponding to the state from the symbol corresponding to the state. */
  fprintf(outfp, "              } /* for */\n"
                 "              stack->pos_ = n + 1;\n");

  fprintf(outfp, "              /* Push the state of the error transition */\n"
                 "              switch (%spush_state(stack, err_action /* action for a shift is the state */)) {\n"
                 "                case -1: /* overflow */ {\n"
                 "                  ", cc_prefix(cc));
  if (cc->on_internal_error_snippet_.num_tokens_) {
    size_t token_idx;
    for (token_idx = 0; token_idx < cc->on_internal_error_snippet_.num_tokens_; ++token_idx) {
      fprintf(outfp, "%s", cc->on_internal_error_snippet_.tokens_[token_idx].text_.original_);
    }
  }
  else {
    fprintf(outfp, "return -2;\n");
  }
  fprintf(outfp, "                }\n"
                 "                break;\n"
                 "                case -2: /* out of memory */ {\n"
                 "                  ");
  if (cc->on_alloc_error_snippet_.num_tokens_) {
    size_t token_idx;
    for (token_idx = 0; token_idx < cc->on_alloc_error_snippet_.num_tokens_; ++token_idx) {
      fprintf(outfp, "%s", cc->on_alloc_error_snippet_.tokens_[token_idx].text_.original_);
    }
  }
  else {
    fprintf(outfp, "return -2;\n");
  }
  fprintf(outfp, "                }\n"
                 "                break;\n"
                 "              } /* switch */\n");

  fprintf(outfp, "              stack->error_recovery_ = 0;\n");
  fprintf(outfp, "              /* Break out of do { .. } while loop, we've recovered */\n"
                 "              break;\n");

  fprintf(outfp, "            } /* if (err_sym_action) (if the current sym can continue after an error transition) */\n");
  fprintf(outfp, "          } /* if (err_action) (if the state at position 'n' can accept an error transition) */\n");

  fprintf(outfp, "        } while (n);\n"
                 "      }\n");

  fprintf(outfp, "      if (stack->error_recovery_) {\n"
                 "        /* Did not yet recover, discard current sym and get next */\n"
                 "        ");
  if (cc->on_next_token_snippet_.num_tokens_) {
    size_t token_idx;
    for (token_idx = 0; token_idx < cc->on_next_token_snippet_.num_tokens_; ++token_idx) {
      fprintf(outfp, "%s", cc->on_next_token_snippet_.tokens_[token_idx].text_.original_);
    }
  }
  else {
    fprintf(outfp, "/* Next token */\n"
                   "        return 1;\n");
  }
  fprintf(outfp, "      }\n");
  fprintf(outfp, "    } /* stack->error_recovery_ */\n");
  fprintf(outfp, "  } /* for (;;) */\n");
  fprintf(outfp, "}\n");

  r = 0;
cleanup_exit:
  return r;
}

void print_usage(void) {
  fprintf(stderr, "Cinder parser generator (C) 2020 Kinglet B.V.\n"
                  "https://firecrest.com/cinder\n"
                  "\n"
                  "cinder <inputfile.cnd> [ --c [ <c_filename> [--h [h_filename] ] ] ]\n"
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

  char **cpv = argv + 1;
  int cr = argc - 1;
  int expecting_inputfile = 1;
  int expecting_hfile = 0;
  int expecting_cfile = 0;
  int generate_cfile = 1;
  int generate_hfile = 0;
  char *input_filename = NULL;
  char *h_output_filename = NULL;
  char *c_output_filename = NULL;
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
      if (h_output_filename) {
        re_error_nowhere("Error: only one C header output file permitted");
        print_usage();
        goto exit_arg_eval;
      }
      if (!strcmp("-", *cpv)) {
        /* H output filename derived from C output filename */
      }
      else {
        h_output_filename = strdup(*cpv);
        if (!h_output_filename) {
          re_error_nowhere("Error: no memory");
          goto exit_arg_eval;
        }
      }
      expecting_hfile = 0;
      cr--; cpv++;
    }
    else if (expecting_cfile) {
      if (c_output_filename) {
        re_error_nowhere("Error: only one C output file permitted");
        print_usage();
        goto exit_arg_eval;
      }
      c_output_filename = strdup(*cpv);
      if (!c_output_filename) {
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

  if (generate_hfile && !h_output_filename) {
    if (!c_output_filename) {
      re_error_nowhere("Error: Need C output filename to derive a C header output filename");
      goto exit_arg_eval;
    }
    const char *ext = strrchr(c_output_filename, '.');
    if (!ext || (strlen(ext) < 2)) {
      re_error_nowhere("Error: Need C output filename that ends in a filename extension to derive a C header output filename");
      goto exit_arg_eval;
    }
    h_output_filename = strdup(c_output_filename);
    memcpy(h_output_filename + (ext - c_output_filename), ".h", 3 /* inc terminator */);
  }

  if (0) {
  exit_arg_eval:
    if (input_filename) free(input_filename);
    if (h_output_filename) free(h_output_filename);
    if (c_output_filename) free(c_output_filename);
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

  struct cinder_context cc;
  cinder_context_init(&cc);

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

  int *state_syms = NULL;

  char *include_guard = NULL;

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
          case LD_CINDER_SCANNER_SECTION_DELIMETER:
            where_are_we = default_mode = SCANNER;
            break;
          case LD_CINDER_GRAMMAR_SECTION_DELIMETER:
            where_are_we = default_mode = GRAMMAR;
            break;
          case LD_CINDER_SECTION_DELIMITER:
            where_are_we = default_mode;
            break;
          case LD_REGULAR:
            /* Preserve line continuations */
            append_part(&prologue, token_buf.num_original_, token_buf.original_);
            break;
          case LD_CINDER_DIRECTIVE:
            r = process_cinder_directive(&tkr_tokens, &token_buf, &cc);
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
          case LD_CINDER_GRAMMAR_SECTION_DELIMETER:
            re_error_tkr(&tkr_lines, "Error, already in grammar area");
            have_error = 1;
            break;
          case LD_CINDER_SCANNER_SECTION_DELIMETER:
            /* Finish up */
            r = process_grammar_tokens(&tkr_tokens, &token_buf, 1, &prds, &prdg, &cc.symtab_);

            where_are_we = default_mode = SCANNER;
            break;
          case LD_CINDER_SECTION_DELIMITER:
            /* Finish up */
            r = process_grammar_tokens(&tkr_tokens, &token_buf, 1, &prds, &prdg, &cc.symtab_);

            where_are_we = EPILOGUE;
            break;
          case LD_REGULAR:
          {
            r = process_grammar_tokens(&tkr_tokens, &token_buf, 0, &prds, &prdg, &cc.symtab_);
            break;
          }
          case LD_CINDER_DIRECTIVE:
            r = process_cinder_directive(&tkr_tokens, &token_buf, &cc);
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
          case LD_CINDER_SCANNER_SECTION_DELIMETER:
            re_error_tkr(&tkr_lines, "Error, already in scanner area");
            have_error = 1;
            break;
          case LD_CINDER_GRAMMAR_SECTION_DELIMETER:
            /* Finish up */
            r = process_scanner_tokens(&tkr_tokens, &token_buf, 1, &rxgs, &prdg, &cc.symtab_);

            where_are_we = default_mode = GRAMMAR;
            break;
          case LD_CINDER_SECTION_DELIMITER:
            /* Finish up */
            r = process_scanner_tokens(&tkr_tokens, &token_buf, 1, &rxgs, &prdg, &cc.symtab_);

            where_are_we = EPILOGUE;
            break;
          case LD_REGULAR:
            r = process_scanner_tokens(&tkr_tokens, &token_buf, 0, &rxgs, &prdg, &cc.symtab_);
            break;
          case LD_CINDER_DIRECTIVE:
            r = process_cinder_directive(&tkr_tokens, &token_buf, &cc);
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
          case LD_CINDER_SCANNER_SECTION_DELIMETER:
          case LD_CINDER_GRAMMAR_SECTION_DELIMETER:
          case LD_CINDER_SECTION_DELIMITER:
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
          if (tkr_lines.best_match_variant_ == LD_CINDER_SCANNER_SECTION_DELIMETER) {
            where_are_we = default_mode = SCANNER;
          }
          else if (tkr_lines.best_match_variant_ == LD_CINDER_GRAMMAR_SECTION_DELIMETER) {
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
          case LD_CINDER_DIRECTIVE:
            r = process_cinder_directive(&tkr_tokens, &token_buf, &cc);
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
  struct typestr *token_ts = NULL;
  if (cc.token_type_.num_tokens_) {
    int is_new;
    token_ts = typestr_find_or_add(&cc.tstab_, &cc.token_type_, &is_new);
    if (!token_ts) {
      r = EXIT_FAILURE;
      goto cleanup_exit;
    }
  }

  sym = cc.symtab_.terminals_;
  if (sym) {
    do {
      sym = sym->next_;

      if (!sym->type_snippet_.num_tokens_) {
        sym->assigned_type_ = token_ts;
      }
      else {
        int is_new;
        struct typestr *ts = typestr_find_or_add(&cc.tstab_, &sym->type_snippet_, &is_new);
        if (!ts) {
          r = EXIT_FAILURE;
          goto cleanup_exit;
        }
        sym->assigned_type_ = ts;
      }
    } while (sym != cc.symtab_.terminals_);
  }
  sym = cc.symtab_.non_terminals_;
  if (sym) {
    do {
      sym = sym->next_;

      if (!sym->type_snippet_.num_tokens_) {
        sym->assigned_type_ = NULL;
      }
      else {
        int is_new;
        struct typestr *ts = typestr_find_or_add(&cc.tstab_, &sym->type_snippet_, &is_new);
        if (!ts) {
          r = EXIT_FAILURE;
          goto cleanup_exit;
        }
        sym->assigned_type_ = ts;
      }

    } while (sym != cc.symtab_.non_terminals_);
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
    if (c_output_filename) {
      outfp = fopen(c_output_filename, "wb");
      if (!outfp) {
        int err = errno;
        re_error_nowhere("Failed to open file \"%s\" for writing: %s", c_output_filename, strerror(err));
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
          re_error_nowhere("Failed to write to \"%s\": %s", c_output_filename, strerror(err));
          r = EXIT_FAILURE;
          goto cleanup_exit;
        }

      } while (pt != prologue);
    }

    /* Start generating files.. */

    if (h_output_filename) {
      /* First we develop the "include_guard" - this is used by the header file as the "#ifndef HEADER_INCLUDED"
       * header duplicate inclusion guard, and used by the C file to detect whether the header has been included
       * (and the declarations already made.) */
      const char *cinder = "CINDER_";
      size_t cinder_len = strlen(cinder);
      size_t prefix_len = strlen(cc_PREFIX(&cc));
      size_t header_filename_len = strlen(h_output_filename);
      const char *included = "_INCLUDED";
      size_t included_len = strlen(included);
      size_t include_guard_size = cinder_len + prefix_len + header_filename_len + included_len + 1;
      include_guard = (char *)malloc(include_guard_size);
      include_guard[0] = '\0';
      strcat(include_guard, cinder);
      strcat(include_guard, cc_PREFIX(&cc));
      size_t n;
      char *p = include_guard + cinder_len + prefix_len;
      for (n = 0; n < header_filename_len; ++n) {
        char c = h_output_filename[n];
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
      include_guard = NULL;
    }

    fprintf(outfp, "/* --------- START OF GENERATED CODE ------------ */\n");

    fprintf(outfp, "#include <stdlib.h> /* realloc(), free(), NULL, size_t */\n");

    fprintf(outfp, "struct %ssym_data {\n", cc_prefix(&cc));
    fprintf(outfp, "  int state_;\n");
    if (cc.common_data_assigned_type_) {
      struct typestr *ts = cc.common_data_assigned_type_;
      fprintf(outfp, "  ");
      int found_placeholder = 0;
      size_t tok_idx;
      for (tok_idx = 0; tok_idx < ts->typestr_snippet_.num_tokens_; ++tok_idx) {
        struct snippet_token *st = ts->typestr_snippet_.tokens_ + tok_idx;
        if (st->variant_ != TOK_SPECIAL_IDENT) {
          fprintf(outfp, "%s%s", tok_idx ? " " : "", st->text_.translated_);
        }
        else {
          found_placeholder = 1;
          fprintf(outfp, " common_");
        }
      }
      if (!found_placeholder) {
        /* Placeholder is implied at the end */
        fprintf(outfp, " common_");
      }
      fprintf(outfp, ";\n");
    }
    if (cc.have_typed_symbols_) {
      fprintf(outfp, "  union {\n");
      size_t ts_idx;
      for (ts_idx = 0; ts_idx < cc.tstab_.num_typestrs_; ++ts_idx) {
        struct typestr *ts;
        ts = cc.tstab_.typestrs_[ts_idx];
        int found_placeholder = 0;

        if (ts == cc.common_data_assigned_type_) {
          /* Skip the common data type, as it is not part of the union but shared by all */
          continue;
        }

        fprintf(outfp, "    ");
        size_t tok_idx;
        for (tok_idx = 0; tok_idx < ts->typestr_snippet_.num_tokens_; ++tok_idx) {
          struct snippet_token *st = ts->typestr_snippet_.tokens_ + tok_idx;
          if (st->variant_ != TOK_SPECIAL_IDENT) {
            fprintf(outfp, "%s%s", tok_idx ? " " : "", st->text_.translated_);
          }
          else {
            found_placeholder = 1;
            fprintf(outfp, " uv%d_", ts->ordinal_);
          }
        }
        if (!found_placeholder) {
          /* Placeholder is implied at the end */
          fprintf(outfp, " uv%d_", ts->ordinal_);
        }
        fprintf(outfp, ";\n");
      }
      fprintf(outfp, "  } v_;\n");
    }
    fprintf(outfp, "};\n");

    if (prdg.num_patterns_) {
      fprintf(outfp, "static const size_t %sscan_table[] = {\n", cc_prefix(&cc));
      size_t row, col;
      char column_widths[256] = {0};

      for (col = 0; col < 256; ++col) {
        column_widths[col] = 1;
        for (row = 0; row < scantable.num_states; ++row) {
          size_t v = scantable.transition_table[row * 256 + col];

          int width_needed = 1;
          if (v < 100) {
            width_needed = 2;
          }
          else if (v < 1000) {
            width_needed = 3;
          }
          else if (v < 10000) {
            width_needed = 4;
          }
          else {
            width_needed = 5;
          }
          if (width_needed > column_widths[col]) {
            column_widths[col] = width_needed;
          }
        }
      }

      for (row = 0; row < scantable.num_states; ++row) {
        for (col = 0; col < 256; ++col) {
          fprintf(outfp, "%s%*zu", col ? "," : "  ", column_widths[col], scantable.transition_table[256 * row + col]);
        }
        if (row != (scantable.num_states - 1)) {
          fprintf(outfp, ",\n");
        }
        else {
          fprintf(outfp, "\n");
        }
      }
      fprintf(outfp, "};\n");
      fprintf(outfp, "static const size_t %sscan_actions[] = { ", cc_prefix(&cc));
      for (col = 0; col < scantable.num_states; ++col) {
        fprintf(outfp, "%s%zu", col ? ", " : "", (size_t)scantable.actions[col].action);
      }
      fprintf(outfp, " };\n");
    }

    size_t num_columns = (size_t)(1 + lalr.max_sym_ - lalr.min_sym_);
    fprintf(outfp, "static const int %sminimum_sym = %d;\n", cc_prefix(&cc), lalr.min_sym_);
    fprintf(outfp, "static const size_t %snum_columns = %zu;\n", cc_prefix(&cc), num_columns);
    fprintf(outfp, "static const size_t %snum_rows = %zu;\n", cc_prefix(&cc), (size_t)lalr.nr_states_);
    fprintf(outfp, "static const size_t %snum_productions = %zu;\n", cc_prefix(&cc), lalr.nr_productions_);
    fprintf(outfp, "static const int %sparse_table[] = {\n", cc_prefix(&cc));
    size_t row, col;
    char *column_widths = (char *)malloc(num_columns);
    if (!column_widths) {
      re_error_nowhere("Error, no memory\n");
      r = EXIT_FAILURE;
      goto cleanup_exit;
    }

    for (col = 0; col < num_columns; ++col) {
      column_widths[col] = 1;
      for (row = 0; row < lalr.nr_states_; ++row) {
        int action = lalr.parse_table_[row * num_columns + col];
        int width_needed = 1;
        if (action <= -1000) {
          width_needed = 5;
        }
        else if (action <= -100) {
          width_needed = 4;
        }
        else if (action <= -10) {
          width_needed = 3;
        }
        else if (action < 100) {
          width_needed = 2;
        }
        else if (action < 1000) {
          width_needed = 3;
        }
        else if (action < 10000) {
          width_needed = 4;
        }
        else {
          width_needed = 5;
        }
        if (width_needed > column_widths[col]) {
          column_widths[col] = width_needed;
        }
      }
    }
    for (row = 0; row < lalr.nr_states_; ++row) {
      for (col = 0; col < num_columns; ++col) {
        int action = lalr.parse_table_[row * num_columns + col];

        fprintf(outfp, "%*d,%s", column_widths[col], action, col == (num_columns - 1) ? "\n" : "");
      }
    }
    free(column_widths);
    fprintf(outfp, "};\n");
    fprintf(outfp, "static const size_t %sproduction_lengths[] = {\n", cc_prefix(&cc));
    for (row = 0; row < lalr.nr_productions_; ++row) {
      fprintf(outfp, " %d%s\n", lalr.production_lengths_[row], (row == lalr.nr_productions_ - 1) ? "" : ",");
    }
    fprintf(outfp, "};\n");
    fprintf(outfp, "static const int %sproduction_syms[] = {\n", cc_prefix(&cc));
    for (row = 0; row < lalr.nr_productions_; ++row) {
      fprintf(outfp, " %d%s\n", lalr.productions_[row][0], (row == lalr.nr_productions_ - 1) ? "" : ",");
    }
    fprintf(outfp, "};\n");

    /* For each state, what is the top-most symbol on the stack? */
    fprintf(outfp, "static const int %sstate_syms[] = {\n", cc_prefix(&cc));

    state_syms = (int *)malloc(sizeof(int) * (size_t)lalr.nr_states_);
    if (!state_syms) {
      re_error_nowhere("Error, no memory");
      r = EXIT_FAILURE;
      goto cleanup_exit;
    }
    for (row = 0; row < lalr.nr_states_; ++row) {
      state_syms[row] = -1;
    }
    for (row = 0; row < lalr.nr_states_; ++row) {
      for (col = 0; col < num_columns; ++col) {
        int action = lalr.parse_table_[row * num_columns + col];
        if (action > 0) {
          /* We're shifting to a destination state. */
          int sym_shifting = ((int)col) + lalr.min_sym_;
          int state_shifting_to = action;
          if (state_syms[state_shifting_to] != sym_shifting) {
            if (state_syms[state_shifting_to] == -1) {
              state_syms[state_shifting_to] = sym_shifting;
            }
            else {
              re_error_nowhere("Inconsistent state entry: each state should be entered by 1 unique symbol");
              free(state_syms);
              r = EXIT_FAILURE;
              goto cleanup_exit;
            }
          }
        }
      }
    }
    for (row = 0; row < lalr.nr_states_; ++row) {
      fprintf(outfp, " %d%s\n", state_syms[row], (row == lalr.nr_states_ - 1) ? "" : ",");
    }
    fprintf(outfp, "};\n");


    if (include_guard) {
      fprintf(outfp, "\n#ifndef %s\n", include_guard);
    }

    fprintf(outfp, "struct %sstack {\n", cc_prefix(&cc));
    fprintf(outfp, "  int error_recovery_:1;\n");
    fprintf(outfp, "  int report_error_:1;\n");
    if (prdg.num_patterns_) {
      fprintf(outfp, "  int need_sym_:1;\n"
                     "  int current_sym_;\n");
    }
    fprintf(outfp, "  int mute_error_turns_;\n");
    fprintf(outfp, "  size_t pos_, num_stack_allocated_;\n");
    fprintf(outfp, "  struct %ssym_data *stack_;\n", cc_prefix(&cc));
    if (prdg.num_patterns_) {
      fprintf(outfp, 
              "  size_t scan_state_;\n"
              "  size_t match_index_;\n"
              "  size_t best_match_action_;\n"
              "  size_t best_match_size_;\n"
              "  size_t input_index_;\n"
              "  size_t input_offset_;\n"
              "  size_t match_buffer_size_;\n"
              "  size_t match_buffer_size_allocated_;\n"
              "  /* offset, line and column at the start of match_buffer_ */\n"
              "  size_t match_offset_;\n"
              "  int match_line_;\n"
              "  int match_col_;\n"
              "  /* offset, line and column at the zero-termination (best_match_size_) of a match \n"
              "   * in match_buffer_ (the actual buffer may well be longer.) */\n"
              "  size_t best_match_offset_;\n"
              "  int best_match_line_;\n"
              "  int best_match_col_;\n"
              "  size_t token_size_;\n"
              "  char *match_buffer_;\n"
              "  char terminator_repair_;\n"
              "  int input_line_;\n"
              "  int input_col_;\n");
    }
    fprintf(outfp, "};\n");

    fprintf(outfp, "#define _%sMATCH 1\n", cc_PREFIX(&cc));
    fprintf(outfp, "#define _%sOVERFLOW 2\n", cc_PREFIX(&cc));
    fprintf(outfp, "#define _%sNO_MEMORY 3\n", cc_PREFIX(&cc));
    fprintf(outfp, "#define _%sFEED_ME 4\n", cc_PREFIX(&cc));
    fprintf(outfp, "#define _%sEND_OF_INPUT 5\n", cc_PREFIX(&cc));
    fprintf(outfp, "#define _%sSYNTAX_ERROR 6\n", cc_PREFIX(&cc));
    fprintf(outfp, "\n");

    sym = cc.symtab_.terminals_;
    if (sym) {
      do {
        sym = sym->next_;

        char *ident = malloc(1 + sym->def_.num_translated_);
        char *s = ident;
        const char *p;
        if (!ident) {
          r = EXIT_FAILURE;
          goto cleanup_exit;
        }
        /* Transform into C identifier */
        for (p = sym->def_.translated_; p < (sym->def_.translated_ + sym->def_.num_translated_); ++p) {
          int c = *p;
          if ((c >= 'a') && (c <= 'z')) c = c - 'a' + 'A';
          if (c == '-') c = '_';
          *s++ = c;
        }
        *s++ = '\0';

        fprintf(outfp, "#define %s%s %d\n", cc_TOKEN_PREFIX(&cc), ident, sym->ordinal_);
        free(ident);
      } while (sym != cc.symtab_.terminals_);
    }
    fprintf(outfp, "\n");
    sym = cc.symtab_.non_terminals_;
    if (sym) {
      do {
        sym = sym->next_;

        char *ident = malloc(1 + sym->def_.num_translated_);
        char *s = ident;
        const char *p;
        if (!ident) {
          r = EXIT_FAILURE;
          goto cleanup_exit;
        }
        /* Transform into C identifier */
        for (p = sym->def_.translated_; p < (sym->def_.translated_ + sym->def_.num_translated_); ++p) {
          int c = *p;
          if ((c >= 'a') && (c <= 'z')) c = c - 'a' + 'A';
          if (c == '-') c = '_';
          *s++ = c;
        }
        *s++ = '\0';

        fprintf(outfp, "#define %s%s %d\n", cc_PREFIX(&cc), ident, sym->ordinal_);
        free(ident);
      } while (sym != cc.symtab_.non_terminals_);
    }

    if (include_guard) {
      fprintf(outfp, "#endif /* %s */\n\n", include_guard);
    }

    /* Emit stack constructor, destructor and reset functions */
    fprintf(outfp,
      "void %sstack_init(struct %sstack *stack) {\n", cc_prefix(&cc), cc_prefix(&cc));
    fprintf(outfp,
      "  stack->error_recovery_ = 0;\n"
      "  stack->report_error_ = 0;\n");
    if (prdg.num_patterns_) {
      fprintf(outfp, "  stack->need_sym_ = 1;\n"
                     "  stack->current_sym_ = 0;\n");
    }
    fprintf(outfp, 
      "  stack->mute_error_turns_ = 0;\n"
      "  stack->pos_ = 0;\n"
      "  stack->num_stack_allocated_ = 0;\n"
      "  stack->stack_ = NULL;\n");
    if (prdg.num_patterns_) {
      fprintf(outfp, "  stack->scan_state_ = %zu;\n", scantable.start_state);
      fprintf(outfp, "  stack->input_index_ = 0;\n"
                     "  stack->input_offset_ = 0;\n"
                     "  stack->input_line_ = 1;\n"
                     "  stack->input_col_ = 1;\n"
                     "  stack->match_index_ = 0;\n"
                     "  stack->match_buffer_ = NULL;\n"
                     "  stack->match_buffer_size_ = 0;\n"
                     "  stack->match_buffer_size_allocated_ = 0;\n"
                     "  stack->terminator_repair_ = '\\0';\n"
                     "  stack->token_size_ = 0;\n"
                     "  stack->match_offset_ = 0;\n"
                     "  stack->match_line_ = 1;\n"
                     "  stack->match_col_ = 1;\n");
      fprintf(outfp, "  stack->best_match_action_ = %zu;\n", (size_t)scantable.actions[scantable.start_state].action);
      fprintf(outfp, "  stack->best_match_size_ = 0;\n"
                     "  stack->best_match_offset_ = 0;\n"
                     "  stack->best_match_line_ = 1;\n"
                     "  stack->best_match_col_ = 1;\n");
    }

    fprintf(outfp,
      "}\n"
      "\n");


    fprintf(outfp,
      "void %sstack_cleanup(struct %sstack *stack) {\n", cc_prefix(&cc), cc_prefix(&cc));
    fprintf(outfp,
      "  size_t n;\n"
      "  for (n = 1; n < stack->pos_; ++n) {\n");
    fprintf(outfp,
      "    switch (stack->stack_[n].state_) {\n");
    size_t typestr_idx;
    for (typestr_idx = 0; typestr_idx < cc.tstab_.num_typestrs_; ++typestr_idx) {
      struct typestr *ts = cc.tstab_.typestrs_[typestr_idx];
      if (ts->destructor_snippet_.num_tokens_) {
        int have_cases = 0; /* always true if all types are always used */
        /* Type has a destructor associated.. Find all state for whose corresponding symbol has the associated type */
        size_t state_idx;
        for (state_idx = 0; state_idx < lalr.nr_states_; ++state_idx) {
          struct symbol *sym = symbol_find_by_ordinal(&cc.symtab_, state_syms[state_idx]);
          if (!sym) continue;
          if (sym->assigned_type_ == ts) {
            fprintf(outfp, "    case %d: /* %s */\n", (int)state_idx, sym->def_.translated_);
            have_cases = 1;
          }
        }
        if (have_cases) {
          fprintf(outfp, "    {\n      ");

          if (emit_destructor_snippet_indexed_by_n(outfp, &cc, ts)) {
            r = EXIT_FAILURE;
            goto cleanup_exit;
          }

          /* Close this compound block  */
          fprintf(outfp, "\n    }\n    break;\n");
        }
      }
    }


    fprintf(outfp, "    } /* switch */\n");
    if (cc.common_data_assigned_type_ && cc.common_data_assigned_type_->constructor_snippet_.num_tokens_) {
      fprintf(outfp, "    {\n"
                     "      ");
      if (emit_common_destructor_snippet_indexed_by_n(outfp, &cc)) {
        r = EXIT_FAILURE;
        goto cleanup_exit;
      }
      fprintf(outfp, "\n"
                     "    }\n");
    }

    fprintf(outfp,
      "  }\n"
      "\n"
      "  if (stack->stack_) free(stack->stack_);\n");
    if (prdg.num_patterns_) {
      fprintf(outfp, "    if (stack->match_buffer_) free(stack->match_buffer_);\n");
    }
    fprintf(outfp,
      "}\n"
      "\n");

    fprintf(outfp,
      "static int %spush_state(struct %sstack *stack, int state) {\n", cc_prefix(&cc), cc_prefix(&cc));
    fprintf(outfp,
      "  if (stack->num_stack_allocated_ == stack->pos_) {\n"
      "    size_t new_num_allocated = 0;\n"
      "    if (stack->num_stack_allocated_) {\n"
      "      new_num_allocated = stack->num_stack_allocated_ * 2;\n"
      "      if (new_num_allocated <= stack->num_stack_allocated_) {\n");
    fprintf(outfp,
      "        /* Overflow in allocation */\n"
      "        return -1;\n");
    fprintf(outfp,
      "      }\n"
      "    }\n"
      "    else {\n"
      "      new_num_allocated = 16;\n"
      "    }\n"
      "\n"
      "    if (new_num_allocated > (SIZE_MAX / sizeof(struct %ssym_data))) {\n", cc_prefix(&cc));
    fprintf(outfp,
      "      /* Overflow in allocation */\n"
      "      return -1;\n");
    fprintf(outfp,
      "    }\n"
      "\n"
      "    void *p = realloc(stack->stack_, new_num_allocated * sizeof(struct %ssym_data));\n", cc_prefix(&cc));
    fprintf(outfp,
      "    if (!p) {\n");
    fprintf(outfp,
      "      /* Out of memory */\n"
      "      return -2;\n");
    fprintf(outfp,
      "    }\n"
      "    stack->stack_ = (struct %ssym_data *)p;\n", cc_prefix(&cc));
    fprintf(outfp,
      "    stack->num_stack_allocated_ = new_num_allocated;\n"
      "  }\n"
      "  stack->stack_[stack->pos_++].state_ = state;\n"
      "  return 0;\n"
      "}\n");

    fprintf(outfp,
      "int %sstack_reset(struct %sstack *stack) {\n", cc_prefix(&cc), cc_prefix(&cc));
    fprintf(outfp,
      "  size_t n;\n"
      "  for (n = 0; n < stack->pos_; ++n) {\n");
    fprintf(outfp,
      "    switch (stack->stack_[n].state_) {\n");
    for (typestr_idx = 0; typestr_idx < cc.tstab_.num_typestrs_; ++typestr_idx) {
      struct typestr *ts = cc.tstab_.typestrs_[typestr_idx];
      if (ts->destructor_snippet_.num_tokens_) {
        int have_cases = 0; /* always true if all types are always used */
        /* Type has a destructor associated.. Find all state for whose corresponding symbol has the associated type */
        size_t state_idx;
        for (state_idx = 0; state_idx < lalr.nr_states_; ++state_idx) {
          struct symbol *sym = symbol_find_by_ordinal(&cc.symtab_, state_syms[state_idx]);
          if (!sym) continue;
          if (sym->assigned_type_ == ts) {
            fprintf(outfp, "    case %d: /* %s */\n", (int)state_idx, sym->def_.translated_);
            have_cases = 1;
          }
        }
        if (have_cases) {
          fprintf(outfp, "    {\n    ");
          if (emit_destructor_snippet_indexed_by_n(outfp, &cc, ts)) {
            r = EXIT_FAILURE;
            goto cleanup_exit;
          }

          fprintf(outfp, "\n"
                         "    }\n"
                         "    break;\n");
        }
      }
    }
    fprintf(outfp, "    } /* switch */\n");
    if (cc.common_data_assigned_type_ && cc.common_data_assigned_type_->constructor_snippet_.num_tokens_) {
      fprintf(outfp, "    {\n"
                     "      ");
      if (emit_common_destructor_snippet_indexed_by_n(outfp, &cc)) {
        r = EXIT_FAILURE;
        goto cleanup_exit;
      }
      fprintf(outfp, "\n"
                     "    }\n");
    }

    fprintf(outfp,
      "  }\n");

    fprintf(outfp,
      "  stack->pos_ = 0;\n"
      "  stack->error_recovery_ = 0;\n"
      "  stack->report_error_ = 0;\n");
    if (prdg.num_patterns_) {
      fprintf(outfp, "  stack->need_sym_ = 1;\n"
                     "  stack->current_sym_ = 0;\n");
    }
    fprintf(outfp,
      "  stack->mute_error_turns_ = 0;\n");

    fprintf(outfp, "  switch (%spush_state(stack, 0)) {\n"
                   "    case -1: /* overflow */ {\n", cc_prefix(&cc));
    fprintf(outfp, "      return -2;\n");
    fprintf(outfp, "    }\n    break;\n"
                   "    case -2: /* out of memory */ {\n");
    fprintf(outfp, "      return -2;\n");
    fprintf(outfp, "    }\n"
                   "    break;\n"
                   "  }\n");

    if (prdg.num_patterns_) {
      fprintf(outfp, "  stack->scan_state_ = %zu;\n", scantable.start_state);
      fprintf(outfp, "  stack->input_index_ = 0;\n"
                     "  stack->input_offset_ = 0;\n"
                     "  stack->input_line_ = 1;\n"
                     "  stack->input_col_ = 1;\n"
                     "  stack->match_index_ = 0;\n"
                     "  stack->match_buffer_size_ = 0;\n"
                     "  stack->terminator_repair_ = '\\0';\n"
                     "  stack->token_size_ = 0;\n"
                     "  stack->match_offset_ = 0;\n"
                     "  stack->match_line_ = 1;\n"
                     "  stack->match_col_ = 1;\n");
      fprintf(outfp, "  stack->best_match_action_ = %zu;\n", (size_t)scantable.actions[scantable.start_state].action);
      fprintf(outfp, "  stack->best_match_size_ = 0;\n"
                     "  stack->best_match_offset_ = 0;\n"
                     "  stack->best_match_line_ = 1;\n"
                     "  stack->best_match_col_ = 1;\n");
    }

    fprintf(outfp, "  return 0;\n"
                   "}\n"
                   "\n");

    if (prdg.num_patterns_) {
      r = emit_lex_function(outfp, &cc, &prdg, &scantable);
      if (r) {
        r = EXIT_FAILURE;
        goto cleanup_exit;
      }
      r = emit_scan_function(outfp, &cc, &prdg, &lalr, state_syms);
    }

    r = emit_parse_function(outfp, &cc, &prdg, &lalr, state_syms);
    if (r) {
      r = EXIT_FAILURE;
      goto cleanup_exit;
    }

    fprintf(outfp, "/* --------- END OF GENERATED CODE ------------ */\n");


    pt = epilogue;
    if (pt) {
      do {
        pt = pt->next_;

        size_t written = fwrite(pt->chars_, 1, pt->num_chars_, outfp);
        if (written != pt->num_chars_) {
          int err = errno;
          re_error_nowhere("Failed to write to \"%s\": %s", c_output_filename, strerror(err));
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
    if (h_output_filename) {
      outfp = fopen(h_output_filename, "wb");
      if (!outfp) {
        int err = errno;
        re_error_nowhere("Error, failed to open file \"%s\" for writing: %s", h_output_filename, strerror(err));
        r = EXIT_FAILURE;
        goto cleanup_exit;
      }
    }
    else {
      re_error_nowhere("Error, generating header file requires output filename");
      r = EXIT_FAILURE;
      goto cleanup_exit;
    }
    
    fprintf(outfp, "#ifndef %s\n"
                   "#define %s\n"
                   "\n", include_guard, include_guard);

    fprintf(outfp, "#include <stddef.h> /* size_t */\n"
                   "\n");

    fprintf(outfp, "#ifdef __cplusplus\n"
                   "extern \"C\" {\n"
                   "#endif\n"
                   "\n");

    fprintf(outfp, "#define _%sMATCH 1\n", cc_PREFIX(&cc));
    fprintf(outfp, "#define _%sOVERFLOW 2\n", cc_PREFIX(&cc));
    fprintf(outfp, "#define _%sNO_MEMORY 3\n", cc_PREFIX(&cc));
    fprintf(outfp, "#define _%sFEED_ME 4\n", cc_PREFIX(&cc));
    fprintf(outfp, "#define _%sEND_OF_INPUT 5\n", cc_PREFIX(&cc));
    fprintf(outfp, "#define _%sSYNTAX_ERROR 6\n", cc_PREFIX(&cc));
    fprintf(outfp, "\n");

    sym = cc.symtab_.terminals_;
    if (sym) {
      do {
        sym = sym->next_;

        char *ident = malloc(1 + sym->def_.num_translated_);
        char *s = ident;
        const char *p;
        if (!ident) {
          r = EXIT_FAILURE;
          goto cleanup_exit;
        }
        /* Transform into C identifier */
        for (p = sym->def_.translated_; p < (sym->def_.translated_ + sym->def_.num_translated_); ++p) {
          int c = *p;
          if ((c >= 'a') && (c <= 'z')) c = c - 'a' + 'A';
          if (c == '-') c = '_';
          *s++ = c;
        }
        *s++ = '\0';

        fprintf(outfp, "#define %s%s %d\n", cc_TOKEN_PREFIX(&cc), ident, sym->ordinal_);
        free(ident);
      } while (sym != cc.symtab_.terminals_);
    }
    fprintf(outfp, "\n");

    fprintf(outfp, "struct %sstack {\n", cc_prefix(&cc));
    fprintf(outfp, "  int error_recovery_:1;\n");
    fprintf(outfp, "  int report_error_:1;\n");
    if (prdg.num_patterns_) {
      fprintf(outfp, "  int need_sym_:1;\n"
                     "  int current_sym_;\n");
    }
    fprintf(outfp, "  int mute_error_turns_;\n");
    fprintf(outfp, "  size_t pos_, num_stack_allocated_;\n");
    fprintf(outfp, "  struct %ssym_data *stack_;\n", cc_prefix(&cc));
    if (prdg.num_patterns_) {
      fprintf(outfp, 
              "  size_t scan_state_;\n"
              "  size_t match_index_;\n"
              "  size_t best_match_action_;\n"
              "  size_t best_match_size_;\n"
              "  size_t input_index_;\n"
              "  size_t input_offset_;\n"
              "  size_t match_buffer_size_;\n"
              "  size_t match_buffer_size_allocated_;\n"
              "  /* offset, line and column at the start of match_buffer_ */\n"
              "  size_t match_offset_;\n"
              "  int match_line_;\n"
              "  int match_col_;\n"
              "  /* offset, line and column at the zero-termination (best_match_size_) of a match \n"
              "   * in match_buffer_ (the actual buffer may well be longer.) */\n"
              "  size_t best_match_offset_;\n"
              "  int best_match_line_;\n"
              "  int best_match_col_;\n"
              "  size_t token_size_;\n"
              "  char *match_buffer_;\n"
              "  char terminator_repair_;\n"
              "  int input_line_;\n"
              "  int input_col_;\n");
    }
    fprintf(outfp, "};\n");
    fprintf(outfp, "\n");
    fprintf(outfp, "void %sstack_init(struct %sstack *stack);\n", cc_prefix(&cc), cc_prefix(&cc));
    fprintf(outfp, "void %sstack_cleanup(struct %sstack *stack);\n", cc_prefix(&cc), cc_prefix(&cc));
    fprintf(outfp, "int %sstack_reset(struct %sstack *stack);\n", cc_prefix(&cc), cc_prefix(&cc));
    if (cc.params_snippet_.num_tokens_) {
      fprintf(outfp, "int %sparse(struct %sstack *stack, int sym, ", cc_prefix(&cc), cc_prefix(&cc));
      size_t token_idx;
      for (token_idx = 0; token_idx < cc.params_snippet_.num_tokens_; ++token_idx) {
        fprintf(outfp, "%s", cc.params_snippet_.tokens_[token_idx].text_.original_);
      }
      fprintf(outfp, ");\n");
    }
    else {
      fprintf(outfp,
        "int %sparse(struct %sstack *stack, int sym);\n", cc_prefix(&cc), cc_prefix(&cc));
    }

    fprintf(outfp, "\n"
                   "#ifdef __cplusplus\n"
                   "} /* extern \"C\" */\n"
                   "#endif\n");

    fprintf(outfp, "\n"
                   "#endif /* %s */\n", include_guard);

    if (outfp != stdout) {
      fclose(outfp);
    }
  }


  r = EXIT_SUCCESS;
cleanup_exit:
  if (include_guard) free(include_guard);

  if (state_syms) free(state_syms);

  xlts_cleanup(&comment_free_line);

  xlts_cleanup(&token_buf);

  lr_cleanup(&lalr);

  sc_scanner_cleanup(&scantable);

  gt_grammar_table_cleanup(&gt);

  cinder_context_cleanup(&cc);

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
  if (h_output_filename) free(h_output_filename);
  if (c_output_filename) free(c_output_filename);

  return r;
}
