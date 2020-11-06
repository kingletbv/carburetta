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

#ifndef ERRNO_H_INCLUDED
#define ERRNO_H_INCLUDED
#include <errno.h>
#endif

#ifndef MUL_H_INCLUDED
#define MUL_H_INCLUDED
#include "mul.h"
#endif

#ifndef EMIT_C_H_INCLUDED
#define EMIT_C_H_INCLUDED
#include "emit_c.h"
#endif

enum dest_type {
  SEDT_NONE,
  SEDT_FMT,
  SEDT_FMT_PREFIX,
  SEDT_FMT_DATATYPE_ORDINAL,
  SEDT_FMT_PREFIX_DATATYPE_ORDINAL,
  SEDT_FMT_PREFIX_TYPESTR_ORDINAL,
  SEDT_FMT_TYPESTR_ORDINAL
};

enum sym_type {
  SEST_NONE,
  SEST_FMT_INDEX_ORDINAL
};

enum common_type {
  SECT_NONE,
  SECT_FMT_PREFIX,
  SECT_FMT
};

enum common_dest_type {
  SECDT_NONE,
  SECDT_FMT_PREFIX,
  SECDT_FMT
};

enum len_type {
  SELT_NONE,
  SELT_FMT
};

enum discard_type {
  SEDIT_NONE,
  SEDIT_FMT
};

enum text_type {
  SETT_NONE,
  SETT_FMT
};

struct snippet_emission {
  struct snippet *code_;

  enum dest_type dest_type_;
  const char *dest_fmt_;
  struct prd_production_sym *dest_;
  struct typestr *dest_typestr_;

  enum sym_type sym_type_;
  const char *sym_fmt_;
  struct prd_production *prod_;

  enum common_type common_type_;
  const char *common_fmt_prefix_;
  const char *common_fmt_suffix_;

  enum common_dest_type common_dest_type_;
  const char *common_dest_fmt_;

  enum len_type len_type_;
  const char *len_fmt_;

  enum discard_type discard_type_;
  const char *discard_fmt_;

  enum text_type text_type_;
  const char *text_fmt_;
};


const char *cc_prefix(struct carburetta_context *cc) {
  if (cc->prefix_.num_translated_) return cc->prefix_.translated_;
  return "carburetta_";
}

const char *cc_PREFIX(struct carburetta_context *cc) {
  if (cc->prefix_uppercase_) return cc->prefix_uppercase_;
  return "CARB_";
}

static const char *cc_TOKEN_PREFIX(struct carburetta_context *cc) {
  if (cc->token_prefix_uppercase_) return cc->token_prefix_uppercase_;
  return cc_PREFIX(cc);
}

static int print_sym_as_c_ident(FILE *fp, struct carburetta_context *cc, struct symbol *sym) {
  char *ident = (char *)malloc(1 + sym->def_.num_translated_);
  char *s = ident;
  const char *p;
  if (!ident) {
    return -1;
  }
  /* Transform into C identifier */
  for (p = sym->def_.translated_; p < (sym->def_.translated_ + sym->def_.num_translated_); ++p) {
    int c = *p;
    if ((c >= 'a') && (c <= 'z')) c = c - 'a' + 'A';
    if (c == '-') c = '_';
    *s++ = c;
  }
  *s++ = '\0';

  fprintf(fp, "%s%s", cc_TOKEN_PREFIX(cc), ident);

  free(ident);

  return 0;
}

static void print_regex_as_comment(FILE *outfp, const char *regex) {
  fwrite("/* ", 1, 3, outfp);
  while (*regex) {
    /* Make sure we don't accidentally close the comment */
    if ((regex[0] == '*') && (regex[1] == '/')) {
      fwrite("* /", 1, 3, outfp);
    }
    else {
      fwrite(regex, 1, 1, outfp);
    }
    regex++;
  }
  fwrite(" */", 1, 3, outfp);
}

