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

#ifndef STDARG_H_INCLUDED
#define STDARG_H_INCLUDED
#include <stdarg.h>
#endif

#ifndef CTYPE_H_INCLUDED
#define CTYPE_H_INCLUDED
#include <ctype.h>
#endif

#ifndef ERRNO_H_INCLUDED
#define ERRNO_H_INCLUDED
#include <errno.h>
#endif

#ifndef ASSERT_H_INCLUDED
#define ASSERT_H_INCLUDED
#include <assert.h>
#endif

#ifndef MUL_H_INCLUDED
#define MUL_H_INCLUDED
#include "mul.h"
#endif

#ifndef EMIT_C_H_INCLUDED
#define EMIT_C_H_INCLUDED
#include "emit_c.h"
#endif

#ifndef INDENTED_PRINTER_H_INCLUDED
#define INDENTED_PRINTER_H_INCLUDED
#include "indented_printer.h"
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

enum line_type {
  SELIT_NONE,
  SELIT_FMT
};

enum col_type {
  SECOT_NONE,
  SECOT_FMT
};

enum offset_type {
  SEOT_NONE,
  SEOT_FMT
};

enum end_line_type {
  SEELIT_NONE,
  SEELIT_FMT
};

enum end_col_type {
  SEECOT_NONE,
  SEECOT_FMT
};

enum end_offset_type {
  SEEOT_NONE,
  SEEOT_FMT
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

  enum line_type line_type_;
  const char *line_fmt_;

  enum col_type col_type_;
  const char *col_fmt_;

  enum offset_type offset_type_;
  const char *offset_fmt_;

  enum end_line_type end_line_type_;
  const char *end_line_fmt_;

  enum end_col_type end_col_type_;
  const char *end_col_fmt_;

  enum end_offset_type end_offset_type_;
  const char *end_offset_fmt_;
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

static int print_sym_as_c_ident(struct indented_printer *ip, struct carburetta_context *cc, struct symbol *sym) {
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

  ip_printf(ip, "%s%s", cc_TOKEN_PREFIX(cc), ident);

  free(ident);

  return 0;
}

static void print_regex_as_comment(struct indented_printer *ip, const char *regex) {
  ip_puts_no_indent(ip, "/* ");
  while (*regex) {
    /* Make sure we don't accidentally close the comment */
    if ((regex[0] == '*') && (regex[1] == '/')) {
      ip_puts_no_indent(ip, "* /");
    }
    else {
      ip_write_no_indent(ip, regex, 1);
    }
    regex++;
  }
  ip_puts_no_indent(ip, " */");
}

static int emit_dest(struct indented_printer *ip, struct carburetta_context *cc, struct snippet_emission *se, struct snippet_token *st) {
  switch (se->dest_type_) {
  case SEDT_NONE:
    re_error(&st->text_, "$$ cannot resolve to a symbol");
    return TKR_SYNTAX_ERROR;
  case SEDT_FMT:
    ip_puts_no_indent(ip, se->dest_fmt_);
    break;
  case SEDT_FMT_PREFIX:
    ip_printf_no_indent(ip, se->dest_fmt_, cc_prefix(cc));
    break;
  case SEDT_FMT_DATATYPE_ORDINAL:
    if (!se->dest_->sym_->assigned_type_) {
      re_error(&st->text_, "$$ cannot resolve to a data type for non-terminal %s", se->dest_->id_.translated_);
      return TKR_SYNTAX_ERROR;
    }
    ip_printf_no_indent(ip, se->dest_fmt_, se->dest_->sym_->assigned_type_->ordinal_);
    break;
  case SEDT_FMT_PREFIX_DATATYPE_ORDINAL:
    if (!se->dest_->sym_->assigned_type_) {
      re_error(&st->text_, "$$ cannot resolve to a data type for non-terminal %s", se->dest_->id_.translated_);
      return TKR_SYNTAX_ERROR;
    }
    ip_printf_no_indent(ip, se->dest_fmt_, cc_prefix(cc), se->dest_->sym_->assigned_type_->ordinal_);
    break;
  case SEDT_FMT_PREFIX_TYPESTR_ORDINAL:
    ip_printf_no_indent(ip, se->dest_fmt_, cc_prefix(cc), se->dest_typestr_->ordinal_);
    break;
  case SEDT_FMT_TYPESTR_ORDINAL:
    ip_printf_no_indent(ip, se->dest_fmt_, se->dest_typestr_->ordinal_);
    break;
  }

  return 0;
}

static int emit_sym_specific_data(struct indented_printer *ip, struct carburetta_context *cc, struct snippet_emission *se, struct snippet_token *st, size_t sym_index) {
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
    ip_printf_no_indent(ip, se->sym_fmt_, sym_index, sym->assigned_type_->ordinal_);
    break;
  }
  return 0;
}

static int emit_dest_commondata(struct indented_printer *ip, struct carburetta_context *cc, struct snippet_emission *se, struct snippet_token *st) {
  if (se->common_dest_type_ == SECDT_NONE) {
    re_error(&st->text_, "$ cannot resolve to a symbol");
    return TKR_SYNTAX_ERROR;
  }
  if (!cc->common_data_assigned_type_) {
    re_error(&st->text_, "$ cannot resolve, no common data type defined");
  }

  switch (se->common_dest_type_) {
  case SECDT_FMT_PREFIX:
    ip_printf_no_indent(ip, se->common_dest_fmt_, cc_prefix(cc));
    break;
  case SECDT_FMT:
    ip_puts_no_indent(ip, se->common_dest_fmt_);
    break;
  }

  return 0;
}

static int emit_len(struct indented_printer *ip, struct carburetta_context *cc, struct snippet_emission *se, struct snippet_token *st) {
  if (se->len_type_ == SELT_NONE) {
    re_error(&st->text_, "$len cannot resolve to a length");
    return TKR_SYNTAX_ERROR;
  }
  switch (se->len_type_) {
  case SELT_FMT:
    ip_puts_no_indent(ip, se->len_fmt_);
    break;
  }
  return 0;
}

static int emit_discard(struct indented_printer *ip, struct carburetta_context *cc, struct snippet_emission *se, struct snippet_token *st) {
  if (se->discard_type_ == SEDIT_NONE) {
    re_error(&st->text_, "$discard use is limited to common actions");
    return TKR_SYNTAX_ERROR;
  }
  switch (se->discard_type_) {
  case SEDIT_FMT:
    ip_puts_no_indent(ip, se->discard_fmt_);
    break;
  }
  return 0;
}

static int emit_text(struct indented_printer *ip, struct carburetta_context *cc, struct snippet_emission *se, struct snippet_token *st) {
  if (se->text_type_ == SETT_NONE) {
    re_error(&st->text_, "$text use is limited to pattern actions");
    return TKR_SYNTAX_ERROR;
  }
  switch (se->text_type_) {
  case SETT_FMT:
    ip_puts_no_indent(ip, se->text_fmt_);
    break;
  }
  return 0;
}

static int emit_line(struct indented_printer *ip, struct carburetta_context *cc, struct snippet_emission *se, struct snippet_token *st) {
  if (se->line_type_ == SELIT_NONE) {
    re_error(&st->text_, "$line use is limited to pattern actions");
    return TKR_SYNTAX_ERROR;
  }
  switch (se->line_type_) {
  case SELIT_FMT:
    ip_puts_no_indent(ip, se->line_fmt_);
    break;
  }
  return 0;
}

static int emit_column(struct indented_printer *ip, struct carburetta_context *cc, struct snippet_emission *se, struct snippet_token *st) {
  if (se->col_type_ == SECOT_NONE) {
    re_error(&st->text_, "$column use is limited to pattern actions");
    return TKR_SYNTAX_ERROR;
  }
  switch (se->col_type_) {
  case SECOT_FMT:
    ip_puts_no_indent(ip, se->col_fmt_);
    break;
  }
  return 0;
}

static int emit_offset(struct indented_printer *ip, struct carburetta_context *cc, struct snippet_emission *se, struct snippet_token *st) {
  if (se->offset_type_ == SEOT_NONE) {
    re_error(&st->text_, "$offset use is limited to pattern actions");
    return TKR_SYNTAX_ERROR;
  }
  switch (se->offset_type_) {
  case SELIT_FMT:
    ip_puts_no_indent(ip, se->offset_fmt_);
    break;
  }
  return 0;
}

static int emit_endline(struct indented_printer *ip, struct carburetta_context *cc, struct snippet_emission *se, struct snippet_token *st) {
  if (se->end_line_type_ == SEELIT_NONE) {
    re_error(&st->text_, "$endline use is limited to pattern actions");
    return TKR_SYNTAX_ERROR;
  }
  switch (se->end_line_type_) {
  case SEELIT_FMT:
    ip_puts_no_indent(ip, se->end_line_fmt_);
    break;
  }
  return 0;
}

static int emit_endcolumn(struct indented_printer *ip, struct carburetta_context *cc, struct snippet_emission *se, struct snippet_token *st) {
  if (se->end_col_type_ == SEECOT_NONE) {
    re_error(&st->text_, "$endcolumn use is limited to pattern actions");
    return TKR_SYNTAX_ERROR;
  }
  switch (se->end_col_type_) {
  case SEECOT_FMT:
    ip_puts_no_indent(ip, se->end_col_fmt_);
    break;
  }
  return 0;
}

static int emit_endoffset(struct indented_printer *ip, struct carburetta_context *cc, struct snippet_emission *se, struct snippet_token *st) {
  if (se->end_offset_type_ == SEEOT_NONE) {
    re_error(&st->text_, "$endoffset use is limited to pattern actions");
    return TKR_SYNTAX_ERROR;
  }
  switch (se->end_offset_type_) {
  case SELIT_FMT:
    ip_puts_no_indent(ip, se->end_offset_fmt_);
    break;
  }
  return 0;
}



static int emit_common(struct indented_printer *ip, struct carburetta_context *cc, struct snippet_emission *se, struct snippet_token *special_ident_token, size_t start_of_index, size_t end_of_index) {
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
    ip_printf_no_indent(ip, se->common_fmt_prefix_, cc_prefix(cc));
    break;
  case SECT_FMT:
    ip_puts_no_indent(ip, se->common_fmt_prefix_);
    break;
  }
  size_t n;
  for (n = start_of_index; n < end_of_index; ++n) {
    ip_printf_no_indent(ip, "%s", se->code_->tokens_[n].text_.original_);
  }
  ip_puts_no_indent(ip, se->common_fmt_suffix_);
  return 0;
}

static int emit_snippet_code_emission(struct indented_printer *ip, struct carburetta_context *cc, struct snippet_emission *se, int retry_on_exit) {
  int r;
  size_t col;
  if (!se->code_->num_tokens_) {
    /* Nothing to emit */
    return 0;
  }
  if (cc->continuation_enabled_) {
    ip_printf(ip, "stack->continue_at_ = %d;\n", cc->current_snippet_continuation_);
    if (retry_on_exit) {
      ip_printf(ip, "C%d:;\n", cc->current_snippet_continuation_++);
    }
  }
  ip_printf(ip, "{\n");
  ip_force_indent_print(ip);
  for (col = 0; col < se->code_->num_tokens_; ++col) {
    /* Print the original code, to preserve formatting and line continuations */
    if (se->code_->tokens_[col].match_ == TOK_SPECIAL_IDENT_DST) {
      /* Expansion of special destination sym identifier */
      r = emit_dest(ip, cc, se, se->code_->tokens_ + col);
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
          r = emit_common(ip, cc, se, se->code_->tokens_ + special_ident_at, index_starts_at, index_ends_at);
          if (r) {
            return r;
          }
        }
      }
      else {
        /* Single $ with no { } following it refers to the destination symbol's common data */
        r = emit_dest_commondata(ip, cc, se, se->code_->tokens_ + col);
        if (r) {
          return r;
        }
      }
    }
    else if ((se->code_->tokens_[col].match_ == TOK_SPECIAL_IDENT_STR) && !strcmp("$len", se->code_->tokens_[col].text_.translated_)) {
      r = emit_len(ip, cc, se, se->code_->tokens_ + col);
      if (r) {
        return r;
      }
    }
    else if ((se->code_->tokens_[col].match_ == TOK_SPECIAL_IDENT_STR) && !strcmp("$discard", se->code_->tokens_[col].text_.translated_)) {
      r = emit_discard(ip, cc, se, se->code_->tokens_ + col);
      if (r) {
        return r;
      }
    }
    else if ((se->code_->tokens_[col].match_ == TOK_SPECIAL_IDENT_STR) && !strcmp("$text", se->code_->tokens_[col].text_.translated_)) {
      r = emit_text(ip, cc, se, se->code_->tokens_ + col);
      if (r) {
        return r;
      }
    }
    else if ((se->code_->tokens_[col].match_ == TOK_SPECIAL_IDENT_STR) && !strcmp("$line", se->code_->tokens_[col].text_.translated_)) {
      r = emit_line(ip, cc, se, se->code_->tokens_ + col);
      if (r) {
        return r;
      }
    }
    else if ((se->code_->tokens_[col].match_ == TOK_SPECIAL_IDENT_STR) && !strcmp("$column", se->code_->tokens_[col].text_.translated_)) {
      r = emit_column(ip, cc, se, se->code_->tokens_ + col);
      if (r) {
        return r;
      }
    }
    else if ((se->code_->tokens_[col].match_ == TOK_SPECIAL_IDENT_STR) && !strcmp("$offset", se->code_->tokens_[col].text_.translated_)) {
      r = emit_offset(ip, cc, se, se->code_->tokens_ + col);
      if (r) {
        return r;
      }
    }
    else if ((se->code_->tokens_[col].match_ == TOK_SPECIAL_IDENT_STR) && !strcmp("$endline", se->code_->tokens_[col].text_.translated_)) {
      r = emit_endline(ip, cc, se, se->code_->tokens_ + col);
      if (r) {
        return r;
      }
    }
    else if ((se->code_->tokens_[col].match_ == TOK_SPECIAL_IDENT_STR) && !strcmp("$endcolumn", se->code_->tokens_[col].text_.translated_)) {
      r = emit_endcolumn(ip, cc, se, se->code_->tokens_ + col);
      if (r) {
        return r;
      }
    }
    else if ((se->code_->tokens_[col].match_ == TOK_SPECIAL_IDENT_STR) && !strcmp("$endoffset", se->code_->tokens_[col].text_.translated_)) {
      r = emit_endoffset(ip, cc, se, se->code_->tokens_ + col);
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
      r = emit_sym_specific_data(ip, cc, se, se->code_->tokens_ + col, special_index);
      if (r) {
        return r;
      }
    }
    else {
      ip_printf_no_indent(ip, "%s", se->code_->tokens_[col].text_.original_);
    }
  }
  ip_printf_no_indent(ip, "\n");
  ip_printf(ip, "}\n");
  if (cc->continuation_enabled_) {
    if (!retry_on_exit) {
      ip_printf(ip, "C%d:;\n", cc->current_snippet_continuation_++);
    }
  }
  return 0;
}

static void emit_snippet_continuation_jump(struct indented_printer *ip, struct carburetta_context *cc) {
  ip_printf(ip, "switch (stack->continue_at_) {\n");
  ip_printf(ip, "case 0: break; /* fall through to regular code path */\n");
  int n;
  for (n = 1; n < cc->current_snippet_continuation_; ++n) {
    ip_printf(ip, "case %d: goto C%d;\n", n, n);
  }
  ip_printf(ip, "} /* continuation switch */\n");
}

static int emit_common_action_snippet(struct indented_printer *ip, struct carburetta_context *cc, struct prd_production *prd) {
  struct snippet_emission se = { 0 };
  if (!prd) return 0;
  se.code_ = &prd->common_action_sequence_;
  se.dest_type_ = SEDT_NONE;
  se.sym_type_ = SEST_NONE;
  se.common_type_ = SECT_FMT;
  se.common_fmt_prefix_ = "(stack->sym_data_[";
  se.common_fmt_suffix_ = "].common_)";
  se.common_dest_type_ = SECDT_FMT;
  se.common_dest_fmt_ = "(stack->stack_[1].common_)";
  se.len_type_ = SELT_FMT;
  se.len_fmt_ = "(stack->current_production_length_)";
  se.discard_type_ = SEDIT_FMT;
  se.discard_fmt_ = "stack->discard_remaining_actions_ = 1;";
  se.text_type_ = SETT_NONE;
  se.line_type_ = SELIT_NONE;
  se.col_type_ = SECOT_NONE;
  se.offset_type_ = SEOT_NONE;
  se.end_line_type_ = SEELIT_NONE;
  se.end_col_type_ = SEECOT_NONE;
  se.end_offset_type_ = SEEOT_NONE;
  return emit_snippet_code_emission(ip, cc, &se, 0);
}

static int emit_action_snippet(struct indented_printer *ip, struct carburetta_context *cc, struct prd_production *prd) {
  struct snippet_emission se = { 0 };
  if (!prd) return 0;
  se.code_ = &prd->action_sequence_;
  se.dest_type_ = SEDT_FMT_DATATYPE_ORDINAL;
  se.dest_fmt_ = "(stack->stack_[1].v_.uv%d_)";
  se.dest_ = &prd->nt_;
  se.sym_type_ = SEST_FMT_INDEX_ORDINAL;
  se.sym_fmt_ = "(stack->sym_data_[%zu].v_.uv%d_)";
  se.prod_ = prd;
  se.common_type_ = SECT_FMT;
  se.common_fmt_prefix_ = "(stack->sym_data_[";
  se.common_fmt_suffix_ = "].common_)";
  se.common_dest_type_ = SECDT_FMT;
  se.common_dest_fmt_ = "(stack->stack_[1].common_)";
  se.len_type_ = SELT_FMT;
  se.len_fmt_ = "(current_production_length_)";
  se.discard_type_ = SEDIT_NONE;
  se.text_type_ = SETT_NONE;
  se.line_type_ = SELIT_NONE;
  se.col_type_ = SECOT_NONE;
  se.offset_type_ = SEOT_NONE;
  se.end_line_type_ = SEELIT_NONE;
  se.end_col_type_ = SEECOT_NONE;
  se.end_offset_type_ = SEEOT_NONE;
  return emit_snippet_code_emission(ip, cc, &se, 0);
}

