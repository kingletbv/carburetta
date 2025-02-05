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

#ifndef FUNC_DEF_H_INCLUDED
#define FUNC_DEF_H_INCLUDED
#include "func_def.h"
#endif

#ifndef C_COMPILER_H_INCLUDED
#define C_COMPILER_H_INCLUDED
#include "c_compiler.h"
#endif

#ifndef DECL_H_INCLUDED
#define DECL_H_INCLUDED
#include "decl.h"
#endif

#ifndef STMT_H_INCLUDED
#define STMT_H_INCLUDED
#include "stmt.h"
#endif

struct func_def *func_def_alloc(struct name_space *ns, int *already_exists, const char *ident, struct situs *ident_loc, sc_storage_class_t sc, int function_specifiers, struct type_node *tn) {
  struct func_def *fd = (struct func_def *)decl_alloc_sized(ns, already_exists, ident, ident_loc, sc, function_specifiers, tn, NULL, sizeof(struct func_def));

  fd->body_ = NULL;
  fd->stackframe_size_required_ = 0;
  fd->parameter_size_required_ = 0;

  return fd;
}


// XXX: We want to mimick consecutive pushes of variables onto the stack; the stack, however,
// grows in the negative direction. Consequently, rather than "align offset, assign offset, add size"
// we should do "add size, align offset, assign offset"
// Consequently, if the offset is 0, and we process a uint32_t, then the assigned offset is 4 (pointing
// to the end of the uint32_t.)
// If we next process a uint64_t, then the assigned offset is align64(4 + 8) = 16, pointing to the end
// of the uint64_t.
//
// Having done this, we take the stackframe_size_required_, align it to 16 bytes, and, for each field thus assigned, we re-assign
// the field as new_offset = stackframe_size_required_ - old_offset.
//
// This is messy. Do we really want to do this?


static int func_def_realize_block_stmt(struct c_compiler *cc, struct func_def *fd, struct stmt *s, uint64_t current_offset) {
  if (!s) return -1;

  if (s->ns_) {
    struct decl *d;
    d = (struct decl *)s->ns_->ordinary_idents_.seq_;
    if (d) {
      do {
        /* Is this declaration a variable? */
        if (!d->is_param_ && !d->is_enum_) {
          uint64_t size = type_node_size(&cc->tb_, d->type_);
          if (size && ((d->sc_ == SC_NONE) || (d->sc_ == SC_AUTO) || (d->sc_ == SC_REGISTER))) {
            /* Align current_offset to alignment requirements of d
             * Alignments are always a power of 2 */
            uint64_t alignment = type_node_align(&cc->tb_, d->type_);
            current_offset = (current_offset + alignment - 1) & ~(alignment - 1);

            /* Assign the current offset and increment it by its size */
            d->local_offset_ = current_offset;

            current_offset += size;

            /* Check if we are expanding the stackframe */
            if (fd->stackframe_size_required_ < current_offset) {
              fd->stackframe_size_required_ = current_offset;
            }
          }
        }

        d = (struct decl *)d->sym_.next_;
      } while (d != (struct decl *)s->ns_->ordinary_idents_.seq_);
    }
  }

  struct stmt *child;
  child = s->child0_;
  if (child) {
    do {
      if (child->type_ == ST_BLOCK) {
        int r = func_def_realize_block_stmt(cc, fd, child, current_offset);
        if (r) return r;
      }

      if (child->child0_ && child->child0_->type_ == ST_BLOCK) {
        int r = func_def_realize_block_stmt(cc, fd, child->child0_, current_offset);
        if (r) return r;
      }

      if (child->child1_ && child->child1_->type_ == ST_BLOCK) {
        int r = func_def_realize_block_stmt(cc, fd, child->child1_, current_offset);
        if (r) return r;
      }

      child = child->next_;
    } while (child != s->child0_);
  }

  return 0;
}

int func_def_realize_locals(struct c_compiler *cc, struct func_def *fd) {
  /* Iterate through the statements and assign offsets to each declared variable */
  struct stmt *s;
  s = fd->body_;
  if (!s) return -1;

  int r;
  
  /* Realize all locals starting at offset 0 */
  fd->stackframe_size_required_ = 0;
  r = func_def_realize_block_stmt(cc, fd, s, 0);
  if (r) return r;

  /* Align stackframe size to multiple of 16 bytes */
  fd->stackframe_size_required_ = (fd->stackframe_size_required_ + 15) & ~15;

  return 0;
}
