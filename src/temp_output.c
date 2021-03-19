/* Copyright 2020-2021 Kinglet B.V.
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

#ifndef STDLIB_H_INCLUDED
#define STDLIB_H_INCLUDED
#include <stdlib.h>
#endif

#ifndef STDIO_H_INCLUDED
#define STDIO_H_INCLUDED
#include <stdio.h>
#endif

#ifndef STDINT_H_INCLUDED
#define STDINT_H_INCLUDED
#include <stdint.h>
#endif

#ifndef ERRNO_H_INCLUDED
#define ERRNO_H_INCLUDED
#include <errno.h>
#endif

#ifndef STRING_H_INCLUDED
#define STRING_H_INCLUDED
#include <string.h>
#endif

#ifdef _WIN32
#ifndef IO_H_INCLUDED
#define IO_H_INCLUDED
#include <io.h> /* _open_osfhandle */
#endif
#ifndef WINDOWS_H_INCLUDED
#define WINDOWS_H_INCLUDED
#include <Windows.h>
#endif
#endif

#ifdef __linux__
#ifndef SYS_TYPES_H_INCLUDED
#define SYS_TYPES_H_INCLUDED
#include <sys/types.h>
#endif
#ifndef SYS_STAT_H_INCLUDED
#define SYS_STAT_H_INCLUDED
#include <sys/stat.h>
#endif
#ifndef UNISTD_H_INCLUDED
#define UNISTD_H_INCLUDED
#include <unistd.h>
#endif
#ifndef FCNTL_H_INCLUDED
#define FCNTL_H_INCLUDED
#include <fcntl.h>
#endif
#endif /* __linux__ */

#ifndef TEMP_OUTPUT_H_INCLUDED
#define TEMP_OUTPUT_H_INCLUDED
#include "temp_output.h"
#endif

static int to_get_random_bytes(void *buf, size_t num_bytes) {
#ifdef __linux__
  FILE *fp = fopen("/dev/urandom", "rb");
  if (!fp) {
    return errno;
  }
  if (fp) {
    if (num_bytes != fread(buf, 1, num_bytes, fp)) {
      fclose(fp);
      return errno;
    }
  }
  fclose(fp);
  return 0;
#elif defined(_WIN32)
  HCRYPTPROV prov;
  if (!CryptAcquireContext(&prov, NULL, NULL, PROV_RSA_FULL, 0)) {
    if (NTE_BAD_KEYSET == GetLastError()) {
      if (!CryptAcquireContext(&prov, NULL, NULL, PROV_RSA_FULL, CRYPT_NEWKEYSET)) {
        prov = 0;
        return GetLastError();
      }
      /* ok */
    }
    else {
      return GetLastError();
    }
  }
  if (!CryptGenRandom(prov, (DWORD)num_bytes, (BYTE *)buf)) {
    return GetLastError();
  }

  CryptReleaseContext(prov, 0);

  return ERROR_SUCCESS;
#else
#error Implement non-win32/posix version
#endif
}

char *to_derive_temp_name(const char *final_destination_name) {
  char random_digits_5bit[] = "abcdefghijklmnopqrstuvwxyz123456";
  unsigned char random_bin[5] = { 0 };
  int r = 0;
  r = to_get_random_bytes(random_bin, sizeof(random_bin));
  if (r) return NULL;
  char random_str[8 + 1] = {0};
  int n;
  /* Split 5 bytes of random bits into 8 packets of 5 bits and convert to characters */
  for (n = 0; n < (sizeof(random_str) - 1); ++n) {
    int offset = n * 5;
    int v = random_bin[offset / 8];
    v = (v << 8) + random_bin[1 + offset / 8];

    int val = (31 & (v >> (16 - 5 - (offset & 7))));
    random_str[n] = random_digits_5bit[val];
  }
  char *s = (char *)malloc(1 + strlen(final_destination_name) + sizeof(random_str));
  if (!s) return NULL;
  strcpy(s, final_destination_name);
  strcat(s, random_str);
  return s;
}

FILE *to_make_temp(const char *final_destination_file, char **temp_filenamep) {
  size_t attempt;
  size_t last_attempt = 49;
  for (attempt = 0; attempt < last_attempt + 1; attempt++) {
    char *temp_filename = to_derive_temp_name(final_destination_file);
    if (!temp_filename) {
      fprintf(stderr, "Failed to create temporary filename based on \"%s\"\n", final_destination_file);
      return NULL;
    }

#ifdef _WIN32
    HANDLE h = CreateFileA(temp_filename, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
    if (INVALID_HANDLE_VALUE == h) {
      /* keep going; we're assuming a collision, which may be naive */
      free(temp_filename);
      continue;
    }
    int osh = _open_osfhandle((intptr_t)h, 0);
    if (osh == -1) {
      /* Failed */
      CloseHandle(h);
      DeleteFileA(final_destination_file);
      free(temp_filename);
      continue;
    }

    FILE *fp = _fdopen(osh, "wb");
    if (!fp) {
      /* Failed */
      _close(osh);
      CloseHandle(h);
      DeleteFileA(final_destination_file);
      free(temp_filename);
      continue;
    }
#endif
#ifdef __linux__
    int fd = open(temp_filename, O_CREAT|O_EXCL|O_RDWR, 0664);
    if (fd == -1) {
      if (attempt == last_attempt) {
        int err = errno;
        fprintf(stderr, "Failed to open temp file \"%s\": %s\n", temp_filename, strerror(err));
      }
      free(temp_filename);
      continue;
    }
    FILE *fp = fdopen(fd, "wb");
    if (!fp) {
      if (attempt == last_attempt) {
        int err = errno;
        fprintf(stderr, "Failed to open temp file \"%s\": %s\n", temp_filename, strerror(err));
      }
      close(fd);
      remove(temp_filename);
      free(temp_filename);
      continue;
    }
#endif
    if (temp_filenamep) {
      *temp_filenamep = temp_filename;
    }
    else {
      free(temp_filename);
    }
    return fp;
  }

  /* Failed */
  return NULL;
}