static int emit_token_action_snippet(struct indented_printer *ip, struct carburetta_context *cc, struct typestr *ts) {
  struct snippet_emission se = { 0 };
  if (!ts) return 0;
  se.code_ = &ts->token_action_snippet_;
  se.dest_type_ = SEDT_FMT_TYPESTR_ORDINAL;
  se.dest_typestr_ = ts;
  se.dest_fmt_ = "(stack->sym_data_->v_.uv%d_)";
  se.sym_type_ = SEST_NONE;
  se.common_type_ = SECT_NONE;
  se.common_dest_type_ = SECDT_FMT;
  se.common_dest_fmt_ = "(stack->sym_data_->common_)";
  se.len_type_ = SELT_NONE;
  se.discard_type_ = SEDIT_NONE;
  se.text_type_ = SETT_NONE;
  se.line_type_ = SELIT_NONE;
  se.col_type_ = SECOT_NONE;
  se.offset_type_ = SEOT_NONE;
  se.end_line_type_ = SEELIT_NONE;
  se.end_col_type_ = SEECOT_NONE;
  se.end_offset_type_ = SEEOT_NONE;
  return emit_snippet_code_emission(ip, cc, &se, 0);
}

static int emit_constructor_snippet(struct indented_printer *ip, struct carburetta_context *cc, struct typestr *ts, int retry_on_exit) {
  struct snippet_emission se = { 0 };
  if (!ts) return 0;
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
  se.line_type_ = SELIT_NONE;
  se.col_type_ = SECOT_NONE;
  se.offset_type_ = SEOT_NONE;
  se.end_line_type_ = SEELIT_NONE;
  se.end_col_type_ = SEECOT_NONE;
  se.end_offset_type_ = SEEOT_NONE;
  return emit_snippet_code_emission(ip, cc, &se, retry_on_exit);
}

static int emit_dst_sym_constructor_snippet(struct indented_printer *ip, struct carburetta_context *cc, struct typestr *ts, int retry_on_exit) {
  struct snippet_emission se = { 0 };
  if (!ts) return 0;
  se.code_ = &ts->constructor_snippet_;
  se.dest_type_ = SEDT_FMT_TYPESTR_ORDINAL;
  se.dest_typestr_ = ts;
  se.dest_fmt_ = "(stack->stack_[1].v_.uv%d_)";
  se.sym_type_ = SEST_NONE;
  se.common_type_ = SECT_NONE;
  se.common_dest_type_ = SECDT_FMT;
  se.common_dest_fmt_ = "(stack->stack_[1].common_)";
  se.len_type_ = SELT_NONE;
  se.discard_type_ = SEDIT_NONE;
  se.text_type_ = SETT_NONE;
  se.line_type_ = SELIT_NONE;
  se.col_type_ = SECOT_NONE;
  se.offset_type_ = SEOT_NONE;
  se.end_line_type_ = SEELIT_NONE;
  se.end_col_type_ = SEECOT_NONE;
  se.end_offset_type_ = SEEOT_NONE;
  return emit_snippet_code_emission(ip, cc, &se, retry_on_exit);
}

static int emit_common_constructor_snippet(struct indented_printer *ip, struct carburetta_context *cc, int retry_on_exit) {
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
  se.line_type_ = SELIT_NONE;
  se.col_type_ = SECOT_NONE;
  se.offset_type_ = SEOT_NONE;
  se.end_line_type_ = SEELIT_NONE;
  se.end_col_type_ = SEECOT_NONE;
  se.end_offset_type_ = SEEOT_NONE;
  return emit_snippet_code_emission(ip, cc, &se, retry_on_exit);
}

static int emit_dst_common_constructor_snippet(struct indented_printer *ip, struct carburetta_context *cc, int retry_on_exit) {
  struct snippet_emission se = { 0 };
  if (!cc->common_data_assigned_type_) return 0;
  se.code_ = &cc->common_data_assigned_type_->constructor_snippet_;
  se.dest_type_ = SEDT_FMT;
  se.dest_fmt_ = "(stack->stack_[1].common_)";
  se.sym_type_ = SEST_NONE;
  se.common_type_ = SECT_NONE;
  se.common_dest_type_ = SECDT_FMT;
  se.common_dest_fmt_ = "(stack->stack_[1].common_)";
  se.len_type_ = SELT_NONE;
  se.discard_type_ = SEDIT_NONE;
  se.text_type_ = SETT_NONE;
  se.line_type_ = SELIT_NONE;
  se.col_type_ = SECOT_NONE;
  se.offset_type_ = SEOT_NONE;
  se.end_line_type_ = SEELIT_NONE;
  se.end_col_type_ = SEECOT_NONE;
  se.end_offset_type_ = SEEOT_NONE;
  return emit_snippet_code_emission(ip, cc, &se, retry_on_exit);
}

static int emit_token_common_constructor_snippet(struct indented_printer *ip, struct carburetta_context *cc, int retry_on_exit) {
  struct snippet_emission se = { 0 };
  if (!cc->common_data_assigned_type_) return 0;
  se.code_ = &cc->common_data_assigned_type_->constructor_snippet_;
  se.dest_type_ = SEDT_FMT;
  se.dest_fmt_ = "(stack->sym_data_->common_)";
  se.sym_type_ = SEST_NONE;
  se.common_type_ = SECT_NONE;
  se.common_dest_type_ = SECDT_FMT;
  se.common_dest_fmt_ = "(stack->sym_data_->common_)";
  se.len_type_ = SELT_NONE;
  se.discard_type_ = SEDIT_NONE;
  se.text_type_ = SETT_NONE;
  se.line_type_ = SELIT_NONE;
  se.col_type_ = SECOT_NONE;
  se.offset_type_ = SEOT_NONE;
  se.end_line_type_ = SEELIT_NONE;
  se.end_col_type_ = SEECOT_NONE;
  se.end_offset_type_ = SEEOT_NONE;
  return emit_snippet_code_emission(ip, cc, &se, retry_on_exit);
}

static int emit_pattern_token_common_constructor_snippet(struct indented_printer *ip, struct carburetta_context *cc, int retry_on_exit) {
  struct snippet_emission se = { 0 };
  if (!cc->common_data_assigned_type_) return 0;
  se.code_ = &cc->common_data_assigned_type_->constructor_snippet_;
  se.dest_type_ = SEDT_FMT;
  se.dest_fmt_ = "(stack->stack_[0].common_)";
  se.sym_type_ = SEST_NONE;
  se.common_type_ = SECT_NONE;
  se.common_dest_type_ = SECDT_FMT;
  se.common_dest_fmt_ = "(stack->stack_[0].common_)";
  se.len_type_ = SELT_FMT;
  se.len_fmt_ = "(stack->token_size_)";
  se.discard_type_ = SEDIT_NONE;
  se.text_type_ = SETT_FMT;
  se.text_fmt_ = "(stack->match_buffer_)";
  se.line_type_ = SELIT_FMT;
  se.line_fmt_ = "(stack->match_line_)";
  se.col_type_ = SECOT_FMT;
  se.col_fmt_ = "(stack->match_col_)";
  se.offset_type_ = SEOT_FMT;
  se.offset_fmt_ = "(stack->match_offset_)";
  se.end_line_type_ = SEELIT_FMT;
  se.end_line_fmt_ = "(stack->best_match_line_)";
  se.end_col_type_ = SEECOT_FMT;
  se.end_col_fmt_ = "(stack->best_match_col_)";
  se.end_offset_type_ = SEEOT_FMT;
  se.end_offset_fmt_ = "(stack->best_match_offset_)";
  return emit_snippet_code_emission(ip, cc, &se, retry_on_exit);
}

static int emit_token_common_action_snippet(struct indented_printer *ip, struct carburetta_context *cc) {
  struct snippet_emission se = { 0 };
  if (!cc->common_data_assigned_type_) return 0;
  se.code_ = &cc->common_data_assigned_type_->token_action_snippet_;
  se.dest_type_ = SEDT_FMT;
  se.dest_fmt_ = "(stack->sym_data_->common_)";
  se.sym_type_ = SEST_NONE;
  se.common_type_ = SECT_NONE;
  se.common_dest_type_ = SECDT_FMT;
  se.common_dest_fmt_ = "(stack->sym_data_->common_)";
  se.len_type_ = SELT_NONE;
  se.discard_type_ = SEDIT_NONE;
  se.text_type_ = SETT_NONE;
  se.line_type_ = SELIT_NONE;
  se.col_type_ = SECOT_NONE;
  se.offset_type_ = SEOT_NONE;
  se.end_line_type_ = SEELIT_NONE;
  se.end_col_type_ = SEECOT_NONE;
  se.end_offset_type_ = SEEOT_NONE;
  return emit_snippet_code_emission(ip, cc, &se, 0);
}

static int emit_token_constructor_snippet(struct indented_printer *ip, struct carburetta_context *cc, struct typestr *ts, int retry_on_exit) {
  struct snippet_emission se = { 0 };
  if (!ts) return 0;
  se.code_ = &ts->constructor_snippet_;
  se.dest_type_ = SEDT_FMT_TYPESTR_ORDINAL;
  se.dest_typestr_ = ts;
  se.dest_fmt_ = "(stack->sym_data_->v_.uv%d_)";
  se.sym_type_ = SEST_NONE;
  se.common_type_ = SECT_NONE;
  se.common_dest_type_ = SECDT_FMT;
  se.common_dest_fmt_ = "(stack->sym_data_->common_)";
  se.len_type_ = SELT_NONE;
  se.discard_type_ = SEDIT_NONE;
  se.text_type_ = SETT_NONE;
  se.line_type_ = SELIT_NONE;
  se.col_type_ = SECOT_NONE;
  se.offset_type_ = SEOT_NONE;
  se.end_line_type_ = SEELIT_NONE;
  se.end_col_type_ = SEECOT_NONE;
  se.end_offset_type_ = SEEOT_NONE;
  return emit_snippet_code_emission(ip, cc, &se, retry_on_exit);
}

static int emit_pattern_token_constructor_snippet(struct indented_printer *ip, struct carburetta_context *cc, struct typestr *ts, int retry_on_exit) {
  struct snippet_emission se = { 0 };
  if (!ts) return 0;
  se.code_ = &ts->constructor_snippet_;
  se.dest_type_ = SEDT_FMT_TYPESTR_ORDINAL;
  se.dest_typestr_ = ts;
  se.dest_fmt_ = "(stack->stack_[0].v_.uv%d_)";
  se.sym_type_ = SEST_NONE;
  se.common_type_ = SECT_NONE;
  se.common_dest_type_ = SECDT_FMT;
  se.common_dest_fmt_ = "(stack->stack_[0].common_)";
  se.len_type_ = SELT_FMT;
  se.len_fmt_ = "(stack->token_size_)";
  se.discard_type_ = SEDIT_NONE;
  se.text_type_ = SETT_FMT;
  se.text_fmt_ = "(stack->match_buffer_)";
  se.line_type_ = SELIT_FMT;
  se.line_fmt_ = "(stack->match_line_)";
  se.col_type_ = SECOT_FMT;
  se.col_fmt_ = "(stack->match_col_)";
  se.offset_type_ = SEOT_FMT;
  se.offset_fmt_ = "(stack->match_offset_)";
  se.end_line_type_ = SEELIT_FMT;
  se.end_line_fmt_ = "(stack->best_match_line_)";
  se.end_col_type_ = SEECOT_FMT;
  se.end_col_fmt_ = "(stack->best_match_col_)";
  se.end_offset_type_ = SEEOT_FMT;
  se.end_offset_fmt_ = "(stack->best_match_offset_)";
  return emit_snippet_code_emission(ip, cc, &se, retry_on_exit);
}

static int emit_destructor_snippet(struct indented_printer *ip, struct carburetta_context *cc, struct typestr *ts) {
  struct snippet_emission se = { 0 };
  if (!ts) return 0;
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
  se.line_type_ = SELIT_NONE;
  se.col_type_ = SECOT_NONE;
  se.offset_type_ = SEOT_NONE;
  se.end_line_type_ = SEELIT_NONE;
  se.end_col_type_ = SEECOT_NONE;
  se.end_offset_type_ = SEEOT_NONE;
  return emit_snippet_code_emission(ip, cc, &se, 0);
}

static int emit_destructor_snippet_indexed_by_n(struct indented_printer *ip, struct carburetta_context *cc, struct typestr *ts) {
  struct snippet_emission se = { 0 };
  if (!ts) return 0;
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
  se.line_type_ = SELIT_NONE;
  se.col_type_ = SECOT_NONE;
  se.offset_type_ = SEOT_NONE;
  se.end_line_type_ = SEELIT_NONE;
  se.end_col_type_ = SEECOT_NONE;
  se.end_offset_type_ = SEEOT_NONE;
  return emit_snippet_code_emission(ip, cc, &se, 0);
}

static int emit_destructor_snippet_indexed_by_0(struct indented_printer *ip, struct carburetta_context *cc, struct typestr *ts) {
  struct snippet_emission se = { 0 };
  if (!ts) return 0;
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
  se.line_type_ = SELIT_NONE;
  se.col_type_ = SECOT_NONE;
  se.offset_type_ = SEOT_NONE;
  se.end_line_type_ = SEELIT_NONE;
  se.end_col_type_ = SEECOT_NONE;
  se.end_offset_type_ = SEEOT_NONE;
  return emit_snippet_code_emission(ip, cc, &se, 0);
}

static int emit_destructor_snippet_indexed_by_1(struct indented_printer *ip, struct carburetta_context *cc, struct typestr *ts) {
  struct snippet_emission se = { 0 };
  if (!ts) return 0;
  se.code_ = &ts->destructor_snippet_;
  se.dest_type_ = SEDT_FMT_TYPESTR_ORDINAL;
  se.dest_typestr_ = ts;
  se.dest_fmt_ = "(stack->stack_[1].v_.uv%d_)";
  se.sym_type_ = SEST_NONE;
  se.common_type_ = SECT_NONE;
  se.common_dest_type_ = SECDT_FMT;
  se.common_dest_fmt_ = "(stack->stack_[1].common_)";
  se.len_type_ = SELT_NONE;
  se.discard_type_ = SEDIT_NONE;
  se.text_type_ = SETT_NONE;
  se.line_type_ = SELIT_NONE;
  se.col_type_ = SECOT_NONE;
  se.offset_type_ = SEOT_NONE;
  se.end_line_type_ = SEELIT_NONE;
  se.end_col_type_ = SEECOT_NONE;
  se.end_offset_type_ = SEEOT_NONE;
  return emit_snippet_code_emission(ip, cc, &se, 0);
}

static int emit_common_destructor_snippet(struct indented_printer *ip, struct carburetta_context *cc) {
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
  se.line_type_ = SELIT_NONE;
  se.col_type_ = SECOT_NONE;
  se.offset_type_ = SEOT_NONE;
  se.end_line_type_ = SEELIT_NONE;
  se.end_col_type_ = SEECOT_NONE;
  se.end_offset_type_ = SEEOT_NONE;
  return emit_snippet_code_emission(ip, cc, &se, 0);
}

static int emit_pattern_token_common_destructor_snippet(struct indented_printer *ip, struct carburetta_context *cc) {
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
  se.line_type_ = SELIT_NONE;
  se.col_type_ = SECOT_NONE;
  se.offset_type_ = SEOT_NONE;
  se.end_line_type_ = SEELIT_NONE;
  se.end_col_type_ = SEECOT_NONE;
  se.end_offset_type_ = SEEOT_NONE;
  return emit_snippet_code_emission(ip, cc, &se, 0);
}

static int emit_common_destructor_snippet_indexed_by_n(struct indented_printer *ip, struct carburetta_context *cc) {
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
  se.line_type_ = SELIT_NONE;
  se.col_type_ = SECOT_NONE;
  se.offset_type_ = SEOT_NONE;
  se.end_line_type_ = SEELIT_NONE;
  se.end_col_type_ = SEECOT_NONE;
  se.end_offset_type_ = SEEOT_NONE;
  return emit_snippet_code_emission(ip, cc, &se, 0);
}

static int emit_common_destructor_snippet_index_0(struct indented_printer *ip, struct carburetta_context *cc) {
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
  se.line_type_ = SELIT_NONE;
  se.col_type_ = SECOT_NONE;
  se.offset_type_ = SEOT_NONE;
  se.end_line_type_ = SEELIT_NONE;
  se.end_col_type_ = SEECOT_NONE;
  se.end_offset_type_ = SEEOT_NONE;
  return emit_snippet_code_emission(ip, cc, &se, 0);
}

static int emit_common_destructor_snippet_index_1(struct indented_printer *ip, struct carburetta_context *cc) {
  struct snippet_emission se = { 0 };
  if (!cc->common_data_assigned_type_) return 0;
  se.code_ = &cc->common_data_assigned_type_->destructor_snippet_;
  se.dest_type_ = SEDT_FMT;
  se.dest_fmt_ = "(stack->stack_[1].common_)";
  se.sym_type_ = SEST_NONE;
  se.common_type_ = SECT_NONE;
  se.common_dest_type_ = SECDT_FMT;
  se.common_dest_fmt_ = "(stack->stack_[1].common_)";
  se.len_type_ = SELT_NONE;
  se.discard_type_ = SEDIT_NONE;
  se.text_type_ = SETT_NONE;
  se.line_type_ = SELIT_NONE;
  se.col_type_ = SECOT_NONE;
  se.offset_type_ = SEOT_NONE;
  se.end_line_type_ = SEELIT_NONE;
  se.end_col_type_ = SEECOT_NONE;
  se.end_offset_type_ = SEEOT_NONE;
  return emit_snippet_code_emission(ip, cc, &se, 0);
}

static int emit_pattern_common_action_snippet(struct indented_printer *ip, struct carburetta_context *cc, struct prd_pattern *pat) {
  struct snippet_emission se = { 0 };
  if (!pat) return 0;
  se.code_ = &pat->common_action_sequence_;
  se.dest_type_ = SEDT_FMT;
  se.dest_fmt_ = "(stack->stack_[0].common_)";
  se.sym_type_ = SEST_NONE;
  se.prod_ = NULL;
  se.common_type_ = SECT_NONE;
  se.common_dest_type_ = SECDT_FMT;
  se.common_dest_fmt_ = "(stack->stack_[0].common_)";
  se.len_type_ = SELT_FMT;
  se.len_fmt_ = "(stack->token_size_)";
  se.discard_type_ = SEDIT_FMT;
  se.discard_fmt_ = "stack->discard_remaining_actions_ = 1;";
  se.text_type_ = SETT_FMT;
  se.text_fmt_ = "(stack->match_buffer_)";
  se.line_type_ = SELIT_FMT;
  se.line_fmt_ = "(stack->match_line_)";
  se.col_type_ = SECOT_FMT;
  se.col_fmt_ = "(stack->match_col_)";
  se.offset_type_ = SEOT_FMT;
  se.offset_fmt_ = "(stack->match_offset_)";
  se.end_line_type_ = SEELIT_FMT;
  se.end_line_fmt_ = "(stack->best_match_line_)";
  se.end_col_type_ = SEECOT_FMT;
  se.end_col_fmt_ = "(stack->best_match_col_)";
  se.end_offset_type_ = SEEOT_FMT;
  se.end_offset_fmt_ = "(stack->best_match_offset_)";
  return emit_snippet_code_emission(ip, cc, &se, 0);
}

