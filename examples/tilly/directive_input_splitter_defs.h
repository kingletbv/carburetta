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

#ifndef DIRECTIVE_INPUT_SPLITTER_DEFS_H
#define DIRECTIVE_INPUT_SPLITTER_DEFS_H

#ifndef SITUS_H_INCLUDED
#define SITUS_H_INCLUDED
#include "situs.h"
#endif

// returned by scanner if the current line is a directive line (starts with %)
#define DIS_DIRECTIVE_LINE_AVAILABLE -1

// returned by scanner if the current line is not a directive line (starts with something other than % or is empty.)
#define DIS_SECTION_LINE_AVAILABLE -2

#endif /* DIRECTIVE_INPUT_SPLITTER_DEFS_H */
