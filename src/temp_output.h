/* Copyright 2020-2025 Kinglet B.V.
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

#ifndef TEMP_OUTPUT_H
#define TEMP_OUTPUT_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef STDIO_H_INCLUDED
#define STDIO_H_INCLUDED
#include <stdio.h>
#endif

FILE *to_make_temp(const char *final_destination_file, char **temp_filenamep);

char *to_derive_temp_name(const char *final_destination_name);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* TEMP_OUTPUT_H */
