/* Copyright 2020 Kinglet B.V.
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

#ifndef MUL_H
#define MUL_H

#ifndef STDDEF_H_INCLUDED
#define STDDEF_H_INCLUDED
#include <stddef.h> /* size_t */
#endif

#ifndef STDINT_H_INCLUDED
#define STDINT_H_INCLUDED
#include <stdint.h> /* uint64_t */
#endif

#ifdef __cplusplus
extern "C" {
#endif

int multiply_uint64(uint64_t a, uint64_t b, uint64_t *rhi, uint64_t *rlo);
int multiply_size_t(size_t a, size_t b, size_t *rhi, size_t *rlo);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* MUL_H */
