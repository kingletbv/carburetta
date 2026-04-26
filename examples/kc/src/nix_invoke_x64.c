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

#ifndef ALLOCA_H_INCLUDED
#define ALLOCA_H_INCLUDED
#include <alloca.h>
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

#ifndef DECL_H_INCLUDED
#define DECL_H_INCLUDED
#include "decl.h"
#endif

#ifndef JITMEM_H_INCLUDED
#define JITMEM_H_INCLUDED
#include "jitmem.h"
#endif

#ifndef INVOKE_X64_H_INCLUDED
#define INVOKE_X64_H_INCLUDED
#include "invoke_x64.h"
#endif


/* should match STRUCT in invoke_call_x64.s */
struct invoke_context {
  uint64_t rdi_, rsi_, rdx_, rcx_, r8_, r9_;
  uint64_t rax_in_; /* used to pass number of xmm args in System V AMD64 ABI */
  double xmm0_, xmm1_, xmm2_, xmm3_, xmm4_, xmm5_, xmm6_, xmm7_;
  void *fnptr_;
  uint64_t rax_result_;
  uint64_t hspace_size_;
  void *hspace_;
};

/* implemented in invoke_call_x64.s */
extern uint64_t invoke_call_x64(struct invoke_context *ic);

void invoke_init(void) {
}

void invoke_cleanup(void) {
}

size_t count_args(struct expr *args) {
  if (!args) return 0;
  if (args->et_ == ET_FUNCTION_ARG_LIST) {
    return count_args(args->children_[0]) + count_args(args->children_[1]);
  }
  return 1;
}

/* defined in nix_invoke_call_x64.s */
uint64_t enter_call_x64(uint64_t a, uint64_t b);

