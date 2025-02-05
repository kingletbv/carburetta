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

#ifndef SCAN_HELPERS_H
#define SCAN_HELPERS_H

#ifndef STDINT_H_INCLUDED
#define STDINT_H_INCLUDED
#include <stdint.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct c_compiler;
struct situs;

/* WARNING: These are not general use functions, instead they factor out functionality that would
 *          otherwise be replicated in pattern action code. The input already has to meticulously
 *          match the pattern expected by the function, or results will be unpredictable / wrong. */

int sch_parse_decimal(struct c_compiler *cc, uint64_t *pval, const char *text, size_t len, struct situs *loc);
int sch_parse_octal(struct c_compiler *cc, uint64_t *pval, const char *text, size_t len, struct situs *loc);
int sch_parse_hexadecimal(struct c_compiler *cc, uint64_t *pval, const char *text, size_t len, struct situs *loc);
int sch_parse_binary(struct c_compiler *cc, uint64_t *pval, const char *text, size_t len, struct situs *loc);
void sch_process_ohb_none(struct c_compiler *cc, uint64_t val, struct expr **expr, struct situs *loc);
void sch_process_ohb_U(struct c_compiler *cc, uint64_t val, struct expr **expr, struct situs *loc);
void sch_process_ohb_L(struct c_compiler *cc, uint64_t val, struct expr **expr, struct situs *loc);
void sch_process_ohb_UL(struct c_compiler *cc, uint64_t val, struct expr **expr, struct situs *loc);
void sch_process_ohb_LL(struct c_compiler *cc, uint64_t val, struct expr **expr, struct situs *loc);
void sch_process_ohb_ULL(struct c_compiler *cc, uint64_t val, struct expr **expr, struct situs *loc);

void sch_process_pptk_ohb_none(struct c_compiler *cc, struct pptk **pp_chain, const char *text, uint64_t val, struct pptk **pptk, struct situs *loc);
void sch_process_pptk_ohb_U(struct c_compiler *cc, struct pptk **pp_chain, const char *text, uint64_t val, struct pptk **pptk, struct situs *loc);
void sch_process_pptk_ohb_L(struct c_compiler *cc, struct pptk **pp_chain, const char *text, uint64_t val, struct pptk **pptk, struct situs *loc);
void sch_process_pptk_ohb_UL(struct c_compiler *cc, struct pptk **pp_chain, const char *text, uint64_t val, struct pptk **pptk, struct situs *loc);
void sch_process_pptk_ohb_LL(struct c_compiler *cc, struct pptk **pp_chain, const char *text, uint64_t val, struct pptk **pptk, struct situs *loc);
void sch_process_pptk_ohb_ULL(struct c_compiler *cc, struct pptk **pp_chain, const char *text, uint64_t val, struct pptk **pptk, struct situs *loc);

int sch_read_oct_esc_value(const char *octal_num, int *val);
int sch_read_hex_esc_value(const char *hex_xx_num, int *val);
int sch_read_char_value(struct c_compiler *cc, const char *lit_pos, int *val, int max_char, struct situs *loc, const char *kind);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* SCAN_HELPERS_H */