static int emit_dest(FILE *outfp, struct carburetta_context *cc, struct snippet_emission *se, struct snippet_token *st) {
  switch (se->dest_type_) {
  case SEDT_NONE:
    re_error(&st->text_, "$$ cannot resolve to a symbol");
    return TKR_SYNTAX_ERROR;
  case SEDT_FMT:
    fputs(se->dest_fmt_, outfp);
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

static int emit_sym_specific_data(FILE *outfp, struct carburetta_context *cc, struct snippet_emission *se, struct snippet_token *st, size_t sym_index) {
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

static int emit_dest_commondata(FILE *outfp, struct carburetta_context *cc, struct snippet_emission *se, struct snippet_token *st) {
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
    fputs(se->common_dest_fmt_, outfp);
    break;
  }

  return 0;
}

static int emit_len(FILE *outfp, struct carburetta_context *cc, struct snippet_emission *se, struct snippet_token *st) {
  if (se->len_type_ == SELT_NONE) {
    re_error(&st->text_, "%%len cannot resolve to a length");
    return TKR_SYNTAX_ERROR;
  }
  switch (se->len_type_) {
  case SELT_FMT:
    fputs(se->len_fmt_, outfp);
    break;
  }
  return 0;
}

static int emit_discard(FILE *outfp, struct carburetta_context *cc, struct snippet_emission *se, struct snippet_token *st) {
  if (se->discard_type_ == SEDIT_NONE) {
    re_error(&st->text_, "%%discard use is limited to common actions");
    return TKR_SYNTAX_ERROR;
  }
  switch (se->discard_type_) {
  case SEDIT_FMT:
    fputs(se->discard_fmt_, outfp);
    break;
  }
  return 0;
}

static int emit_text(FILE *outfp, struct carburetta_context *cc, struct snippet_emission *se, struct snippet_token *st) {
  if (se->text_type_ == SETT_NONE) {
    re_error(&st->text_, "%%text use is limited to pattern actions");
    return TKR_SYNTAX_ERROR;
  }
  switch (se->text_type_) {
  case SETT_FMT:
    fputs(se->text_fmt_, outfp);
    break;
  }
  return 0;
}

static int emit_common(FILE *outfp, struct carburetta_context *cc, struct snippet_emission *se, struct snippet_token *special_ident_token, size_t start_of_index, size_t end_of_index) {
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
    fputs(se->common_fmt_prefix_, outfp);
    break;
  }
  size_t n;
  for (n = start_of_index; n < end_of_index; ++n) {
    fprintf(outfp, "%s", se->code_->tokens_[n].text_.original_);
  }
  fputs(se->common_fmt_suffix_, outfp);
  return 0;
}

static int emit_snippet_code_emission(FILE *outfp, struct carburetta_context *cc, struct snippet_emission *se) {
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
    else if ((se->code_->tokens_[col].match_ == TOK_SPECIAL_IDENT_STR) && !strcmp("$text", se->code_->tokens_[col].text_.translated_)) {
      r = emit_text(outfp, cc, se, se->code_->tokens_ + col);
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

static int emit_common_action_snippet(FILE *outfp, struct carburetta_context *cc, struct prd_production *prd) {
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
  se.discard_type_ = SEDIT_FMT;
  se.discard_fmt_ = "discard_action = 1;";
  se.text_type_ = SETT_NONE;
  return emit_snippet_code_emission(outfp, cc, &se);
}

static int emit_action_snippet(FILE *outfp, struct carburetta_context *cc, struct prd_production *prd) {
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
  se.discard_type_ = SEDIT_NONE;
  se.text_type_ = SETT_NONE;
  return emit_snippet_code_emission(outfp, cc, &se);
}

static int emit_pattern_token_action_snippet(FILE *outfp, struct carburetta_context *cc, struct typestr *ts) {
  struct snippet_emission se = { 0 };
  se.code_ = &ts->token_action_snippet_;
  se.dest_type_ = SEDT_FMT_TYPESTR_ORDINAL;
  se.dest_typestr_ = ts;
  se.dest_fmt_ = "(stack->stack_[0].v_.uv%d_)";
  se.sym_type_ = SEST_NONE;
  se.common_type_ = SECT_NONE;
  se.common_dest_type_ = SECDT_FMT;
  se.common_dest_fmt_ = "(stack->stack_[0].common_)";
  se.len_type_ = SELT_NONE;
  se.discard_type_ = SEDIT_NONE;
  se.text_type_ = SETT_NONE;
  return emit_snippet_code_emission(outfp, cc, &se);
}

static int emit_token_action_snippet(FILE *outfp, struct carburetta_context *cc, struct typestr *ts) {
  struct snippet_emission se = { 0 };
  se.code_ = &ts->token_action_snippet_;
  se.dest_type_ = SEDT_FMT_TYPESTR_ORDINAL;
  se.dest_typestr_ = ts;
  se.dest_fmt_ = "(sym_data->v_.uv%d_)";
  se.sym_type_ = SEST_NONE;
  se.common_type_ = SECT_NONE;
  se.common_dest_type_ = SECDT_FMT;
  se.common_dest_fmt_ = "(sym_data->common_)";
  se.len_type_ = SELT_NONE;
  se.discard_type_ = SEDIT_NONE;
  se.text_type_ = SETT_NONE;
  return emit_snippet_code_emission(outfp, cc, &se);
}

static int emit_constructor_snippet(FILE *outfp, struct carburetta_context *cc, struct typestr *ts) {
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
  se.discard_type_ = SEDIT_NONE;
  se.text_type_ = SETT_NONE;
  return emit_snippet_code_emission(outfp, cc, &se);
}

static int emit_dst_sym_constructor_snippet(FILE *outfp, struct carburetta_context *cc, struct typestr *ts) {
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
  se.discard_type_ = SEDIT_NONE;
  se.text_type_ = SETT_NONE;
  return emit_snippet_code_emission(outfp, cc, &se);
}

static int emit_common_constructor_snippet(FILE *outfp, struct carburetta_context *cc) {
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
  se.discard_type_ = SEDIT_NONE;
  se.text_type_ = SETT_NONE;
  return emit_snippet_code_emission(outfp, cc, &se);
}

static int emit_dst_common_constructor_snippet(FILE *outfp, struct carburetta_context *cc) {
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
  se.discard_type_ = SEDIT_NONE;
  se.text_type_ = SETT_NONE;
  return emit_snippet_code_emission(outfp, cc, &se);
}

static int emit_token_common_constructor_snippet(FILE *outfp, struct carburetta_context *cc) {
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
  se.discard_type_ = SEDIT_NONE;
  se.text_type_ = SETT_NONE;
  return emit_snippet_code_emission(outfp, cc, &se);
}

static int emit_pattern_token_common_constructor_snippet(FILE *outfp, struct carburetta_context *cc) {
  struct snippet_emission se = { 0 };
  if (!cc->common_data_assigned_type_) return 0;
  se.code_ = &cc->common_data_assigned_type_->constructor_snippet_;
  se.dest_type_ = SEDT_FMT;
  se.dest_fmt_ = "(stack->stack_[0].common_)";
  se.sym_type_ = SEST_NONE;
  se.common_type_ = SECT_NONE;
  se.common_dest_type_ = SECDT_FMT;
  se.common_dest_fmt_ = "(stack->stack_[0].common_)";
  se.len_type_ = SELT_NONE;
  se.discard_type_ = SEDIT_NONE;
  se.text_type_ = SETT_NONE;
  return emit_snippet_code_emission(outfp, cc, &se);
}

static int emit_token_common_action_snippet(FILE *outfp, struct carburetta_context *cc) {
  struct snippet_emission se = { 0 };
  if (!cc->common_data_assigned_type_) return 0;
  se.code_ = &cc->common_data_assigned_type_->token_action_snippet_;
  se.dest_type_ = SEDT_FMT;
  se.dest_fmt_ = "(sym_data->common_)";
  se.sym_type_ = SEST_NONE;
  se.common_type_ = SECT_NONE;
  se.common_dest_type_ = SECDT_FMT;
  se.common_dest_fmt_ = "(sym_data->common_)";
  se.len_type_ = SELT_NONE;
  se.discard_type_ = SEDIT_NONE;
  se.text_type_ = SETT_NONE;
  return emit_snippet_code_emission(outfp, cc, &se);
}

static int emit_pattern_token_common_action_snippet(FILE *outfp, struct carburetta_context *cc) {
  struct snippet_emission se = { 0 };
  if (!cc->common_data_assigned_type_) return 0;
  se.code_ = &cc->common_data_assigned_type_->token_action_snippet_;
  se.dest_type_ = SEDT_FMT;
  se.dest_fmt_ = "(stack->stack_[0].common_)";
  se.sym_type_ = SEST_NONE;
  se.common_type_ = SECT_NONE;
  se.common_dest_type_ = SECDT_FMT;
  se.common_dest_fmt_ = "(stack->stack_[0].common_)";
  se.len_type_ = SELT_NONE;
  se.discard_type_ = SEDIT_NONE;
  se.text_type_ = SETT_NONE;
  return emit_snippet_code_emission(outfp, cc, &se);
}

static int emit_token_constructor_snippet(FILE *outfp, struct carburetta_context *cc, struct typestr *ts) {
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
  se.discard_type_ = SEDIT_NONE;
  se.text_type_ = SETT_NONE;
  return emit_snippet_code_emission(outfp, cc, &se);
}

static int emit_pattern_token_constructor_snippet(FILE *outfp, struct carburetta_context *cc, struct typestr *ts) {
  struct snippet_emission se = { 0 };
  se.code_ = &ts->constructor_snippet_;
  se.dest_type_ = SEDT_FMT_TYPESTR_ORDINAL;
  se.dest_typestr_ = ts;
  se.dest_fmt_ = "(stack->stack_[0].v_.uv%d_)";
  se.sym_type_ = SEST_NONE;
  se.common_type_ = SECT_NONE;
  se.common_dest_type_ = SECDT_FMT;
  se.common_dest_fmt_ = "(stack->stack_[0].common_)";
  se.len_type_ = SELT_NONE;
  se.discard_type_ = SEDIT_NONE;
  se.text_type_ = SETT_NONE;
  return emit_snippet_code_emission(outfp, cc, &se);
}

static int emit_destructor_snippet(FILE *outfp, struct carburetta_context *cc, struct typestr *ts) {
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
  se.discard_type_ = SEDIT_NONE;
  se.text_type_ = SETT_NONE;
  return emit_snippet_code_emission(outfp, cc, &se);
}

static int emit_destructor_snippet_indexed_by_n(FILE *outfp, struct carburetta_context *cc, struct typestr *ts) {
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
  se.discard_type_ = SEDIT_NONE;
  se.text_type_ = SETT_NONE;
  return emit_snippet_code_emission(outfp, cc, &se);
}

static int emit_destructor_snippet_indexed_by_0(FILE *outfp, struct carburetta_context *cc, struct typestr *ts) {
  struct snippet_emission se = { 0 };
  se.code_ = &ts->destructor_snippet_;
  se.dest_type_ = SEDT_FMT_TYPESTR_ORDINAL;
  se.dest_typestr_ = ts;
  se.dest_fmt_ = "(stack->stack_[0].v_.uv%d_)";
  se.sym_type_ = SEST_NONE;
  se.common_type_ = SECT_NONE;
  se.common_dest_type_ = SECDT_FMT;
  se.common_dest_fmt_ = "(stack->stack_[0].common_)";
  se.len_type_ = SELT_NONE;
  se.discard_type_ = SEDIT_NONE;
  se.text_type_ = SETT_NONE;
  return emit_snippet_code_emission(outfp, cc, &se);
}

static int emit_common_destructor_snippet(FILE *outfp, struct carburetta_context *cc) {
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
  se.discard_type_ = SEDIT_NONE;
  se.text_type_ = SETT_NONE;
  return emit_snippet_code_emission(outfp, cc, &se);
}

static int emit_pattern_token_common_destructor_snippet(FILE *outfp, struct carburetta_context *cc) {
  struct snippet_emission se = { 0 };
  if (!cc->common_data_assigned_type_) return 0;
  se.code_ = &cc->common_data_assigned_type_->destructor_snippet_;
  se.dest_type_ = SEDT_FMT;;
  se.dest_fmt_ = "(stack->stack_[0].common_)";
  se.sym_type_ = SEST_NONE;
  se.common_type_ = SECT_NONE;
  se.common_dest_type_ = SECDT_FMT;
  se.common_dest_fmt_ = "(stack->stack_[0].common_)";
  se.len_type_ = SELT_NONE;
  se.discard_type_ = SEDIT_NONE;
  se.text_type_ = SETT_NONE;
  return emit_snippet_code_emission(outfp, cc, &se);
}

static int emit_common_destructor_snippet_indexed_by_n(FILE *outfp, struct carburetta_context *cc) {
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
  se.discard_type_ = SEDIT_NONE;
  se.text_type_ = SETT_NONE;
  return emit_snippet_code_emission(outfp, cc, &se);
}

static int emit_common_destructor_snippet_index_0(FILE *outfp, struct carburetta_context *cc) {
  struct snippet_emission se = { 0 };
  if (!cc->common_data_assigned_type_) return 0;
  se.code_ = &cc->common_data_assigned_type_->destructor_snippet_;
  se.dest_type_ = SEDT_FMT;
  se.dest_fmt_ = "(stack->stack_[0].common_)";
  se.sym_type_ = SEST_NONE;
  se.common_type_ = SECT_NONE;
  se.common_dest_type_ = SECDT_FMT;
  se.common_dest_fmt_ = "(stack->stack_[0].common_)";
  se.len_type_ = SELT_NONE;
  se.discard_type_ = SEDIT_NONE;
  se.text_type_ = SETT_NONE;
  return emit_snippet_code_emission(outfp, cc, &se);
}


static int emit_pattern_action_snippet(FILE *outfp, struct carburetta_context *cc, struct prd_pattern *pat) {
  struct snippet_emission se = { 0 };
  struct typestr *ts = pat->term_.sym_ ? pat->term_.sym_->assigned_type_ : NULL;
  se.code_ = &pat->action_sequence_;
  if (ts) {
    se.dest_type_ = SEDT_FMT_TYPESTR_ORDINAL;
    se.dest_typestr_ = ts;
    se.dest_fmt_ = "(stack->stack_[0].v_.uv%d_)";
  }
  else {
    se.dest_type_ = SEDT_NONE;
  }
  se.sym_type_ = SEST_NONE;
  se.prod_ = NULL;
  se.common_type_ = SECT_NONE;
  se.common_dest_type_ = SECDT_FMT;
  se.common_dest_fmt_ = "(stack->stack_[0].common_)";
  se.len_type_ = SELT_FMT;
  se.len_fmt_ = "(stack->token_size_)";
  se.discard_type_ = SEDIT_NONE;
  se.text_type_ = SETT_FMT;
  se.text_fmt_ = "(stack->match_buffer_)";
  return emit_snippet_code_emission(outfp, cc, &se);
}

static int emit_lex_function(FILE *outfp, struct carburetta_context *cc, struct prd_grammar *prdg, struct sc_scanner *scantable) {
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
                  "\n"
                  "      stack->input_index_ = input_index;\n"
                  "      stack->input_offset_ = input_offset;\n"
                  "      stack->input_line_ = input_line;\n"
                  "      stack->input_col_ = input_col;\n"
                  );
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
                  "      r = %sappend_match_buffer(stack, input + stack->input_index_, input_index - stack->input_index_);\n", cc_prefix(cc));
  fprintf(outfp,  "      if (r) return r;\n"
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
                  "\n"
                  "      stack->input_index_ = input_index;\n"
                  "      stack->input_offset_ = input_offset;\n"
                  "      stack->input_line_ = input_line;\n"
                  "      stack->input_col_ = input_col;\n");
  fprintf(outfp,  "      return _%sMATCH;\n", cc_PREFIX(cc));
  fprintf(outfp,  "    }\n"
                  "  }\n"
                  "\n"
                  "  r = %sappend_match_buffer(stack, input + stack->input_index_, input_index - stack->input_index_);\n", cc_prefix(cc));
  fprintf(outfp,  "  if (r) return r;\n"
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
                  "    stack->input_col_ = input_col;\n");
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
                  "\n"
                  "  stack->input_index_ = input_index;\n"
                  "  stack->input_offset_ = input_offset;\n"
                  "  stack->input_line_ = input_line;\n"
                  "  stack->input_col_ = input_col;\n");
  fprintf(outfp,  "  return _%sMATCH;\n", cc_PREFIX(cc));
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
                  "    r = %sappend_match_buffer(stack, input + stack->input_index_, 1);\n", cc_prefix(cc));
  fprintf(outfp,  "    if (r) return r;\n"
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

static int emit_scan_function(FILE *outfp, struct carburetta_context *cc, struct prd_grammar *prdg, struct lr_generator *lalr, int *state_syms) {
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
  fprintf(outfp, "  if (stack->pending_reset_) {\n"
                 "    int r;\n"
                 "    r = %sstack_reset(stack);\n"
                 "    if (r) return r;\n"
                 "  }\n", cc_prefix(cc));
  fprintf(outfp, "  for (;;) {\n");
  fprintf(outfp, "    if (stack->need_sym_) {\n");
  fprintf(outfp, "      switch (%slex(stack, input, input_size, is_final_input)) {\n", cc_prefix(cc));
  fprintf(outfp, "        case _%sMATCH:\n", cc_PREFIX(cc));
  fprintf(outfp, "          stack->need_sym_ = 0;\n");
  fprintf(outfp, "          stack->current_sym_ = ");
  if (print_sym_as_c_ident(outfp, cc, cc->input_end_sym_)) {
    r = EXIT_FAILURE;
    goto cleanup_exit;
  }
  fprintf(outfp, "; /* no sym specific type associated, changed when we successfully construct it below */\n");
  fprintf(outfp, "          if (stack->mute_error_turns_) stack->mute_error_turns_--;\n");

  if (cc->common_data_assigned_type_ && cc->common_data_assigned_type_->constructor_snippet_.num_tokens_) {
    fprintf(outfp, "          {\n"
                   "            ");
    if (emit_pattern_token_common_constructor_snippet(outfp, cc)) {
      r = EXIT_FAILURE;
      goto cleanup_exit;
    }
    fprintf(outfp, "          }\n");
  }
  if (cc->common_data_assigned_type_ && cc->common_data_assigned_type_->token_action_snippet_.num_tokens_) {
    fprintf(outfp, "          {\n"
                   "            ");
    if (emit_pattern_token_common_action_snippet(outfp, cc)) {
      r = EXIT_FAILURE;
      goto cleanup_exit;
    }
    fprintf(outfp, "          }\n");
  }

  fprintf(outfp, "          switch (stack->best_match_action_) {\n");
  size_t pat_idx;
  for (pat_idx = 0; pat_idx < prdg->num_patterns_; ++pat_idx) {
    struct prd_pattern *pat = prdg->patterns_ + pat_idx;
    fprintf(outfp, "            case %zu: ", pat_idx + 1);
    print_regex_as_comment(outfp, pat->regex_);
    fprintf(outfp, "\n");
    if (pat->term_.sym_) {
      fprintf(outfp, "              stack->current_sym_ = ");
      if (print_sym_as_c_ident(outfp, cc, pat->term_.sym_)) {
        r = EXIT_FAILURE;
        goto cleanup_exit;
      }
      fprintf(outfp, ";\n");
      if (pat->term_.sym_->assigned_type_ && pat->term_.sym_->assigned_type_->constructor_snippet_.num_tokens_) {
        fprintf(outfp, "              {\n"
                       "                ");
        if (emit_pattern_token_constructor_snippet(outfp, cc, pat->term_.sym_->assigned_type_)) {
          r = EXIT_FAILURE;
          goto cleanup_exit;
        }
        fprintf(outfp, "              }\n");
      }
      if (pat->term_.sym_->assigned_type_ && pat->term_.sym_->assigned_type_->token_action_snippet_.num_tokens_) {
        fprintf(outfp, "              {\n"
                        "                ");

        if (emit_pattern_token_action_snippet(outfp, cc, pat->term_.sym_->assigned_type_)) {
          r = EXIT_FAILURE;
          goto cleanup_exit;
        }

        fprintf(outfp, "              }\n");
      }
      if (pat->action_sequence_.num_tokens_) {
        fprintf(outfp, "              {\n"
          "                ");
        emit_pattern_action_snippet(outfp, cc, pat);
        fprintf(outfp, "              }\n");
      }
    }
    else {
      /* Pattern matches no symbol */
      fprintf(outfp, "              /* Pattern does not have a symbol */\n"
                     "              stack->current_sym_ = ");
      if (print_sym_as_c_ident(outfp, cc, cc->input_end_sym_)) {
        r = EXIT_FAILURE;
        goto cleanup_exit;
      }
      fprintf(outfp, ";\n"
                     "              stack->need_sym_ = 1; /* keep scanning */\n");
      if (pat->action_sequence_.num_tokens_) {
        fprintf(outfp, "              {\n"
                       "                ");
        emit_pattern_action_snippet(outfp, cc, pat);
        fprintf(outfp, "              }\n");
      }
      /* Assuming we still need a sym, we should deconstruct the common data. */
      if (cc->common_data_assigned_type_ && cc->common_data_assigned_type_->destructor_snippet_.num_tokens_) {
        fprintf(outfp, "              if (stack->need_sym_) {\n"
                       "                ");
        if (emit_pattern_token_common_destructor_snippet(outfp, cc)) {
          r = EXIT_FAILURE;
          goto cleanup_exit;
          }
        fprintf(outfp, "              }\n");
      }
    }
    fprintf(outfp, "              break;\n");
  }
  fprintf(outfp, "          } /* switch */\n");
  
  fprintf(outfp, "          break;\n");
  fprintf(outfp, "        case _%sOVERFLOW:\n", cc_PREFIX(cc));
  fprintf(outfp, "          return _%sOVERFLOW;\n", cc_PREFIX(cc));
  fprintf(outfp, "        case _%sNO_MEMORY:\n", cc_PREFIX(cc));
  fprintf(outfp, "          return _%sNO_MEMORY;\n", cc_PREFIX(cc));
  fprintf(outfp, "        case _%sFEED_ME:\n", cc_PREFIX(cc));
  fprintf(outfp, "          return _%sFEED_ME;\n", cc_PREFIX(cc));
  fprintf(outfp, "        case _%sEND_OF_INPUT:\n", cc_PREFIX(cc));
  fprintf(outfp, "          stack->current_sym_ = ");
  if (print_sym_as_c_ident(outfp, cc, cc->input_end_sym_)) {
    r = EXIT_FAILURE;
    goto cleanup_exit;
  }
  fprintf(outfp, ";\n");
  fprintf(outfp, "          stack->need_sym_ = 0;\n");
  fprintf(outfp, "          if (stack->mute_error_turns_) stack->mute_error_turns_--;\n");
  fprintf(outfp, "          break;\n");
  fprintf(outfp, "        case _%sSYNTAX_ERROR:\n", cc_PREFIX(cc));
  fprintf(outfp, "          return _%sSYNTAX_ERROR;\n", cc_PREFIX(cc));
  fprintf(outfp, "      } /* switch */\n");
  fprintf(outfp, "    } /* if (need_sym_) */\n");
  fprintf(outfp, "    else {\n");
  fprintf(outfp, "      int sym = stack->current_sym_;\n"
                 "      if (!stack->error_recovery_) {\n"
                 "        int action = %sparse_table[%snum_columns * stack->stack_[stack->pos_ - 1].state_ + (sym - %sminimum_sym)];\n", cc_prefix(cc), cc_prefix(cc), cc_prefix(cc));
  /* Shift logic */
  fprintf(outfp, "        if (action > 0) {\n");
  fprintf(outfp, "          switch (%spush_state(stack, action /* action for a shift is the ordinal */)) {\n", cc_prefix(cc));
  fprintf(outfp, "            case _%sOVERFLOW: {\n", cc_PREFIX(cc));
  fprintf(outfp, "              ");
  if (cc->on_internal_error_snippet_.num_tokens_) {
    size_t token_idx;
    for (token_idx = 0; token_idx < cc->on_internal_error_snippet_.num_tokens_; ++token_idx) {
      fprintf(outfp, "%s", cc->on_internal_error_snippet_.tokens_[token_idx].text_.original_);
    }
  }
  else {
    fprintf(outfp, "return _%sOVERFLOW;\n", cc_PREFIX(cc));
  }
  fprintf(outfp, "            }\n"
                 "            break;\n");
  fprintf(outfp, "            case _%sNO_MEMORY: {\n", cc_PREFIX(cc));
  fprintf(outfp, "              ");
  if (cc->on_alloc_error_snippet_.num_tokens_) {
    size_t token_idx;
    for (token_idx = 0; token_idx < cc->on_alloc_error_snippet_.num_tokens_; ++token_idx) {
      fprintf(outfp, "%s", cc->on_alloc_error_snippet_.tokens_[token_idx].text_.original_);
    }
  }
  else {
    fprintf(outfp, "return _%sNO_MEMORY;\n", cc_PREFIX(cc));
  }
  fprintf(outfp, "            }\n"
                 "            break;\n"
                 "          } /* switch */\n");

  fprintf(outfp, "\n"
                 "          /* Fill in the sym from the tokenizer */\n");
  fprintf(outfp, "          stack->need_sym_ = 1;\n");
  if (cc->common_data_assigned_type_) {
    fprintf(outfp, "          stack->stack_[stack->pos_ - 1].common_ = stack->stack_[0].common_;\n");
  }
  if (cc->have_typed_symbols_) {
    fprintf(outfp, "          stack->stack_[stack->pos_ - 1].v_ = stack->stack_[0].v_;\n");
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
                   "            return _%sSYNTAX_ERROR;\n", cc_PREFIX(cc));
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
                   "            stack->pending_reset_ = 1;\n"
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
                   "            return _%sINTERNAL_ERROR;\n", cc_PREFIX(cc));
  }
  fprintf(outfp, "          }\n");

  fprintf(outfp, "          switch (%spush_state(stack, action /* action for a \"goto\" shift is the ordinal */)) {\n", cc_prefix(cc));
  fprintf(outfp, "            case _%sOVERFLOW: {\n", cc_PREFIX(cc));
  fprintf(outfp, "              ");
  if (cc->on_internal_error_snippet_.num_tokens_) {
    size_t token_idx;
    for (token_idx = 0; token_idx < cc->on_internal_error_snippet_.num_tokens_; ++token_idx) {
      fprintf(outfp, "%s", cc->on_internal_error_snippet_.tokens_[token_idx].text_.original_);
    }
  }
  else {
    fprintf(outfp, "return _%sOVERFLOW;\n", cc_PREFIX(cc));
  }
  fprintf(outfp, "            }\n"
                 "            break;\n");
  fprintf(outfp, "            case _%sNO_MEMORY: {\n", cc_PREFIX(cc));
  fprintf(outfp, "              ");
  if (cc->on_alloc_error_snippet_.num_tokens_) {
    size_t token_idx;
    for (token_idx = 0; token_idx < cc->on_alloc_error_snippet_.num_tokens_; ++token_idx) {
      fprintf(outfp, "%s", cc->on_alloc_error_snippet_.tokens_[token_idx].text_.original_);
    }
  }
  else {
    fprintf(outfp, "return _%sNO_MEMORY;\n", cc_PREFIX(cc));
  }
  fprintf(outfp, "            }\n"
                 "            break;\n"
                 "          } /* switch */\n");
  fprintf(outfp, "          struct %ssym_data *sd = stack->stack_ + stack->pos_ - 1;\n", cc_prefix(cc));
  fprintf(outfp, "          *sd = nonterminal_sym_data_reduced_to;\n"
                 "          sd->state_ = action;\n");
  fprintf(outfp, "        } /* action < 0 */\n"
                 "        else /* action == 0 */ {\n");
  fprintf(outfp, "          /* check if we can recover using an error token. */\n"
                 "          size_t n;\n"
                 "          for (n = 0; n < stack->pos_; ++n) {\n");
  fprintf(outfp, "            int err_action = %sparse_table[%snum_columns * stack->stack_[n].state_ + (%d /* error token */ - %sminimum_sym)];\n", cc_prefix(cc), cc_prefix(cc), cc->error_sym_->ordinal_, cc_prefix(cc));
  fprintf(outfp, "            if (err_action > 0) {\n"
                 "              /* we can transition on the error token somewhere on the stack */\n"
                 "              break;\n"
                 "            }\n"
                 "          }\n");
  fprintf(outfp, "          if (n != stack->pos_) {\n"
                 "            /* Enter error-token recovery mode given that such a recovery is possible */\n");
  fprintf(outfp, "            stack->error_recovery_ = (n != stack->pos_);\n"
                 "            stack->report_error_ = !stack->mute_error_turns_;\n"
                 "            stack->mute_error_turns_ = 3;\n"
                 "          }\n"
                 "          else {\n"
                 "            /* Cannot recover, issue the error here */\n"
                 "            if (!stack->mute_error_turns_) {\n"
                 "              stack->mute_error_turns_ = 3;\n"
                 "              ");
  if (cc->on_syntax_error_snippet_.num_tokens_) {
    size_t token_idx;
    for (token_idx = 0; token_idx < cc->on_syntax_error_snippet_.num_tokens_; ++token_idx) {
      fprintf(outfp, "%s", cc->on_syntax_error_snippet_.tokens_[token_idx].text_.original_);
    }
  }
  else {
    fprintf(outfp, "/* Syntax error */\n"
                   "              return _%sSYNTAX_ERROR;\n", cc_PREFIX(cc));
  }
  fprintf(outfp, "            }\n"
                 "            else {\n"
                 "              stack->mute_error_turns_--;\n"
                 "            }\n"
                 "          }\n"
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
                 "                switch (%spush_state(stack, err_action /* action for a shift is the state */)) {\n", cc_prefix(cc));
  fprintf(outfp, "                  case _%sOVERFLOW: {\n", cc_PREFIX(cc));
  fprintf(outfp, "                    ");

  if (cc->on_internal_error_snippet_.num_tokens_) {
    size_t token_idx;
    for (token_idx = 0; token_idx < cc->on_internal_error_snippet_.num_tokens_; ++token_idx) {
      fprintf(outfp, "%s", cc->on_internal_error_snippet_.tokens_[token_idx].text_.original_);
    }
  }
  else {
    fprintf(outfp, "return _%sOVERFLOW;\n", cc_PREFIX(cc));
  }
  fprintf(outfp, "                  }\n"
                 "                  break;\n");
  fprintf(outfp, "                  case _%sNO_MEMORY: {\n", cc_PREFIX(cc));
  fprintf(outfp, "                    ");

  if (cc->on_alloc_error_snippet_.num_tokens_) {
    size_t token_idx;
    for (token_idx = 0; token_idx < cc->on_alloc_error_snippet_.num_tokens_; ++token_idx) {
      fprintf(outfp, "%s", cc->on_alloc_error_snippet_.tokens_[token_idx].text_.original_);
    }
  }
  else {
    fprintf(outfp, "return _%sNO_MEMORY;\n", cc_PREFIX(cc));
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


static int emit_parse_function(FILE *outfp, struct carburetta_context *cc, struct prd_grammar *prdg, struct lr_generator *lalr, int *state_syms) {
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
  fprintf(outfp, "  if (stack->pending_reset_) {\n"
                 "    int r;\n"
                 "    r = %sstack_reset(stack);\n"
                 "    if (r) return r;\n"
                 "  }\n", cc_prefix(cc));
  fprintf(outfp, "  if (stack->mute_error_turns_) stack->mute_error_turns_--;\n");
  fprintf(outfp, "  for (;;) {\n"
                 "    if (!stack->error_recovery_) {\n"
                 "      int action = %sparse_table[%snum_columns * stack->stack_[stack->pos_ - 1].state_ + (sym - %sminimum_sym)];\n", cc_prefix(cc), cc_prefix(cc), cc_prefix(cc));

  /* Shift logic */
  fprintf(outfp, "      if (action > 0) {\n");
  fprintf(outfp, "        switch (%spush_state(stack, action /* action for a shift is the ordinal */)) {\n", cc_prefix(cc));
  fprintf(outfp, "          case _%sOVERFLOW: {\n", cc_PREFIX(cc));
  fprintf(outfp, "            ");
  if (cc->on_internal_error_snippet_.num_tokens_) {
    size_t token_idx;
    for (token_idx = 0; token_idx < cc->on_internal_error_snippet_.num_tokens_; ++token_idx) {
      fprintf(outfp, "%s", cc->on_internal_error_snippet_.tokens_[token_idx].text_.original_);
    }
  }
  else {
    fprintf(outfp, "return _%sOVERFLOW;\n", cc_PREFIX(cc));
  }
  fprintf(outfp, "          }\n"
                 "          break;\n");
  fprintf(outfp, "          case _%sNO_MEMORY: {\n", cc_PREFIX(cc));
  fprintf(outfp, "            ");
  if (cc->on_alloc_error_snippet_.num_tokens_) {
    size_t token_idx;
    for (token_idx = 0; token_idx < cc->on_alloc_error_snippet_.num_tokens_; ++token_idx) {
      fprintf(outfp, "%s", cc->on_alloc_error_snippet_.tokens_[token_idx].text_.original_);
    }
  }
  else {
    fprintf(outfp, "return _%sNO_MEMORY;\n", cc_PREFIX(cc));
  }
  fprintf(outfp, "          }\n"
                 "          break;\n"
                 "        } /* switch */\n");

  fprintf(outfp, "\n");
  int need_sym_data = 0;
  fprintf(outfp, "        struct %ssym_data *sym_data = stack->stack_ + stack->pos_ - 1;\n", cc_prefix(cc));

  if (cc->common_data_assigned_type_ && cc->common_data_assigned_type_->constructor_snippet_.num_tokens_) {
    fprintf(outfp, "        {\n"
                   "          ");
    if (emit_token_common_constructor_snippet(outfp, cc)) {
      r = EXIT_FAILURE;
      goto cleanup_exit;
    }
    fprintf(outfp, "        }\n");
  }
  if (cc->common_data_assigned_type_ && cc->common_data_assigned_type_->token_action_snippet_.num_tokens_) {
    fprintf(outfp, "        {\n"
                   "          ");
    if (emit_token_common_action_snippet(outfp, cc)) {
      r = EXIT_FAILURE;
      goto cleanup_exit;
    }
    fprintf(outfp, "        }\n");
  }

  fprintf(outfp, "        switch (sym) {\n");
  size_t ts_idx;
  for (ts_idx = 0; ts_idx < cc->tstab_.num_typestrs_; ++ts_idx) {
    int found_matching_terms = 0;
    struct typestr *ts = cc->tstab_.typestrs_[ts_idx];
    struct symbol *term = cc->symtab_.terminals_;
    if (term) {
      do {
        term = term->next_;

        if (term->assigned_type_ == ts) {
          found_matching_terms = 1;
          fprintf(outfp, "          case ");
          print_sym_as_c_ident(outfp, cc, term);
          fprintf(outfp, ":\n");
        }

      } while (term != cc->symtab_.terminals_);
    }
    if (found_matching_terms) {
      if (ts->constructor_snippet_.num_tokens_) {
        fprintf(outfp, "            {\n"
                       "              ");
        if (emit_token_constructor_snippet(outfp, cc, ts)) {
          r = EXIT_FAILURE;
          goto cleanup_exit;
        }
        fprintf(outfp, "            }\n");
      }
      if (ts->token_action_snippet_.num_tokens_) {
        fprintf(outfp, "            {\n"
                       "              ");

        if (emit_token_action_snippet(outfp, cc, ts)) {
          r = EXIT_FAILURE;
          goto cleanup_exit;
        }

        fprintf(outfp, "            }\n");
      }
      fprintf(outfp, "            break;\n");
    }
  }
  fprintf(outfp, "        } /* switch */\n");
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
                   "          return _%sSYNTAX_ERROR;\n", cc_PREFIX(cc));
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
                   "          return _%sFEED_ME;\n", cc_PREFIX(cc));
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
                   "          stack->pending_reset_ = 1;\n"
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
                   "          return _%sINTERNAL_ERROR;\n", cc_PREFIX(cc));
  }
  fprintf(outfp, "        }\n");

  fprintf(outfp, "        switch (%spush_state(stack, action /* action for a \"goto\" shift is the ordinal */)) {\n", cc_prefix(cc));
  fprintf(outfp, "          case _%sOVERFLOW: {\n", cc_PREFIX(cc));
  fprintf(outfp, "            ");
  if (cc->on_internal_error_snippet_.num_tokens_) {
    size_t token_idx;
    for (token_idx = 0; token_idx < cc->on_internal_error_snippet_.num_tokens_; ++token_idx) {
      fprintf(outfp, "%s", cc->on_internal_error_snippet_.tokens_[token_idx].text_.original_);
    }
  }
  else {
    fprintf(outfp, "return _%sOVERFLOW;\n", cc_PREFIX(cc));
  }
  fprintf(outfp, "          }\n"
                 "          break;\n");
  fprintf(outfp, "          case _%sNO_MEMORY: /* out of memory */ {\n", cc_PREFIX(cc));
  fprintf(outfp, "            ");
  if (cc->on_alloc_error_snippet_.num_tokens_) {
    size_t token_idx;
    for (token_idx = 0; token_idx < cc->on_alloc_error_snippet_.num_tokens_; ++token_idx) {
      fprintf(outfp, "%s", cc->on_alloc_error_snippet_.tokens_[token_idx].text_.original_);
    }
  }
  else {
    fprintf(outfp, "return _%sNO_MEMORY;\n", cc_PREFIX(cc));
  }
  fprintf(outfp, "          }\n"
                 "          break;\n"
                 "        } /* switch */\n");
  fprintf(outfp, "        struct %ssym_data *sd = stack->stack_ + stack->pos_ - 1;\n", cc_prefix(cc));
  fprintf(outfp, "        *sd = nonterminal_sym_data_reduced_to;\n"
                 "        sd->state_ = action;\n");
  fprintf(outfp, "      } /* action < 0 */\n"
                 "      else /* action == 0 */ {\n");
  fprintf(outfp, "        /* check if we can recover using an error token. */\n"
                 "        size_t n;\n"
                 "        for (n = 0; n < stack->pos_; ++n) {\n");
  fprintf(outfp, "          int err_action = %sparse_table[%snum_columns * stack->stack_[n].state_ + (%d /* error token */ - %sminimum_sym)];\n", cc_prefix(cc), cc_prefix(cc), cc->error_sym_->ordinal_, cc_prefix(cc));
  fprintf(outfp, "          if (err_action > 0) {\n"
                 "            /* we can transition on the error token somewhere on the stack */\n"
                 "            break;\n"
                 "          }\n"
                 "        }\n");
  fprintf(outfp, "        if (n != stack->pos_) {\n"
                 "          /* Enter error-token recovery mode given that such a recovery is possible */\n");
  fprintf(outfp, "          stack->error_recovery_ = (n != stack->pos_);\n"
                 "          stack->report_error_ = !stack->mute_error_turns_;\n"
                 "          stack->mute_error_turns_ = 3;\n"
                 "        }\n"
                 "        else {\n"
                 "          /* Cannot recover, issue the error here */\n"
                 "          if (!stack->mute_error_turns_) {\n"
                 "            stack->mute_error_turns_ = 3;\n"
                 "            ");
  if (cc->on_syntax_error_snippet_.num_tokens_) {
    size_t token_idx;
    for (token_idx = 0; token_idx < cc->on_syntax_error_snippet_.num_tokens_; ++token_idx) {
      fprintf(outfp, "%s", cc->on_syntax_error_snippet_.tokens_[token_idx].text_.original_);
    }
  }
  else {
    fprintf(outfp, "/* Syntax error */\n"
                   "            return _%sSYNTAX_ERROR;\n", cc_PREFIX(cc));
  }
  fprintf(outfp, "          }\n"
                 "          else {\n"
                 "            stack->mute_error_turns_--;\n"
                 "          }\n"
                 "        }\n"
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

  fprintf(outfp, "              /* Push the state of the error transition */\n");
  fprintf(outfp, "              switch (%spush_state(stack, err_action /* action for a shift is the state */)) {\n", cc_prefix(cc));
  fprintf(outfp, "                case _%sOVERFLOW: {\n", cc_PREFIX(cc));
  fprintf(outfp, "                  ");
  if (cc->on_internal_error_snippet_.num_tokens_) {
    size_t token_idx;
    for (token_idx = 0; token_idx < cc->on_internal_error_snippet_.num_tokens_; ++token_idx) {
      fprintf(outfp, "%s", cc->on_internal_error_snippet_.tokens_[token_idx].text_.original_);
    }
  }
  else {
    fprintf(outfp, "return _%sOVERFLOW;\n", cc_PREFIX(cc));
  }
  fprintf(outfp, "                }\n"
                 "                break;\n");
  fprintf(outfp, "                case _%sNO_MEMORY: {\n", cc_PREFIX(cc));
  fprintf(outfp, "                  ");
  if (cc->on_alloc_error_snippet_.num_tokens_) {
    size_t token_idx;
    for (token_idx = 0; token_idx < cc->on_alloc_error_snippet_.num_tokens_; ++token_idx) {
      fprintf(outfp, "%s", cc->on_alloc_error_snippet_.tokens_[token_idx].text_.original_);
    }
  }
  else {
    fprintf(outfp, "return _%sNO_MEMORY;\n", cc_PREFIX(cc));
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
                   "        return _%sFEED_ME;\n", cc_PREFIX(cc));
  }
  fprintf(outfp, "      }\n");
  fprintf(outfp, "    } /* stack->error_recovery_ */\n");
  fprintf(outfp, "  } /* for (;;) */\n");
  fprintf(outfp, "}\n");

  r = 0;
