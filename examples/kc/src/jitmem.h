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

#ifndef JITMEM_H
#define JITMEM_H

#ifndef STDLIB_H_INCLUDED
#define STDLIB_H_INCLUDED
#include <stdlib.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct c_compiler;

void jitmem_init(void);
void jitmem_cleanup(void);

/* Reserves num_bytes of JIT memory and returns a pointer to it.
 * Note that this is a *reservation*; consecutive jitmem_reserve()
 * calls will return the same pointer! It still needs to be
 * acquired using jitmem_acquire().
 * jitmem_reserve is useful if the JIT code is not position 
 * independant but needs to know the pointer of the memory.
 * Note that the returned pointer is write-protected and executable,
 * you should use jitmem_acquire() to copy data into jit memory.
 */
void *jitmem_reserve(struct c_compiler *cc, size_t num_bytes);

/* Allocates and returns num_bytes of jit memory, if the num_bytes
 * value is the same, then it returns the same pointer as any
 * preceeding jitmem_reserve() call. Calling jitmem_reserve() before
 * jitmem_acquire() is however optional, and only required if you 
 * need to know the pointer before initializing the data at it.
 * The memory returned is initialized with num_bytes of data at the
 * data pointer.
 * Note that the returned pointer is write-protected and executable.
 */
void *jitmem_acquire(struct c_compiler *cc, size_t num_bytes, const void *data);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* JITMEM_H */

