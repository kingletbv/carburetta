/* Copyright 2020-2021 Kinglet B.V.
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

#ifndef DECOMMENT_H
#define DECOMMENT_H

#ifndef XLTS_H_INCLUDED
#define XLTS_H_INCLUDED
#include "xlts.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Removes C-Style and C++ Style comments from an xlts string, useful when the tokenizer that next parses the string does not support comments. */
int dct_init(void);
void dct_cleanup(void);

int dct_decomment(struct xlts *val);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* DECOMMENT_H */
