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

#ifndef C_COMPILER_H
#define C_COMPILER_H

#ifndef STDARG_H_INCLUDED
#define STDARG_H_INCLUDED
#include <stdarg.h>
#endif

struct pptk;
struct c_compiler;

#ifndef PP_MACRO_EXPANDER_H_INCLUDED
#define PP_MACRO_EXPANDER_H_INCLUDED
#include "pp_macro_expander.h"
#endif

#ifndef C_PARSER_H_INCLUDED
#define C_PARSER_H_INCLUDED
#include "c_parser.h"
#endif

#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED
#include "types.h"
#endif

#ifndef DATA_SECTION_H_INCLUDED
#define DATA_SECTION_H_INCLUDED
#include "data_section.h"
#endif

#ifndef NAME_SPACE_H_INCLUDED
#define NAME_SPACE_H_INCLUDED
#include "name_space.h"
#endif

#ifndef SYMTAB_H_INCLUDED
#define SYMTAB_H_INCLUDED
#include "symtab.h"
#endif

#ifndef TEMPL_PARSER_H_INCLUDED
#define TEMPL_PARSER_H_INCLUDED
#include "templ_parser.h"
#endif

#ifndef SITUS_H_INCLUDED
#define SITUS_H_INCLUDED
#include "situs.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

enum c_compiler_result {
  CCR_SUCCESS,
  CCR_NEED_INPUT,
  CCR_NEW_INCLUDE,
  CCR_OLD_INCLUDE,
  CCR_HAVE_ERRORS,
  CCR_FAILED
};

enum cc_if_section_state {
  CC_IFSS_SKIP, /* all groups in if-section should be skipped */
  CC_IFSS_SELECTED, /* current group should have tokens selected, subsequent #else or #elif groups should skip */
  CC_IFSS_NOT_YET_SELECTED /* current group should be skipped, subsequent #else should be selected, subsequent #elif should be evaluated */
};

enum cc_mode {
  CC_REPL,
  CC_STMT,
  CC_SCAN,
  CC_TEMPL,
  CC_EXPR
};

typedef int (*cc_template_handler_fn)(struct c_compiler *cc, char *templ_str, size_t templ_str_len, struct stmt **new_stmt, struct situs *location);

/* Preprocessor #if section */
struct cc_if_section {
  enum cc_if_section_state state_;
  struct cc_if_section *parent_; /* #endif pops to parent, if there is a parent. */
};

struct cc_filename_buffer {
  struct cc_filename_buffer *next_;
  char filename_[1];
};

struct cc_link_name {
  struct sym sym_;
  void *ptr_;
};

struct c_compiler {
  enum cc_mode mode_;
  int mode_set_:1;

  /* this is the character used to decorate symbols for assembly / link name output; default '$'
   * one common alternative would be '@'; the character should make it impossible to conflict
   * with C generated names. */
  char asm_decoration_char_; 

  int ppme_input_final_:1;
  struct pptk *ppme_input_;
  const char *ppme_input_file_;
  int ppme_input_line_;
  struct ppme_stack ppme_; /* macro-expander */

  int ppld_input_final_:1;
  int have_ppld_input_line_:1;
  struct pptk *ppld_input_;

  struct name_space global_ns_;

  /* if non-zero, the compiler is done */
  int all_done_:1;
  int have_error_:1;
  int fatal_error_:1;

  /* if non-zero, cp_input_ == NULL means there is no more input, not that more input is requested */
  int cp_input_final_; 
  struct pptk *cp_input_;
  const char *cp_input_file_;
  int cp_input_line_;
  struct templ_context ctx_;
  struct cp_stack cp_;     /* C parser */

  struct cc_if_section *if_section_stack_;

  struct input_file *input_stack_;

  struct type_base tb_;
  struct data_section ds_;
  struct symtab macro_table_;

  int (*vprintf_handler)(void *printf_baton, const char *file, int line_num, const char *fmt, va_list args);
  void *vprintf_baton_;

  const struct situs *current_sit_;

  struct cc_filename_buffer *filename_buffers_;

  /* The filename argument for the most recent #include preprocessing directive. */
  int pp_include_is_system_include_ : 1;  /* if non-zero, #include <header.h> was used, if zero #include "header.h" was used. */
  int pp_include_is_template_emit_ : 1;   /* if non-zero, #emit "header.h" was used, if zero, #include was used. */
  char *pp_include_file_arg_;

  /* The function to be used for template literals (PPTK_TEMPLATE_LIT); if not explicitly initialized
   * by the caller, then a function will be called with the name in handler_fn_name_, this function
   * will have the prototype:
   *   void default_handler(void *baton_value, const char *text, size_t text_len);
   * where "default_handler" is the function's name as found in default_handler_fn_name, "text" is a
   * pointer to the literal text in the template string, and text_len is the length of that string
   * as measured in bytes, excluding the null terminator. The function's implementation can rely on text 
   * having been null-terminated. */
  cc_template_handler_fn template_handler_;

