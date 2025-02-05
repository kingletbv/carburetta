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

#ifndef WINDOWS_H_INCLUDED
#define WINDOWS_H_INCLUDED
#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#ifndef STDLIB_H_INCLUDED
#define STDLIB_H_INCLUDED
#include <stdlib.h>
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

#ifndef EXPR_H_INCLUDED
#define EXPR_H_INCLUDED
#include "expr.h"
#endif

#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED
#include "types.h"
#endif

#ifndef FUNC_DEF_H_INCLUDED
#define FUNC_DEF_H_INCLUDED
#include "func_def.h"
#endif

#ifndef JITMEM_H_INCLUDED
#define JITMEM_H_INCLUDED
#include "jitmem.h"
#endif

#ifndef INVOKE_X64_H_INCLUDED
#define INVOKE_X64_H_INCLUDED
#include "invoke_x64.h"
#endif

/* should match STRUCT in invoke_call_x64.asm */
struct invoke_context {
  uint64_t rcx_, rdx_, r8_, r9_;
  double xmm0_, xmm1_, xmm2_, xmm3_;
  void *fnptr_;
  uint64_t rax_result_;
  uint64_t hspace_size_;
  void *hspace_;
};

/* implemented in invoke_call_x64.asm */
extern uint64_t invoke_call_x64(struct invoke_context *ic);

size_t count_args(struct expr *args) {
  if (!args) return 0;
  if (args->et_ == ET_FUNCTION_ARG_LIST) {
    return count_args(args->children_[0]) + count_args(args->children_[1]);
  }
  return 1;
}

void invoke_init(void) {
}

void invoke_cleanup(void) {
}

/* defined in invoke_call_x64.asm */
uint64_t enter_call_x64(uint64_t a, uint64_t b);

void *invoke_alloc_function_entry(struct c_compiler *cc, struct func_def *fd) {
  uint8_t trampoline_template[] = {
                                  //49 BA 00 00 00 00 00 00 00 00   mov r10, 0x0000000000000000h
                                  0x49, 0xBA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                  //49 BB 00 00 00 00 00 00 00 00   mov r11, 0x0000000000000000h
                                  0x49, 0xBB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                  //66 40 68 00 00                     push imm16  ; REX.W operand size to 0, legacy prefix 66 to get 16 bits... There is no push imm32 or push imm64 (the imm32 is sign-extended to 64)
                                  0x66, 0x40, 0x68, 0x00, 0x00,
                                  0x66, 0x40, 0x68, 0x00, 0x00,
                                  0x66, 0x40, 0x68, 0x00, 0x00,
                                  0x66, 0x40, 0x68, 0x00, 0x00,
                                  //C3                              ret
                                  0xC3
  };
  void *p = jitmem_reserve(cc, sizeof(trampoline_template));
  if (!p) return NULL;
  *((uint64_t *)(trampoline_template + 2)) = (uint64_t)fd;
  *((uint64_t *)(trampoline_template + 12)) = (uint64_t)cc;
  uint64_t tgt = (uint64_t)enter_call_x64;
  *((uint16_t *)(trampoline_template + 23)) = (uint16_t)(tgt >> 48);
  *((uint16_t *)(trampoline_template + 28)) = (uint16_t)(tgt >> 32);
  *((uint16_t *)(trampoline_template + 33)) = (uint16_t)(tgt >> 16);
  *((uint16_t *)(trampoline_template + 38)) = (uint16_t)(tgt);

  return jitmem_acquire(cc, sizeof(trampoline_template), trampoline_template);
}

uint64_t invoke_enter_call(struct invoke_context *ic, void *rsp, struct func_def *fd, struct c_compiler *cc) {
#if 0
  cc_printf(cc, "Invoked function: %s\n", fd->d_.sym_.ident_);
  cc_printf(cc, "   first argument %08" PRIx64 "\n", ic->rcx_);
  cc_printf(cc, "  second argument %08" PRIx64 "\n", ic->rdx_);
  cc_printf(cc, "   third argument %08" PRIx64 "\n", ic->r8_);
  cc_printf(cc, "   Stackpointer   %08" PRIx64 "\n", (uint64_t)rsp);
#endif
  void *stack_frame = NULL;
  if (fd->stackframe_size_required_) {
    stack_frame = malloc(fd->stackframe_size_required_);
    if (!stack_frame) {
      return (uint64_t)-1;
    }
  }
  
  void *param_frame = NULL;
  /* Skip over CALL return instruction pointer */
  param_frame = ((char *)rsp) + 8;

  ic->rax_result_ = 0;

  int r;
  r = stmt_exec(cc, fd->body_, stack_frame, param_frame, &ic->rax_result_);
#if 0
  if (r) {
    cc_printf(cc, "warning: stmt_exec() for \"%s\" returned: %d\n", fd->d_.sym_.ident_, r);
  }

  cc_printf(cc, "           result %08" PRIx64 "\n", ic->rax_result_);
#endif
  if (stack_frame) {
    free(stack_frame);
  }

  return (uint64_t)-1;
}