static int emit_pattern_action_snippet(struct indented_printer *ip, struct carburetta_context *cc, struct prd_pattern *pat) {
  struct snippet_emission se = { 0 };
  if (!pat) return 0;
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
  se.discard_type_ = SEDIT_FMT;
  se.discard_fmt_ = "stack->discard_remaining_actions_ = 1;";
  se.text_type_ = SETT_FMT;
  se.text_fmt_ = "(stack->match_buffer_)";
  se.line_type_ = SELIT_FMT;
  se.line_fmt_ = "(stack->match_line_)";
  se.col_type_ = SECOT_FMT;
  se.col_fmt_ = "(stack->match_col_)";
  se.offset_type_ = SEOT_FMT;
  se.offset_fmt_ = "(stack->match_offset_)";
  se.end_line_type_ = SEELIT_FMT;
  se.end_line_fmt_ = "(stack->best_match_line_)";
  se.end_col_type_ = SEECOT_FMT;
  se.end_col_fmt_ = "(stack->best_match_col_)";
  se.end_offset_type_ = SEEOT_FMT;
  se.end_offset_fmt_ = "(stack->best_match_offset_)";
  return emit_snippet_code_emission(ip, cc, &se, 0);
}

static void emit_lex_function(struct indented_printer *ip, struct carburetta_context *cc, struct prd_grammar *prdg) {
  /* Emit the scan function, it scans the input for regex matches without actually executing any actions */
  /* (we're obviously in need of a templating language..) */
  ip_printf(ip, "static int %sappend_match_buffer(struct %sstack *stack, const char *s, size_t len) {\n", cc_prefix(cc), cc_prefix(cc));
  ip_printf(ip, "  size_t size_needed = len;\n"
                "  size_needed += stack->match_buffer_size_;\n"
                "  if (size_needed < stack->match_buffer_size_) {\n");
  ip_printf(ip, "    return _%sOVERFLOW;\n", cc_PREFIX(cc));
  ip_printf(ip, "  }\n"
                "  if (size_needed == SIZE_MAX) {\n"
                "    /* cannot fit null terminator */\n");
  ip_printf(ip, "    return _%sOVERFLOW;\n", cc_PREFIX(cc));
  ip_printf(ip, "  }\n"
                "  size_needed++; /* null terminator */\n"
                "  if (size_needed < 128) {\n"
                "    size_needed = 128;\n"
                "  }\n"
                "  if (size_needed > stack->match_buffer_size_allocated_) {\n"
                "    /* intent of code: grow buffer size by powers of 2-1, unless our needs require more now. */\n"
                "    size_t size_to_allocate = stack->match_buffer_size_allocated_ * 2 + 1;\n"
                "    if (size_to_allocate <= stack->match_buffer_size_allocated_) {\n");
  ip_printf(ip, "      return _%sOVERFLOW;\n", cc_PREFIX(cc));
  ip_printf(ip,"    }\n"
                "    if (size_to_allocate < size_needed) {\n"
                "      size_to_allocate = size_needed;\n"
                "    }\n"
                "    void *buf = realloc(stack->match_buffer_, size_to_allocate);\n"
                "    if (!buf) {\n");
  ip_printf(ip, "      return _%sNO_MEMORY;\n", cc_PREFIX(cc));
  ip_printf(ip, "    }\n"
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
  ip_printf(ip, "void %sset_input(struct %sstack *stack, const char *input, size_t input_size, int is_final_input) {\n", cc_prefix(cc), cc_prefix(cc));
  ip_printf(ip, "  stack->input_ = input;\n"
                "  stack->input_size_ = input_size;\n"
                "  stack->is_final_input_ = is_final_input;\n"
                "  stack->input_index_ = 0;\n");
  ip_printf(ip, "}\n"
                "\n");

  ip_printf(ip,  "void %sset_location(struct %sstack *stack, int line, int col, size_t offset) {\n", cc_prefix(cc), cc_prefix(cc));
  ip_printf(ip,  "  if (stack->token_size_) {\n");
  ip_printf(ip,  "    /* Parsing of next token not in progress, set end location of this token as\n");
  ip_printf(ip,  "    ** it will be the start of the next token. */\n");
  ip_printf(ip,  "    stack->input_line_ = stack->input_line_ - stack->best_match_line_ + line;\n");
  ip_printf(ip,  "    stack->input_col_ = stack->input_col_ - stack->best_match_col_ + col;\n");
  ip_printf(ip,  "    stack->input_offset_ = stack->input_offset_ - stack->best_match_offset_ + offset;\n");
  ip_printf(ip,  "\n");
  ip_printf(ip,  "    stack->best_match_line_ = line;\n");
  ip_printf(ip,  "    stack->best_match_col_ = col;\n");
  ip_printf(ip,  "    stack->best_match_offset_ = offset;\n");
  ip_printf(ip,  "    return;\n");
  ip_printf(ip,  "  }\n");
  ip_printf(ip,  "  /* Parsing of token in progress, dynamically move the start of the token, as\n");
  ip_printf(ip,  "  ** well as the relative current partial end of the token, to the desired location. */\n");
  ip_printf(ip,  "  stack->input_line_ = stack->input_line_ - stack->match_line_ + line;\n");
  ip_printf(ip,  "  stack->input_col_ = stack->input_col_ - stack->match_col_ + col;\n");
  ip_printf(ip,  "  stack->input_offset_ = stack->input_offset_ - stack->match_offset_ + offset;\n");
  ip_printf(ip,  "\n");
  ip_printf(ip,  "  stack->best_match_line_ = stack->best_match_line_ - stack->match_line_ + line;\n");
  ip_printf(ip,  "  stack->best_match_col_ = stack->best_match_col_ - stack->match_col_ + col;\n");
  ip_printf(ip,  "  stack->best_match_offset_ = stack->best_match_offset_ - stack->match_offset_ + offset;\n");
  ip_printf(ip,  "  stack->match_line_ = line;\n");
  ip_printf(ip,  "  stack->match_col_ = col;\n");
  ip_printf(ip,  "  stack->match_offset_ = offset;\n");
  ip_printf(ip, "}\n"
                "\n");

  ip_printf(ip, "const char *%stext(struct %sstack *stack) {\n"
                "  return stack->match_buffer_;\n"
                "}\n"
                "\n", cc_prefix(cc), cc_prefix(cc));

  ip_printf(ip, "size_t %slen(struct %sstack *stack) {\n"
                "  return stack->token_size_;\n"
                "}\n"
                "\n", cc_prefix(cc), cc_prefix(cc));

  ip_printf(ip, "int %sline(struct %sstack *stack) {\n"
                "  return stack->match_line_;\n"
                "}\n"
                "\n", cc_prefix(cc), cc_prefix(cc));

  ip_printf(ip, "int %scolumn(struct %sstack *stack) {\n"
                "  return stack->match_col_;\n"
                "}\n"
                "\n", cc_prefix(cc), cc_prefix(cc));

  ip_printf(ip, "size_t %soffset(struct %sstack *stack) {\n"
                "  return stack->match_offset_;\n"
                "}\n"
                "\n", cc_prefix(cc), cc_prefix(cc));

  ip_printf(ip, "int %sendline(struct %sstack *stack) {\n"
                "  return stack->best_match_line_;\n"
                "}\n"
                "\n", cc_prefix(cc), cc_prefix(cc));

  ip_printf(ip, "int %sendcolumn(struct %sstack *stack) {\n"
                "  return stack->best_match_col_;\n"
                "}\n"
                "\n", cc_prefix(cc), cc_prefix(cc));

  ip_printf(ip, "size_t %sendoffset(struct %sstack *stack) {\n"
                "  return stack->best_match_offset_;\n"
                "}\n"
                "\n", cc_prefix(cc), cc_prefix(cc));

  ip_printf(ip,  "int %slex(struct %sstack *stack) {\n", cc_prefix(cc), cc_prefix(cc));
  ip_printf(ip,  "  int r;\n"
                 "  unsigned char c;\n"
                 "  const char *input = stack->input_;\n"
                 "  size_t input_size = stack->input_size_;\n"
                 "  int is_final_input = !!stack->is_final_input_;\n"
                 "  size_t scan_state = stack->scan_state_;\n");
#if 0
  ip_printf(ip,  "  const size_t *transition_table = %sscan_table;\n", cc_prefix(cc));
  ip_printf(ip,  "  const size_t *actions = %sscan_actions;\n", cc_prefix(cc));
  ip_printf(ip,  "  const size_t row_size = 256;\n");
#else
  ip_printf(ip,  "  const size_t *transition_table = %sscan_table_rex;\n", cc_prefix(cc));
  ip_printf(ip,  "  const size_t *actions = %sscan_actions_rex;\n", cc_prefix(cc));
  ip_printf(ip,  "  const size_t row_size = 260;\n");
#endif
  ip_printf(ip,  "  const size_t default_action = %zu;\n", 0);
  ip_printf(ip,  "  const size_t start_state = %zu;\n", 1);
  ip_printf(ip,  "  const size_t start_action = 0;\n", cc_prefix(cc));
  ip_printf(ip,  "\n"
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
                 "    stack->scan_state_ = scan_state = stack->current_mode_start_state_;\n"
                 "    stack->token_size_ = 0;\n"
                 "    \n"
                 "  }\n"
                 "\n"
                 "  size_t at_match_index_offset = stack->match_offset_;\n"
                 "  int at_match_index_line = stack->match_line_;\n"
                 "  int at_match_index_col = stack->match_col_;\n"
                 "  while (match_index < stack->match_buffer_size_) {\n"
                 "    c = (unsigned char)stack->match_buffer_[match_index];\n"
                 "    for (;;) {\n"
                 "      /* Check for start of input */\n"
                 /* 256 + REX_ANCHOR_START_OF_INPUT */
                 "      if ((transition_table[row_size * scan_state + 256] != scan_state) && (!at_match_index_offset)) {\n"
                 "        scan_state = transition_table[row_size * scan_state + 256];\n"
                 "      }\n"
                 "      /* Check for start of line */\n"
                 /* 256 + REX_ANCHOR_START_OF_LINE */
                 "      else if ((transition_table[row_size * scan_state + 257] != scan_state) && (at_match_index_col == 1)) {\n"
                 "        scan_state = transition_table[row_size * scan_state + 257];\n"
                 "      }\n"
                 "      /* Check for end of line */\n"
                 /* 256 + REX_ANCHOR_END_OF_LINE */
                 "      else if ((transition_table[row_size * scan_state + 258] != scan_state) && ('\\n' == c)) {\n"
                 "        scan_state = transition_table[row_size * scan_state + 258];\n"
                 "      }\n"
                 "      /* (No need to check for end of input; we have at least 1 character ahead) */\n"
                 "      else {\n"
                 "        break;\n"
                 "      }\n"
                 "    }\n"
                 "    size_t state_action = actions[scan_state];\n"
                 "    if (state_action != default_action) /* replace with actual */ {\n"
                 "      best_match_action = state_action;\n"
                 "      best_match_size = match_index;\n"
                 "      best_match_offset = at_match_index_offset;\n"
                 "      best_match_line = at_match_index_line;\n"
                 "      best_match_col = at_match_index_col;\n"
                 "    }\n"
                 "    scan_state = transition_table[row_size * scan_state + c];\n"
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
  ip_printf(ip,  "      return _%sMATCH;\n", cc_PREFIX(cc));
  ip_printf(ip,  "    }\n"
                 "  }\n"
                 "\n"
                 "  while (input_index < input_size) {\n"
                 "    c = (unsigned char)input[input_index];\n"
                 "    for (;;) {\n"
                 "      /* Check for start of input */\n"
                 /* 256 + REX_ANCHOR_START_OF_INPUT */
                 "      if ((transition_table[row_size * scan_state + 256] != scan_state) && (!input_offset)) {\n"
                 "        scan_state = transition_table[row_size * scan_state + 256];\n"
                 "      }\n"
                 "      /* Check for start of line */\n"
                 /* 256 + REX_ANCHOR_START_OF_LINE */
                 "      else if ((transition_table[row_size * scan_state + 257] != scan_state) && (input_col == 1)) {\n"
                 "        scan_state = transition_table[row_size * scan_state + 257];\n"
                 "      }\n"
                 "      /* Check for end of line */\n"
                 /* 256 + REX_ANCHOR_END_OF_LINE */
                 "      else if ((transition_table[row_size * scan_state + 258] != scan_state) && ('\\n' == c)) {\n"
                 "        scan_state = transition_table[row_size * scan_state + 258];\n"
                 "      }\n"
                 "      /* (No need to check for end of input; we have at least 1 character ahead) */\n"
                 "      else {\n"
                 "        break;\n"
                 "      }\n"
                 "    }\n"
                 "    size_t state_action = actions[scan_state];\n"
                 "    if (state_action != default_action) /* replace with actual */ {\n"
                 "      best_match_action = state_action;\n"
                 "      best_match_size = stack->match_buffer_size_ + input_index - stack->input_index_;\n"
                 "      best_match_offset = input_offset;\n"
                 "      best_match_col = input_col;\n"
                 "      best_match_line = input_line;\n"
                 "    }\n"
                 "    scan_state = transition_table[row_size * scan_state + c];\n"
                 "    if (scan_state) {\n"
                 "      input_offset++;\n"
                 "      if (c != '\\n') {\n"
                 "        input_col++;\n"
                 "      }\n"
                 "      else {\n"
                 "        input_col = 1;\n"
                 "        input_line++;\n"
                 "      }\n"
                 "      input_index++;\n"
                 "    }\n"
                 "    else {\n"
                 "      /* Append from stack->input_index_ to input_index, excluding input_index itself */\n"
                 "      r = %sappend_match_buffer(stack, input + stack->input_index_, input_index - stack->input_index_);\n", cc_prefix(cc));
  ip_printf(ip,  "      if (r) return r;\n"
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
  ip_printf(ip,  "      return _%sMATCH;\n", cc_PREFIX(cc));
  ip_printf(ip,  "    }\n"
                 "  }\n"
                 "\n"
                 "  r = %sappend_match_buffer(stack, input + stack->input_index_, input_index - stack->input_index_);\n", cc_prefix(cc));
  ip_printf(ip,  "  if (r) return r;\n"
                 "\n"
                 "  if (!is_final_input) {\n"
                 "    /* Need more input */\n"
                 "    stack->scan_state_ = scan_state;\n"
                 "    stack->token_size_ = 0; /* no match yet */\n"
                 "    stack->input_index_ = input_index;\n"
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
  ip_printf(ip,  "    return _%sFEED_ME;\n", cc_PREFIX(cc));
  ip_printf(ip,  "  }\n"
                 "  for (;;) {\n"
                 "    /* Check for start of input */\n"
                 /* 256 + REX_ANCHOR_START_OF_INPUT */
                 "    if ((transition_table[row_size * scan_state + 256] != scan_state) && (!input_offset)) {\n"
                 "      scan_state = transition_table[row_size * scan_state + 256];\n"
                 "    }\n"
                 "    /* Check for start of line */\n"
                 /* 256 + REX_ANCHOR_START_OF_LINE */
                 "    else if ((transition_table[row_size * scan_state + 257] != scan_state) && (input_col == 1)) {\n"
                 "      scan_state = transition_table[row_size * scan_state + 257];\n"
                 "    }\n"
                 "    /* Check for end of line (always true at end of input) */\n"
                 /* 256 + REX_ANCHOR_END_OF_LINE */
                 "    else if (transition_table[row_size * scan_state + 258] != scan_state) {\n"
                 "      scan_state = transition_table[row_size * scan_state + 258];\n"
                 "    }\n"
                 "    /* Check for end of input (always true) */\n"
                 /* 256 + REX_ANCHOR_END_OF_INPUT */
                 "    else if (transition_table[row_size * scan_state + 259] != scan_state) {\n"
                 "      scan_state = transition_table[row_size * scan_state + 259];\n"
                 "    }\n"
                 "    /* (No need to check for end of input; we have at least 1 character ahead) */\n"
                 "    else {\n"
                 "      break;\n"
                 "    }\n"
                 "  }\n"
                 "  size_t state_action = actions[scan_state];\n"
                 "  if (state_action != default_action) /* replace with actual */ {\n"
                 "    best_match_action = state_action;\n"
                 "    best_match_size = stack->match_buffer_size_;\n"
                 "    best_match_offset = input_offset;\n"
                 "    best_match_col = input_col;\n"
                 "    best_match_line = input_line;\n"
                 "  }\n"
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
                 "    stack->scan_state_ = scan_state = stack->current_mode_start_state_;\n"
                 "\n"
                 "    stack->token_size_ = 0;\n"
                 "    stack->input_index_ = 0;\n"
                 "    stack->input_offset_ = input_offset;\n"
                 "    stack->input_line_ = input_line;\n"
                 "    stack->input_col_ = input_col;\n");
  ip_printf(ip,  "    return _%sEND_OF_INPUT;\n", cc_PREFIX(cc));
  ip_printf(ip,  "  }\n"
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
  ip_printf(ip,  "  return _%sMATCH;\n", cc_PREFIX(cc));
  ip_printf(ip,  "syntax_error:\n"
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
  ip_printf(ip,  "    if (r) return r;\n"
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
  ip_printf(ip,  "  return _%sLEXICAL_ERROR;\n", cc_PREFIX(cc));
  ip_printf(ip,  "}\n");
}

void emit_syntax_error(struct indented_printer *ip, struct carburetta_context *cc) {
  ip_printf(ip, "stack->continue_at_ = 0;\n");
  if (cc->on_syntax_error_snippet_.num_tokens_) {
    size_t token_idx;
    ip_printf(ip, "{\n");
    ip_force_indent_print(ip);
    for (token_idx = 0; token_idx < cc->on_syntax_error_snippet_.num_tokens_; ++token_idx) {
      ip_printf(ip, "%s", cc->on_syntax_error_snippet_.tokens_[token_idx].text_.original_);
    }
    ip_printf(ip, "\n}\n");
  }
  else {
    ip_printf(ip, "/* Syntax error */\n"
                  "return _%sSYNTAX_ERROR;\n", cc_PREFIX(cc));
  }
}

void emit_lexical_error(struct indented_printer *ip, struct carburetta_context *cc) {
  ip_printf(ip, "stack->continue_at_ = 0;\n");
  if (cc->on_lexical_error_snippet_.num_tokens_) {
    size_t token_idx;
    ip_printf(ip, "{\n");
    ip_force_indent_print(ip);
    for (token_idx = 0; token_idx < cc->on_lexical_error_snippet_.num_tokens_; ++token_idx) {
      ip_printf(ip, "%s", cc->on_lexical_error_snippet_.tokens_[token_idx].text_.original_);
    }
    ip_printf(ip, "\n}\n");
  }
  else {
    ip_printf(ip, "/* Lexical error */\n"
                  "return _%sLEXICAL_ERROR;\n", cc_PREFIX(cc));
  }
}

void emit_overflow_error(struct indented_printer *ip, struct carburetta_context *cc) {
  ip_printf(ip, "stack->continue_at_ = 0;\n");
  if (cc->on_internal_error_snippet_.num_tokens_) {
    size_t token_idx;
    ip_printf(ip, "{\n");
    ip_force_indent_print(ip);
    for (token_idx = 0; token_idx < cc->on_internal_error_snippet_.num_tokens_; ++token_idx) {
      ip_printf(ip, "%s", cc->on_internal_error_snippet_.tokens_[token_idx].text_.original_);
    }
    ip_printf(ip, "\n}\n");
  }
  else {
    ip_printf(ip, "return _%sOVERFLOW;\n", cc_PREFIX(cc));
  }
}

void emit_alloc_error(struct indented_printer *ip, struct carburetta_context *cc) {
  ip_printf(ip, "stack->continue_at_ = 0;\n");
  if (cc->on_alloc_error_snippet_.num_tokens_) {
    size_t token_idx;
    ip_printf(ip, "{\n");
    ip_force_indent_print(ip);
    for (token_idx = 0; token_idx < cc->on_alloc_error_snippet_.num_tokens_; ++token_idx) {
      ip_printf(ip, "%s", cc->on_alloc_error_snippet_.tokens_[token_idx].text_.original_);
    }
    ip_printf(ip, "\n}\n");
  }
  else {
    ip_printf(ip, "return _%sNO_MEMORY;\n", cc_PREFIX(cc));
  }
}

void emit_internal_error(struct indented_printer *ip, struct carburetta_context *cc) {
  ip_printf(ip, "stack->continue_at_ = 0;\n");
  if (cc->on_internal_error_snippet_.num_tokens_) {
    size_t token_idx;
    ip_printf(ip, "{\n");
    ip_force_indent_print(ip);
    for (token_idx = 0; token_idx < cc->on_internal_error_snippet_.num_tokens_; ++token_idx) {
      ip_printf(ip, "%s", cc->on_internal_error_snippet_.tokens_[token_idx].text_.original_);
    }
    ip_printf(ip, "\n}\n");
  }
  else {
    ip_printf(ip, "/* Internal error */\n"
                  "return _%sINTERNAL_ERROR;\n", cc_PREFIX(cc));
  }
}

void emit_on_next(struct indented_printer *ip, struct carburetta_context *cc) {
  ip_printf(ip, "stack->continue_at_ = 0;\n");
  if (cc->on_next_token_snippet_.num_tokens_) {
    ip_printf(ip, "{\n");
    ip_force_indent_print(ip);
    size_t token_idx;
    for (token_idx = 0; token_idx < cc->on_next_token_snippet_.num_tokens_; ++token_idx) {
      ip_printf(ip, "%s", cc->on_next_token_snippet_.tokens_[token_idx].text_.original_);
    }
    ip_printf(ip, "\n}\n");
  }
  else {
    ip_printf(ip, "/* Next token */\n"
                  "return _%sFEED_ME;\n", cc_PREFIX(cc));
  }
}

void emit_feed_me(struct indented_printer *ip, struct carburetta_context *cc) {
  ip_printf(ip, "stack->continue_at_ = 0;\n");
  if (cc->on_feed_me_snippet_.num_tokens_) {
    ip_printf(ip, "{\n");
    ip_force_indent_print(ip);
    size_t token_idx;
    for (token_idx = 0; token_idx < cc->on_feed_me_snippet_.num_tokens_; ++token_idx) {
      ip_printf(ip, "%s", cc->on_feed_me_snippet_.tokens_[token_idx].text_.original_);
    }
    ip_printf(ip, "\n}\n");
  }
  else {
    ip_printf(ip, "/* Need more input */\n"
                  "return _%sFEED_ME;\n", cc_PREFIX(cc));
  }
}

static void emit_push_state(struct indented_printer *ip, struct carburetta_context *cc, const char *action) {
  ip_printf(ip, "switch (%spush_state(stack, %s)) {\n", cc_prefix(cc), action);
  ip_printf(ip, "  case _%sOVERFLOW: {\n", cc_PREFIX(cc));
  emit_overflow_error(ip, cc);
  ip_printf(ip, "  }\n"
                "  break;\n");
  ip_printf(ip, "  case _%sNO_MEMORY: {\n", cc_PREFIX(cc));
  emit_alloc_error(ip, cc);
  ip_printf(ip, "  }\n"
                "  break;\n"
                "} /* switch */\n");
}

static void emit_scan_function(struct indented_printer *ip, struct carburetta_context *cc, struct prd_grammar *prdg, struct lr_generator *lalr, int *state_syms) {
  /* Emit the parse function */
  cc->current_snippet_continuation_ = 1;
  if (cc->params_snippet_.num_tokens_) {
    ip_printf(ip, "int %sscan(struct %sstack *stack, ", cc_prefix(cc), cc_prefix(cc));
    size_t token_idx;
    for (token_idx = 0; token_idx < cc->params_snippet_.num_tokens_; ++token_idx) {
      ip_printf(ip, "%s", cc->params_snippet_.tokens_[token_idx].text_.original_);
    }
    ip_printf(ip, ") {\n");
  }
  else {
    ip_printf(ip, "int %sscan(struct %sstack *stack) {\n", cc_prefix(cc), cc_prefix(cc));
  }
  ip_printf(ip, "  if (stack->pending_reset_) {\n"
                "    int r;\n"
                "    r = %sstack_reset(stack);\n"
                "    if (r) return r;\n"
                "  }\n", cc_prefix(cc));
  
  ip_set_retained_output(ip, 1);

  ip_printf(ip, "  for (;;) {\n");
  ip_printf(ip, "    stack->continue_at_ = 0;\n");
  ip_printf(ip, "    if (stack->need_sym_) {\n");
  ip_printf(ip, "      switch (%slex(stack)) {\n", cc_prefix(cc));
  ip_printf(ip, "        case _%sMATCH:\n", cc_PREFIX(cc));
  ip_printf(ip, "          stack->need_sym_ = 0;\n");
  ip_printf(ip, "          stack->discard_remaining_actions_ = 0;\n");
  ip_printf(ip, "          stack->current_sym_ = ");
  if (print_sym_as_c_ident(ip, cc, cc->input_end_sym_)) {
    ip->had_error_ = 1;
    goto cleanup_exit;
  }
  ip_printf(ip, "; /* no sym specific type associated, changed when we successfully construct it below */\n");
  ip_printf(ip, "          if (stack->mute_error_turns_) stack->mute_error_turns_--;\n");
  if (cc->common_data_assigned_type_) {
    if (!cc->common_data_assigned_type_->is_raii_constructor_) {
      ip_printf(ip, "          stack->slot_0_has_common_data_ = 1;\n");
    }
    if (emit_pattern_token_common_constructor_snippet(ip, cc, cc->common_data_assigned_type_->is_raii_constructor_)) {
      ip->had_error_ = 1;
      goto cleanup_exit;
    }
    if (cc->common_data_assigned_type_->is_raii_constructor_) {
      ip_printf(ip, "          stack->slot_0_has_common_data_ = 1;\n");
    }
  }

  ip_printf(ip, "          switch (stack->best_match_action_) {\n");
  size_t pat_idx;
  for (pat_idx = 0; pat_idx < prdg->num_patterns_; ++pat_idx) {
    struct prd_pattern *pat = prdg->patterns_ + pat_idx;
    ip_printf(ip, "            case %zu: ", pat_idx + 1);
    print_regex_as_comment(ip, pat->regex_);
    ip_printf(ip, "\n");
    if (pat->term_.sym_) {
      ip_printf(ip, "              stack->current_sym_ = ");
      if (print_sym_as_c_ident(ip, cc, pat->term_.sym_)) {
        ip->had_error_ = 1;
        goto cleanup_exit;
      }
      ip_printf(ip, ";\n");
      if (pat->term_.sym_->assigned_type_ && !pat->term_.sym_->assigned_type_->is_raii_constructor_) {
        ip_printf(ip, "              stack->slot_0_has_current_sym_data_ = 1;\n");
      }
      if (emit_pattern_token_constructor_snippet(ip, cc, pat->term_.sym_->assigned_type_, 0)) {
        ip->had_error_ = 1;
        goto cleanup_exit;
      }
      if (pat->term_.sym_->assigned_type_ && pat->term_.sym_->assigned_type_->is_raii_constructor_) {
        ip_printf(ip, "              stack->slot_0_has_current_sym_data_ = 1;\n");
      }
      if (pat->common_action_sequence_.num_tokens_) {
        ip_printf(ip, "              if (!stack->discard_remaining_actions_) {\n");
        if (emit_pattern_common_action_snippet(ip, cc, pat)) {
          ip->had_error_ = 1;
          goto cleanup_exit;
        }
        ip_printf(ip, "              }\n");
      }
      if (pat->action_sequence_.num_tokens_) {
        ip_printf(ip, "              if (!stack->discard_remaining_actions_) {\n");
        if (emit_pattern_action_snippet(ip, cc, pat)) {
          ip->had_error_ = 1;
          goto cleanup_exit;
        }
        ip_printf(ip, "              }\n");
      }
    }
    else {
      /* Pattern matches no symbol */
      ip_printf(ip, "              /* Pattern does not have a symbol */\n"
                    "              stack->current_sym_ = ");
      if (print_sym_as_c_ident(ip, cc, cc->input_end_sym_)) {
        ip->had_error_ = 1;
        goto cleanup_exit;
      }
      ip_printf(ip, ";\n"
                    "              stack->need_sym_ = 1; /* keep scanning */\n");
      if (emit_pattern_action_snippet(ip, cc, pat)) {
        ip->had_error_ = 1;
        goto cleanup_exit;
      }
      /* Assuming we still need a sym, we should deconstruct the common data. */
      if (cc->common_data_assigned_type_ && cc->common_data_assigned_type_->destructor_snippet_.num_tokens_) {
        ip_printf(ip, "              if (stack->need_sym_) {\n"
                      "                ");
        if (emit_pattern_token_common_destructor_snippet(ip, cc)) {
          ip->had_error_ = 1;
          goto cleanup_exit;
          }
        ip_printf(ip, "              }\n");
      }
    }
    ip_printf(ip, "              break;\n");
  }
  ip_printf(ip, "          } /* switch */\n");
  
  ip_printf(ip, "          break;\n");
  ip_printf(ip, "        case _%sOVERFLOW:\n", cc_PREFIX(cc));
  emit_overflow_error(ip, cc);
  ip_printf(ip, "        case _%sNO_MEMORY:\n", cc_PREFIX(cc));
  emit_alloc_error(ip, cc);
  ip_printf(ip, "        case _%sFEED_ME:\n", cc_PREFIX(cc));
  emit_feed_me(ip, cc);
  ip_printf(ip, "        case _%sEND_OF_INPUT:\n", cc_PREFIX(cc));
  ip_printf(ip, "          stack->current_sym_ = ");
  if (print_sym_as_c_ident(ip, cc, cc->input_end_sym_)) {
    ip->had_error_ = 1;
    goto cleanup_exit;
  }
  ip_printf(ip, ";\n");
  ip_printf(ip, "          stack->need_sym_ = 0;\n");
  ip_printf(ip, "          if (stack->mute_error_turns_) stack->mute_error_turns_--;\n");
  ip_printf(ip, "          break;\n");
  ip_printf(ip, "        case _%sLEXICAL_ERROR:\n", cc_PREFIX(cc));
  emit_lexical_error(ip, cc);
  ip_printf(ip, "      } /* switch */\n");
  ip_printf(ip, "    } /* if (need_sym_) */\n");
  ip_printf(ip, "    else {\n");
  ip_printf(ip, "      int sym = stack->current_sym_;\n"
                "      if (!stack->error_recovery_) {\n"
                "        int action = %sparse_table[%snum_columns * stack->stack_[stack->pos_ - 1].state_ + (sym - %sminimum_sym)];\n", cc_prefix(cc), cc_prefix(cc), cc_prefix(cc));
  /* Shift logic */
  ip_printf(ip, "        if (action > 0) {\n");
  emit_push_state(ip, cc, "action");
  ip_printf(ip, "\n"
                "          /* Fill in the sym from the tokenizer */\n");
  ip_printf(ip, "          stack->need_sym_ = 1;\n");
  if (cc->common_data_assigned_type_) {
    ip_printf(ip, "          stack->stack_[stack->pos_ - 1].common_ = stack->stack_[0].common_;\n");
  }
  if (cc->have_typed_symbols_) {
    ip_printf(ip, "          stack->stack_[stack->pos_ - 1].v_ = stack->stack_[0].v_;\n");
  }
  ip_printf(ip, "          stack->slot_0_has_current_sym_data_ = 0;\n");
  ip_printf(ip, "          stack->slot_0_has_common_data_ = 0;\n");
  ip_printf(ip, "          stack->top_of_stack_has_sym_data_ = 1;\n");
  ip_printf(ip, "          stack->top_of_stack_has_common_data_ = 1;\n");

  if (cc->on_next_token_snippet_.num_tokens_) {
    ip_printf(ip, "          {\n"
                  "            ");
    size_t token_idx;
    for (token_idx = 0; token_idx < cc->on_next_token_snippet_.num_tokens_; ++token_idx) {
      ip_printf(ip, "%s", cc->on_next_token_snippet_.tokens_[token_idx].text_.original_);
    }
    ip_printf(ip, "          }\n");
  }

  ip_printf(ip, "        } /* action > 0 */\n");

  ip_printf(ip, "        else if (action < 0) {\n"
                "          int production = -action - 1;\n"
                "          stack->discard_remaining_actions_ = 0;\n"
                "          stack->current_production_length_ = %sproduction_lengths[production];\n", cc_prefix(cc));
  ip_printf(ip, "          stack->current_production_nonterminal_ = %sproduction_syms[production];\n", cc_prefix(cc));
  ip_printf(ip, "          if (0 == production) {\n"
                "            stack->pending_reset_ = 1;\n"
                "            ");
  if (cc->on_finish_snippet_.num_tokens_) {
    size_t token_idx;
    for (token_idx = 0; token_idx < cc->on_finish_snippet_.num_tokens_; ++token_idx) {
      ip_printf(ip, "%s", cc->on_finish_snippet_.tokens_[token_idx].text_.original_);
    }
  }
  else {
    ip_printf(ip, "return _%sFINISH;\n", cc_PREFIX(cc));
  }

  ip_printf(ip, "          }\n"
                "\n"
                "          memset(&stack->stack_[1], 0, sizeof(struct %ssym_data));\n", cc_prefix(cc));

  ip_printf(ip, "          { /* scope guard */\n"
                "            stack->sym_data_ = stack->stack_ + stack->pos_ - stack->current_production_length_;\n", cc_prefix(cc));
  /* Emit any requested locals */
  if (cc->locals_snippet_.num_tokens_) {
    ip_printf(ip, "          ");
    size_t token_idx;
    for (token_idx = 0; token_idx < cc->locals_snippet_.num_tokens_; ++token_idx) {
      ip_printf(ip, "%s", cc->locals_snippet_.tokens_[token_idx].text_.original_);
    }
    ip_printf(ip, "\n");
  }

  ip_printf(ip, "            switch (production) {\n");
  size_t row;
  for (row = 0; row < prdg->num_productions_; ++row) {
    struct prd_production *pd = prdg->productions_ + row;
    ip_printf(ip, "            /* %s:", pd->nt_.id_.translated_);
    size_t n;
    for (n = 0; n < pd->num_syms_; ++n) {
      ip_printf(ip, " %s", pd->syms_[n].id_.translated_);
    }
    ip_printf(ip, " */\n");
    ip_printf(ip, "              case %d: {\n", (int)row + 1);
    if (cc->common_data_assigned_type_) {
      if (!cc->common_data_assigned_type_->is_raii_constructor_) {
        ip_printf(ip, "                stack->slot_1_has_common_data_ = 1;\n");
      }
      /* Emit dst_sym_data constructor first */
      if (emit_dst_common_constructor_snippet(ip, cc, cc->common_data_assigned_type_->is_raii_constructor_)) {
        ip->had_error_ = 1;
        goto cleanup_exit;
      }
      if (cc->common_data_assigned_type_->is_raii_constructor_) {
        ip_printf(ip, "                stack->slot_1_has_common_data_ = 1;\n");
      }
    }

    if (pd->nt_.sym_->assigned_type_) {
      if (!pd->nt_.sym_->assigned_type_->is_raii_constructor_) {
        ip_printf(ip, "                stack->slot_1_has_sym_data_ = 1;\n");
      }
      ip_printf(ip, "                stack->slot_1_sym_ = ");
      print_sym_as_c_ident(ip, cc, pd->nt_.sym_);
      ip_printf(ip, ";\n");
      if (emit_dst_sym_constructor_snippet(ip, cc, pd->nt_.sym_->assigned_type_, pd->nt_.sym_->assigned_type_->is_raii_constructor_)) {
        ip->had_error_ = 1;
        goto cleanup_exit;
      }
      if (pd->nt_.sym_->assigned_type_->is_raii_constructor_) {
        ip_printf(ip, "                stack->slot_1_has_sym_data_ = 1;\n");
      }
    }
    if (pd->common_action_sequence_.num_tokens_) {
      if (emit_common_action_snippet(ip, cc, pd)) {
        ip->had_error_ = 1;
        goto cleanup_exit;
      }
    }
    int emit_discard = pd->common_action_sequence_.num_tokens_ && pd->action_sequence_.num_tokens_;
    if (emit_discard) {
      ip_printf(ip, "              if (!stack->discard_remaining_actions_) {\n");
    }
    if (emit_action_snippet(ip, cc, pd)) {
      ip->had_error_ = 1;
      goto cleanup_exit;
    }

    if (emit_discard) {
      ip_printf(ip, "              }\n");
    }

    ip_printf(ip, "              }\n"
                  "              break;\n");
  }
  ip_printf(ip, "            } /* switch */\n");
  ip_printf(ip, "          } /* scope guard */\n");
  ip_printf(ip, "\n");
  ip_printf(ip, "          /* Free symdata for every symbol in the production, including the first slot where we will soon\n"
                "           * push nonterminal_data_reduced_to */\n"
                "          size_t %ssym_idx;\n", cc_prefix(cc));
  ip_printf(ip, "          for (%ssym_idx = stack->pos_ - stack->current_production_length_; %ssym_idx < stack->pos_; ++%ssym_idx) {\n", cc_prefix(cc), cc_prefix(cc), cc_prefix(cc));

  ip_printf(ip, "            switch (stack->stack_[%ssym_idx].state_) {\n", cc_prefix(cc));
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
          ip_printf(ip, "              case %d: /* %s */\n", (int)state_idx, sym->def_.translated_);
          have_cases = 1;
        }
      }
      if (have_cases) {
        ip_printf(ip, "              {\n"
                       "                ");
        if (emit_destructor_snippet(ip, cc, ts)) {
          ip->had_error_ = 1;
          goto cleanup_exit;
        }

        ip_printf(ip, "\n"
                      "              }\n"
                      "              break;\n");
      }
    }
  }
  /* Enumerate all states, for each state, determine the type corresponding to the state from the symbol corresponding to the state. */
  ip_printf(ip, "            } /* switch */\n");
  if (emit_common_destructor_snippet(ip, cc)) {
    ip->had_error_ = 1;
    goto cleanup_exit;
  }

  ip_printf(ip, "          } /* for */\n"
                "          stack->pos_ -= stack->current_production_length_;\n"
                "          action = %sparse_table[%snum_columns * stack->stack_[stack->pos_ - 1].state_ + (stack->current_production_nonterminal_ - %sminimum_sym)];\n", cc_prefix(cc), cc_prefix(cc), cc_prefix(cc));
  ip_printf(ip, "          if (action <= 0) {\n");
  emit_internal_error(ip, cc);
  ip_printf(ip, "          }\n");

  emit_push_state(ip, cc, "action /* action for a \"goto\" shift is the ordinal */");
  ip_printf(ip, "          struct %ssym_data *sd = stack->stack_ + stack->pos_ - 1;\n", cc_prefix(cc));
  ip_printf(ip, "          *sd = stack->stack_[1];\n"
                "          sd->state_ = action;\n");
  ip_printf(ip, "          stack->slot_1_has_common_data_ = 0;\n");
  ip_printf(ip, "          stack->slot_1_has_sym_data_ = 0;\n");
  ip_printf(ip, "          stack->top_of_stack_has_sym_data_ = 1;\n");
  ip_printf(ip, "          stack->top_of_stack_has_common_data_ = 1;\n");

  ip_printf(ip, "        } /* action < 0 */\n"
                "        else /* action == 0 */ {\n");
  ip_printf(ip, "          /* check if we can recover using an error token. */\n"
                "          size_t n;\n"
                "          for (n = 0; n < stack->pos_; ++n) {\n");
  ip_printf(ip, "            int err_action = %sparse_table[%snum_columns * stack->stack_[n].state_ + (%d /* error token */ - %sminimum_sym)];\n", cc_prefix(cc), cc_prefix(cc), cc->error_sym_->ordinal_, cc_prefix(cc));
  ip_printf(ip, "            if (err_action > 0) {\n"
                "              /* we can transition on the error token somewhere on the stack */\n"
                "              break;\n"
                "            }\n"
                "          }\n");
  ip_printf(ip, "          if (n != stack->pos_) {\n"
                "            /* Enter error-token recovery mode given that such a recovery is possible */\n");
  ip_printf(ip, "            stack->error_recovery_ = (n != stack->pos_);\n"
                "          }\n"
                "          else {\n");
  ip_printf(ip, "            if (sym != ");
  if (print_sym_as_c_ident(ip, cc, cc->input_end_sym_)) {
    ip->had_error_ = 1;
    goto cleanup_exit;
  }
  ip_printf(ip, ") {\n");
  ip_printf(ip, "              /* Retain EOF but discard any other sym so we make progress */\n"
                "              stack->need_sym_ = 1;\n");

  ip_printf(ip, "              /* Deconstruct placeholder location for discarded symbol */\n");
  ip_printf(ip, "              switch (stack->current_sym_) {\n");
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
            ip_printf(ip, "                case %d:\n", sym->ordinal_);

          }

          sym = sym->next_;
        } while (sym != the_syms[n]);
      }
    }
    if (have_some) {
      /* Execute destructors for typestr ts at stack->stack_[0] */
      if (emit_destructor_snippet_indexed_by_0(ip, cc, ts)) {
        ip->had_error_ = 1;
        goto cleanup_exit;
      }
      ip_printf(ip, "                  break;\n");
    }
  }
  ip_printf(ip, "              } /* switch */\n");

  if (cc->common_data_assigned_type_ && cc->common_data_assigned_type_->destructor_snippet_.num_tokens_) {
    if (emit_common_destructor_snippet_index_0(ip, cc)) {
      ip->had_error_ = 1;
      goto cleanup_exit;
    }
  }
  ip_printf(ip, "            }\n"
                "          }\n");
  ip_printf(ip, "          /* Issue the error here */\n"
                "          if (!stack->mute_error_turns_) {\n"
                "            stack->mute_error_turns_ = 3;\n"
                "            ");
  emit_syntax_error(ip, cc);
  ip_printf(ip, "          }\n"
                "          else {\n"
                "            stack->mute_error_turns_--;\n"
                "            if (sym == ");
  if (print_sym_as_c_ident(ip, cc, cc->input_end_sym_)) {
    ip->had_error_ = 1;
    goto cleanup_exit;
  }
  ip_printf(ip, ") {\n");
  ip_printf(ip, "              /* EOF means we cannot shift to recover, and errors are muted, so return completion */\n"
                "              stack->pending_reset_ = 1;\n");
  if (cc->on_finish_snippet_.num_tokens_) {
    size_t token_idx;
    for (token_idx = 0; token_idx < cc->on_finish_snippet_.num_tokens_; ++token_idx) {
      ip_printf(ip, "%s", cc->on_finish_snippet_.tokens_[token_idx].text_.original_);
    }
  }
  else {
    ip_printf(ip, "return _%sFINISH;\n", cc_PREFIX(cc));
  }

  ip_printf(ip, "            }\n"
                "          }\n"
                "        }\n");
  ip_printf(ip, "      } /* !stack->error_recovery_ */\n"
                "      if (stack->error_recovery_) {\n");
  ip_printf(ip, "        size_t n;\n"
                "        n = stack->pos_;\n"
                "        if (n) {\n"
                "          do {\n"
                "            --n;\n"
                "            /* Can we shift an error token? */\n");
  ip_printf(ip, "            int err_action = %sparse_table[%snum_columns * stack->stack_[n].state_ + (%d /* error token */ - %sminimum_sym)];\n", cc_prefix(cc), cc_prefix(cc), cc->error_sym_->ordinal_, cc_prefix(cc));
  ip_printf(ip, "            if (err_action > 0) {\n");
  ip_printf(ip, "              /* Does the resulting state accept the current symbol? */\n"
                "              int err_sym_action = %sparse_table[%snum_columns * err_action + (sym - %sminimum_sym)];\n", cc_prefix(cc), cc_prefix(cc), cc_prefix(cc));
  ip_printf(ip, "              if (err_sym_action) {\n"
                "                /* Current symbol is accepted, recover error condition by shifting the error token and then process the symbol as usual */\n");

  ip_printf(ip, "                /* Free symdata for every symbol up to the state where we will shift the error token */\n"
                "                size_t %ssym_idx;\n", cc_prefix(cc));
  ip_printf(ip, "                for (%ssym_idx = n + 1; %ssym_idx < stack->pos_; ++%ssym_idx) {\n", cc_prefix(cc), cc_prefix(cc), cc_prefix(cc));

  ip_printf(ip, "                  switch (stack->stack_[%ssym_idx].state_) {\n", cc_prefix(cc));
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
          ip_printf(ip, "                    case %d: /* %s */\n", (int)state_idx, sym->def_.translated_);
          have_cases = 1;
        }
      }
      if (have_cases) {
        if (emit_destructor_snippet(ip, cc, ts)) {
          ip->had_error_ = 1;
          goto cleanup_exit;
        }

        /* Close this compound block  */
        ip_printf(ip, "                    break;\n");
      }
    }
  }
  ip_printf(ip, "                  } /* switch */\n");
  if (emit_common_destructor_snippet(ip, cc)) {
    ip->had_error_ = 1;
    goto cleanup_exit;
  }
  /* Enumerate all states, for each state, determine the type corresponding to the state from the symbol corresponding to the state. */
  ip_printf(ip, "                } /* for */\n"
                "                stack->pos_ = n + 1;\n");
  emit_push_state(ip, cc, "err_action");
  ip_printf(ip, "                stack->error_recovery_ = 0;\n");
  ip_printf(ip, "                /* Break out of do { .. } while loop, we've recovered */\n"
                "                break;\n");

  ip_printf(ip, "              } /* if (err_sym_action) (if the current sym can continue after an error transition) */\n");
  ip_printf(ip, "            } /* if (err_action) (if the state at position 'n' can accept an error transition) */\n");

  ip_printf(ip, "          } while (n);\n"
                "        }\n");

  ip_printf(ip, "        if (stack->error_recovery_) {\n"
                "          /* Did not yet recover, discard current sym and get next */\n");
  ip_printf(ip, "          if (stack->current_sym_ == ");
  if (print_sym_as_c_ident(ip, cc, cc->input_end_sym_)) {
    ip->had_error_ = 1;
    goto cleanup_exit;
  }
  ip_printf(ip, ") {\n");
  ip_printf(ip, "            /* EOF means we cannot shift to recover, so return completion */\n"
                "            stack->pending_reset_ = 1;\n");
  if (cc->on_finish_snippet_.num_tokens_) {
    size_t token_idx;
    for (token_idx = 0; token_idx < cc->on_finish_snippet_.num_tokens_; ++token_idx) {
      ip_printf(ip, "%s", cc->on_finish_snippet_.tokens_[token_idx].text_.original_);
    }
  }
  else {
    ip_printf(ip, "return _%sFINISH;\n", cc_PREFIX(cc));
  }

  ip_printf(ip, "          }\n");

  ip_printf(ip, "          stack->need_sym_ = 1;\n");
  if (cc->on_next_token_snippet_.num_tokens_) {
    size_t token_idx;
    for (token_idx = 0; token_idx < cc->on_next_token_snippet_.num_tokens_; ++token_idx) {
      ip_printf(ip, "%s", cc->on_next_token_snippet_.tokens_[token_idx].text_.original_);
    }
  }
  ip_printf(ip, "          /* Deconstruct placeholder location for discarded symbol */\n");
  ip_printf(ip, "          switch (stack->current_sym_) {\n");
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
            ip_printf(ip, "            case %d:\n", sym->ordinal_);

          }

          sym = sym->next_;
        } while (sym != the_syms[n]);
      }
    }
    if (have_some) {
      /* Execute destructors for typestr ts at stack->stack_[0] */
      if (emit_destructor_snippet_indexed_by_0(ip, cc, ts)) {
        ip->had_error_ = 1;
        goto cleanup_exit;
      }
      ip_printf(ip, "              break;\n");
    }
  }
  ip_printf(ip, "          } /* switch */\n");

  if (emit_common_destructor_snippet_index_0(ip, cc)) {
    ip->had_error_ = 1;
    goto cleanup_exit;
  }

  ip_printf(ip, "        }\n");
  ip_printf(ip, "      } /* stack->error_recovery_ */\n");
  ip_printf(ip, "    }\n");
  ip_printf(ip, "  } /* for (;;) */\n");

  struct ip_retained_output_bucket *retained_output = ip_extract_retained_output(ip);
  ip_set_retained_output(ip, 0);
  emit_snippet_continuation_jump(ip, cc);
  cc->current_snippet_continuation_ = 0;
  ip_write_retained_output_bucket_chain(ip, retained_output);
  ip_free_retained_output_bucket_chain(retained_output);

  ip_printf(ip, "}\n");