void *invoke_alloc_function_entry(struct c_compiler *cc, struct decl *fd) {
  uint8_t trampoline_template[] = {
                                  //49 BA 00 00 00 00 00 00 00 00   mov r10, 0x0000000000000000h
                                  0x49, 0xBA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                  //49 BB 00 00 00 00 00 00 00 00   mov r11, 0x0000000000000000h
                                  0x49, 0xBB, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                  //66 40 68 00 00                  push imm16  ; REX.W operand size to 0, legacy prefix 66 to get 16 bits... There is no push imm32 or push imm64 (the imm32 is sign-extended to 64)
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

uint64_t invoke_enter_call(struct invoke_context *ic, void *rsp, struct decl *fd, struct c_compiler *cc) {
#if 0
  cc_printf(cc, "Invoked function: %s\n", fd->sym_.ident_);
  cc_printf(cc, "   first argument %08" PRIx64 "\n", ic->rdi_);
  cc_printf(cc, "  second argument %08" PRIx64 "\n", ic->rsi_);
  cc_printf(cc, "   third argument %08" PRIx64 "\n", ic->rdx_);
  cc_printf(cc, "   Stackpointer   %08" PRIx64 "\n", (uint64_t)rsp);
#endif
  void *stack_frame = NULL;
  if (fd->stackframe_size_required_) {
    stack_frame = malloc(fd->stackframe_size_required_);
    if (!stack_frame) {
      return (uint64_t)-1;
    }
  }

  /* Build param_frame according to System V AMD64 ABI register assignment.
   * Integer args use rdi, rsi, rdx, rcx, r8, r9 (6 registers).
   * Float/double args use xmm0-xmm7 (8 registers).
   * Additional args are passed on the caller's stack. */
  size_t num_params = 0;
  struct type_param *param_chain = fd->type_->params_;
  if (param_chain) {
    struct type_param *p = param_chain;
    do {
      p = p->chain_;
      if (!p->is_varadic_) num_params++;
    } while (p != param_chain);
  }

  void *param_frame = NULL;
  if (num_params) {
    param_frame = alloca(sizeof(uint64_t) * num_params);

    size_t int_reg_idx = 0;
    size_t float_reg_idx = 0;
    size_t param_idx = 0;
    /* Stack-passed args start past saved rbp and return address */
    uint64_t *stack_args = (uint64_t *)((char *)rsp + 16);
    size_t stack_arg_idx = 0;

    struct type_param *p = param_chain;
    do {
      p = p->chain_;
      if (p->is_varadic_) break;

      struct type_node *tn = p->type_ ? type_node_unqualified(p->type_) : NULL;
      int is_float_param = 0;

      if (tn) {
        /* Structs, arrays, and large types are passed by pointer (integer class) */
        uint64_t param_size = type_node_size(&cc->tb_, p->type_);
        if ((param_size <= sizeof(uint64_t)) &&
            !type_node_is_struct_or_union(p->type_) &&
            !type_node_is_array(p->type_)) {
          is_float_param = (tn->kind_ == tk_float) || (tn->kind_ == tk_double);
        }
      }

      if (is_float_param) {
        if (float_reg_idx < 8) {
          double *xmm_src = NULL;
          switch (float_reg_idx) {
            case 0: xmm_src = &ic->xmm0_; break;
            case 1: xmm_src = &ic->xmm1_; break;
            case 2: xmm_src = &ic->xmm2_; break;
            case 3: xmm_src = &ic->xmm3_; break;
            case 4: xmm_src = &ic->xmm4_; break;
            case 5: xmm_src = &ic->xmm5_; break;
            case 6: xmm_src = &ic->xmm6_; break;
            case 7: xmm_src = &ic->xmm7_; break;
          }
          float_reg_idx++;
          ((uint64_t *)param_frame)[param_idx] = 0;
          if (tn->kind_ == tk_float) {
            /* Float: lower 32 bits of xmm register contain the IEEE 754 float */
            *(float *)(((uint64_t *)param_frame) + param_idx) = *(float *)xmm_src;
          } else {
            *(double *)(((uint64_t *)param_frame) + param_idx) = *xmm_src;
          }
        } else {
          ((uint64_t *)param_frame)[param_idx] = stack_args[stack_arg_idx++];
        }
      } else {
        if (int_reg_idx < 6) {
          uint64_t val = 0;
          switch (int_reg_idx) {
            case 0: val = ic->rdi_; break;
            case 1: val = ic->rsi_; break;
            case 2: val = ic->rdx_; break;
            case 3: val = ic->rcx_; break;
            case 4: val = ic->r8_; break;
            case 5: val = ic->r9_; break;
          }
          int_reg_idx++;
          ((uint64_t *)param_frame)[param_idx] = val;
        } else {
          ((uint64_t *)param_frame)[param_idx] = stack_args[stack_arg_idx++];
        }
      }

      param_idx++;
    } while (p != param_chain);
  }

  ic->rax_result_ = 0;

  int r;
  r = stmt_exec(cc, fd->body_, stack_frame, param_frame, &ic->rax_result_);
#if 0
  if (r) {
    cc_printf(cc, "warning: stmt_exec() for \"%s\" returned: %d\n", fd->sym_.ident_, r);
  }

  cc_printf(cc, "           result %08" PRIx64 "\n", ic->rax_result_);
#endif

  if (stack_frame) {
    free(stack_frame);
  }

  return (uint64_t)-1;
}


/* System V AMD64 ABI: integer and floating-point arguments consume registers
 * from independent banks. Integer args: rdi, rsi, rdx, rcx, r8, r9 (6).
 * Float/double args: xmm0-xmm7 (8). Overflow goes to the stack (hspace). */
struct fill_state {
  size_t int_reg_idx;
  size_t float_reg_idx;
  size_t stack_offset;
  size_t num_xmm_args;
};

static void fill_args(struct c_compiler *cc, struct invoke_context *ic, struct fill_state *fs, struct expr *args, struct expr_temp *temps) {
  if (!args) return;
  if (args->et_ == ET_FUNCTION_ARG_LIST) {
    fill_args(cc, ic, fs, args->children_[0], temps);
    fill_args(cc, ic, fs, args->children_[1], temps);
    return;
  }

  struct type_node *tn = expr_type(cc, args);
  if (!tn) return;
  tn = type_node_unqualified(tn);

  if ((tn->kind_ == tk_float) || (tn->kind_ == tk_double)) {
    if (fs->float_reg_idx < 8) {
      double *xmm_slot = NULL;
      switch (fs->float_reg_idx) {
        case 0: xmm_slot = &ic->xmm0_; break;
        case 1: xmm_slot = &ic->xmm1_; break;
        case 2: xmm_slot = &ic->xmm2_; break;
        case 3: xmm_slot = &ic->xmm3_; break;
        case 4: xmm_slot = &ic->xmm4_; break;
        case 5: xmm_slot = &ic->xmm5_; break;
        case 6: xmm_slot = &ic->xmm6_; break;
        case 7: xmm_slot = &ic->xmm7_; break;
      }
      if (tn->kind_ == tk_float) {
        /* Store float in lower 32 bits of the xmm slot (slot was zeroed on init) */
        *(float *)xmm_slot = temps[args->ord_].v_.f_;
      } else {
        *xmm_slot = temps[args->ord_].v_.d_;
      }
      fs->float_reg_idx++;
      fs->num_xmm_args++;
    } else {
      uint64_t *slot = (uint64_t *)(((char *)ic->hspace_) + fs->stack_offset);
      if (tn->kind_ == tk_float) {
        *slot = 0;
        *(float *)slot = temps[args->ord_].v_.f_;
      } else {
        *(double *)slot = temps[args->ord_].v_.d_;
      }
      fs->stack_offset += sizeof(uint64_t);
    }
  } else {
    if (fs->int_reg_idx < 6) {
      uint64_t val = temps[args->ord_].v_.u64_;
      switch (fs->int_reg_idx) {
        case 0: ic->rdi_ = val; break;
        case 1: ic->rsi_ = val; break;
        case 2: ic->rdx_ = val; break;
        case 3: ic->rcx_ = val; break;
        case 4: ic->r8_ = val; break;
        case 5: ic->r9_ = val; break;
      }
      fs->int_reg_idx++;
    } else {
      *(uint64_t *)(((char *)ic->hspace_) + fs->stack_offset) = temps[args->ord_].v_.u64_;
      fs->stack_offset += sizeof(uint64_t);
    }
  }
}

int invoke_x64(struct c_compiler *cc, struct expr *x, struct expr_temp *temps) {
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

  /* Allocate hspace large enough for the worst case (all args overflow to stack).
   * The actual hspace_size is set after fill_args determines the overflow count. */
  size_t max_hspace = ((num_args * sizeof(uint64_t)) + 15) & ~(size_t)15;

  struct invoke_context *ic;
  ic = (struct invoke_context *)alloca(sizeof(struct invoke_context) + max_hspace);

  ic->fnptr_ = (void *)temps[x->children_[0]->ord_].v_.u64_;
  ic->rdi_ = 0;
  ic->rsi_ = 0;
  ic->rdx_ = 0;
  ic->rcx_ = 0;
  ic->r8_ = 0;
  ic->r9_ = 0;
  ic->rax_in_ = 0;
  ic->xmm0_ = 0.;
  ic->xmm1_ = 0.;
  ic->xmm2_ = 0.;
  ic->xmm3_ = 0.;
  ic->xmm4_ = 0.;
  ic->xmm5_ = 0.;
  ic->xmm6_ = 0.;
  ic->xmm7_ = 0.;
  ic->hspace_size_ = 0;
  ic->hspace_ = (void *)(ic + 1);
  ic->rax_result_ = 0;

  struct fill_state fs = { 0, 0, 0, 0 };
  fill_args(cc, ic, &fs, x->children_[1], temps);

  /* Set actual hspace_size (stack-passed args only), rounded to 16 bytes for alignment */
  ic->hspace_size_ = (fs.stack_offset + 15) & ~(size_t)15;
  ic->rax_in_ = fs.num_xmm_args;

  invoke_call_x64(ic);

  /* After the call, integer results are in rax_result_, float/double in xmm0_
   * (the ASM saves xmm0 to the xmm0_ field after the call). */
  struct type_node *ret_type = type_node_unqualified(func_type->derived_from_);
  if (ret_type && ret_type->kind_ == tk_float) {
    temps[x->ord_].v_.f_ = *(float *)&ic->xmm0_;
  } else if (ret_type && ret_type->kind_ == tk_double) {
    temps[x->ord_].v_.d_ = ic->xmm0_;
  } else {
    temps[x->ord_].v_.u64_ = ic->rax_result_;
  }

  return 0;
}


