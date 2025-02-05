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


#ifndef _MSC_VER
/* We need the declaration for vasprintf() which is a GNU extension,
 * this instructs clang to include the declaration. */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#endif

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

#ifdef _WIN32
#include <tchar.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include "helpers.h"

#ifdef _MSC_VER
std::string mstrprintf(const char *format, ...) {
  std::string s;
  va_list args;
  va_start(args, format);
  int len = _vscprintf(format, args);
  s.resize(len + 1); /* +1 to accommodate null terminator */
  vsprintf(&s[0], format, args);
  s.resize(len); /* -1 to remove null terminator from the string (std::string will have its own null terminator. */
  va_end(args);
  return s;
}
#else
std::string mstrprintf(const char *format, ...) {
  va_list args;
  va_start(args, format);
  char *p;
  int len = vasprintf(&p, format, args);
  va_end(args);
  if (len == -1) {
    return std::string("");
  }
  std::string s(p, len);
  free(p);
  return s;
}
#endif

#ifdef _TCHAR_DEFINED
std::string tstr2str(const TCHAR *tsrc) {
  char *s = tstr2acstr(tsrc);
  std::string str(s);
  free(s);
  return str;
}

TCHAR *str2tstr(const std::string &str) {
  return cstr2atstr(str.c_str());
}

freeing_tstr str2ftstr(const std::string &ftstr) {
  freeing_tstr f;
  f.s = str2tstr(ftstr);
  return f; /* Rely on move ellision to not call destructor here.. */
}
#endif /* _TCHAR_DEFINED */