cleanup_exit:
  ;
}


static void emit_parse_function(struct indented_printer *ip, struct carburetta_context *cc, struct prd_grammar *prdg, struct lr_generator *lalr, int *state_syms) {
  /* Emit the parse function */
  cc->current_snippet_continuation_ = 1;
  if (cc->params_snippet_.num_tokens_) {
    ip_printf(ip, "int %sparse(struct %sstack *stack, int sym, ", cc_prefix(cc), cc_prefix(cc));
    size_t token_idx;
    for (token_idx = 0; token_idx < cc->params_snippet_.num_tokens_; ++token_idx) {
      ip_printf(ip, "%s", cc->params_snippet_.tokens_[token_idx].text_.original_);
    }
    ip_printf(ip, ") {\n");
  }
  else {
    ip_printf(ip, "int %sparse(struct %sstack *stack, int sym) {\n", cc_prefix(cc), cc_prefix(cc));
  }
  ip_printf(ip, "  if (stack->pending_reset_) {\n"
                "    int r;\n"
                "    r = %sstack_reset(stack);\n"
                "    if (r) return r;\n"
                "  }\n", cc_prefix(cc));

  ip_set_retained_output(ip, 1);

  ip_printf(ip, "  if (stack->mute_error_turns_) stack->mute_error_turns_--;\n");
  ip_printf(ip, "  for (;;) {\n"
                "    if (!stack->error_recovery_) {\n"
                "      int action = %sparse_table[%snum_columns * stack->stack_[stack->pos_ - 1].state_ + (sym - %sminimum_sym)];\n", cc_prefix(cc), cc_prefix(cc), cc_prefix(cc));

  /* Shift logic */
  ip_printf(ip, "      if (action > 0) {\n");
  emit_push_state(ip, cc, "action");

  ip_printf(ip, "\n");
  int need_sym_data = 0;
  ip_printf(ip, "        stack->sym_data_ = stack->stack_ + stack->pos_ - 1;\n", cc_prefix(cc));
  ip_printf(ip, "        stack->top_of_stack_has_sym_data_ = 0;\n");
  if (cc->common_data_assigned_type_) {
    if (!cc->common_data_assigned_type_->is_raii_constructor_) {
      ip_printf(ip, "              stack->top_of_stack_has_common_data_ = 1;\n");
    }
    else {
      ip_printf(ip, "              stack->top_of_stack_has_common_data_ = 0;\n");
    }
    if (emit_token_common_constructor_snippet(ip, cc, cc->common_data_assigned_type_->is_raii_constructor_)) {
      ip->had_error_ = 1;
      goto cleanup_exit;
    }
    if (cc->common_data_assigned_type_->is_raii_constructor_) {
      ip_printf(ip, "              stack->top_of_stack_has_common_data_ = 1;\n");
    }
  }

  if (emit_token_common_action_snippet(ip, cc)) {
    ip->had_error_ = 1;
    goto cleanup_exit;
  }

  ip_printf(ip, "        switch (sym) {\n");
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
          ip_printf(ip, "          case ");
          print_sym_as_c_ident(ip, cc, term);
          ip_printf(ip, ":\n");
        }

      } while (term != cc->symtab_.terminals_);
    }
    if (found_matching_terms) {
      if (!ts->is_raii_constructor_) {
        ip_printf(ip, "        stack->top_of_stack_has_sym_data_ = 1;\n");
      }
      if (emit_token_constructor_snippet(ip, cc, ts, ts->is_raii_constructor_)) {
        ip->had_error_ = 1;
        goto cleanup_exit;
      }
      if (ts->is_raii_constructor_) {
        ip_printf(ip, "        stack->top_of_stack_has_sym_data_ = 1;\n");
      }
      if (emit_token_action_snippet(ip, cc, ts)) {
        ip->had_error_ = 1;
        goto cleanup_exit;
      }
      ip_printf(ip, "            break;\n");
    }
  }
  ip_printf(ip, "        } /* switch */\n");

  emit_on_next(ip, cc);
  ip_printf(ip, "      } /* action > 0 */\n");

  ip_printf(ip, "      else if (action < 0) {\n"
                "        int production = -action - 1;\n"
                "        stack->discard_remaining_actions_ = 0;\n"
                "        stack->current_production_length_ = %sproduction_lengths[production];\n", cc_prefix(cc));
  ip_printf(ip, "        stack->current_production_nonterminal_ = %sproduction_syms[production];\n", cc_prefix(cc));
  ip_printf(ip, "        if (0 == production) {\n"
                "          stack->pending_reset_ = 1;\n");
  if (cc->on_finish_snippet_.num_tokens_) {
    size_t token_idx;
    for (token_idx = 0; token_idx < cc->on_finish_snippet_.num_tokens_; ++token_idx) {
      ip_printf(ip, "%s", cc->on_finish_snippet_.tokens_[token_idx].text_.original_);
    }
  }
  else {
    ip_printf(ip, "/* Synth S we're done */\n"
                  "return _%sFINISH;\n", cc_PREFIX(cc));
  }

  ip_printf(ip, "        }\n"
                "\n"
                "          memset(&stack->stack_[1], 0, sizeof(struct %ssym_data));\n", cc_prefix(cc));

  ip_printf(ip, "        { /* scope guard */\n"
                "          stack->sym_data_ = stack->stack_ + stack->pos_ - stack->current_production_length_;\n", cc_prefix(cc));
  /* Emit any requested locals */
  if (cc->locals_snippet_.num_tokens_) {
    ip_printf(ip, "          ");
    size_t token_idx;
    for (token_idx = 0; token_idx < cc->locals_snippet_.num_tokens_; ++token_idx) {
      ip_printf(ip, "%s", cc->locals_snippet_.tokens_[token_idx].text_.original_);
    }
    ip_printf(ip, "\n");
  }
   
  ip_printf(ip, "          switch (production) {\n");
  size_t row;
  for (row = 0; row < prdg->num_productions_; ++row) {
    struct prd_production *pd = prdg->productions_ + row;
    ip_printf(ip, "            /* %s:", pd->nt_.id_.translated_);
    size_t n;
    for (n = 0; n < pd->num_syms_; ++n) {
      ip_printf(ip, " %s", pd->syms_[n].id_.translated_);
    }
    ip_printf(ip, " */\n");
    ip_printf(ip, "            case %d: {\n", (int)row + 1);
    if (cc->common_data_assigned_type_) {
      if (!cc->common_data_assigned_type_->is_raii_constructor_) {
        ip_printf(ip, "              stack->slot_1_has_common_data_ = 1;\n");
      }
      /* Emit dst_sym_data constructor first */
      if (emit_dst_common_constructor_snippet(ip, cc, cc->common_data_assigned_type_->is_raii_constructor_)) {
        ip->had_error_ = 1;
        goto cleanup_exit;
      }
      if (cc->common_data_assigned_type_->is_raii_constructor_) {
        ip_printf(ip, "              stack->slot_1_has_common_data_ = 1;\n");
      }
    }

    if (pd->nt_.sym_->assigned_type_) {
      if (!pd->nt_.sym_->assigned_type_->is_raii_constructor_) {
        ip_printf(ip, "              stack->slot_1_has_sym_data_ = 1;\n");
      }
      ip_printf(ip, "              stack->slot_1_sym_ = ");
      print_sym_as_c_ident(ip, cc, pd->nt_.sym_);
      ip_printf(ip, ";\n");

      if (emit_dst_sym_constructor_snippet(ip, cc, pd->nt_.sym_->assigned_type_, pd->nt_.sym_->assigned_type_->is_raii_constructor_)) {
        ip->had_error_ = 1;
        goto cleanup_exit;
      }

      if (pd->nt_.sym_->assigned_type_->is_raii_constructor_) {
        ip_printf(ip, "              stack->slot_1_has_sym_data_ = 1;\n");
      }
    }

    if (emit_common_action_snippet(ip, cc, pd)) {
      ip->had_error_ = 1;
      goto cleanup_exit;
    }
      
    if (pd->common_action_sequence_.num_tokens_) {
      ip_printf(ip, "            if (!stack->discard_remaining_actions_) {\n"
                    "              ");
    }

    if (emit_action_snippet(ip, cc, pd)) {
      ip->had_error_ = 1;
      goto cleanup_exit;
    }

    if (pd->common_action_sequence_.num_tokens_) {
      ip_printf(ip, "            }\n");
    }

    ip_printf(ip, "            }\n");
    ip_printf(ip, "            break;\n");
  }
  ip_printf(ip, "          } /* switch */\n");
  ip_printf(ip, "        } /* scope guard */\n");
  ip_printf(ip, "\n");
  ip_printf(ip, "        /* Free symdata for every symbol in the production, including the first slot where we will soon\n"
                "         * push nonterminal_data_reduced_to */\n"
                "        size_t %ssym_idx;\n", cc_prefix(cc));
  ip_printf(ip, "        for (%ssym_idx = stack->pos_ - stack->current_production_length_; %ssym_idx < stack->pos_; ++%ssym_idx) {\n", cc_prefix(cc), cc_prefix(cc), cc_prefix(cc));

  ip_printf(ip, "          switch (stack->stack_[%ssym_idx].state_) {\n", cc_prefix(cc));
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
          ip_printf(ip, "            case %d: /* %s */\n", (int)state_idx, sym->def_.translated_);
          have_cases = 1;
        }
      }
      if (have_cases) {
        if (emit_destructor_snippet(ip, cc, ts)) {
          ip->had_error_ = 1;
          goto cleanup_exit;
        }

        ip_printf(ip, "            break;\n");
      }
    }
  }
  /* Enumerate all states, for each state, determine the type corresponding to the state from the symbol corresponding to the state. */
  ip_printf(ip, "          } /* switch */\n");
  if (emit_common_destructor_snippet(ip, cc)) {
    ip->had_error_ = 1;
    goto cleanup_exit;
  }

  ip_printf(ip, "        } /* for */\n"
                "        stack->pos_ -= stack->current_production_length_;\n"
                "        action = %sparse_table[%snum_columns * stack->stack_[stack->pos_ - 1].state_ + (stack->current_production_nonterminal_ - %sminimum_sym)];\n", cc_prefix(cc), cc_prefix(cc), cc_prefix(cc));
  ip_printf(ip, "        if (action <= 0) {\n"
                "          ");
  emit_internal_error(ip, cc);
  ip_printf(ip, "        }\n");

  emit_push_state(ip, cc, "action /* action for a \"goto\" shift is the ordinal */");

  ip_printf(ip, "        struct %ssym_data *sd = stack->stack_ + stack->pos_ - 1;\n", cc_prefix(cc));
  ip_printf(ip, "        *sd = stack->stack_[1];\n"
                "        sd->state_ = action;\n");
  ip_printf(ip, "        stack->slot_1_has_common_data_ = 0;\n");
  ip_printf(ip, "        stack->slot_1_has_sym_data_ = 0;\n");
  ip_printf(ip, "        stack->top_of_stack_has_common_data_ = 1;\n");
  ip_printf(ip, "        stack->top_of_stack_has_sym_data_ = 1;\n");
  ip_printf(ip, "      } /* action < 0 */\n"
                "      else /* action == 0 */ {\n");
  ip_printf(ip, "        /* check if we can recover using an error token. */\n"
                "        size_t n;\n"
                "        for (n = 0; n < stack->pos_; ++n) {\n");
  ip_printf(ip, "          int err_action = %sparse_table[%snum_columns * stack->stack_[n].state_ + (%d /* error token */ - %sminimum_sym)];\n", cc_prefix(cc), cc_prefix(cc), cc->error_sym_->ordinal_, cc_prefix(cc));
  ip_printf(ip, "          if (err_action > 0) {\n"
                "            /* we can transition on the error token somewhere on the stack */\n"
                "            break;\n"
                "          }\n"
                "        }\n");
  ip_printf(ip, "        if (n != stack->pos_) {\n"
                "          /* Enter error-token recovery mode given that such a recovery is possible */\n");
  ip_printf(ip, "          stack->error_recovery_ = (n != stack->pos_);\n"
                "        }\n"
                "        else {\n");
  ip_printf(ip, "          if (sym != ");
  if (print_sym_as_c_ident(ip, cc, cc->input_end_sym_)) {
    ip->had_error_ = 1;
    goto cleanup_exit;
  }
  ip_printf(ip, ") {\n");
  ip_printf(ip, "            /* Retain EOF but discard any other sym so we make progress */\n");
  emit_on_next(ip, cc);
  ip_printf(ip, "          }\n"
                "        }\n");
  ip_printf(ip, "        /* Issue the error here */\n"
                "        if (!stack->mute_error_turns_) {\n"
                "          stack->mute_error_turns_ = 3;\n");
  emit_syntax_error(ip, cc);
  ip_printf(ip, "        }\n"
                "        else {\n"
                "          stack->mute_error_turns_--;\n"
                "          if (sym == ");
  if (print_sym_as_c_ident(ip, cc, cc->input_end_sym_)) {
    ip->had_error_ = 1;
    goto cleanup_exit;
  }
  ip_printf(ip, ") {\n");
  ip_printf(ip, "            /* EOF means we cannot shift to recover, and errors are muted, so return completion */\n"
                "            stack->pending_reset_ = 1;\n");
  if (cc->on_finish_snippet_.num_tokens_) {
    size_t token_idx;
    for (token_idx = 0; token_idx < cc->on_finish_snippet_.num_tokens_; ++token_idx) {
      ip_printf(ip, "%s", cc->on_finish_snippet_.tokens_[token_idx].text_.original_);
    }
  }
  else {
    ip_printf(ip, "return _%sFINISH;\n", cc_PREFIX(cc));
  }

  ip_printf(ip, "          }\n"
                "        }\n"
                "      }\n");

  ip_printf(ip, "    } /* !stack->error_recovery_ */\n"
                "    if (stack->error_recovery_) {\n");
  ip_printf(ip, "      size_t n;\n"
                "      n = stack->pos_;\n"
                "      if (n) {\n"
                "        do {\n"
                "          --n;\n"
                "          /* Can we shift an error token? */\n");
  ip_printf(ip, "          int err_action = %sparse_table[%snum_columns * stack->stack_[n].state_ + (%d /* error token */ - %sminimum_sym)];\n", cc_prefix(cc), cc_prefix(cc), cc->error_sym_->ordinal_, cc_prefix(cc));
  ip_printf(ip, "          if (err_action > 0) {\n");
  ip_printf(ip, "            /* Does the resulting state accept the current symbol? */\n"
                "            int err_sym_action = %sparse_table[%snum_columns * err_action + (sym - %sminimum_sym)];\n", cc_prefix(cc), cc_prefix(cc), cc_prefix(cc));
  ip_printf(ip, "            if (err_sym_action) {\n"
                "              /* Current symbol is accepted, recover error condition by shifting the error token and then process the symbol as usual */\n");

  ip_printf(ip, "              /* Free symdata for every symbol up to the state where we will shift the error token */\n"
                "              size_t %ssym_idx;\n", cc_prefix(cc));
  ip_printf(ip, "              for (%ssym_idx = n + 1; %ssym_idx < stack->pos_; ++%ssym_idx) {\n", cc_prefix(cc), cc_prefix(cc), cc_prefix(cc));

  ip_printf(ip, "                switch (stack->stack_[%ssym_idx].state_) {\n", cc_prefix(cc));
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
          ip_printf(ip, "                  case %d: /* %s */\n", (int)state_idx, sym->def_.translated_);
          have_cases = 1;
        }
      }
      if (have_cases) {
        if (emit_destructor_snippet(ip, cc, ts)) {
          ip->had_error_ = 1;
          goto cleanup_exit;
        }

        ip_printf(ip, "                  break;\n");
      }
    }
  }
  ip_printf(ip, "                } /* switch */\n");

  if (emit_common_destructor_snippet(ip, cc)) {
    ip->had_error_ = 1;
    goto cleanup_exit;
  }

  /* Enumerate all states, for each state, determine the type corresponding to the state from the symbol corresponding to the state. */
  ip_printf(ip, "              } /* for */\n"
                "              stack->pos_ = n + 1;\n");

  ip_printf(ip, "              /* Push the state of the error transition */\n");
  emit_push_state(ip, cc, "err_action");

  ip_printf(ip, "              stack->error_recovery_ = 0;\n");
  ip_printf(ip, "              /* Break out of do { .. } while loop, we've recovered */\n"
                "              break;\n");

  ip_printf(ip, "            } /* if (err_sym_action) (if the current sym can continue after an error transition) */\n");
  ip_printf(ip, "          } /* if (err_action) (if the state at position 'n' can accept an error transition) */\n");

  ip_printf(ip, "        } while (n);\n"
                "      }\n");

  ip_printf(ip, "      if (stack->error_recovery_) {\n"
                "        /* Did not yet recover, discard current sym and get next */\n");
  ip_printf(ip, "          if (sym == ");
  if (print_sym_as_c_ident(ip, cc, cc->input_end_sym_)) {
    ip->had_error_ = 1;
    goto cleanup_exit;
  }
  ip_printf(ip, ") {\n");
  ip_printf(ip, "            /* EOF means we cannot shift to recover, so return completion */\n"
                "            stack->pending_reset_ = 1;\n");
  if (cc->on_finish_snippet_.num_tokens_) {
    size_t token_idx;
    for (token_idx = 0; token_idx < cc->on_finish_snippet_.num_tokens_; ++token_idx) {
      ip_printf(ip, "%s", cc->on_finish_snippet_.tokens_[token_idx].text_.original_);
    }
  }
  else {
    ip_printf(ip, "return _%sFINISH;\n", cc_PREFIX(cc));
  }
  ip_printf(ip, "          }\n");
  emit_on_next(ip, cc);
  ip_printf(ip, "      }\n");
  ip_printf(ip, "    } /* stack->error_recovery_ */\n");
  ip_printf(ip, "  } /* for (;;) */\n");

  struct ip_retained_output_bucket *retained_output = ip_extract_retained_output(ip);
  ip_set_retained_output(ip, 0);
  emit_snippet_continuation_jump(ip, cc);
  cc->current_snippet_continuation_ = 0;
  ip_write_retained_output_bucket_chain(ip, retained_output);
  ip_free_retained_output_bucket_chain(retained_output);

  ip_printf(ip, "}\n");

