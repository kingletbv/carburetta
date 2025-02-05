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

#ifndef DYNAMIC_RUNTIME_LINKING_H
#define DYNAMIC_RUNTIME_LINKING_H

#ifdef __cplusplus
extern "C" {
#endif

/* Resolving symbols at runtime when running C code interpretively */

struct c_compiler;
struct decl;

int drl_resolve_sym(struct c_compiler *cc, struct decl *d, void **ptr);
int drl_register_sym(struct c_compiler *cc, const char *link_name, void *ptr);

#ifdef __cplusplus
}
#endif

#endif /* DYNAMIC_RUNTIME_LINKING_H */
