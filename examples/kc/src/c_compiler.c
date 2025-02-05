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

#ifndef STDLIB_H_INCLUDED
#define STDLIB_H_INCLUDED
#include <stdlib.h>
#endif

#ifndef STDIO_H_INCLUDED
#define STDIO_H_INCLUDED
#include <stdio.h>
#endif

#ifndef STRING_H_INCLUDED
#define STRING_H_INCLUDED
#include <string.h>
#endif

#ifndef ASSERT_H_INCLUDED
#define ASSERT_H_INCLUDED
#include <assert.h>
#endif

#ifndef PP_TOKENS_H_INCLUDED
#define PP_TOKENS_H_INCLUDED
#include "pp_tokens.h"
#endif

#ifndef PP_TOKENIZER_H_INCLUDED
#define PP_TOKENIZER_H_INCLUDED
#include "pp_tokenizer.h"
#endif

#ifndef PP_LINE_DIRECTIVES_H_INCLUDED
#define PP_LINE_DIRECTIVES_H_INCLUDED
#include "pp_line_directives.h"
#endif

#ifndef PP_MACRO_EXPANDER_H_INCLUDED
#define PP_MACRO_EXPANDER_H_INCLUDED
#include "pp_macro_expander.h"
#endif

#ifndef INPUT_FILE_H_INCLUDED
#define INPUT_FILE_H_INCLUDED
#include "input_file.h"
#endif

#ifndef C_COMPILER_H_INCLUDED
#define C_COMPILER_H_INCLUDED
#include "c_compiler.h"
#endif

#ifndef SITUS_H_INCLUDED
#define SITUS_H_INCLUDED
#include "situs.h"
#endif

#define CC_FILE_FP_BUFFER_SIZE 16834

static int cc_template_default_handler(struct c_compiler *cc, char *templ_str, size_t templ_str_len, struct stmt **new_stmt, struct situs *location);

static int cc_stderr_vprintf_handler(void *baton, const char *file, int line_num, const char *fmt, va_list args) {
  if (file) {
    fprintf(stderr, "%s(%d): ", file, line_num);
  }

  int r;
  r = vfprintf(stderr, fmt, args);

  return r < 0;
}

int cc_printf(struct c_compiler *cc, const char *fmt, ...) {
  int r;
  va_list args;
  va_start(args, fmt);
  r = cc->vprintf_handler(cc->vprintf_baton_, NULL, 0, fmt, args);
  va_end(args);
  return r;
}

static struct input_file *cc_push_input_file(struct c_compiler *cc, const char *filename, size_t aux_data) {
  char *sf = cc_preserve_filename(cc, filename);
  if (!sf) return NULL;
  struct input_file *ifile = (struct input_file *)malloc(sizeof(struct input_file) + aux_data);
  if (!ifile) {
    free(sf);
    return NULL;
  }
  if_init(ifile, cc);
  ifile->filename_ = sf;
  ifile->ppld_input_line_ = 0;
  ifile->ppld_input_line_filename_ = NULL;
  ifile->pptk_input_line_ = 1;
  ifile->pptk_input_line_filename_ = sf;
  return ifile;
}

static void cc_pop_input_file(struct c_compiler *cc) {
  struct input_file *ifile = cc->input_stack_;
  if (ifile) {
    if_cleanup(ifile);
    free(ifile);
  }
}

static char *g_cc_default_handler_fn_name_ = "__kc_template_handler";

void cc_init(struct c_compiler *cc) {
  cc->mode_ = CC_SCAN;
  cc->mode_set_ = 0;
  cc->asm_decoration_char_ = '$';

  cc->ppme_input_final_ = 0;
  cc->ppme_input_ = NULL;
  cc->ppme_input_file_ = NULL;
  cc->ppme_input_line_ = 0;
  ppme_stack_init(&cc->ppme_);

  cc->ppld_input_final_ = 0;
  cc->have_ppld_input_line_ = 0;
  cc->ppld_input_ = NULL;

  ns_init(&cc->global_ns_);

  cc->all_done_ = 0;
  cc->have_error_ = 0;
  cc->fatal_error_ = 0;

  cc->cp_input_final_ = 0;
  cc->cp_input_ = NULL;
  cc->cp_input_file_ = "";
  cc->cp_input_line_ = 0;
  templ_context_init(&cc->ctx_);
  cp_stack_init(&cc->cp_);

  cc->if_section_stack_ = NULL;

  cc->input_stack_ = NULL;

  type_base_init(&cc->tb_);
  ds_init(&cc->ds_);
  st_init(&cc->macro_table_);
  int is_new = 0;
  static const char *builtins[] = {
    "__FILE__", "__LINE__"
  };
  size_t n;
  for (n = 0; n < sizeof(builtins) / sizeof(builtins[0]); ++n) {
    struct macro *m = (struct macro *)st_find_or_insert(&cc->macro_table_, builtins[n], &is_new, sizeof(struct macro));
    if (!m) {
      cc_no_memory(cc);
      return;
    }
    m->refcount_ = 1; /* the macro_table references it */
    m->is_function_style_ = 0;
    m->is_variadic_ = 0;
    m->nested_invocation_ = 0;
    m->args_ = NULL;
    m->replacement_list_ = NULL;
  }


  cc->vprintf_handler = cc_stderr_vprintf_handler;
  cc->vprintf_baton_ = NULL;

  cc->filename_buffers_ = NULL;

  cc->pp_include_is_system_include_ = 0;
  cc->pp_include_is_template_emit_ = 0;
  cc->pp_include_file_arg_ = NULL;

  cc->template_handler_ = cc_template_default_handler;
  cc->default_handler_fn_name_ = g_cc_default_handler_fn_name_;
  st_init(&cc->link_table_);
}

