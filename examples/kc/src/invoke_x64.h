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

#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED
#include "types.h"
#endif

#ifndef EXPR_H_INCLUDED
#define EXPR_H_INCLUDED
#include "expr.h"
#endif

struct c_compiler;

void invoke_init(void);
void invoke_cleanup(void);
void *invoke_alloc_function_entry(struct c_compiler *cc, struct func_def *fd);

int invoke_x64(struct c_compiler *cc, struct expr *x, struct expr_temp *temps);
