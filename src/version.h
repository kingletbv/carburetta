/* Copyright 2020-2024 Kinglet B.V.
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

#ifndef VERSION_H
#define VERSION_H

#ifdef __cplusplus
extern "C" {
#endif

#define CARBURETTA_MAJOR_VER 0
#define CARBURETTA_MINOR_VER 8
#define CARBURETTA_PATCH_VER 22

#define CARBURETTA_STRINGIFY_i(x) #x
#define CARBURETTA_STRINGIFY(x) CARBURETTA_STRINGIFY_i(x)

#if CARBURETTA_PATCH_VER & 1
// Odd patch version means development version
#define CARBURETTA_VERSION_STR CARBURETTA_STRINGIFY(CARBURETTA_MAJOR_VER) "." CARBURETTA_STRINGIFY(CARBURETTA_MINOR_VER) "." CARBURETTA_STRINGIFY(CARBURETTA_PATCH_VER) " (development)"
#else
#define CARBURETTA_VERSION_STR CARBURETTA_STRINGIFY(CARBURETTA_MAJOR_VER) "." CARBURETTA_STRINGIFY(CARBURETTA_MINOR_VER) "." CARBURETTA_STRINGIFY(CARBURETTA_PATCH_VER)
#endif

#define CARBURETTA_COPYRIGHT_STR "(C) 2020-2024 Kinglet B.V."

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* VERSION_H */