void cc_cleanup(struct c_compiler *cc) {
  while (cc->macro_table_.root_) {
    struct macro *m = (struct macro *)cc->macro_table_.root_;
    st_remove(&cc->macro_table_, &m->sym_);
    macro_free(m);
  }
  st_cleanup(&cc->macro_table_);
  ds_cleanup(&cc->ds_);
  type_base_cleanup(&cc->tb_);
  while (cc->input_stack_) cc_pop_input_file(cc);
  while (cc->if_section_stack_) cc_if_pop(cc);
  cp_stack_cleanup(&cc->cp_);
  templ_context_cleanup(&cc->ctx_);

  pptk_free(cc->cp_input_);
  
  /* XXX: ns_cleanup does not free the symbols, only detach them, as symtab does not understand the derived datastructures */
  ns_cleanup(&cc->global_ns_);

  pptk_free(cc->ppld_input_);

  ppme_stack_cleanup(&cc->ppme_);

  pptk_free(cc->ppme_input_);

  while (cc->filename_buffers_) {
    struct cc_filename_buffer *fb = cc->filename_buffers_;
    cc->filename_buffers_ = fb->next_;
    free(fb);
  }

  if (cc->pp_include_file_arg_) {
    free(cc->pp_include_file_arg_);
  }

  if (cc->default_handler_fn_name_ != g_cc_default_handler_fn_name_) {
    free(cc->default_handler_fn_name_);
  }
  while (cc->link_table_.root_) {
    struct cc_link_name *name = (struct cc_link_name *)cc->link_table_.root_;
    st_remove(&cc->link_table_, cc->link_table_.root_);
    free(name);
  }

  st_cleanup(&cc->link_table_);
}

char *cc_preserve_filename(struct c_compiler *cc, const char *filename) {
  size_t len = strlen(filename);
  size_t size_needed = len + sizeof(struct cc_filename_buffer /* includes 1 byte sized array, hence len does not need to include null-terminator */);
  if (size_needed <= len) {
    return NULL; /* overflow */
  }
  struct cc_filename_buffer *fb = (struct cc_filename_buffer *)malloc(size_needed);
  if (!fb) return NULL;
  fb->next_ = cc->filename_buffers_;
  cc->filename_buffers_ = fb;
  memcpy(fb->filename_, filename, len);
  fb->filename_[len] = '\0';
  return fb->filename_;
}

static int cc_file_fp_input_request_callback(void *baton, struct input_file *ifile) {
  size_t num_bytes_read;
  num_bytes_read = fread(ifile->aux_, 1, CC_FILE_FP_BUFFER_SIZE, ifile->fp_);
  if (num_bytes_read != CC_FILE_FP_BUFFER_SIZE) {
    if (ferror(ifile->fp_)) {
      return -1; /* an error occurred */
    }
  }
  if_set_input(ifile, ifile->aux_, num_bytes_read, feof(ifile->fp_));
  return 0;
}

struct input_file *cc_push_input_file_fp(struct c_compiler *cc, const char *filename, FILE *fp) {
  struct input_file *ifile = cc_push_input_file(cc, filename, CC_FILE_FP_BUFFER_SIZE /* aux space for memory reads */);
  if (!ifile) return NULL;
  ifile->fp_ = fp;
  ifile->input_request_fn_ = cc_file_fp_input_request_callback;
  return ifile;
}

struct mem_input_file {
  struct input_file if_;
  const void *mem_;
  size_t memsize_;
};

static int cc_file_mem_input_request_callback(void *baton, struct input_file *ifile) {
  struct mem_input_file *mif = (struct mem_input_file *)ifile;
  if_set_input(ifile, mif->mem_, mif->memsize_, 1);
  return 0;
}

struct input_file *cc_push_input_file_mem(struct c_compiler *cc, const char *filename, const void *mem, size_t memsize) {
  struct mem_input_file *ifile = (struct mem_input_file *)cc_push_input_file(cc, filename, sizeof(struct mem_input_file) - sizeof(struct input_file));
  ifile->if_.input_request_fn_ = cc_file_mem_input_request_callback;
  ifile->mem_ = mem;
  ifile->memsize_ = memsize;
  return &ifile->if_;
}

struct cc_if_section *cc_if_push(struct c_compiler *cc) {
  struct cc_if_section *ldifs = (struct cc_if_section *)malloc(sizeof(struct cc_if_section));
  if (!ldifs) return NULL;
  ldifs->parent_ = cc->if_section_stack_;
  cc->if_section_stack_ = ldifs;
  ldifs->state_ = CC_IFSS_SKIP;
  return ldifs;
}

void cc_if_pop(struct c_compiler *cc) {
  struct cc_if_section *ldifs = cc->if_section_stack_;
  cc->if_section_stack_ = ldifs->parent_;
  free(ldifs);
}

enum c_compiler_result cc_preprocessor_stage(struct c_compiler *cc) {
  macro_expander:
  if (cc->ppme_input_ || cc->ppme_input_final_) {
    int r;
    int next_sym;
    if (cc->ppme_input_) {
      next_sym= g_pptk_to_ppme_[cc->ppme_input_->tok_];
    }
    else {
      next_sym = PPME_INPUT_END;
    }
    if (next_sym == PPME_IDENT) {
      struct macro *m = (struct macro *)st_find(&cc->macro_table_, cc->ppme_input_->text_);

      if (m) {
        if (m->is_function_style_) {
          next_sym = PPME_FUNCTION;
        }
        else {
          next_sym = PPME_OBJECT;
        }
      }
    }
    r = ppme_parse(&cc->ppme_, next_sym, cc, &cc->ppme_input_, cc->ppme_input_final_, &cc->cp_input_);
    switch (r) {
      case _PPME_FINISH:
        cc->cp_input_final_ = 1;
        /* Preprocessor is done */
        return CCR_SUCCESS;

      case _PPME_FEED_ME:
        /* Need more input; however, if we have a bit of output ready for the C parser, process that first so
         * we minimize the number of tokens concurrently in-flight. */
        if (cc->cp_input_) {
          return CCR_SUCCESS;
        }
        break; /* get more input .. */

      case _PPME_SYNTAX_ERROR:
        if (cc->ppme_input_) {
          cc_printf(cc, "%s(%d): syntax error at column %d: \"%s\"\n", cc->ppme_input_file_, situs_line(&cc->ppme_input_->situs_), situs_col(&cc->ppme_input_->situs_), cc->ppme_input_->text_);
        }
        else {
          cc_printf(cc, "%s(%d): syntax error at end of file\n", cc->ppme_input_file_, cc->ppme_input_line_);
        }
        /* keep going */
        goto macro_expander;

      case _PPME_MATCH:
      case _PPME_OVERFLOW:
      case _PPME_NO_MEMORY:
      case _PPME_END_OF_INPUT:
      case _PPME_LEXICAL_ERROR:
      case _PPME_INTERNAL_ERROR:
      default:
        if (cc->ppme_input_) {
          cc_printf(cc, "%s(%d): internal error (%d) from ppme_parse at column %d: \"%s\"\n", cc->ppme_input_file_, situs_line(&cc->ppme_input_->situs_), r, situs_col(&cc->ppme_input_->situs_), cc->ppme_input_->text_);
        }
        else {
          cc_printf(cc, "%s(%d): internal error (%d) from ppme_parse at end of file\n", cc->ppme_input_file_, cc->ppme_input_line_, r);
        }
        return CCR_FAILED;
    }
  }

line_directives:
  if (!cc->input_stack_) {
    cc->ppme_input_final_ = 1;
    goto macro_expander;
  }

