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

#ifdef _WIN32
#ifndef WINDOWS_H_INCLUDED
#define WINDOWS_H_INCLUDED
#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif
#else
#ifndef SYS_MMAN_H_INCLUDED
#define SYS_MMAN_H_INCLUDED
#include <sys/mman.h>
#endif

#ifndef ERRNO_H_INCLUDED
#define ERRNO_H_INLCUDED
#include <errno.h>
#endif

#ifndef UNISTD_H_INCLUDED
#define UNISTD_H_INCLUDED
#include <unistd.h>
#endif
#endif

#ifndef STDLIB_H_INCLUDED
#define STDLIB_H_INCLUDED
#include <stdlib.h>
#endif

#ifndef STRING_H_INCLUDED
#define STRING_H_INCLUDED
#include <string.h>
#endif

#ifndef ASSERT_H_INCLUDED
#define ASSERT_H_INCLUDED
#include <assert.h>
#endif

#ifndef INTTYPES_H_INCLUDED
#define INTTYPES_H_INCLUDED
#include <inttypes.h>
#endif

#ifndef C_COMPILER_H_INCLUDED
#define C_COMPILER_H_INCLUDED
#include "c_compiler.h"
#endif

#ifndef JITMEM_H_INCLUDED
#define JITMEM_H_INCLUDED
#include "jitmem.h"
#endif

struct jitmem_bucket {
  struct jitmem_bucket *next_;
  size_t size_;
  void *page_data_;
};

static struct jitmem_bucket *g_jitmem_buckets_ = NULL;
static size_t g_page_data_size_ = 0;
static size_t g_jitmem_bucket_offset_ = 0;

void jitmem_init(void) {
#ifdef _WIN32
  SYSTEM_INFO si;
  GetSystemInfo(&si);
  g_page_data_size_ = si.dwAllocationGranularity > si.dwPageSize ? si.dwAllocationGranularity : si.dwPageSize;
#else
  int page_size = getpagesize();
  g_page_data_size_ = (size_t)page_size;
#endif
  g_jitmem_bucket_offset_ = 0;
}

void jitmem_cleanup(void) {
  while (g_jitmem_buckets_) {
    struct jitmem_bucket *doomed = g_jitmem_buckets_;
    g_jitmem_buckets_ = doomed->next_;
#ifdef _WIN32
    VirtualFree(doomed->page_data_, 0, MEM_RELEASE);
#else
    munmap(doomed->page_data_, doomed->size_);
#endif
    free(doomed);
  }
}

void *jitmem_reserve(struct c_compiler *cc, size_t num_bytes) {
  if (!g_jitmem_buckets_ || ((g_jitmem_bucket_offset_ + num_bytes) > g_jitmem_buckets_->size_)) {
    size_t page_data_size_needed = g_page_data_size_;
    if (page_data_size_needed < num_bytes) {
      page_data_size_needed = (num_bytes + g_page_data_size_ - 1) & ~g_page_data_size_;
    }
    struct jitmem_bucket *bkt = (struct jitmem_bucket *)malloc(sizeof(struct jitmem_bucket));
    if (!bkt) {
      cc_printf(cc, "no memory\n");
      return NULL;
    }
    bkt->next_ = g_jitmem_buckets_;
    bkt->size_ = page_data_size_needed;
#ifdef _WIN32
    bkt->page_data_ = VirtualAlloc(NULL, bkt->size_, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READ);
    if (!bkt->page_data_) {
      cc_printf(cc, "VirtualAlloc() failed\n");
      free(bkt);
      return NULL;
    }
#else
    bkt->page_data_ = mmap(NULL, bkt->size_, PROT_READ | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (bkt->page_data_ == (void *)-1) {
      int err = errno;
      cc_printf(cc, "mmap failed: %s\n", strerror(err));
      free(bkt);
      return NULL;
    }
#endif
    g_jitmem_buckets_ = bkt;
    g_jitmem_bucket_offset_ = 0;
  }
  void *ptr = ((char *)g_jitmem_buckets_->page_data_) + g_jitmem_bucket_offset_;
  return ptr;
}

void *jitmem_acquire(struct c_compiler *cc, size_t num_bytes, const void *data) {
  void *p = jitmem_reserve(cc, num_bytes);
  if (!p) return NULL;
  /* p is guaranteed to be in the bucket in g_trampoline_buckets_ */
#ifdef _WIN32
  DWORD old_protection = 0;
  BOOL rc;
  rc = VirtualProtect(g_jitmem_buckets_->page_data_, g_jitmem_buckets_->size_, PAGE_EXECUTE_READWRITE, &old_protection);
  if (!rc) {
    cc_printf(cc, "VirtualProtect failed\n");
    return NULL;
  }
#else
  int r;
  r = mprotect(g_jitmem_buckets_->page_data_, g_jitmem_buckets_->size_, PROT_READ | PROT_WRITE | PROT_EXEC);
  if (r == -1) {
    int err = errno;
    cc_printf(cc, "mprotect failed: %s\n", strerror(err));
    return NULL;
  }
#endif
  memcpy(p, data, num_bytes);
  /* lock it up again. */
#ifdef _WIN32
  old_protection = 0;
  rc = VirtualProtect(g_jitmem_buckets_->page_data_, g_jitmem_buckets_->size_, PAGE_EXECUTE_READ, &old_protection);
  if (!rc) {
    cc_printf(cc, "VirtualProtect failed\n");
    return NULL;
  }
#else
  r = mprotect(g_jitmem_buckets_->page_data_, g_jitmem_buckets_->size_, PROT_READ | PROT_EXEC);
  if (r == -1) {
    int err = errno;
    cc_printf(cc, "mprotect (2nd) failed: %s\n", strerror(err));
    return NULL;
  }
#endif
  g_jitmem_bucket_offset_ += num_bytes;
  return p;
}