cleanup_exit:
  ;
}

int emit_sym_data_struct(struct indented_printer *ip, struct carburetta_context *cc) {
  ip_printf(ip, "struct %ssym_data {\n", cc_prefix(cc));
  ip_printf(ip, "  int state_;\n");
  if (cc->common_data_assigned_type_) {
    struct typestr *ts = cc->common_data_assigned_type_;
    ip_printf(ip, "  ");
    int found_placeholder = 0;
    size_t tok_idx;
    for (tok_idx = 0; tok_idx < ts->typestr_snippet_.num_tokens_; ++tok_idx) {
      struct snippet_token *st = ts->typestr_snippet_.tokens_ + tok_idx;
      if (st->variant_ != TOK_SPECIAL_IDENT) {
        ip_printf(ip, "%s%s", tok_idx ? " " : "", st->text_.translated_);
      }
      else {
        found_placeholder = 1;
        ip_printf(ip, " common_");
      }
    }
    if (!found_placeholder) {
      /* Placeholder is implied at the end */
      ip_printf(ip, " common_");
    }
    ip_printf(ip, ";\n");
  }
  if (cc->have_typed_symbols_) {
    ip_printf(ip, "  union {\n");
    size_t ts_idx;
    for (ts_idx = 0; ts_idx < cc->tstab_.num_typestrs_; ++ts_idx) {
      struct typestr *ts;
      ts = cc->tstab_.typestrs_[ts_idx];
      int found_placeholder = 0;

      if (!ts->is_symbol_type_) {
        /* Skip the common data type, as it is not part of the union but shared by all */
        continue;
      }

      ip_printf(ip, "    ");
      size_t tok_idx;
      for (tok_idx = 0; tok_idx < ts->typestr_snippet_.num_tokens_; ++tok_idx) {
        struct snippet_token *st = ts->typestr_snippet_.tokens_ + tok_idx;
        if (st->variant_ != TOK_SPECIAL_IDENT) {
          ip_printf(ip, "%s%s", tok_idx ? " " : "", st->text_.translated_);
        }
        else {
          found_placeholder = 1;
          ip_printf(ip, " uv%d_", ts->ordinal_);
        }
      }
      if (!found_placeholder) {
        /* Placeholder is implied at the end */
        ip_printf(ip, " uv%d_", ts->ordinal_);
      }
      ip_printf(ip, ";\n");
    }
    ip_printf(ip, "  } v_;\n");
  }
  ip_printf(ip, "};\n");
  return 0;
}