  if (cc->input_stack_->ppld_end_processed_pop_now_) {
    /* Current input file has finished; so there is no sense in requesting more input.
     * Pop to parent if possible. */
    if (!cc->input_stack_->unpoppable_) {
      cc->ppld_input_final_ = 0;
      cc_pop_input_file(cc);
      return CCR_OLD_INCLUDE;
    }
    else {
      cc->ppme_input_final_ = 1;
      goto macro_expander;
    }
  }

  if (!cc->ppld_input_ && !cc->have_ppld_input_line_ && cc->ppld_input_final_) {
    cc->input_stack_->ppld_end_processed_pop_now_ = 1;
    goto macro_expander;
  }

  if (cc->ppld_input_ || cc->have_ppld_input_line_ || cc->input_stack_->ppld_input_not_finished_) {
    for (;;) {
      int ppld_r;
      /* Single lines are always final from the perspective of the ppld parser (hence we pass 1 for is_final),
       * whether the total file is final (ppld_input_final_) determines if it is the last line of the file,
       * this line might be empty. */
      if (!cc->input_stack_->ppld_input_not_finished_) {
        struct pptk *pp_current = cc->ppld_input_ ? pptk_pop_front(&cc->ppld_input_) : NULL;
        ppld_r = ppld_parse(&cc->input_stack_->ppld_, pp_current ? g_pptk_to_ppld_[pp_current->tok_] : /* have_ppld_input_line_ == true */ PPLD_INPUT_END, cc, &pp_current, &cc->ppld_input_, cc->have_ppld_input_line_, &cc->ppme_input_);
        if (pp_current) {
          /* Front (in current) was not processed, recombine */
          cc->ppld_input_ = pptk_join(pp_current, cc->ppld_input_);
        }
      }
      else {
        // Finish up last line before processing tokens for a new one.
        ppld_r = ppld_parse(&cc->input_stack_->ppld_, PPLD_INPUT_END, cc, NULL, NULL, 0, &cc->ppme_input_);
        cc->input_stack_->ppld_input_not_finished_ = 0;
      }
      switch (ppld_r) {
        case PPLD_INCLUDE_FILE:
          cc->input_stack_->ppld_input_not_finished_ = 1;
          cc->have_ppld_input_line_ = 0;
          return CCR_NEW_INCLUDE;
        case _PPLD_FINISH:
          cc->ppme_input_file_ = cc->input_stack_->ppld_input_line_filename_;
          cc->ppme_input_line_ = cc->input_stack_->ppld_input_line_;
          cc->have_ppld_input_line_ = 0;
          goto macro_expander;
        case _PPLD_SYNTAX_ERROR:
          cc_printf(cc, "%s(%d): syntax error reading line directive\n", cc->input_stack_->ppld_input_line_filename_, cc->input_stack_->ppld_input_line_);
          break;
        case _PPLD_MATCH:
        case _PPLD_END_OF_INPUT:
          /* Should not reach here - lexer internal codes */
          assert(0 && "Should never return lexer internal codes");
          return CCR_FAILED;
        case _PPLD_LEXICAL_ERROR:
          /* Not lexing, should not return lexer errors */
          assert(0 && "Should never return lexer codes");
          return CCR_FAILED;
        case _PPLD_FEED_ME:
          /* Needs more input */
          /* If we have input for the macro-expander, process it first before creating more tokens */
          if (cc->ppme_input_) {
            cc->ppme_input_file_ = cc->input_stack_->ppld_input_line_filename_;
            cc->ppme_input_line_ = cc->input_stack_->ppld_input_line_;
            goto macro_expander;
          }
          goto tokenizer;
        case _PPLD_OVERFLOW:
          cc_printf(cc, "Overflow while parsing line directives\n");
          return CCR_FAILED;
        case _PPLD_NO_MEMORY:
          cc_printf(cc, "No memory while parsing line directives\n");
          return CCR_FAILED;
        case _PPLD_INTERNAL_ERROR:
          cc_printf(cc, "Internal error while parsing line directives\n");
          return CCR_FAILED;
        default:
          cc_printf(cc, "Unexpected returncode while parsing line directives\n");
          return CCR_FAILED;
      }
    }
  }

tokenizer:;
  int pptk_r;
  do {
    int break_out = 0;
    if (ppld_stack_accepts(&cc->input_stack_->ppld_, PPLD_HEADER_NAME)) {
      switch (pptk_mode(&cc->input_stack_->pptk_)) {
        case M_PPTK_DEFAULT:
        case M_PPTK_HEADER_NAME_POSSIBLE:
          pptk_set_mode(&cc->input_stack_->pptk_, M_PPTK_HEADER_NAME_POSSIBLE);
          break;
        case M_PPTK_TEMPLATE_DEFAULT:
        case M_PPTK_TEMPLATE_HEADER_NAME_POSSIBLE:
        case M_PPTK_TEMPLATE_START:
          pptk_set_mode(&cc->input_stack_->pptk_, M_PPTK_TEMPLATE_HEADER_NAME_POSSIBLE);
          break;
      }
      
    }
    else {
      switch (pptk_mode(&cc->input_stack_->pptk_)) {
      case M_PPTK_DEFAULT:
      case M_PPTK_HEADER_NAME_POSSIBLE:
        pptk_set_mode(&cc->input_stack_->pptk_, M_PPTK_DEFAULT);
        break;
      case M_PPTK_TEMPLATE_DEFAULT:
      case M_PPTK_TEMPLATE_HEADER_NAME_POSSIBLE:
        pptk_set_mode(&cc->input_stack_->pptk_, M_PPTK_TEMPLATE_DEFAULT);
        break;
      case M_PPTK_TEMPLATE_START:
        /* Keep as is; the pptk tokenizer itself will change in due course */
        break;
      }
    }
    
    pptk_r = pptk_scan(&cc->input_stack_->pptk_, cc, &cc->ppld_input_, &cc->input_stack_->post_line_continuation_situs_);
    switch (pptk_r) {
      case PPTK_TOKENIZER_LINE_READY:
        cc->have_ppld_input_line_ = 1;
        cc->input_stack_->ppld_input_line_ = cc->input_stack_->pptk_input_line_;
        cc->input_stack_->ppld_input_line_filename_ = cc->input_stack_->pptk_input_line_filename_;
        int current_line = pptk_endline(&cc->input_stack_->pptk_);
        cc->input_stack_->pptk_input_line_ += current_line - cc->input_stack_->pptk_true_input_line_number_at_start_;
        cc->input_stack_->pptk_true_input_line_number_at_start_ = current_line;
        goto line_directives;
      case _PPTK_FINISH:
        break;
      case _PPTK_MATCH:
      case _PPTK_END_OF_INPUT:
        assert(0); /* lexer internal code */
        break;
      case _PPTK_SYNTAX_ERROR: {
        struct situs *situs = (struct situs *)pptk_token_common_data(&cc->input_stack_->pptk_);
        if (situs) {
          cc_printf(cc, "%s(%d): syntax error at column %d: \"%s\"\n", situs_filename(situs), situs_line(situs), situs_col(situs), pptk_text(&cc->input_stack_->pptk_));
        }
        else {
          cc_printf(cc, "%s(%d): syntax error at column %d: \"%s\"\n", cc->input_stack_->pptk_input_line_filename_, pptk_line(&cc->input_stack_->pptk_), pptk_column(&cc->input_stack_->pptk_), pptk_text(&cc->input_stack_->pptk_));
        }
        break;
      }
      case _PPTK_LEXICAL_ERROR:
        cc_printf(cc, "%s(%d): lexical error at column %d: \"%s\"\n", cc->input_stack_->pptk_input_line_filename_, pptk_line(&cc->input_stack_->pptk_), pptk_column(&cc->input_stack_->pptk_), pptk_text(&cc->input_stack_->pptk_));
        break;
      case _PPTK_INTERNAL_ERROR:
        cc_printf(cc, "%s(%d): internal error at column %d: \"%s\"\n", cc->input_stack_->pptk_input_line_filename_, pptk_line(&cc->input_stack_->pptk_), pptk_column(&cc->input_stack_->pptk_), pptk_text(&cc->input_stack_->pptk_));
        break;
      case PPTK_TOKENIZER_HEADERNAME_CHECK:
        /* Scanner thinks we should check if a headername is acceptable, this depends on whether the line directive parser can handle it. */
        /* Go to the line directives parser, parse to the extent input is available, and then come back here (via _PPLD_FEED_ME) to
         * check ppld_stack_accepts(PPLD_HEADER_NAME) and set the pptk mode appropriately. */
        goto line_directives;
      case _PPTK_FEED_ME:
        if (cc->ppld_input_) {
          /* Process input first; this is not a complete line but ensures we push as much as possible prior to asking
           * for more input. */
          goto line_directives;
        }
        break;
    }
  } while ((pptk_r != _PPTK_FEED_ME) && (pptk_r != _PPTK_FINISH));

