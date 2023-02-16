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

#ifndef LINE_DEFS_H
#define LINE_DEFS_H

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
xx("[ \\x9\\xb\\xc\\xd]*%scanner%[ \\x9\\xb\\xc\\xd]*\\n?", LD_CARBURETTA_SCANNER_SECTION_DELIMETER) \
xx("[ \\x9\\xb\\xc\\xd]*%grammar%[ \\x9\\xb\\xc\\xd]*\\n?", LD_CARBURETTA_GRAMMAR_SECTION_DELIMETER) \
xx("[ \\x9\\xb\\xc\\xd]*%%[ \\x9\\xb\\xc\\xd]*\\n?", LD_CARBURETTA_SECTION_DELIMITER) \
xx("[ \\x9\\xb\\xc\\xd]*%.*\\n?", LD_CARBURETTA_DIRECTIVE) \
xx(".*\\n?", LD_REGULAR)

typedef enum ld_line_type {
#define xx(regex, type_of_line) type_of_line,
#define xz(type_of_line) type_of_line,
  ENUM_LINE_DEFS
#undef xx
#undef xz
} ld_line_type_t;

int ldl_init(void);
void ldl_cleanup(void);

const char *ld_line_type_to_str(ld_line_type_t ldlt);

void ldl_init_tokenizer(struct tkr_tokenizer *tkr);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LINE_DEFS_H */