int emit_return_code_defines(struct indented_printer *ip, struct carburetta_context *cc) {
  ip_printf(ip, "#define _%sFINISH 0\n", cc_PREFIX(cc));
  ip_printf(ip, "#define _%sMATCH 1\n", cc_PREFIX(cc));
  ip_printf(ip, "#define _%sOVERFLOW 2\n", cc_PREFIX(cc));
  ip_printf(ip, "#define _%sNO_MEMORY 3\n", cc_PREFIX(cc));
  ip_printf(ip, "#define _%sFEED_ME 4\n", cc_PREFIX(cc));
  ip_printf(ip, "#define _%sEND_OF_INPUT 5\n", cc_PREFIX(cc));
  ip_printf(ip, "#define _%sSYNTAX_ERROR 6\n", cc_PREFIX(cc)); 
  ip_printf(ip, "#define _%sLEXICAL_ERROR 7\n", cc_PREFIX(cc));
  ip_printf(ip, "#define _%sINTERNAL_ERROR 8\n", cc_PREFIX(cc));
  return 0;
}

int emit_stack_struct_decl(struct indented_printer *ip, struct carburetta_context *cc, struct prd_grammar *prdg) {
  ip_printf(ip, "struct %sstack {\n", cc_prefix(cc));
  ip_printf(ip, "  int error_recovery_:1;\n");
  ip_printf(ip, "  int pending_reset_:1;\n");
  ip_printf(ip, "  int discard_remaining_actions_:1;\n");
  ip_printf(ip, "  int slot_1_has_sym_data_:1;\n"
                "  int slot_1_has_common_data_:1;\n");
  ip_printf(ip, "  int top_of_stack_has_sym_data_:1;\n");
  ip_printf(ip, "  int top_of_stack_has_common_data_:1;\n");
  if (prdg->num_patterns_) {
    ip_printf(ip, "  int need_sym_:1;\n"
                  "  int is_final_input_:1;\n"
                  "  int slot_0_has_current_sym_data_:1;\n"
                  "  int slot_0_has_common_data_:1;\n"
                  "  int current_sym_;\n"
                  "  size_t input_size_;\n"
                  "  const char *input_;\n");
  }
  ip_printf(ip, "  int slot_1_sym_;\n");

  ip_printf(ip, "  int continue_at_;\n");
  ip_printf(ip, "  int mute_error_turns_;\n");
  ip_printf(ip, "  size_t pos_, num_stack_allocated_;\n");
  ip_printf(ip, "  struct %ssym_data *stack_;\n", cc_prefix(cc));
  ip_printf(ip, "  struct %ssym_data *sym_data_;\n", cc_prefix(cc));
  ip_printf(ip, "  size_t current_production_length_;\n");
  ip_printf(ip, "  int current_production_nonterminal_;\n");
  if (prdg->num_patterns_) {
    ip_printf(ip, "  size_t scan_state_;\n"
                  "  size_t current_mode_start_state_;\n"
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
  ip_printf(ip, "};\n");
  return 0;
}

void emit_c_file(struct indented_printer *ip, struct carburetta_context *cc, struct prd_grammar *prdg, struct rex_scanner *rex, struct lr_generator *lalr) {
  int *state_syms;
  state_syms = NULL;

  struct part *pt;
  pt = cc->prologue_;
  if (pt) {
    do {
      pt = pt->next_;

      ip_write_no_indent(ip, pt->chars_, pt->num_chars_);

    } while (pt != cc->prologue_);
  }

  ip_printf(ip, "/* --------- START OF GENERATED CODE ------------ */\n");

  ip_printf(ip, "#include <stdlib.h> /* realloc(), free(), NULL, size_t */\n");
  ip_printf(ip, "#include <string.h> /* memcpy() */\n");
  ip_printf(ip, "#include <stddef.h> /* size_t */\n");
  ip_printf(ip, "#include <stdint.h> /* SIZE_MAX */\n");

  emit_sym_data_struct(ip, cc);

  if (prdg->num_patterns_) {
    ip_printf(ip, "static const size_t %sscan_table_rex[] = {\n", cc_prefix(cc));
    size_t col;
    char column_widths[256 + 4] = {0};
    struct rex_dfa_node *dn = rex->dfa_.nodes_;
    if (dn) {
      do {
        dn = dn->chain_;

        col = 0;

        struct rex_dfa_trans *dt = dn->outbound_;
        if (dt) {
          do {
            dt = dt->from_peer_;

            if (!dt->is_anchor_) {
              while (col < dt->symbol_start_) {
                /* No transition encodes 0 action, width 2 */
                if (column_widths[col] < 2) {
                  column_widths[col] = 2;
                }
                col++;
              }
              uint32_t input_sym;
              int dst_ordinal = dt->to_->ordinal_;
              int width_needed = 1;
              if (dst_ordinal < 100) {
                width_needed = 2;
              }
              else if (dst_ordinal < 1000) {
                width_needed = 3;
              }
              else if (dst_ordinal < 10000) {
                width_needed = 4;
              }
              else {
                width_needed = 5;
              }
              for (input_sym = dt->symbol_start_; input_sym < dt->symbol_end_; ++input_sym) {
                assert(col == input_sym);
                if (width_needed > column_widths[col]) {
                  column_widths[col] = width_needed;
                }
                col++;
              }
            }

          } while (dt != dn->outbound_);
        }

        while (col < 256) {
          /* No transition encodes 0 action, width 2 */
          if (column_widths[col] < 2) {
            column_widths[col] = 2;
          }
          col++;
        }

        dt = dn->outbound_;
        if (dt) {
          do {
            dt = dt->from_peer_;

            if (dt->is_anchor_) {
              while (col < (dt->symbol_start_ + 256)) {
                /* No transition encodes same state, width depends on max state ordinal. */
                int width_needed = 1;
                if (rex->dfa_.next_dfa_node_ordinal_ < 100) {
                  width_needed = 2;
                }
                else if (rex->dfa_.next_dfa_node_ordinal_ < 1000) {
                  width_needed = 3;
                }
                else if (rex->dfa_.next_dfa_node_ordinal_ < 10000) {
                  width_needed = 4;
                }
                else {
                  width_needed = 5;
                }
                if (width_needed > column_widths[col]) {
                  column_widths[col] = width_needed;
                }
                col++;
              }

              int width_needed = 1;
              int dst_ordinal = dt->to_->ordinal_;
              if (dst_ordinal < 100) {
                width_needed = 2;
              }
              else if (dst_ordinal < 1000) {
                width_needed = 3;
              }
              else if (dst_ordinal < 10000) {
                width_needed = 4;
              }
              else {
                width_needed = 5;
              }
              if (width_needed > column_widths[col]) {
                column_widths[col] = width_needed;
              }
              col++;
            }

          } while (dt != dn->outbound_);
        }

        while (col < (256 + 4)) {
          /* No transition encodes 0 action, width 2 */
          /* No transition encodes same state, width depends on max state ordinal. */
          int width_needed = 1;
          if (rex->dfa_.next_dfa_node_ordinal_ < 100) {
            width_needed = 2;
          }
          else if (rex->dfa_.next_dfa_node_ordinal_ < 1000) {
            width_needed = 3;
          }
          else if (rex->dfa_.next_dfa_node_ordinal_ < 10000) {
            width_needed = 4;
          }
          else {
            width_needed = 5;
          }
          if (width_needed > column_widths[col]) {
            column_widths[col] = width_needed;
          }
          col++;
        }

      } while (dn != rex->dfa_.nodes_);
    }

    /* Emit dummy "State 0" row */
    ip_force_indent_print(ip);
    for (col = 0; col < 256 + 4; ++col) {
      ip_printf(ip, "%s%*d", col ? "," : "", column_widths[col], 0);
    }
    if (dn) {
      ip_printf(ip, ",\n");
    }
    else {
      ip_printf(ip, "\n");
    }

    if (dn) {
      do {
        dn = dn->chain_;

        col = 0;

        ip_force_indent_print(ip);

        struct rex_dfa_trans *dt = dn->outbound_;
        if (dt) {
          do {
            dt = dt->from_peer_;

            if (!dt->is_anchor_) {
              while (col < dt->symbol_start_) {
                ip_printf(ip, "%s%*d", col ? "," : "", column_widths[col], 0);
                col++;
              }

              uint32_t input_sym;
              for (input_sym = dt->symbol_start_; input_sym < dt->symbol_end_; ++input_sym) {
                assert(col == input_sym);
                ip_printf(ip, "%s%*d", col ? "," : "", column_widths[col], dt->to_->ordinal_);
                col++;
              }
            }
          } while (dt != dn->outbound_);
        }
        
        while (col < 256) {
          ip_printf(ip, "%s%*d", col ? "," : "", column_widths[col], 0);
          col++;
        }

        if (dt) {
          do {
            dt = dt->from_peer_;

            if (dt->is_anchor_) {
              while (col < (256 + dt->symbol_start_)) {
                /* Aim towards own state, not an invalid transition, just one that has no effect */
                ip_printf(ip, "%s%*d", col ? "," : "", column_widths[col], dn->ordinal_);
                col++;
              }

              /* (Anchor transitions, unlike symbols, are never ranges..) */
              ip_printf(ip, "%s%*d", col ? "," : "", column_widths[col], dt->to_->ordinal_);
              col++;
            }
          } while (dt != dn->outbound_);
        }

        while (col < (256 + 4)) {
          ip_printf(ip, "%s%*d", col ? "," : "", column_widths[col], dn->ordinal_);
          col++;
        }

        if (dn != rex->dfa_.nodes_) {
          ip_printf(ip, ",\n");
        }
        else {
          ip_printf(ip, "\n");
        }
      } while (dn != rex->dfa_.nodes_);
    }

    ip_printf(ip, "};\n");

    ip_printf(ip, "static const size_t %sscan_actions_rex[] = { ", cc_prefix(cc));
    ip_printf(ip, "0"); /* dummy state 0 action */
    if (dn) {
      do {
        dn = dn->chain_;

        ip_printf(ip, ", %zu", (size_t)dn->pattern_matched_ ? dn->pattern_matched_->action_ : 0);

      } while (dn != rex->dfa_.nodes_);
    }
    ip_printf(ip, " };\n");

  }

  size_t num_columns;
  num_columns = (size_t)(1 + lalr->max_sym_ - lalr->min_sym_);
  ip_printf(ip, "static const int %sminimum_sym = %d;\n", cc_prefix(cc), lalr->min_sym_);
  ip_printf(ip, "static const size_t %snum_columns = %zu;\n", cc_prefix(cc), num_columns);
  ip_printf(ip, "static const size_t %snum_rows = %zu;\n", cc_prefix(cc), (size_t)lalr->nr_states_);
  ip_printf(ip, "static const size_t %snum_productions = %zu;\n", cc_prefix(cc), lalr->nr_productions_);
  ip_printf(ip, "static const int %sparse_table[] = {\n", cc_prefix(cc));
  size_t row, col;
  char *column_widths;
  column_widths = (char *)malloc(num_columns);
  if (!column_widths) {
    re_error_nowhere("Error, no memory\n");
    ip->had_error_ = 1;
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
    ip_force_indent_print(ip);
    for (col = 0; col < num_columns; ++col) {
      int action = lalr->parse_table_[row * num_columns + col];

      ip_printf(ip, "%*d,%s", column_widths[col], action, col == (num_columns - 1) ? "\n" : "");
    }
  }
  free(column_widths);
  ip_printf(ip, "};\n");
  ip_printf(ip, "static const size_t %sproduction_lengths[] = {\n", cc_prefix(cc));
  for (row = 0; row < lalr->nr_productions_; ++row) {
    ip_printf(ip, " %d%s\n", lalr->production_lengths_[row], (row == lalr->nr_productions_ - 1) ? "" : ",");
  }
  ip_printf(ip, "};\n");
  ip_printf(ip, "static const int %sproduction_syms[] = {\n", cc_prefix(cc));
  for (row = 0; row < lalr->nr_productions_; ++row) {
    ip_printf(ip, " %d%s\n", lalr->productions_[row][0], (row == lalr->nr_productions_ - 1) ? "" : ",");
  }
  ip_printf(ip, "};\n");

  state_syms = (int *)malloc(sizeof(int) * (size_t)lalr->nr_states_);
  if (!state_syms) {
    re_error_nowhere("Error, no memory");
    ip->had_error_ = 1;
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
            ip->had_error_ = 1;
            goto cleanup_exit;
          }
        }
      }
    }
  }

  if (cc->include_guard_) {
    ip_printf(ip, "\n#ifndef %s\n", cc->include_guard_);
  }

  emit_stack_struct_decl(ip, cc, prdg);

  emit_return_code_defines(ip, cc);
  ip_printf(ip, "\n");

  struct symbol *sym;

  sym = cc->symtab_.terminals_;
  if (sym) {
    do {
      sym = sym->next_;

      ip_printf(ip, "#define ");
      if (print_sym_as_c_ident(ip, cc, sym)) {
        ip->had_error_ = 1;
        goto cleanup_exit;
      }
      ip_printf(ip, " %d\n", sym->ordinal_);

    } while (sym != cc->symtab_.terminals_);
  }
  ip_printf(ip, "\n");
  sym = cc->symtab_.non_terminals_;
  if (sym) {
    do {
      sym = sym->next_;

      char *ident = (char *)malloc(1 + sym->def_.num_translated_);
      char *s = ident;
      const char *p;
      if (!ident) {
        ip->had_error_ = 1;
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

      ip_printf(ip, "#define %s%s %d\n", cc_PREFIX(cc), ident, sym->ordinal_);
      free(ident);
    } while (sym != cc->symtab_.non_terminals_);
  }
  ip_printf(ip, "\n");
  struct mode *m;
  m = cc->modetab_.modes_;
  if (m) {
    do {
      m = m->next_;

      char *ident = (char *)malloc(1 + m->def_.num_translated_);
      char *s = ident;
      const char *p;
      if (!ident) {
        ip->had_error_ = 1;
        goto cleanup_exit;
      }
      /* Transform into C identifier */
      for (p = m->def_.translated_; p < (m->def_.translated_ + m->def_.num_translated_); ++p) {
        int c = *p;
        if ((c >= 'a') && (c <= 'z')) c = c - 'a' + 'A';
        if (c == '-') c = '_';
        *s++ = c;
      }
      *s++ = '\0';

      ip_printf(ip, "#define M_%s%s %d\n", cc_PREFIX(cc), ident, m->rex_mode_->dfa_node_->ordinal_);
      free(ident);
    } while (m != cc->modetab_.modes_);
  }
  ip_printf(ip, "\n");
  if (cc->include_guard_) {
    ip_printf(ip, "#endif /* %s */\n\n", cc->include_guard_);
  }

  /* Emit stack constructor, destructor and reset functions */
  ip_printf(ip, "void %sstack_init(struct %sstack *stack) {\n", cc_prefix(cc), cc_prefix(cc));
  ip_printf(ip, "  stack->error_recovery_ = 0;\n"
                "  stack->pending_reset_ = 1;\n"
                "  stack->discard_remaining_actions_ = 0;\n");
  if (prdg->num_patterns_) {
    ip_printf(ip, "  stack->need_sym_ = 1;\n"
                  "  stack->current_sym_ = 0;\n");

    ip_printf(ip, "  stack->input_ = NULL;\n"
                  "  stack->input_size_ = 0;\n"
                  "  stack->is_final_input_ = 0;\n");
  }
  ip_printf(ip, "  stack->slot_1_has_sym_data_ = stack->slot_1_has_common_data_ = 0;\n"
                "  stack->slot_1_sym_ = 0;\n");
  ip_printf(ip, "  stack->top_of_stack_has_sym_data_ = stack->top_of_stack_has_common_data_ = 0;\n");
  ip_printf(ip, "  stack->continue_at_ = 0;\n");
  ip_printf(ip, "  stack->mute_error_turns_ = 0;\n"
                "  stack->pos_ = 0;\n"
                "  stack->num_stack_allocated_ = 0;\n"
                "  stack->stack_ = NULL;\n"
                "  stack->sym_data_ = NULL;\n"
                "  stack->current_production_length_ = 0;\n"
                "  stack->current_production_nonterminal_ = 0;\n");
  if (prdg->num_patterns_) {
    ip_printf(ip, "  stack->slot_0_has_current_sym_data_ = stack->slot_0_has_common_data_ = 0;\n");
    ip_printf(ip, "  stack->current_mode_start_state_ = M_%sDEFAULT;\n", cc_PREFIX(cc));
    ip_printf(ip, "  stack->scan_state_ = stack->current_mode_start_state_;\n");
    ip_printf(ip, "  stack->input_index_ = 0;\n"
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
    ip_printf(ip, "  stack->best_match_action_ = %zu;\n", 0);
    ip_printf(ip, "  stack->best_match_size_ = 0;\n"
                  "  stack->best_match_offset_ = 0;\n"
                  "  stack->best_match_line_ = 1;\n"
                  "  stack->best_match_col_ = 1;\n");
  }

  ip_printf(ip, "}\n"
                 "\n");


  cc->continuation_enabled_ = 0;
  ip_printf(ip, "void %sstack_cleanup(struct %sstack *stack) {\n", cc_prefix(cc), cc_prefix(cc));
  ip_printf(ip, "  size_t n;\n"
                "  for (n = 2; n < stack->pos_; ++n) {\n");
  ip_printf(ip, "    if ((n != (stack->pos_ - 1)) || stack->top_of_stack_has_sym_data_) {\n");
  ip_printf(ip, "    switch (stack->stack_[n].state_) {\n");
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
          ip_printf(ip, "    case %d: /* %s */\n", (int)state_idx, sym->def_.translated_);
          have_cases = 1;
        }
      }
      if (have_cases) {
        ip_printf(ip, "    {\n      ");
        if (emit_destructor_snippet_indexed_by_n(ip, cc, ts)) {
          ip->had_error_ = 1;
          goto cleanup_exit;
        }

        /* Close this compound block  */
        ip_printf(ip, "\n    }\n    break;\n");
      }
    }
  }


  ip_printf(ip, "    } /* switch */\n");
  ip_printf(ip, "    }\n");
  if (cc->common_data_assigned_type_ && cc->common_data_assigned_type_->destructor_snippet_.num_tokens_) {
    ip_printf(ip, "    if ((n != (stack->pos_ - 1)) || stack->top_of_stack_has_common_data_) {\n"
                  "      ");
    if (emit_common_destructor_snippet_indexed_by_n(ip, cc)) {
      ip->had_error_ = 1;
      goto cleanup_exit;
    }
    ip_printf(ip, "\n"
                  "    }\n");
  }

  ip_printf(ip, "  }\n");

  ip_printf(ip, "  if (stack->slot_1_has_sym_data_) {\n");
  ip_printf(ip, "    switch (stack->slot_1_sym_) {\n");

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
            ip_printf(ip, "      case %d:\n", sym->ordinal_);
          }

          sym = sym->next_;
        } while (sym != the_syms[n]);
      }
    }
    if (have_some) {
      /* Execute destructors for typestr ts at stack->stack_[0] */
      if (ts->destructor_snippet_.num_tokens_) {
        ip_printf(ip, "        {\n"
                      "          ");
        if (emit_destructor_snippet_indexed_by_1(ip, cc, ts)) {
          ip->had_error_ = 1;
          goto cleanup_exit;
        }
        ip_printf(ip, "\n"
                      "        }\n");
      }
      ip_printf(ip, "        break;\n");
    }
  }
  ip_printf(ip, "    } /* switch */\n");
  ip_printf(ip, "  }\n");

  if (cc->common_data_assigned_type_ && cc->common_data_assigned_type_->destructor_snippet_.num_tokens_) {
    ip_printf(ip, "  if (stack->slot_1_has_common_data_) {\n");
    if (emit_common_destructor_snippet_index_0(ip, cc)) {
      ip->had_error_ = 1;
      goto cleanup_exit;
    }
    ip_printf(ip, "\n"
                  "  }\n");
  }

  if (prdg->num_patterns_) {
    ip_printf(ip, "  if (stack->slot_0_has_current_sym_data_) {\n"
                  "    /* Deconstruct placeholder location for terminal not yet shifted */\n");
    ip_printf(ip, "    switch (stack->current_sym_) {\n");

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
              ip_printf(ip, "      case %d:\n", sym->ordinal_);
            }

            sym = sym->next_;
          } while (sym != the_syms[n]);
        }
      }
      if (have_some) {
        /* Execute destructors for typestr ts at stack->stack_[0] */
        if (ts->destructor_snippet_.num_tokens_) {
          ip_printf(ip, "        {\n"
                        "          ");
          if (emit_destructor_snippet_indexed_by_0(ip, cc, ts)) {
            ip->had_error_ = 1;
            goto cleanup_exit;
          }
          ip_printf(ip, "\n"
                        "        }\n");
                           
        }
        ip_printf(ip, "        break;\n");
      }
    }
    ip_printf(ip, "    } /* switch */\n");

    if (cc->common_data_assigned_type_ && cc->common_data_assigned_type_->destructor_snippet_.num_tokens_) {
      ip_printf(ip, "  if (stack->slot_0_has_common_data_) {\n");
      if (emit_common_destructor_snippet_index_0(ip, cc)) {
        ip->had_error_ = 1;
        goto cleanup_exit;
      }
      ip_printf(ip, "\n"
                    "    }\n");
    }
    ip_printf(ip, "  }\n");
  }


  ip_printf(ip, "\n"
                "  if (stack->stack_) free(stack->stack_);\n");
  if (prdg->num_patterns_) {
    ip_printf(ip, "  if (stack->match_buffer_) free(stack->match_buffer_);\n");
  }
  ip_printf(ip, "}\n"
                "\n");

  ip_printf(ip, "static int %spush_state(struct %sstack *stack, int state) {\n", cc_prefix(cc), cc_prefix(cc));
  ip_printf(ip, "  if (stack->num_stack_allocated_ == stack->pos_) {\n"
                "    size_t new_num_allocated = 0;\n"
                "    if (stack->num_stack_allocated_) {\n"
                "      new_num_allocated = stack->num_stack_allocated_ * 2;\n"
                "      if (new_num_allocated <= stack->num_stack_allocated_) {\n");
  ip_printf(ip, "        /* Overflow in allocation */\n"
                "        return _%sOVERFLOW;\n", cc_PREFIX(cc));
  ip_printf(ip, "      }\n"
                "    }\n"
                "    else {\n"
                "      new_num_allocated = 16;\n"
                "    }\n"
                "\n"
                "    if (new_num_allocated > (SIZE_MAX / sizeof(struct %ssym_data))) {\n", cc_prefix(cc));
  ip_printf(ip, "      /* Overflow in allocation */\n"
                "      return _%sOVERFLOW;\n", cc_PREFIX(cc));
  ip_printf(ip, "    }\n"
                "\n"
                "    void *p = realloc(stack->stack_, new_num_allocated * sizeof(struct %ssym_data));\n", cc_prefix(cc));
  ip_printf(ip, "    if (!p) {\n");
  ip_printf(ip, "      /* Out of memory */\n"
                "        return _%sNO_MEMORY;\n", cc_PREFIX(cc));
  ip_printf(ip, "    }\n"
                "    stack->stack_ = (struct %ssym_data *)p;\n", cc_prefix(cc));
  ip_printf(ip, "    stack->num_stack_allocated_ = new_num_allocated;\n"
                "  }\n"
                "  stack->stack_[stack->pos_++].state_ = state;\n"
                "  return 0;\n"
                "}\n"
                "\n");

  ip_printf(ip, "int %sstack_reset(struct %sstack *stack) {\n", cc_prefix(cc), cc_prefix(cc));
  ip_printf(ip, "  size_t n;\n"
                "  stack->pending_reset_ = 0;\n"
                "  stack->discard_remaining_actions_ = 0;\n"
                "  for (n = 2; n < stack->pos_; ++n) {\n");
  ip_printf(ip, "    if ((n != (stack->pos_ - 1)) || stack->top_of_stack_has_sym_data_) {\n");
  ip_printf(ip, "    switch (stack->stack_[n].state_) {\n");
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
          ip_printf(ip, "    case %d: /* %s */\n", (int)state_idx, sym->def_.translated_);
          have_cases = 1;
        }
      }
      if (have_cases) {
        ip_printf(ip, "    {\n    ");
        if (emit_destructor_snippet_indexed_by_n(ip, cc, ts)) {
          ip->had_error_ = 1;
          goto cleanup_exit;
        }

        ip_printf(ip, "\n"
                      "    }\n"
                      "    break;\n");
      }
    }
  }
  ip_printf(ip, "    } /* switch */\n");
  ip_printf(ip, "    }\n");
  if (cc->common_data_assigned_type_ && cc->common_data_assigned_type_->destructor_snippet_.num_tokens_) {
    ip_printf(ip, "    if ((n != (stack->pos_ - 1)) || stack->top_of_stack_has_common_data_) {\n"
                  "      ");
    if (emit_common_destructor_snippet_indexed_by_n(ip, cc)) {
      ip->had_error_ = 1;
      goto cleanup_exit;
    }
    ip_printf(ip, "\n"
                  "    }\n");
  }

  ip_printf(ip, "  }\n");

  ip_printf(ip, "  if (stack->slot_1_has_sym_data_) {\n");
  ip_printf(ip, "    switch (stack->slot_1_sym_) {\n");

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
            ip_printf(ip, "      case %d:\n", sym->ordinal_);
          }

          sym = sym->next_;
        } while (sym != the_syms[n]);
      }
    }
    if (have_some) {
      /* Execute destructors for typestr ts at stack->stack_[0] */
      if (ts->destructor_snippet_.num_tokens_) {
        ip_printf(ip, "        {\n"
                      "          ");
        if (emit_destructor_snippet_indexed_by_1(ip, cc, ts)) {
          ip->had_error_ = 1;
          goto cleanup_exit;
        }
        ip_printf(ip, "\n"
                      "        }\n");
      }
      ip_printf(ip, "        break;\n");
    }
  }
  ip_printf(ip, "    } /* switch */\n");
  ip_printf(ip, "  }\n");

  if (cc->common_data_assigned_type_ && cc->common_data_assigned_type_->destructor_snippet_.num_tokens_) {
    ip_printf(ip, "  if (stack->slot_1_has_common_data_) {\n");
    if (emit_common_destructor_snippet_index_1(ip, cc)) {
      ip->had_error_ = 1;
      goto cleanup_exit;
    }
    ip_printf(ip, "\n"
                  "  }\n");
  }

  ip_printf(ip, "  stack->slot_1_has_sym_data_ = stack->slot_1_has_common_data_ = 0;\n"
                "  stack->slot_1_sym_ = 0;\n");

  ip_printf(ip, "  stack->top_of_stack_has_sym_data_ = 0;\n"
                "  stack->top_of_stack_has_common_data_ = 0;\n");

  if (prdg->num_patterns_) {
    ip_printf(ip, "  if (stack->slot_0_has_current_sym_data_) {\n"
                  "    /* Deconstruct placeholder location for terminal not yet shifted */\n");
    ip_printf(ip, "    switch (stack->current_sym_) {\n");

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
              ip_printf(ip, "      case %d:\n", sym->ordinal_);
            }

            sym = sym->next_;
          } while (sym != the_syms[n]);
        }
      }
      if (have_some) {
        /* Execute destructors for typestr ts at stack->stack_[0] */
        if (ts->destructor_snippet_.num_tokens_) {
          ip_printf(ip, "        {\n"
                        "          ");
          if (emit_destructor_snippet_indexed_by_0(ip, cc, ts)) {
            ip->had_error_ = 1;
            goto cleanup_exit;
          }
          ip_printf(ip, "\n"
                        "        }\n");
        }
        ip_printf(ip, "        break;\n");
      }
    }
    ip_printf(ip, "    } /* switch */\n");

    if (cc->common_data_assigned_type_ && cc->common_data_assigned_type_->destructor_snippet_.num_tokens_) {
      ip_printf(ip, "  if (stack->slot_0_has_common_data_) {\n");
      if (emit_common_destructor_snippet_index_0(ip, cc)) {
        ip->had_error_ = 1;
        goto cleanup_exit;
      }
      ip_printf(ip, "\n"
                    "    }\n");
    }
    ip_printf(ip, "  }\n");

    ip_printf(ip, "  stack->slot_0_has_current_sym_data_ = stack->slot_0_has_common_data_ = 0;\n");
  }

  ip_printf(ip, "  stack->sym_data_ = NULL;\n"
                "  stack->current_production_length_ = 0;\n"
                "  stack->current_production_nonterminal_ = 0;\n");
  ip_printf(ip, "  stack->pos_ = 0;\n"
                "  stack->error_recovery_ = 0;\n");
  if (prdg->num_patterns_) {
    ip_printf(ip, "  stack->need_sym_ = 1;\n"
                  "  stack->current_sym_ = 0;\n");
  }
  ip_printf(ip, "  stack->continue_at_ = 0;\n");
  ip_printf(ip, "  stack->mute_error_turns_ = 0;\n");

  ip_printf(ip, "  /* reserve slot [0] for the token sym data */\n");
  ip_printf(ip, "  switch (%spush_state(stack, 0)) {\n"
                "    case _%sOVERFLOW:{\n", cc_prefix(cc), cc_PREFIX(cc));
  ip_printf(ip, "      return _%sOVERFLOW;\n", cc_PREFIX(cc));
  ip_printf(ip, "    }\n    break;\n"
                "    case _%sNO_MEMORY: {\n", cc_PREFIX(cc));
  ip_printf(ip, "      return _%sNO_MEMORY;\n", cc_PREFIX(cc));
  ip_printf(ip, "    }\n"
                "    break;\n"
                "  }\n");
  ip_printf(ip, "  /* reserve slot [1] for the initial state 0, corresponding to the synthetic S production\n"
                "  ** sym_data in slot [1] is used for temporary nonterminal data storage */\n");
  ip_printf(ip, "  switch (%spush_state(stack, 0)) {\n"
                "    case _%sOVERFLOW:{\n", cc_prefix(cc), cc_PREFIX(cc));
  ip_printf(ip, "      return _%sOVERFLOW;\n", cc_PREFIX(cc));
  ip_printf(ip, "    }\n    break;\n"
                "    case _%sNO_MEMORY: {\n", cc_PREFIX(cc));
  ip_printf(ip, "      return _%sNO_MEMORY;\n", cc_PREFIX(cc));
  ip_printf(ip, "    }\n"
                "    break;\n"
                "  }\n");

  if (prdg->num_patterns_) {
    ip_printf(ip, "  stack->scan_state_ = stack->current_mode_start_state_;\n");
    ip_printf(ip, "  stack->input_offset_ = 0;\n"
                  "  stack->input_line_ = 1;\n"
                  "  stack->input_col_ = 1;\n"
                  "  stack->match_index_ = 0;\n"
                  "  stack->match_buffer_size_ = 0;\n"
                  "  stack->terminator_repair_ = '\\0';\n"
                  "  stack->token_size_ = 0;\n"
                  "  stack->match_offset_ = 0;\n"
                  "  stack->match_line_ = 1;\n"
                  "  stack->match_col_ = 1;\n");
    ip_printf(ip, "  stack->best_match_action_ = %zu;\n", 0);
    ip_printf(ip, "  stack->best_match_size_ = 0;\n"
                  "  stack->best_match_offset_ = 0;\n"
                  "  stack->best_match_line_ = 1;\n"
                  "  stack->best_match_col_ = 1;\n");
  }

  ip_printf(ip, "  return 0;\n"
                "}\n"
                "\n");

  ip_printf(ip, "int %sstack_can_recover(struct %sstack *stack) {\n", cc_prefix(cc), cc_prefix(cc));
  ip_printf(ip, "  return !!stack->error_recovery_;\n");
  ip_printf(ip, "}\n");
  ip_printf(ip, "\n");
  ip_printf(ip, "int %sstack_accepts(struct %sstack *stack, int sym) {\n", cc_prefix(cc), cc_prefix(cc));
  ip_printf(ip, "  if (!stack->pos_) return 0;\n");
  ip_printf(ip, "  return 0 != %sparse_table[%snum_columns * stack->stack_[stack->pos_ - 1].state_ + (sym - %sminimum_sym)];", cc_prefix(cc), cc_prefix(cc), cc_prefix(cc));
  ip_printf(ip, "}\n");
  ip_printf(ip, "\n");

  cc->continuation_enabled_ = 1;

  if (prdg->num_patterns_) {
    ip_printf(ip, "void %sset_mode(struct %sstack *stack, int mode) {\n", cc_prefix(cc), cc_prefix(cc));
    ip_printf(ip, "  if (stack->current_mode_start_state_ == stack->scan_state_) {\n");
    ip_printf(ip, "    stack->scan_state_ = mode;\n");
    ip_printf(ip, "  }\n");
    ip_printf(ip, "  stack->current_mode_start_state_ = mode;\n");
    ip_printf(ip, "}\n");
    emit_lex_function(ip, cc, prdg);
    ip_printf(ip, "\n");
    emit_scan_function(ip, cc, prdg, lalr, state_syms);
    ip_printf(ip, "\n");
  }

  emit_parse_function(ip, cc, prdg, lalr, state_syms);

  ip_printf(ip, "/* --------- END OF GENERATED CODE ------------ */\n");

  pt = cc->epilogue_;
  if (pt) {
    do {
      pt = pt->next_;

      ip_write_no_indent(ip, pt->chars_, pt->num_chars_);

    } while (pt != cc->epilogue_);
  }