  if ((pptk_r == _PPTK_FINISH) && !cc->input_stack_->pptk_pushed_final_source_line_) {
    /* Complete last line directive, the last line never ends with a newline. */
    cc->have_ppld_input_line_ = 1;
    cc->input_stack_->ppld_input_line_ = cc->input_stack_->pptk_input_line_;
    cc->input_stack_->ppld_input_line_filename_ = cc->input_stack_->pptk_input_line_filename_;
    cc->input_stack_->pptk_pushed_final_source_line_ = 1;
    goto line_directives;
  }

  if (cc->ppld_input_) {
    /* Process input first; this is not a complete line but ensures we push as much as possible prior to asking
     * for more input. */
    goto line_directives;
  }

  if (pptk_r == _PPTK_FINISH) {
    /* we only get here if trigraphs has depleted its input (or pptk
     * would not return _PPTK_FINISH but _PPTK_FEED_ME) */
    /* Cannot pop current input source, we've reached the end; bubble
     * any remaining input up through the stages until we ultimately 
     * set all_done_ at the top... */
    cc->ppld_input_final_ = 1;
    goto line_directives;
  }
line_continuations:;
  int pplc_r;
  do {
    pplc_r = pplc_scan(&cc->input_stack_->pplc_, cc, &cc->input_stack_->post_line_continuation_, &cc->input_stack_->post_line_continuation_size_, &cc->input_stack_->post_line_continuation_size_allocated_, &cc->input_stack_->post_line_continuation_situs_, &cc->input_stack_->post_trigraph_situs_);
    switch (pplc_r) {
      case _PPLC_FINISH:
        break;
      case _PPLC_MATCH:
      case _PPLC_END_OF_INPUT:
        assert(0); /* lexer internal code */
        break;
      case _PPLC_SYNTAX_ERROR: {
        struct situs *situs = (struct situs *)pplc_token_common_data(&cc->input_stack_->pplc_);
        if (situs) {
          cc_printf(cc, "%s(%d): syntax error at column %d: \"%s\"\n", situs_filename(situs), situs_line(situs), situs_col(situs), pplc_text(&cc->input_stack_->pplc_));
        }
        else {
          cc_printf(cc, "%s(%d): syntax error at column %d: \"%s\"\n", cc->input_stack_->ppld_input_line_filename_, pplc_line(&cc->input_stack_->pplc_), pplc_column(&cc->input_stack_->pplc_), pplc_text(&cc->input_stack_->pplc_));
        }
        break;
      }
      case _PPLC_LEXICAL_ERROR:
        cc_printf(cc, "(%d): lexical error at column %d: \"%s\"\n", pplc_line(&cc->input_stack_->pplc_), pplc_column(&cc->input_stack_->pplc_), pplc_text(&cc->input_stack_->pplc_));
        break;
      case _PPLC_INTERNAL_ERROR:
        cc_printf(cc, "(%d): internal error at column %d: \"%s\"\n", pplc_line(&cc->input_stack_->pplc_), pplc_column(&cc->input_stack_->pplc_), pplc_text(&cc->input_stack_->pplc_));
        break;
      case _PPLC_FEED_ME:
        break;
    }
  } while ((pplc_r != _PPLC_FEED_ME) && (pplc_r != _PPLC_FINISH));