  /* The function name to be used for template literal processing, default is "__kc_template_handler" */
  char *default_handler_fn_name_;

  /* A baton value passed to the template literal processing function by the default template_handler_. */
  void *default_handler_fn_baton_;

  /* Link table, contains all dynamic link names and their void pointers */
  struct symtab link_table_;
};

void cc_init(struct c_compiler *cc);
void cc_cleanup(struct c_compiler *cc);

int cc_printf(struct c_compiler *cc, const char *fmt, ...);

/* push and pop preprocessor #if sections */
struct cc_if_section *cc_if_push(struct c_compiler *cc);
void cc_if_pop(struct c_compiler *cc);

struct input_file *cc_push_input_file_fp(struct c_compiler *cc, const char *filename, FILE *fp);

/* preprocess the input stack; puts resulting tokens as a chain in cc->cp_input_ ; 
 * sets cc->cp_input_final_ when there is no more input.
 * Returns CCR_SUCCESS if the compiler should process the cc->cp_input_ tokens and/or
 * the cc->cp_input_final_ flag.
 * Returns CCR_NEED_INPUT if more input is needed
 * 
 * Please see the implementation of cc_compile() to understand how this function
 * might be used with cc_compiler_stage() to implement the compiler.
 * One reason for implementing your own cc_compile() like function is to intercept
 * tokens during the compilation process. For instance, when running the compiler in
 * a templating mode (running pp_tokenizer.cbrt in input_stack->pptk_ with mode
 * M_PPTK_TEMPLATE_START at initialization,) the pp_tokenizer.cbrt will start to
 * emit PPTK_TEMPLATE_LIT tokens, that are not part of the C language. These need
 * to be intercepted before they are passed to the C parser, but after the preprocessor.
 * Re-implementing cc_compiler will allow you to do so.
 */
enum c_compiler_result cc_preprocessor_stage(struct c_compiler *cc);

/*
 * cc_compiler_stage - Compiles the input tokens in cc->cp_input_ using the flags cc->all_done_,
 * cc->fatal_error_, and cc->cp_input_final_.
 *
 * Returns CCR_SUCCESS if the compiler completed successfully, CCR_NEED_INPUT if
 * cc->cp_input_ has been drained without cc->cp_input_final_ being set,
 * or another value indicating an error.
 * 
 * Please see the implementation of cc_compile() to understand how this function
 * might be used with cc_preprocessor_stage() to implement the compiler.
 * See cc_preprocessor_stage() for more information on why you might want to.
 */
enum c_compiler_result cc_compiler_stage(struct c_compiler *cc);

/* Comp*/
enum c_compiler_result cc_compile(struct c_compiler *cc);



void cc_set_situs(struct c_compiler *cc, const struct situs *sit);
int cc_error_loc(struct c_compiler *cc, struct situs *sit, const char *fmt, ...);
int cc_error(struct c_compiler *cc, const char *fmt, ...);
int cc_fatal_loc(struct c_compiler *cc, struct situs *sit, const char *fmt, ...);
int cc_fatal(struct c_compiler *cc, const char *fmt, ...);
int cc_warn_loc(struct c_compiler *cc, struct situs *sit, const char *fmt, ...);
int cc_warn(struct c_compiler *cc, const char *fmt, ...);
void cc_no_memory(struct c_compiler *cc);

/* creates a copy of the string 'filename' and preserves that copy inside cc
 * (the copy will remain valid for the remainder of cc's lifetime.) */
char *cc_preserve_filename(struct c_compiler *cc, const char *filename);

const char *cc_get_include_filename(struct c_compiler *cc);
int cc_is_system_include(struct c_compiler *cc);

/* Helper functions for pre-tokenization translation phases implementation. */
int cc_pp_concat_sub_output_b(struct c_compiler *cc, struct situs *output_situs, char **output_buf, size_t *output_pos, size_t *output_buf_size, struct situs *span_situs, const char *span_text, size_t span_text_len);
int cc_pp_concat_sub_output(struct c_compiler *cc, struct situs *output_situs, char **output_buf, size_t *output_pos, size_t *output_buf_size, struct situs *span_situs, const char *span_text);
int cc_pp_concat_output_b(struct c_compiler *cc, struct situs *output_situs, char **output_buf, size_t *output_pos, size_t *output_buf_size, struct situs *span_situs, const char *span_text, size_t span_text_len);
int cc_pp_concat_output(struct c_compiler *cc, struct situs *output_situs, char **output_buf, size_t *output_pos, size_t *output_buf_size, struct situs *span_situs, const char *span_text);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* C_COMPILER_H */