cleanup_exit:
  if (state_syms) free(state_syms);
}


void emit_h_file(struct indented_printer *ip, struct carburetta_context *cc, struct prd_grammar *prdg) {
  struct symbol *sym;
  ip_printf(ip, "#ifndef %s\n"
                 "#define %s\n"
                 "\n", cc->include_guard_, cc->include_guard_);

  ip_printf(ip, "#include <stddef.h> /* size_t */\n"
                 "\n");

  ip_printf(ip, "#ifdef __cplusplus\n"
                 "extern \"C\" {\n"
                 "#endif\n"
                 "\n");

  emit_return_code_defines(ip, cc);
  ip_printf(ip, "\n");

  sym = cc->symtab_.terminals_;
  if (sym) {
    do {
      sym = sym->next_;

      ip_printf(ip, "#define ");
      if (print_sym_as_c_ident(ip, cc, sym)) {
        ip->had_error_ = 1;
        goto cleanup_exit;
      }
      ip_printf(ip, " %d\n", sym->ordinal_);

    } while (sym != cc->symtab_.terminals_);
  }
  ip_printf(ip, "\n");
  sym = cc->symtab_.non_terminals_;
  if (sym) {
    do {
      sym = sym->next_;

      char *ident = (char *)malloc(1 + sym->def_.num_translated_);
      char *s = ident;
      const char *p;
      if (!ident) {
        ip->had_error_ = 1;
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

      ip_printf(ip, "#define %s%s %d\n", cc_PREFIX(cc), ident, sym->ordinal_);
      free(ident);
    } while (sym != cc->symtab_.non_terminals_);
  }

  ip_printf(ip, "\n");

  struct mode *m;
  m = cc->modetab_.modes_;
  if (m) {
    do {
      m = m->next_;

      char *ident = (char *)malloc(1 + m->def_.num_translated_);
      char *s = ident;
      const char *p;
      if (!ident) {
        ip->had_error_ = 1;
        goto cleanup_exit;
      }
      /* Transform into C identifier */
      for (p = m->def_.translated_; p < (m->def_.translated_ + m->def_.num_translated_); ++p) {
        int c = *p;
        if ((c >= 'a') && (c <= 'z')) c = c - 'a' + 'A';
        if (c == '-') c = '_';
        *s++ = c;
      }
      *s++ = '\0';

      ip_printf(ip, "#define M_%s%s %d\n", cc_PREFIX(cc), ident, m->rex_mode_->dfa_node_->ordinal_);
      free(ident);
    } while (m != cc->modetab_.modes_);
  }

  ip_printf(ip, "\n");

  emit_stack_struct_decl(ip, cc, prdg);

  ip_printf(ip, "\n");
  ip_printf(ip, "void %sstack_init(struct %sstack *stack);\n", cc_prefix(cc), cc_prefix(cc));
  ip_printf(ip, "void %sstack_cleanup(struct %sstack *stack);\n", cc_prefix(cc), cc_prefix(cc));
  ip_printf(ip, "int %sstack_reset(struct %sstack *stack);\n", cc_prefix(cc), cc_prefix(cc));
  ip_printf(ip, "int %sstack_can_recover(struct %sstack *stack);\n", cc_prefix(cc), cc_prefix(cc));
  ip_printf(ip, "int %sstack_accepts(struct %sstack *stack, int sym);\n", cc_prefix(cc), cc_prefix(cc));
  if (prdg->num_patterns_) {
    ip_printf(ip, "void %sset_mode(struct %sstack *stack, int mode);", cc_prefix(cc), cc_prefix(cc));
    ip_printf(ip, "void %sset_input(struct %sstack *stack, const char *input, size_t input_size, int is_final_input);\n", cc_prefix(cc), cc_prefix(cc));

    if (cc->params_snippet_.num_tokens_) {
      ip_printf(ip, "int %sscan(struct %sstack *stack, ", cc_prefix(cc), cc_prefix(cc));
      size_t token_idx;
      for (token_idx = 0; token_idx < cc->params_snippet_.num_tokens_; ++token_idx) {
        ip_printf(ip, "%s", cc->params_snippet_.tokens_[token_idx].text_.original_);
      }
      ip_printf(ip, ");\n");
    }
    else {
      ip_printf(ip, "int %sscanstruct %sstack *stack);\n", cc_prefix(cc), cc_prefix(cc));
    }

    ip_printf(ip, "void %sset_location(struct %sstack *stack, int line, int col, size_t offset);\n", cc_prefix(cc), cc_prefix(cc));
    ip_printf(ip, "const char *%stext(struct %sstack *stack);\n", cc_prefix(cc), cc_prefix(cc));
    ip_printf(ip, "size_t %slen(struct %sstack *stack);\n", cc_prefix(cc), cc_prefix(cc));

    ip_printf(ip, "int %sline(struct %sstack *stack);\n", cc_prefix(cc), cc_prefix(cc));
    ip_printf(ip, "int %scolumn(struct %sstack *stack);\n", cc_prefix(cc), cc_prefix(cc));
    ip_printf(ip, "size_t %soffset(struct %sstack *stack);\n", cc_prefix(cc), cc_prefix(cc));
    ip_printf(ip, "int %sendline(struct %sstack *stack);\n", cc_prefix(cc), cc_prefix(cc));
    ip_printf(ip, "int %sendcolumn(struct %sstack *stack);\n", cc_prefix(cc), cc_prefix(cc));
    ip_printf(ip, "size_t %sendoffset(struct %sstack *stack);\n", cc_prefix(cc), cc_prefix(cc));
    ip_printf(ip, "int %slex(struct %sstack *stack);\n", cc_prefix(cc), cc_prefix(cc));

  }

  if (cc->params_snippet_.num_tokens_) {
    ip_printf(ip, "int %sparse(struct %sstack *stack, int sym, ", cc_prefix(cc), cc_prefix(cc));
    size_t token_idx;
    for (token_idx = 0; token_idx < cc->params_snippet_.num_tokens_; ++token_idx) {
      ip_printf(ip, "%s", cc->params_snippet_.tokens_[token_idx].text_.original_);
    }
    ip_printf(ip, ");\n");
  }
  else {
    ip_printf(ip, "int %sparse(struct %sstack *stack, int sym);\n", cc_prefix(cc), cc_prefix(cc));
  }

  ip_printf(ip, "\n"
                "#ifdef __cplusplus\n"
                "} /* extern \"C\" */\n"
                "#endif\n");

  ip_printf(ip, "\n"
                "#endif /* %s */\n", cc->include_guard_);

cleanup_exit:
  ;
}