  if (cc->input_stack_->post_line_continuation_size_ || (pplc_r == _PPLC_FINISH)) {
    /* Process output of line continuation translation phase first, to ensure we push as much as possible before asking
     * more input. */
    pptk_set_input(&cc->input_stack_->pptk_, cc->input_stack_->post_line_continuation_, cc->input_stack_->post_line_continuation_size_, pplc_r == _PPLC_FINISH);
    cc->input_stack_->post_line_continuation_size_ = 0;
    goto tokenizer;
  }

trigraphs:;
  int pptg_r;
  do {
    pptg_r = pptg_scan(&cc->input_stack_->pptg_, cc, cc->input_stack_->filename_, &cc->input_stack_->post_trigraph_, &cc->input_stack_->post_trigraph_size_, &cc->input_stack_->post_trigraph_size_allocated_, &cc->input_stack_->post_trigraph_situs_);
    switch (pptg_r) {
      case _PPTG_FINISH:
        break;
      case _PPTG_MATCH:
      case _PPTG_END_OF_INPUT:
        assert(0); /* lexer internal code */
        break;
      case _PPTG_SYNTAX_ERROR: {
        struct situs *situs = (struct situs *)pptg_token_common_data(&cc->input_stack_->pptg_);
        if (situs) {
          cc_printf(cc, "%s(%d): syntax error at column %d: \"%s\"\n", situs_filename(situs), situs_line(situs), situs_col(situs), pptg_text(&cc->input_stack_->pptg_));
        }
        else {
          cc_printf(cc, "%s(%d): syntax error at column %d: \"%s\"\n", cc->input_stack_->ppld_input_line_filename_, pptg_line(&cc->input_stack_->pptg_), pptg_column(&cc->input_stack_->pptg_), pptg_text(&cc->input_stack_->pptg_));
        }
        break;
      }
      case _PPTG_LEXICAL_ERROR:
        cc_printf(cc, "(%d): lexical error at column %d: \"%s\"\n", pptg_line(&cc->input_stack_->pptg_), pptg_column(&cc->input_stack_->pptg_), pptg_text(&cc->input_stack_->pptg_));
        break;
      case _PPTG_INTERNAL_ERROR:
        cc_printf(cc, "(%d): internal error at column %d: \"%s\"\n", pptg_line(&cc->input_stack_->pptg_), pptg_column(&cc->input_stack_->pptg_), pptg_text(&cc->input_stack_->pptg_));
        break;
      case _PPTG_FEED_ME:
        break;
    }
  } while ((pptg_r != _PPTG_FEED_ME) && (pptg_r != _PPTG_FINISH));

  if (cc->input_stack_->post_trigraph_size_ || (pptg_r == _PPTG_FINISH)) {
    /* Process output of trigraph translation phase first, to ensure we push as much as possible before asking
     * more input. */
    pplc_set_input(&cc->input_stack_->pplc_, cc->input_stack_->post_trigraph_, cc->input_stack_->post_trigraph_size_, pptg_r == _PPTG_FINISH);
    cc->input_stack_->post_trigraph_size_ = 0;
    goto line_continuations;
  }

  /* Get more input from the current file. */
  if (!cc->input_stack_->input_request_fn_) {
    return CCR_NEED_INPUT;
  }

  /* Call for more input */
  int ir = cc->input_stack_->input_request_fn_(cc->input_stack_->baton_, cc->input_stack_);
  if (ir) {
    return ir;
  }
  /* And jump to trigraphs to process what we got. If we got nothing, and the caller or input_request_fn_
   * did not mark it as the end of the file, then we might loop infinitely here, but that'd be a bug in
   * the calling code (for not setting is_final() when calling if_set_input(), or not calling if_set_input()
   * at all.) */
  goto trigraphs;

}

static void cc_c_parser_skip_template_cruft(struct c_compiler *cc) {
  while ((cc->cp_input_) && 
          ((cc->cp_input_->tok_ == PPTK_TEMPLATE_DOUBLE_CURLY_OPEN) ||
          (cc->cp_input_->tok_ == PPTK_TEMPLATE_DOUBLE_CURLY_CLOSE) ||
          ((cc->cp_input_->tok_ == PPTK_TEMPLATE_LIT) && !cc->cp_input_->text_len_))) {
    pptk_free(pptk_pop_front(&cc->cp_input_));
  }
}

static void cc_c_parser_next_token(struct c_compiler *cc) {
  pptk_free(pptk_pop_front(&cc->cp_input_));

  cc_c_parser_skip_template_cruft(cc);
}

