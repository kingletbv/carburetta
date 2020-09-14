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

int ldl_init();
void ldl_cleanup();

const char *ld_line_type_to_str(ld_line_type_t ldlt);

void ldl_init_tokenizer(struct tkr_tokenizer *tkr);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LINE_DEFS_H */
