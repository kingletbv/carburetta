/* Copyright 2020-2023 Kinglet B.V.
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

#ifndef STRING_H_INCLUDED
#define STRING_H_INCLUDED
#include <string.h>
#endif

#ifndef INTTYPES_H_INCLUDED
#define INTTYPES_H_INCLUDED
#include <inttypes.h>
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

#ifndef REPORT_ERROR_H_INCLUDED
#define REPORT_ERROR_H_INCLUDED
#include "report_error.h"
#endif

#ifndef REX_H_INCLUDED
#define REX_H_INCLUDED
#include "rex.h"
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

#ifndef PARSE_INPUT_H_INCLUDED
#define PARSE_INPUT_H_INCLUDED
#include "parse_input.h"
#endif

#ifndef TEMP_OUTPUT_H_INCLUDED
#define TEMP_OUTPUT_H_INCLUDED
#include "temp_output.h"
#endif

#ifndef INDENTED_PRINTER_H_INCLUDED
#define INDENTED_PRINTER_H_INCLUDED
#include "indented_printer.h"
#endif


/* Following globals control the output file. If we fail execution (eg. user error in the input or an
 * unforeseen problem internally) then we wish to retain whatever output was already there.
 * Consequently, we write output to a temp file, and only rename it to the final file if execution
 * completes successfully. An atexit() handler guards the case where we fail early, and deletes the
 * created temp file.
 */

char *g_temp_output_filename_ = NULL;
FILE *g_temp_output_file = NULL;

void at_exit_delete_temp_output_handler(void) {
  if (g_temp_output_file) {
    fclose(g_temp_output_file);
    g_temp_output_file = NULL;
  }
  if (g_temp_output_filename_) {
    remove(g_temp_output_filename_);
    free(g_temp_output_filename_);
    g_temp_output_filename_ = NULL;
  }
}

void print_dbg_char(FILE *fp, int c) {
  if ((c >= 0) && (c <= 255) && isprint(c) && (c != '\\') && (c != '\'') && (c != '\"')) {
    printf("%c", c);
    return;
  }
  switch (c) {
  case '\a': printf("\\a"); break;
  case '\b': printf("\\b"); break;
  case '\f': printf("\\f"); break;
  case '\n': printf("\\n"); break;
  case '\r': printf("\\r"); break;
  case '\t': printf("\\t"); break;
  case '\v': printf("\\v"); break;
  case '\\': printf("\\\\"); break;
  case '\'': printf("\\'"); break;
  case '\"': printf("\\\""); break;
  case '\?': printf("\\?"); break;
  default:
    if (c <= 0xFFFF) {
      printf("\\u%04x", c);
    }
    else {
      printf("\\U%08x", c);
    }
  }
}

void print_usage(void) {
  fprintf(stderr, "Carburetta parser generator (C) 2020-2023 Kinglet B.V.\n"
                  "version 0.8.9\n"
                  "https://carburetta.com/\n"
                  "\n"
                  "carburetta <inputfile.cbrt> [ --c [ <c_filename> [--h [h_filename] ] ] ]\n"
                  "\n"
                  "<inputfile.cbrt>\n"
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
                  "--x-utf8\n"
                  "         use UTF-8 encoding for the input file. This is experimental and will\n"
                  "         generate a parser that reads the input file as UTF-8.\n"
  );
}