enum c_compiler_result cc_compiler_stage(struct c_compiler *cc) {
  int r;
final_completion:
  if (cc->all_done_) {
    return CCR_SUCCESS;
  }
  if (cc->fatal_error_) {
    return CCR_FAILED;
  }

c_parser_input:
  if (cc->cp_input_ || cc->cp_input_final_) {
    cc->cp_input_line_ = cc->ppme_input_line_;
    cc->cp_input_file_ = cc->ppme_input_file_;

    do {
      int sym = 0;
      if (!cc->mode_set_) {
        switch (cc->mode_) {
          case CC_REPL:
            sym = CP_REPL_MODE;
            break;
          case CC_STMT:
            sym = CP_STMT_MODE;
            break;
          case CC_SCAN:
            sym = CP_SCAN_MODE;
            break;
          case CC_TEMPL:
            sym = CP_TEMPL_MODE;
            break;
          case CC_EXPR:
            sym = CP_EXPR_MODE;
            break;
          default:
            cc_printf(cc, "internal error, unknown compiler mode\n");
            return CCR_FAILED;
        }
        r = cp_parse(&cc->cp_, sym, cc, cc->cp_input_);
        cc->mode_set_ = 1;
      }

      do {
        if (cc->cp_input_) {
          struct pptk *tk = cc->cp_input_;
          sym = g_pptk_to_cp_[tk->tok_];
          if (sym == CP_IDENT) {
            /* Possibly retarget to TYPEDEF_NAME at runtime, depending on:
              * 1) if a TYPEDEF_NAME is currently accepted by the grammar (cp_stack_accepts(stack, CP_TYPEDEF_NAME))
              *    AND
              * 2) TYPEDEF_NAME is permitted based on the context. If we successfully retarget as TYPEDEF_NAME, we must
              *    immediately *disable* this permission. It will be re-enabled by the reduction of declaration-specifiers.
              *    (A second occurrance of a typedef name is actually the declarator's identifier even if it clashes with
              *    an existing typedef name.)
              * See also the reduction of declaration-specifiers in particular. */
            int became_typedefname = 0;
            if (cc->ctx_.is_typedefname_permitted_ && cp_stack_accepts(&cc->cp_, CP_TYPEDEF_NAME)) {
              struct name_space *pns = cc->ctx_.block_ ? cc->ctx_.block_->ns_ : &cc->global_ns_;
              while (pns) {
                struct sym *s = st_find(&pns->ordinary_idents_, tk->text_);
                if (s) {
                  struct decl *d = (struct decl *)s;
                  if (d->sc_ == SC_TYPEDEF) {
                    /* Found a typedef where one is accepted, rewrite */
                    cc->ctx_.is_typedefname_permitted_ = 0;
                    sym = CP_TYPEDEF_NAME;
                    tk->v_.type_ = d->type_;
                    tk->tok_ = PPTK_TYPEDEF_NAME;
                    became_typedefname = 1;
                    break;
                  }
                }

                pns = pns->parent_;
              }
            }
          }
        }
        else {
          if (cc->cp_input_final_) {
            sym = CP_INPUT_END;
          }
          else {
            /* Need to get more input */
            goto preprocessor;
          }
        }
        if (sym == CP_WHITESPACE) {
          /* Skip whitespace as input -- step to next tk */
          cc_c_parser_next_token(cc);
        }
      } while (sym == CP_WHITESPACE);
      r = cp_parse(&cc->cp_, sym, cc, cc->cp_input_);
      if ((r == _CP_SYNTAX_ERROR) || (r == _CP_FEED_ME)) {
        if (r == _CP_SYNTAX_ERROR) {
          if (cc->cp_input_) {
            cc_error(cc, "syntax error at column %d: \"%s\"\n", situs_col(&cc->cp_input_->situs_), cc->cp_input_->text_);
          }
          else {
            cc_error(cc, "syntax error, unexpected end of file\n", cc->cp_input_file_, cc->cp_input_line_);
          }
          /* keep going */
          goto c_parser_input;
        }

        /* Step to next tk */
        cc_c_parser_next_token(cc);
      }

      if (r == _CP_FINISH) {
        cc->all_done_ = 1;
        goto final_completion;
      }
      else if ((r != _CP_SYNTAX_ERROR) && (r != _CP_FEED_ME)) {
        if (cc->cp_input_) {
          cc_printf(cc, "%s(%d): internal error (%d) from cp_parse at column %d: \"%s\"\n", cc->cp_input_file_, cc->cp_input_line_, r, situs_col(&cc->cp_input_->situs_), cc->cp_input_->text_);
        }
        else {
          cc_printf(cc, "%s(%d): internal error (%d) from cp_parse at end of file\n", cc->cp_input_file_, cc->cp_input_line_, r);
        }
        return CCR_FAILED;
      }
    } while (r == _CP_FEED_ME);
  }

preprocessor:
  return CCR_NEED_INPUT;
}

enum c_compiler_result cc_compile(struct c_compiler *cc) {
  enum c_compiler_result cr;

  do {
    cc_c_parser_skip_template_cruft(cc);

    cr = cc_compiler_stage(cc);
    if (cr != CCR_NEED_INPUT) {
      return cr;
    }

    cr = cc_preprocessor_stage(cc);
  } while (cr == CCR_SUCCESS);

  return cr;
}

void cc_set_situs(struct c_compiler *cc, const struct situs *sit) {
  cc->current_sit_ = sit;
}

int cc_error_loc(struct c_compiler *cc, struct situs *sit, const char *fmt, ...) {
  int r;
  va_list args;
  // Duplicate fmt to append newline.
  size_t fmt_len = strlen(fmt);
  char *dup_fmt = malloc(fmt_len + 2);
  if (!dup_fmt) {
    cc_no_memory(cc);
    return -1;
  }
  memcpy(dup_fmt, fmt, fmt_len);
  dup_fmt[fmt_len] = '\n';
  dup_fmt[fmt_len + 1] = '\0';
  cc->have_error_ = 1;
  va_start(args, fmt);
  r = cc->vprintf_handler(cc->vprintf_baton_, situs_filename(sit), situs_line(sit), dup_fmt, args);
  va_end(args);
  free(dup_fmt);
  return r;
}