cleanup_exit:
  return r;
}


int emit_c_file(FILE *outfp, struct carburetta_context *cc, struct prd_grammar *prdg, struct sc_scanner *scantable, struct lr_generator *lalr) {
  int r;

  struct part *pt;
  pt = cc->prologue_;
  if (pt) {
    do {
      pt = pt->next_;

      size_t written = fwrite(pt->chars_, 1, pt->num_chars_, outfp);
      if (written != pt->num_chars_) {
        int err = errno;
        re_error_nowhere("Failed to write to \"%s\": %s", cc->c_output_filename_, strerror(err));
        r = EXIT_FAILURE;
        goto cleanup_exit;
      }

    } while (pt != cc->prologue_);
  }

  fprintf(outfp, "/* --------- START OF GENERATED CODE ------------ */\n");

  fprintf(outfp, "#include <stdlib.h> /* realloc(), free(), NULL, size_t */\n");
  fprintf(outfp, "#include <string.h> /* memcpy() */\n");

  fprintf(outfp, "struct %ssym_data {\n", cc_prefix(cc));
  fprintf(outfp, "  int state_;\n");
  if (cc->common_data_assigned_type_) {
    struct typestr *ts = cc->common_data_assigned_type_;
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
  if (cc->have_typed_symbols_) {
    fprintf(outfp, "  union {\n");
    size_t ts_idx;
    for (ts_idx = 0; ts_idx < cc->tstab_.num_typestrs_; ++ts_idx) {
      struct typestr *ts;
      ts = cc->tstab_.typestrs_[ts_idx];
      int found_placeholder = 0;

      if (ts == cc->common_data_assigned_type_) {
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

  if (prdg->num_patterns_) {
    fprintf(outfp, "static const size_t %sscan_table[] = {\n", cc_prefix(cc));
    size_t row, col;
    char column_widths[256] = {0};

    for (col = 0; col < 256; ++col) {
      column_widths[col] = 1;
      for (row = 0; row < scantable->num_states; ++row) {
        size_t v = scantable->transition_table[row * 256 + col];

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

    for (row = 0; row < scantable->num_states; ++row) {
      for (col = 0; col < 256; ++col) {
        fprintf(outfp, "%s%*zu", col ? "," : "  ", column_widths[col], scantable->transition_table[256 * row + col]);
      }
      if (row != (scantable->num_states - 1)) {
        fprintf(outfp, ",\n");
      }
      else {
        fprintf(outfp, "\n");
      }
    }
    fprintf(outfp, "};\n");
    fprintf(outfp, "static const size_t %sscan_actions[] = { ", cc_prefix(cc));
    for (col = 0; col < scantable->num_states; ++col) {
      fprintf(outfp, "%s%zu", col ? ", " : "", (size_t)scantable->actions[col].action);
    }
    fprintf(outfp, " };\n");
  }

  size_t num_columns;
  num_columns = (size_t)(1 + lalr->max_sym_ - lalr->min_sym_);
  fprintf(outfp, "static const int %sminimum_sym = %d;\n", cc_prefix(cc), lalr->min_sym_);
  fprintf(outfp, "static const size_t %snum_columns = %zu;\n", cc_prefix(cc), num_columns);
  fprintf(outfp, "static const size_t %snum_rows = %zu;\n", cc_prefix(cc), (size_t)lalr->nr_states_);
  fprintf(outfp, "static const size_t %snum_productions = %zu;\n", cc_prefix(cc), lalr->nr_productions_);
  fprintf(outfp, "static const int %sparse_table[] = {\n", cc_prefix(cc));
  size_t row, col;
  char *column_widths;
  column_widths = (char *)malloc(num_columns);
  if (!column_widths) {
    re_error_nowhere("Error, no memory\n");
    r = EXIT_FAILURE;
    goto cleanup_exit;
  }

  for (col = 0; col < num_columns; ++col) {
    column_widths[col] = 1;
    for (row = 0; row < lalr->nr_states_; ++row) {
      int action = lalr->parse_table_[row * num_columns + col];
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
  for (row = 0; row < lalr->nr_states_; ++row) {
    for (col = 0; col < num_columns; ++col) {
      int action = lalr->parse_table_[row * num_columns + col];

      fprintf(outfp, "%*d,%s", column_widths[col], action, col == (num_columns - 1) ? "\n" : "");
    }
  }
  free(column_widths);
  fprintf(outfp, "};\n");
  fprintf(outfp, "static const size_t %sproduction_lengths[] = {\n", cc_prefix(cc));
  for (row = 0; row < lalr->nr_productions_; ++row) {
    fprintf(outfp, " %d%s\n", lalr->production_lengths_[row], (row == lalr->nr_productions_ - 1) ? "" : ",");
  }
  fprintf(outfp, "};\n");
  fprintf(outfp, "static const int %sproduction_syms[] = {\n", cc_prefix(cc));
  for (row = 0; row < lalr->nr_productions_; ++row) {
    fprintf(outfp, " %d%s\n", lalr->productions_[row][0], (row == lalr->nr_productions_ - 1) ? "" : ",");
  }
  fprintf(outfp, "};\n");

  /* For each state, what is the top-most symbol on the stack? */
  fprintf(outfp, "static const int %sstate_syms[] = {\n", cc_prefix(cc));

  int *state_syms;
  state_syms = NULL;

  state_syms = (int *)malloc(sizeof(int) * (size_t)lalr->nr_states_);
  if (!state_syms) {
    re_error_nowhere("Error, no memory");
    r = EXIT_FAILURE;
    goto cleanup_exit;
  }
  for (row = 0; row < lalr->nr_states_; ++row) {
    state_syms[row] = -1;
  }
  for (row = 0; row < lalr->nr_states_; ++row) {
    for (col = 0; col < num_columns; ++col) {
      int action = lalr->parse_table_[row * num_columns + col];
      if (action > 0) {
        /* We're shifting to a destination state. */
        int sym_shifting = ((int)col) + lalr->min_sym_;
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
  for (row = 0; row < lalr->nr_states_; ++row) {
    fprintf(outfp, " %d%s\n", state_syms[row], (row == lalr->nr_states_ - 1) ? "" : ",");
  }
  fprintf(outfp, "};\n");


  if (cc->include_guard_) {
    fprintf(outfp, "\n#ifndef %s\n", cc->include_guard_);
  }

  fprintf(outfp, "struct %sstack {\n", cc_prefix(cc));
  fprintf(outfp, "  int error_recovery_:1;\n");
  fprintf(outfp, "  int report_error_:1;\n");
  fprintf(outfp, "  int pending_reset_:1;\n");
  if (prdg->num_patterns_) {
    fprintf(outfp, "  int need_sym_:1;\n"
                    "  int current_sym_;\n");
  }
  fprintf(outfp, "  int mute_error_turns_;\n");
  fprintf(outfp, "  size_t pos_, num_stack_allocated_;\n");
  fprintf(outfp, "  struct %ssym_data *stack_;\n", cc_prefix(cc));
  if (prdg->num_patterns_) {
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

  fprintf(outfp, "#define _%sMATCH 1\n", cc_PREFIX(cc));
  fprintf(outfp, "#define _%sOVERFLOW 2\n", cc_PREFIX(cc));
  fprintf(outfp, "#define _%sNO_MEMORY 3\n", cc_PREFIX(cc));
  fprintf(outfp, "#define _%sFEED_ME 4\n", cc_PREFIX(cc));
  fprintf(outfp, "#define _%sEND_OF_INPUT 5\n", cc_PREFIX(cc));
  fprintf(outfp, "#define _%sSYNTAX_ERROR 6\n", cc_PREFIX(cc));
  fprintf(outfp, "#define _%sINTERNAL_ERROR 7\n", cc_PREFIX(cc));
  fprintf(outfp, "\n");

  struct symbol *sym;

  sym = cc->symtab_.terminals_;
  if (sym) {
    do {
      sym = sym->next_;

      fprintf(outfp, "#define ");
      if (print_sym_as_c_ident(outfp, cc, sym)) {
        r = EXIT_FAILURE;
        goto cleanup_exit;
      }
      fprintf(outfp, " %d\n", sym->ordinal_);

    } while (sym != cc->symtab_.terminals_);
  }
  fprintf(outfp, "\n");
  sym = cc->symtab_.non_terminals_;
  if (sym) {
    do {
      sym = sym->next_;

      char *ident = (char *)malloc(1 + sym->def_.num_translated_);
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

      fprintf(outfp, "#define %s%s %d\n", cc_PREFIX(cc), ident, sym->ordinal_);
      free(ident);
    } while (sym != cc->symtab_.non_terminals_);
  }

  if (cc->include_guard_) {
    fprintf(outfp, "#endif /* %s */\n\n", cc->include_guard_);
  }

  /* Emit stack constructor, destructor and reset functions */
  fprintf(outfp,
    "void %sstack_init(struct %sstack *stack) {\n", cc_prefix(cc), cc_prefix(cc));
  fprintf(outfp,
    "  stack->error_recovery_ = 0;\n"
    "  stack->report_error_ = 0;\n"
    "  stack->pending_reset_ = 1;\n");
  if (prdg->num_patterns_) {
    fprintf(outfp, "  stack->need_sym_ = 1;\n"
                    "  stack->current_sym_ = 0;\n");
  }
  fprintf(outfp, 
    "  stack->mute_error_turns_ = 0;\n"
    "  stack->pos_ = 0;\n"
    "  stack->num_stack_allocated_ = 0;\n"
    "  stack->stack_ = NULL;\n");
  if (prdg->num_patterns_) {
    fprintf(outfp, "  stack->scan_state_ = %zu;\n", scantable->start_state);
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
    fprintf(outfp, "  stack->best_match_action_ = %zu;\n", (size_t)scantable->actions[scantable->start_state].action);
    fprintf(outfp, "  stack->best_match_size_ = 0;\n"
                    "  stack->best_match_offset_ = 0;\n"
                    "  stack->best_match_line_ = 1;\n"
                    "  stack->best_match_col_ = 1;\n");
  }

  fprintf(outfp,
    "}\n"
    "\n");


  fprintf(outfp,
    "void %sstack_cleanup(struct %sstack *stack) {\n", cc_prefix(cc), cc_prefix(cc));
  fprintf(outfp,
    "  size_t n;\n"
    "  for (n = 1; n < stack->pos_; ++n) {\n");
  fprintf(outfp,
    "    switch (stack->stack_[n].state_) {\n");
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
          fprintf(outfp, "    case %d: /* %s */\n", (int)state_idx, sym->def_.translated_);
          have_cases = 1;
        }
      }
      if (have_cases) {
        fprintf(outfp, "    {\n      ");

        if (emit_destructor_snippet_indexed_by_n(outfp, cc, ts)) {
          r = EXIT_FAILURE;
          goto cleanup_exit;
        }

        /* Close this compound block  */
        fprintf(outfp, "\n    }\n    break;\n");
      }
    }
  }


  fprintf(outfp, "    } /* switch */\n");
  if (cc->common_data_assigned_type_ && cc->common_data_assigned_type_->constructor_snippet_.num_tokens_) {
    fprintf(outfp, "    {\n"
                    "      ");
    if (emit_common_destructor_snippet_indexed_by_n(outfp, cc)) {
      r = EXIT_FAILURE;
      goto cleanup_exit;
    }
    fprintf(outfp, "\n"
                    "    }\n");
  }

  fprintf(outfp, "  }\n");
  if (prdg->num_patterns_) {
    fprintf(outfp, "  if ((!stack->need_sym_) && stack->pos_) {\n"
                    "    /* Deconstruct placeholder location for terminal not yet shifted */\n");
    fprintf(outfp, "    switch (stack->current_sym_) {\n");

    size_t ts_idx;
    for (ts_idx = 0; ts_idx < cc->tstab_.num_typestrs_; ++ts_idx) {
      struct typestr *ts = cc->tstab_.typestrs_[ts_idx];
      struct symbol *the_syms[] = { cc->symtab_.terminals_, cc->symtab_.non_terminals_ };
      size_t n;
      int have_some = 0;
      for (n = 0; n < sizeof(the_syms) / sizeof(*the_syms); ++n) {
        struct symbol *sym = the_syms[n];
        if (sym) {
          do {
            if (sym->assigned_type_ == ts) {
              have_some = 1;
              fprintf(outfp, "      case %d:\n", sym->ordinal_);
            }

            sym = sym->next_;
          } while (sym != the_syms[n]);
        }
      }
      if (have_some) {
        /* Execute destructors for typestr ts at stack->stack_[0] */
        if (ts->destructor_snippet_.num_tokens_) {
          fprintf(outfp, "        {\n"
                          "          ");
          if (emit_destructor_snippet_indexed_by_0(outfp, cc, ts)) {
            r = EXIT_FAILURE;
            goto cleanup_exit;
          }
          fprintf(outfp, "\n"
                          "        }\n");
                           
        }
        fprintf(outfp, "        break;\n");
      }
    }
    fprintf(outfp, "    } /* switch */\n");

    if (cc->common_data_assigned_type_ && cc->common_data_assigned_type_->constructor_snippet_.num_tokens_) {
      fprintf(outfp, "    {\n"
                      "      ");
      if (emit_common_destructor_snippet_index_0(outfp, cc)) {
        r = EXIT_FAILURE;
        goto cleanup_exit;
      }
      fprintf(outfp, "\n"
                      "    }\n");
    }
    fprintf(outfp, "  }\n");
  }


  fprintf(outfp, "\n"
                  "  if (stack->stack_) free(stack->stack_);\n");
  if (prdg->num_patterns_) {
    fprintf(outfp, "  if (stack->match_buffer_) free(stack->match_buffer_);\n");
  }
  fprintf(outfp, "}\n"
                 "\n");

  fprintf(outfp, "static int %spush_state(struct %sstack *stack, int state) {\n", cc_prefix(cc), cc_prefix(cc));
  fprintf(outfp, "  if (stack->num_stack_allocated_ == stack->pos_) {\n"
                 "    size_t new_num_allocated = 0;\n"
                 "    if (stack->num_stack_allocated_) {\n"
                 "      new_num_allocated = stack->num_stack_allocated_ * 2;\n"
                 "      if (new_num_allocated <= stack->num_stack_allocated_) {\n");
  fprintf(outfp, "        /* Overflow in allocation */\n"
                 "        return _%sOVERFLOW;\n", cc_PREFIX(cc));
  fprintf(outfp, "      }\n"
                 "    }\n"
                 "    else {\n"
                 "      new_num_allocated = 16;\n"
                 "    }\n"
                 "\n"
                 "    if (new_num_allocated > (SIZE_MAX / sizeof(struct %ssym_data))) {\n", cc_prefix(cc));
  fprintf(outfp,
    "      /* Overflow in allocation */\n"
    "      return _%sOVERFLOW;\n", cc_PREFIX(cc));
  fprintf(outfp,
    "    }\n"
    "\n"
    "    void *p = realloc(stack->stack_, new_num_allocated * sizeof(struct %ssym_data));\n", cc_prefix(cc));
  fprintf(outfp,
    "    if (!p) {\n");
  fprintf(outfp,
    "      /* Out of memory */\n"
    "        return _%sNO_MEMORY;\n", cc_PREFIX(cc));
  fprintf(outfp,
    "    }\n"
    "    stack->stack_ = (struct %ssym_data *)p;\n", cc_prefix(cc));
  fprintf(outfp,
    "    stack->num_stack_allocated_ = new_num_allocated;\n"
    "  }\n"
    "  stack->stack_[stack->pos_++].state_ = state;\n"
    "  return 0;\n"
    "}\n");

  fprintf(outfp,
    "int %sstack_reset(struct %sstack *stack) {\n", cc_prefix(cc), cc_prefix(cc));
  fprintf(outfp,
    "  size_t n;\n"
    "  stack->pending_reset_ = 0;\n"
    "  for (n = 1; n < stack->pos_; ++n) {\n");
  fprintf(outfp,
    "    switch (stack->stack_[n].state_) {\n");
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
          fprintf(outfp, "    case %d: /* %s */\n", (int)state_idx, sym->def_.translated_);
          have_cases = 1;
        }
      }
      if (have_cases) {
        fprintf(outfp, "    {\n    ");
        if (emit_destructor_snippet_indexed_by_n(outfp, cc, ts)) {
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
  if (cc->common_data_assigned_type_ && cc->common_data_assigned_type_->constructor_snippet_.num_tokens_) {
    fprintf(outfp, "    {\n"
                    "      ");
    if (emit_common_destructor_snippet_indexed_by_n(outfp, cc)) {
      r = EXIT_FAILURE;
      goto cleanup_exit;
    }
    fprintf(outfp, "\n"
                    "    }\n");
  }

  fprintf(outfp,
    "  }\n");

  if (prdg->num_patterns_) {
    fprintf(outfp, "  if ((!stack->need_sym_) && stack->pos_) {\n"
                    "    /* Deconstruct placeholder location for terminal not yet shifted */\n");
    fprintf(outfp, "    switch (stack->current_sym_) {\n");

    size_t ts_idx;
    for (ts_idx = 0; ts_idx < cc->tstab_.num_typestrs_; ++ts_idx) {
      struct typestr *ts = cc->tstab_.typestrs_[ts_idx];
      struct symbol *the_syms[] = { cc->symtab_.terminals_, cc->symtab_.non_terminals_ };
      size_t n;
      int have_some = 0;
      for (n = 0; n < sizeof(the_syms) / sizeof(*the_syms); ++n) {
        struct symbol *sym = the_syms[n];
        if (sym) {
          do {
            if (sym->assigned_type_ == ts) {
              have_some = 1;
              fprintf(outfp, "      case %d:\n", sym->ordinal_);
            }

            sym = sym->next_;
          } while (sym != the_syms[n]);
        }
      }
      if (have_some) {
        /* Execute destructors for typestr ts at stack->stack_[0] */
        if (ts->destructor_snippet_.num_tokens_) {
          fprintf(outfp, "        {\n"
                          "          ");
          if (emit_destructor_snippet_indexed_by_0(outfp, cc, ts)) {
            r = EXIT_FAILURE;
            goto cleanup_exit;
          }
          fprintf(outfp, "\n"
                          "        }\n");
        }
        fprintf(outfp, "        break;\n");
      }
    }
    fprintf(outfp, "    } /* switch */\n");

    if (cc->common_data_assigned_type_ && cc->common_data_assigned_type_->constructor_snippet_.num_tokens_) {
      fprintf(outfp, "    {\n"
                      "      ");
      if (emit_common_destructor_snippet_index_0(outfp, cc)) {
        r = EXIT_FAILURE;
        goto cleanup_exit;
      }
      fprintf(outfp, "\n"
                      "    }\n");
    }
    fprintf(outfp, "  }\n");
  }

  fprintf(outfp,
    "  stack->pos_ = 0;\n"
    "  stack->error_recovery_ = 0;\n"
    "  stack->report_error_ = 0;\n");
  if (prdg->num_patterns_) {
    fprintf(outfp, "  stack->need_sym_ = 1;\n"
                    "  stack->current_sym_ = 0;\n");
  }
  fprintf(outfp,
    "  stack->mute_error_turns_ = 0;\n");

  fprintf(outfp, "  switch (%spush_state(stack, 0)) {\n"
                  "    case _%sOVERFLOW:{\n", cc_prefix(cc), cc_PREFIX(cc));
  fprintf(outfp, "      return _%sOVERFLOW;\n", cc_PREFIX(cc));
  fprintf(outfp, "    }\n    break;\n"
                  "    case _%sNO_MEMORY: {\n", cc_PREFIX(cc));
  fprintf(outfp, "      return _%sNO_MEMORY;\n", cc_PREFIX(cc));
  fprintf(outfp, "    }\n"
                  "    break;\n"
                  "  }\n");

  if (prdg->num_patterns_) {
    fprintf(outfp, "  stack->scan_state_ = %zu;\n", scantable->start_state);
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
    fprintf(outfp, "  stack->best_match_action_ = %zu;\n", (size_t)scantable->actions[scantable->start_state].action);
    fprintf(outfp, "  stack->best_match_size_ = 0;\n"
                    "  stack->best_match_offset_ = 0;\n"
                    "  stack->best_match_line_ = 1;\n"
                    "  stack->best_match_col_ = 1;\n");
  }

  fprintf(outfp, "  return 0;\n"
                  "}\n"
                  "\n");

  if (prdg->num_patterns_) {
    r = emit_lex_function(outfp, cc, prdg, scantable);
    if (r) {
      r = EXIT_FAILURE;
      goto cleanup_exit;
    }
    r = emit_scan_function(outfp, cc, prdg, lalr, state_syms);
  }

  r = emit_parse_function(outfp, cc, prdg, lalr, state_syms);
  if (r) {
    r = EXIT_FAILURE;
    goto cleanup_exit;
  }

  fprintf(outfp, "/* --------- END OF GENERATED CODE ------------ */\n");

  pt = cc->epilogue_;
  if (pt) {
    do {
      pt = pt->next_;

      size_t written = fwrite(pt->chars_, 1, pt->num_chars_, outfp);
      if (written != pt->num_chars_) {
        int err = errno;
        re_error_nowhere("Failed to write to \"%s\": %s", cc->c_output_filename_, strerror(err));
        r = EXIT_FAILURE;
        goto cleanup_exit;
      }

    } while (pt != cc->epilogue_);
  }


  r = 0;
cleanup_exit:
  if (state_syms) free(state_syms);

  return r;
}

int emit_h_file(FILE *outfp, struct carburetta_context *cc, struct prd_grammar *prdg) {
  int r;
  struct symbol *sym;
  fprintf(outfp, "#ifndef %s\n"
                  "#define %s\n"
                  "\n", cc->include_guard_, cc->include_guard_);

  fprintf(outfp, "#include <stddef.h> /* size_t */\n"
                  "\n");

  fprintf(outfp, "#ifdef __cplusplus\n"
                  "extern \"C\" {\n"
                  "#endif\n"
                  "\n");

  fprintf(outfp, "#define _%sMATCH 1\n", cc_PREFIX(cc));
  fprintf(outfp, "#define _%sOVERFLOW 2\n", cc_PREFIX(cc));
  fprintf(outfp, "#define _%sNO_MEMORY 3\n", cc_PREFIX(cc));
  fprintf(outfp, "#define _%sFEED_ME 4\n", cc_PREFIX(cc));
  fprintf(outfp, "#define _%sEND_OF_INPUT 5\n", cc_PREFIX(cc));
  fprintf(outfp, "#define _%sSYNTAX_ERROR 6\n", cc_PREFIX(cc));
  fprintf(outfp, "#define _%sINTERNAL_ERROR 7\n", cc_PREFIX(cc));
  fprintf(outfp, "\n");

  sym = cc->symtab_.terminals_;
  if (sym) {
    do {
      sym = sym->next_;

      char *ident = (char *)malloc(1 + sym->def_.num_translated_);
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

      fprintf(outfp, "#define %s%s %d\n", cc_TOKEN_PREFIX(cc), ident, sym->ordinal_);
      free(ident);
    } while (sym != cc->symtab_.terminals_);
  }
  fprintf(outfp, "\n");

  fprintf(outfp, "struct %sstack {\n", cc_prefix(cc));
  fprintf(outfp, "  int error_recovery_:1;\n");
  fprintf(outfp, "  int report_error_:1;\n");
  fprintf(outfp, "  int pending_reset_:1;\n");
  if (prdg->num_patterns_) {
    fprintf(outfp, "  int need_sym_:1;\n"
                    "  int current_sym_;\n");
  }
  fprintf(outfp, "  int mute_error_turns_;\n");
  fprintf(outfp, "  size_t pos_, num_stack_allocated_;\n");
  fprintf(outfp, "  struct %ssym_data *stack_;\n", cc_prefix(cc));
  if (prdg->num_patterns_) {
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
  fprintf(outfp, "void %sstack_init(struct %sstack *stack);\n", cc_prefix(cc), cc_prefix(cc));
  fprintf(outfp, "void %sstack_cleanup(struct %sstack *stack);\n", cc_prefix(cc), cc_prefix(cc));
  fprintf(outfp, "int %sstack_reset(struct %sstack *stack);\n", cc_prefix(cc), cc_prefix(cc));
  if (cc->params_snippet_.num_tokens_) {
    fprintf(outfp, "int %sparse(struct %sstack *stack, int sym, ", cc_prefix(cc), cc_prefix(cc));
    size_t token_idx;
    for (token_idx = 0; token_idx < cc->params_snippet_.num_tokens_; ++token_idx) {
      fprintf(outfp, "%s", cc->params_snippet_.tokens_[token_idx].text_.original_);
    }
    fprintf(outfp, ");\n");
  }
  else {
    fprintf(outfp,
      "int %sparse(struct %sstack *stack, int sym);\n", cc_prefix(cc), cc_prefix(cc));
  }

  fprintf(outfp, "\n"
                  "#ifdef __cplusplus\n"
                  "} /* extern \"C\" */\n"
                  "#endif\n");

  fprintf(outfp, "\n"
                  "#endif /* %s */\n", cc->include_guard_);

  r = 0;

cleanup_exit:
  return r;
}