int main(int argc, char **argv) {
  int r;

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

  struct carburetta_context cc;
  carburetta_context_init(&cc);

  struct prd_grammar prdg;
  prd_grammar_init(&prdg);

  struct grammar_table gt;
  gt_grammar_table_init(&gt);

  struct lr_generator lalr;
  lr_init(&lalr);

  struct rex_scanner rex;
  rex_init(&rex);

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
    else if (!strcmp("--x-utf8", *cpv)) {
      cr--; cpv++;
      cc.utf8_experimental_ = 1;
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

  FILE *fp = NULL;
  if (input_filename) {
    fp = fopen(input_filename, "rb");
    if (!fp) {
      int err = errno;
      re_error_nowhere("Failed to open file \"%s\": %s", input_filename, strerror(err));
      r = EXIT_FAILURE;
      goto cleanup_exit;
    }
    fprintf(stderr, "%s\n", input_filename);
  }
  else {
    fp = stdin;
  }

  r = pi_parse_input(fp, input_filename, &cc, &prdg);
  if (r) {
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
  int NT_END, RULE_END, GRAMMAR_END;
  NT_END = 0;
  RULE_END = 1;
  GRAMMAR_END = 2;
  int next_ordinal;
  next_ordinal = 3;
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
  int INPUT_END;
  INPUT_END = cc.input_end_sym_->ordinal_;
  sym = cc.symtab_.non_terminals_;
  if (sym) {
    do {
      sym = sym->next_;

      sym->ordinal_ = next_ordinal++;
    } while (sym != cc.symtab_.non_terminals_);
  }
  int SYNTHETIC_S;
  SYNTHETIC_S = next_ordinal++;

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

  //gt_debug_grammar(&gt, prdg.num_productions_, prdg.productions_, RULE_END, GRAMMAR_END);

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

        char *msg = (char *)malloc(msg_size);
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

  struct mode *default_mode;
  struct xlts default_keyword;
  xlts_init(&default_keyword);
  xlts_append_xlat(&default_keyword, strlen("default"), "default");
  int is_default_new = -1;
  default_mode = mode_find_or_add(&cc.modetab_, &default_keyword, &is_default_new);
  xlts_cleanup(&default_keyword);
  if (!default_mode) {
    re_error_nowhere("Error, no memory");
    r = EXIT_FAILURE;
    goto cleanup_exit;
  }
  if (!is_default_new) {
    re_error(&default_mode->def_, "Error, \"default\" mode is implicit and should not be explicitly declared");
    r = EXIT_FAILURE;
    goto cleanup_exit;
  }
  /* Make sure that the "default" mode gets the first rex_mode allocation is
   * this has consequences for the order of the states in the final table */
  r = rex_add_mode(&rex, &default_mode->rex_mode_);
  if (r) {
    switch (r) {
    case _REX_NO_MEMORY:
      re_error_nowhere("Error, no memory");
      r = EXIT_FAILURE;
      goto cleanup_exit;
    default:
      /* All errors here are internal */
      re_error_nowhere("Internal error");
      r = EXIT_FAILURE;
      goto cleanup_exit;
    }
  }

  struct mode *m;
  m = cc.modetab_.modes_;
  if (m) {
    do {
      m = m->next_;

      if (!m->rex_mode_) {
        r = rex_add_mode(&rex, &m->rex_mode_);
        if (r) {
          switch (r) {
          case _REX_NO_MEMORY:
            re_error_nowhere("Error, no memory");
            r = EXIT_FAILURE;
            goto cleanup_exit;
          default:
            /* All errors here are internal */
            re_error_nowhere("Internal error");
            r = EXIT_FAILURE;
            goto cleanup_exit;
          }
        }
      }
    } while (m != cc.modetab_.modes_);
  }

  if (prdg.num_patterns_) {
    size_t n;
    for (n = 0; n < prdg.num_patterns_; ++n) {
      struct prd_pattern *prd_pat = prdg.patterns_ + n;
      r = rex_add_pattern(&rex, prd_pat->regex_, n + 1, &prd_pat->pat_);
      if (r) {
        /* Failure occurred, report */
        switch (r) {
        case _REX_NO_MEMORY:
          re_error_nowhere("Error, no memory");
          r = EXIT_FAILURE;
          goto cleanup_exit;
        case _REX_SYNTAX_ERROR:
        case _REX_LEXICAL_ERROR:
          /* The regex should already be validated to be
          * a correct regular expression (otherwise */
          re_error_nowhere("Internal error, inconsistent syntax");
          r = EXIT_FAILURE;
          goto cleanup_exit;
        default:
          /* All errors here are internal */
          re_error_nowhere("Internal error");
          r = EXIT_FAILURE;
          goto cleanup_exit;
        }
      }
    }
  }

  size_t mode_group_idx;
  int have_error = 0;
  for (mode_group_idx = 0; mode_group_idx < prdg.num_mode_groups_; ++mode_group_idx) {
    struct prd_mode_group *mg = prdg.mode_groups_ + mode_group_idx;
    size_t mode_group_mode_idx;
    for (mode_group_mode_idx = 0; mode_group_mode_idx < mg->num_modes_; ++mode_group_mode_idx) {
      struct prd_mode *md = mg->modes_ + mode_group_mode_idx;
      struct mode *m = mode_find(&cc.modetab_, md->id_.translated_);
      if (!m) {
        re_error(&md->id_, "Error, mode \"%s\" not declared using %%mode", md->id_.translated_);
        have_error = 1;
      }
      else {
        size_t n;
        for (n = mg->pattern_start_index_; n < mg->pattern_end_index_; ++n) {
          struct prd_pattern *prd_pat = prdg.patterns_ + n;
          r = rex_add_pattern_to_mode(m->rex_mode_, prd_pat->pat_);
          prd_pat->touched_by_mode_ = 1;
          if (r) {
            switch (r) {
            case _REX_NO_MEMORY:
              re_error_nowhere("Error, no memory");
              r = EXIT_FAILURE;
              goto cleanup_exit;
            default:
              /* All errors here are internal */
              re_error_nowhere("Internal error");
              r = EXIT_FAILURE;
              goto cleanup_exit;
            }
          }
        }
      }
    }
  }
  if (have_error) {
    r = EXIT_FAILURE;
    goto cleanup_exit;
  }
  /* Add any untouched patterns to the default mode */
  size_t pattern_index;
  for (pattern_index = 0; pattern_index < prdg.num_patterns_; ++pattern_index) {
    struct prd_pattern *pat = prdg.patterns_ + pattern_index;
    if (!pat->touched_by_mode_) {
      r = rex_add_pattern_to_mode(default_mode->rex_mode_, pat->pat_);
      if (r) {
        switch (r) {
        case _REX_NO_MEMORY:
          re_error_nowhere("Error, no memory");
          r = EXIT_FAILURE;
          goto cleanup_exit;
        default:
          /* All errors here are internal */
          re_error_nowhere("Internal error");
          r = EXIT_FAILURE;
          goto cleanup_exit;
        }
      }
    }
  }

  if (prdg.num_patterns_) {
    r = rex_realize_modes(&rex);
    if (!r) {
      r = rex_dfa_make_symbol_groups(&rex.dfa_);
    }
    if (r) {
      switch (r) {
      case _REX_NO_MEMORY:
        re_error_nowhere("Error, no memory");
        r = EXIT_FAILURE;
        goto cleanup_exit;
      default:
        /* All errors here are internal */
        re_error_nowhere("Internal error");
        r = EXIT_FAILURE;
        goto cleanup_exit;
      }
    }

  }

  FILE *outfp;
  outfp = NULL;

  g_temp_output_filename_ = NULL;
  g_temp_output_file = NULL;
  
  /* Set the exit handler to delete any temp file we have created.. This helps to cleanup in case of failure. */
  atexit(at_exit_delete_temp_output_handler);

  if (generate_cfile) {
    if (cc.c_output_filename_) {
      outfp = to_make_temp(cc.c_output_filename_, &g_temp_output_filename_);
      if (!outfp) {
        int err = errno;
        re_error_nowhere("Failed to open file \"%s\" for writing: %s", cc.c_output_filename_, strerror(err));
        r = EXIT_FAILURE;
        goto cleanup_exit;
      }
      g_temp_output_file = outfp;
    }
    else {
      outfp = stdout;
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

    struct indented_printer ip;
    ip_init(&ip, outfp, cc.c_output_filename_);

    emit_c_file(&ip, &cc, &prdg, &rex, &lalr);

    if (ip.had_error_) {
      r = EXIT_FAILURE;
      ip_cleanup(&ip);
      goto cleanup_exit;
    }
    ip_cleanup(&ip);

    if (r) goto cleanup_exit;

    if (outfp != stdout) {
      fclose(outfp);
      g_temp_output_file = NULL;

      /* don't care if this fails, file likely does not exist. */
      remove(cc.c_output_filename_);

      if (rename(g_temp_output_filename_, cc.c_output_filename_)) {
        int err = errno;
        re_error_nowhere("Failed to complete output to file \"%s\": %s", cc.c_output_filename_, strerror(err));
        r = EXIT_FAILURE;
        goto cleanup_exit;
      }

      free(g_temp_output_filename_);
      g_temp_output_filename_ = NULL;
    }
  } /* generate_cfile */

  if (generate_hfile) {
    if (cc.h_output_filename_) {
      outfp = to_make_temp(cc.h_output_filename_, &g_temp_output_filename_);
      if (!outfp) {
        int err = errno;
        re_error_nowhere("Error, failed to open file \"%s\" for writing: %s", cc.h_output_filename_, strerror(err));
        r = EXIT_FAILURE;
        goto cleanup_exit;
      }
      g_temp_output_file = outfp;
    }
    else {
      re_error_nowhere("Error, generating header file requires output filename");
      r = EXIT_FAILURE;
      goto cleanup_exit;
    }

    struct indented_printer ip;
    ip_init(&ip, outfp, cc.c_output_filename_);

    emit_h_file(&ip, &cc, &prdg);

    if (ip.had_error_) {
      r = EXIT_FAILURE;
      ip_cleanup(&ip);
      goto cleanup_exit;
    }
    ip_cleanup(&ip);

    if (outfp != stdout) {
      fclose(outfp);
      g_temp_output_file = NULL;

      /* don't care if this fails, file likely does not exist. */
      remove(cc.h_output_filename_);

      if (rename(g_temp_output_filename_, cc.h_output_filename_)) {
        int err = errno;
        re_error_nowhere("Failed to complete output to file \"%s\": %s", cc.h_output_filename_, strerror(err));
        r = EXIT_FAILURE;
        goto cleanup_exit;
      }

      free(g_temp_output_filename_);
      g_temp_output_filename_ = NULL;
    }
  }


  r = EXIT_SUCCESS;
cleanup_exit:
  lr_cleanup(&lalr);

  rex_cleanup(&rex);

  gt_grammar_table_cleanup(&gt);

  carburetta_context_cleanup(&cc);

  prd_grammar_cleanup(&prdg);

  dct_cleanup();
  las_cleanup();
  tok_cleanup();
  ldl_cleanup();

  if (input_filename) free(input_filename);

  return r;
}