int cc_error(struct c_compiler *cc, const char *fmt, ...) {
  int r;
  va_list args;
  cc->have_error_ = 1;
  va_start(args, fmt);
  r = cc->vprintf_handler(cc->vprintf_baton_, cc->cp_input_file_, cc->cp_input_line_, fmt, args);
  va_end(args);
  return r;
}

int cc_fatal_loc(struct c_compiler *cc, struct situs *sit, const char *fmt, ...) {
  int r;
  va_list args;
  cc->have_error_ = cc->fatal_error_ = 1;
  va_start(args, fmt);
  r = cc->vprintf_handler(cc->vprintf_baton_, situs_filename(sit), situs_line(sit), fmt, args);
  va_end(args);
  return r;
}

int cc_fatal(struct c_compiler *cc, const char *fmt, ...) {
  int r;
  va_list args;
  cc->have_error_ = cc->fatal_error_ = 1;
  va_start(args, fmt);
  r = cc->vprintf_handler(cc->vprintf_baton_, cc->cp_input_file_, cc->cp_input_line_, fmt, args);
  va_end(args);
  return r;
}

int cc_warn_loc(struct c_compiler *cc, struct situs *sit, const char *fmt, ...) {
  int r;
  va_list args;
  // Duplicate fmt to append newline.
  size_t fmt_len = strlen(fmt);
  char *dup_fmt = malloc(fmt_len + 2);
  if (!dup_fmt) {
    cc_no_memory(cc);
    return -1;
  }
  memcpy(dup_fmt, fmt, fmt_len);
  dup_fmt[fmt_len] = '\n';
  dup_fmt[fmt_len + 1] = '\0';
  va_start(args, fmt);
  r = cc->vprintf_handler(cc->vprintf_baton_, situs_filename(sit), situs_line(sit), dup_fmt, args);
  va_end(args);
  free(dup_fmt);
  return r;
}

int cc_warn(struct c_compiler *cc, const char *fmt, ...) {
  int r;
  va_list args;
  va_start(args, fmt);
  r = cc->vprintf_handler(cc->vprintf_baton_, cc->cp_input_file_, cc->cp_input_line_, fmt, args);
  va_end(args);
  return r;
}

void cc_no_memory(struct c_compiler *cc) {
  cc_fatal(cc, "No memory\n");
}

const char *cc_get_include_filename(struct c_compiler *cc) {
  return cc->pp_include_file_arg_;
}

int cc_is_system_include(struct c_compiler *cc) {
  return !!cc->pp_include_is_system_include_;
}

static int cc_template_default_handler(struct c_compiler *cc, char *templ_str, size_t templ_str_len, struct stmt **new_stmt, struct situs *location) {
  const char *handler_fn_name = cc->default_handler_fn_name_;

  struct expr *fn_exp = NULL;

  struct sym *s = ns_find_ordinary_ident(cc->ctx_.block_ ? cc->ctx_.block_->ns_ : &cc->global_ns_, handler_fn_name);
  if (!s) {
    cc_error(cc, "unknown identifier \"%s\"\n", handler_fn_name);
  }
  else {
    struct decl *d = (struct decl *)s;
    struct expr *x;
    if (d->is_external_) {
      x = expr_alloc(ET_ADDRESS_E);
    }
    else if (d->dsp_) {
      x = expr_alloc(ET_ADDRESS_G);
      if (x) {
        x->dsp_ = d->dsp_;
      }
    }
    else if (d->is_param_) {
      x = expr_alloc(ET_ADDRESS_P);
    }
    else if (d->is_enum_) {
      x = expr_alloc(ET_C_INT);
      if (x) {
        x->v_.i64_ = d->enum_value_;
        fn_exp = x;
      }
    }
    else {
      /* By process of elimination, d must be a local */
      x = expr_alloc(ET_ADDRESS_L);
    }
    if (!x) {
      cc_no_memory(cc);
      return -1;
    }
    x->decl_ = d;

    if (!fn_exp) {
      /* Check for double-indirection if the value is passed by pointer */
      if (d->is_passed_by_pointer_) {
        struct expr *pbp = expr_alloc(ET_INDIRECTION_PTR);
        if (!pbp) {
          expr_free(x);
          cc_no_memory(cc);
          return -1;
        }
        pbp->children_[0] = x;
        x = pbp;
      }
      x->decl_ = d;
      fn_exp = expr_alloc(ET_INDIRECTION_PTR);
      if (!fn_exp) {
        expr_free(x);
        cc_no_memory(cc);
        return _CP_NO_MEMORY;
      }
      else {
        fn_exp->children_[0] = x;
      }
    }
  }

  /* exp is now the address of the handler function */

  enum expr_type et_uintptr = ET_INVALID;
  switch (cc->tb_.uintptr_equivalent_) {
  case tk_unsigned_int:
    et_uintptr = ET_C_UNSIGNED_INT;
    break;
  case tk_unsigned_long_int:
    et_uintptr = ET_C_UNSIGNED_LONG_INT;
    break;
  case tk_unsigned_long_long_int:
    et_uintptr = ET_C_UNSIGNED_LONG_LONG_INT;
    break;
  default:
    assert(0 && "Cannot work with uintptr type");
    break;
  }

  struct expr *baton_expr = expr_alloc(et_uintptr);
  if (!baton_expr) {
    expr_free(fn_exp);
    cc_no_memory(cc);
    return _CP_NO_MEMORY;
  }
  baton_expr->v_.u64_ = (uint64_t)cc->default_handler_fn_baton_;
  /* Convert baton_expr to void* */
  struct expr *baton_expr_cast = expr_alloc(ET_CVUINTPTR2PTR);
  if (!baton_expr_cast) {
    expr_free(baton_expr);
    expr_free(fn_exp);
    cc_no_memory(cc);
    return _CP_NO_MEMORY;
  }
  baton_expr_cast->children_[0] = baton_expr;
  baton_expr_cast->type_arg_ = type_base_pointer(&cc->tb_, type_base_simple(&cc->tb_, tk_void));
  baton_expr = baton_expr_cast;

  /* Build a data section pointer for the template string data */
  struct ds_portion *dsp = ds_alloc_portion(&cc->ds_);
  if (!dsp) {
    expr_free(baton_expr);
    expr_free(fn_exp);
    cc_no_memory(cc);
    return _CP_NO_MEMORY;
  }

  char *p = NULL;

  if (ds_reserve(dsp, templ_str_len + 1, (void **)&p)) {
    expr_free(baton_expr);
    expr_free(fn_exp);
    cc_no_memory(cc);
    return _CP_NO_MEMORY;
  }

  memcpy(p, templ_str, templ_str_len);
  p[templ_str_len++] = '\0';

  /* Build a string expression representing templ_str */
  struct expr *strarg = expr_alloc(ET_C_STRING_LIT);
  if (!strarg) {
    expr_free(baton_expr);
    expr_free(fn_exp);
    cc_no_memory(cc);
    return _CP_NO_MEMORY;
  }
  strarg->dsp_ = dsp;

  /* Build string length value */
  struct expr *strlen_expr = expr_alloc(et_uintptr);
  if (!strlen_expr) {
    expr_free(strarg);
    expr_free(baton_expr);
    expr_free(fn_exp);
    cc_no_memory(cc);
    return _CP_NO_MEMORY;
  }
  strlen_expr->v_.u64_ = (uint64_t)templ_str_len;

  /* Build argument list for invocation "fn_exp(baton_expr, strarg, strlen_expr)" */
  struct expr *arg01 = expr_alloc(ET_FUNCTION_ARG_LIST);
  if (!arg01) {
    expr_free(strlen_expr);
    expr_free(strarg);
    expr_free(baton_expr);
    expr_free(fn_exp);
    cc_no_memory(cc);
    return _CP_NO_MEMORY;
  }
  arg01->children_[0] = baton_expr;
  arg01->children_[1] = strarg;

  struct expr *arg012 = expr_alloc(ET_FUNCTION_ARG_LIST);
  if (!arg012) {
    expr_free(arg01);
    expr_free(strlen_expr);
    expr_free(fn_exp);
    cc_no_memory(cc);
    return _CP_NO_MEMORY;
  }
  arg012->children_[0] = arg01;
  arg012->children_[1] = strlen_expr;

  int r;
  struct expr *call = NULL;
  r = expr_function_call(cc, &call, location, &fn_exp, location, location, &arg012);
  if (r) {
    expr_free(arg012);
    expr_free(fn_exp);
    return -1;
  }

  struct stmt *st_exp = stmt_alloc(cc, ST_EXPR, location);
  if (!st_exp) {
    expr_free(call);
    cc_no_memory(cc);
    return -1;
  }
  st_exp->expr0_ = call;
  *new_stmt = st_exp;

  return 0;
}

