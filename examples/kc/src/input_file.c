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

#ifndef INPUT_FILE_H_INCLUDED
#define INPUT_FILE_H_INCLUDED
#include "input_file.h"
#endif

#ifndef PP_TOKENIZER_H_INCLUDED
#define PP_TOKENIZER_H_INCLUDED
#include "pp_tokenizer.h"
#endif

#ifndef C_COMPILER_H_INCLUDED
#define C_COMPILER_H_INCLUDED
#include "c_compiler.h"
#endif

void if_init(struct input_file *ifile, struct c_compiler *cc) {
  ifile->unpoppable_ = 0;
  ifile->cc_ = cc;
  ifile->containing_file_ = cc->input_stack_;
  cc->input_stack_ = ifile;

  ifile->filename_ = NULL;
  ifile->baton_ = NULL;
  ifile->fp_ = NULL;

  ifile->input_request_fn_ = NULL;

  pptk_stack_init(&ifile->pptk_);
  ppld_stack_init(&ifile->ppld_);
  ifile->ppld_input_not_finished_ = 0;
  ifile->ppld_end_processed_pop_now_ = 0;

  ifile->ppld_input_line_ = 0;
  ifile->ppld_input_line_filename_ = NULL;

  ifile->pptk_input_line_ = 0;
  ifile->pptk_input_line_filename_ = NULL;
  ifile->pptk_true_input_line_number_at_start_ = 1;
  ifile->pptk_pushed_final_source_line_ = 0;

  pplc_stack_init(&ifile->pplc_);

  situs_init(&ifile->post_line_continuation_situs_);
  ifile->post_line_continuation_ = NULL;
  ifile->post_line_continuation_size_ = 0;
  ifile->post_line_continuation_size_allocated_ = 0;

  pptg_stack_init(&ifile->pptg_);

  situs_init(&ifile->post_trigraph_situs_);
  ifile->post_trigraph_ = NULL;
  ifile->post_trigraph_size_ = 0;
  ifile->post_trigraph_size_allocated_ = 0;
}

int if_set_filename(struct c_compiler *cc, struct input_file *ifile, const char *filename) {
  char *sf = cc_preserve_filename(cc, filename);
  if (!sf) {
    return -1;
  }

  ifile->filename_ = sf;
  ifile->pptk_input_line_ = 1;
  ifile->pptk_input_line_filename_ = sf;
  ifile->ppld_input_line_ = 0;
  ifile->ppld_input_line_filename_ = NULL;
  return 0;
}

void if_cleanup(struct input_file *ifile) {
  ppld_stack_cleanup(&ifile->ppld_);
  pptk_stack_cleanup(&ifile->pptk_);

  /* Remove ourselves from anywhere in the input stack, this ensures
   * we don't impose a dependency on the order of destruction if multiple
   * if_cleanup()s are needed. */
  struct input_file **ppif = &ifile->cc_->input_stack_;
  while (*ppif) {
    if (*ppif == ifile) {
      *ppif = ifile->containing_file_;
      break;
    }
    ppif = &(*ppif)->containing_file_;
  }
  /* NOTE: ifile->filename_ contains pointers allocated from cc_preserve_filename(), to simplify
   *       passing filenames around by pointer value. The input_file therefore does not own the
   *       pointer (and doesn't have to free it upon cleanup.) */
  if (ifile->fp_) {
    fclose(ifile->fp_);
  }

  pplc_stack_cleanup(&ifile->pplc_);

  situs_cleanup(&ifile->post_line_continuation_situs_);
  if (ifile->post_line_continuation_) {
    free(ifile->post_line_continuation_);
  }

  pptg_stack_cleanup(&ifile->pptg_);

  situs_cleanup(&ifile->post_trigraph_situs_);
  if (ifile->post_trigraph_) {
    free(ifile->post_trigraph_);
  }
}

void if_set_input(struct input_file *ifile, const void *input_buf, size_t input_buf_size, int is_final) {
#if 0
  pptk_set_input(&ifile->pptk_, input_buf, input_buf_size, is_final);
#else
  pptk_set_input(&ifile->pptk_, NULL, 0, 0); // reset pptk so it doesn't think it already finished
  pplc_set_input(&ifile->pplc_, NULL, 0, 0); // reset pplc so it doesn't think it already finished
  pptg_set_input(&ifile->pptg_, input_buf, input_buf_size, is_final);
#endif
  if (ifile->cc_->all_done_) {
    ifile->cc_->all_done_ = 0;
    ifile->cc_->mode_set_ = 0;
    ifile->cc_->cp_input_final_ = 0;
    ifile->cc_->ppld_input_final_ = 0;
    ifile->cc_->ppme_input_final_ = 0;
    ifile->ppld_end_processed_pop_now_ = 0;
    ifile->pptk_pushed_final_source_line_ = 0;
    ifile->pptk_true_input_line_number_at_start_ = 1;
  }
}