size_t fill_args(struct c_compiler *cc, struct invoke_context *ic, size_t arg_index, struct expr *args, struct expr_temp *temps) {
  if (!args) return arg_index;
  if (args->et_ == ET_FUNCTION_ARG_LIST) {
    arg_index = fill_args(cc, ic, arg_index, args->children_[0], temps);
    arg_index = fill_args(cc, ic, arg_index, args->children_[1], temps);
    return arg_index;
  }

#define PROCESS_ARG(x, ireg, dreg) \
  case x: { \
    struct type_node *tn = expr_type(cc, args); \
    if (!tn) { \
      return arg_index + 1; \
    } \
    tn = type_node_unqualified(tn); \
    if ((tn->kind_ == tk_float) || (tn->kind_ == tk_double)) { \
      /* XXX: Always passing as double *this is a problem* */ \
      ic->dreg = temps[args->ord_].v_.d_; \
      ic->ireg = *(uint64_t *)&temps[args->ord_].v_.d_; \
      *(double *)(((uint64_t *)ic->hspace_) + arg_index) = temps[args->ord_].v_.d_; \
    } \
    else { \
      ic->ireg = temps[args->ord_].v_.u64_; \
      *(((uint64_t *)ic->hspace_) + arg_index) = temps[args->ord_].v_.u64_; \
    } \
    break;\
  }
  switch (arg_index) {
    PROCESS_ARG(0, rcx_, xmm0_)
    PROCESS_ARG(1, rdx_, xmm1_)
    PROCESS_ARG(2, r8_, xmm2_)
    PROCESS_ARG(3, r9_, xmm3_)
    default: {
      /* Beyond 4th argument */
      struct type_node *tn = expr_type(cc, args);
      if (!tn) {
        return arg_index + 1;
      }
      tn = type_node_unqualified(tn);
      if ((tn->kind_ == tk_float) || (tn->kind_ == tk_double)) {
        /* XXX: Always passing as double *this is a problem* */
        *(double *)(((uint64_t *)ic->hspace_) + arg_index) = temps[args->ord_].v_.d_;
      }
      else {
        *(((uint64_t *)ic->hspace_) + arg_index) = temps[args->ord_].v_.u64_;
      }
    }
  }
#undef PROCESS_ARG
  return arg_index + 1;
}

int invoke_x64(struct c_compiler *cc, struct expr *x, struct expr_temp *temps) {
  // XXX: Use temps for access!
  /* Type of x must be pointer to function... */
  struct type_node *func_type;
  func_type = type_node_unqualified(expr_type(cc, x->children_[0]));
  if (!func_type) {
    return -1;
  }
  assert(func_type->kind_ == tk_pointer);
  func_type = type_node_unqualified(func_type->derived_from_);
  assert(func_type->kind_ == tk_function);

  size_t num_args = count_args(x->children_[1]);

  uint64_t hspace_size;
  if (num_args <= 4) {
    hspace_size = 4 * sizeof(uint64_t);
  }
  else {
    hspace_size = ((num_args + 1) & ~(uint64_t)1) * sizeof(uint64_t);
  }

  struct invoke_context *ic;
  ic = (struct invoke_context *)_alloca(sizeof(struct invoke_context) + hspace_size);

  ic->fnptr_ = (void *)temps[x->children_[0]->ord_].v_.u64_;
  ic->rcx_ = 0;
  ic->rdx_ = 0;
  ic->r8_ = 0;
  ic->r9_ = 0;
  ic->xmm0_ = 0.;
  ic->xmm1_ = 0.;
  ic->xmm2_ = 0.;
  ic->xmm3_ = 0.;
  ic->hspace_size_ = hspace_size;
  ic->hspace_ = (void *)(ic + 1);
  ic->rax_result_ = 0;

  fill_args(cc, ic, 0, x->children_[1], temps);
  invoke_call_x64(ic);

  temps[x->ord_].v_.u64_ = ic->rax_result_;

  return 0;
}