int cc_pp_concat_sub_output(struct c_compiler *cc, struct situs *output_situs, char **output_buf, size_t *output_pos, size_t *output_buf_size, struct situs *span_situs, const char *span_text) {
  size_t span_text_len = strlen(span_text);
  return cc_pp_concat_output_b(cc, output_situs, output_buf, output_pos, output_buf_size, span_situs, span_text, span_text_len);
}

int cc_pp_concat_sub_output_b(struct c_compiler *cc, struct situs *output_situs, char **output_buf, size_t *output_pos, size_t *output_buf_size, struct situs *span_situs, const char *span_text, size_t span_text_len) {
  size_t buf_size_needed = (*output_pos) + span_text_len;
  if (buf_size_needed > *output_buf_size) {
    char *newbuf = (char *)realloc(*output_buf, buf_size_needed + 1);
    if (!newbuf) {
      cc_no_memory(cc);
      return -1;
    }
    *output_buf = newbuf;
    *output_buf_size = buf_size_needed;
  }
  memcpy((*output_buf) + *output_pos, span_text, span_text_len);
  (*output_buf)[(*output_pos) + span_text_len] = '\0';
  struct situs substitution;
  situs_init(&substitution);
  if (situs_clone(&substitution, span_situs)) {
    cc_no_memory(cc);
    return -1;
  }

  struct situs_span *sss = (span_situs->num_spans_ > 1) ? span_situs->u_.many_.spans_ : &span_situs->u_.one_;
  /* Mark all of the situs a substitution */
  size_t n;
  sss->is_substitution_ = 1;
  for (n = 1; n < substitution.num_spans_; ++n) {
    sss[n].is_aux_ = 1;
    sss[n].num_bytes_ = 0;
  }
  sss[0].num_bytes_ = span_text_len;
  if (situs_concat(output_situs, &substitution)) {
    situs_cleanup(&substitution);
    cc_no_memory(cc);
    return -1;
  }
  situs_cleanup(&substitution);
  (*output_pos) += span_text_len;
  return 0;
}

int cc_pp_concat_output(struct c_compiler *cc, struct situs *output_situs, char **output_buf, size_t *output_pos, size_t *output_buf_size, struct situs *span_situs, const char *span_text) {
  size_t span_text_len = situs_len(span_situs);
  return cc_pp_concat_output_b(cc, output_situs, output_buf, output_pos, output_buf_size, span_situs, span_text, span_text_len);
}

int cc_pp_concat_output_b(struct c_compiler *cc, struct situs *output_situs, char **output_buf, size_t *output_pos, size_t *output_buf_size, struct situs *span_situs, const char *span_text, size_t span_text_len) {
  size_t buf_size_needed = (*output_pos) + span_text_len;
  if (buf_size_needed > *output_buf_size) {
    char *newbuf = (char *)realloc(*output_buf, buf_size_needed + 1);
    if (!newbuf) {
      cc_no_memory(cc);
      return -1;
    }
    *output_buf = newbuf;
    *output_buf_size = buf_size_needed;
  }
  memcpy((*output_buf) + *output_pos, span_text, span_text_len);
  (*output_buf)[(*output_pos) + span_text_len] = '\0';
  if (situs_concat(output_situs, span_situs)) {
    cc_no_memory(cc);
    return -1;
  }
  (*output_pos) += span_text_len;
  return 0;
}
