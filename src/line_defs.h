#ifndef LINE_DEFS_H
#define LINE_DEFS_H

#ifndef STDINT_H_INCLUDED
#define STDINT_H_INCLUDED
#include <stdint.h>
#endif

#ifndef TOKENIZER_H_INCLUDED
#define TOKENIZER_H_INCLUDED
#include "tokenizer.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* XXX: Line continuations \CRLF don't work... We will need them for terseness. */

#define ENUM_LINE_DEFS \
xz(LD_UNKNOWN) \
xx("[ \\x9\\xb\\xc\\xd]*#.*\\n?", LD_C_PREPROCESSOR) \
xx("[ \\x9\\xb\\xc\\xd]*%%[ \\x9\\xb\\xc\\xd]*\\n?", LD_CINDER_SECTION_DELIMITER) \
xx("[ \\x9\\xb\\xc\\xd]*%.*\\n?", LD_CINDER_DIRECTIVE) \
xx("([^\\\\\\n]|\\\\+[^\\\\])*\\n?", LD_REGULAR)

typedef enum ld_line_type {
#define xx(regex, type_of_line) type_of_line,
#define xz(type_of_line) type_of_line,
  ENUM_LINE_DEFS
#undef xx
#undef xz
} ld_line_type_t;

struct ld_line {
  ld_line_type_t type_of_line_;

  int free_chars_on_cleanup_ : 1;

  size_t num_chars_;
  size_t num_chars_allocated_;
  char *chars_;

  const char *filename_;
  size_t start_offset_;
  int start_line_;
  int start_col_;
  size_t end_offset_;
  int end_line_;
  int end_col_;
};

int ldl_init();
void ldl_cleanup();

void ld_line_init(struct ld_line *ldl);
void ld_line_cleanup(struct ld_line *ldl);

const char *ld_line_type_to_str(ld_line_type_t ldlt);

void ldl_init_tokenizer(struct tkr_tokenizer *tkr);

int ldl_fill_token(struct ld_line *ldl, struct tkr_tokenizer *tkr);



#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LINE_DEFS_H */
