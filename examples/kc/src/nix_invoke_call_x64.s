# Copyright 2023-2025 Kinglet B.V.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

.intel_syntax noprefix

.struct 0
Invocation:
Invocation_rdi_:
	.struct Invocation_rdi_ + 8
Invocation_rsi_:
	.struct Invocation_rsi_ + 8
Invocation_rdx_:
	.struct Invocation_rdx_ + 8
Invocation_rcx_:
	.struct Invocation_rcx_ + 8
Invocation_r8_:
	.struct Invocation_r8_ + 8
Invocation_r9_:
	.struct Invocation_r9_ + 8
Invocation_rax_in_:
	.struct Invocation_rax_in_ + 8
Invocation_xmm0_:
	.struct Invocation_xmm0_ + 8
Invocation_xmm1_:
	.struct Invocation_xmm1_ + 8
Invocation_xmm2_:
	.struct Invocation_xmm2_ + 8
Invocation_xmm3_:
	.struct Invocation_xmm3_ + 8
Invocation_xmm4_:
	.struct Invocation_xmm4_ + 8
Invocation_xmm5_:
	.struct Invocation_xmm5_ + 8
Invocation_xmm6_:
	.struct Invocation_xmm6_ + 8
Invocation_xmm7_:
	.struct Invocation_xmm7_ + 8
Invocation_fnptr_:
	.struct Invocation_fnptr_ + 8
Invocation_rax_result_:
	.struct Invocation_rax_result_ + 8
Invocation_hspace_size_:
	.struct Invocation_hspace_size_ + 8
Invocation_hspace_:
	.struct Invocation_hspace_ + 8
Invocation_padding_:
        # 16 byte alignment.
	.struct Invocation_padding_ + 8 
Invocation_sizeof:

.text

.global invoke_call_x64
invoke_call_x64:
  push rbp
  mov rbp, rsp
  sub rsp, 8 * (2)
  sub rsp, [rdi + Invocation_hspace_size_]

  mov [rbp - 16], rdi
  mov rcx, rdi

  mov rsi, [rcx + Invocation_hspace_]
  mov rcx, [rcx + Invocation_hspace_size_]
  mov rdi, rsp
  rep movsb

  mov rcx, [rbp - 16]
  mov rax, qword ptr [rcx + Invocation_fnptr_]

  mov rdi, qword ptr [rcx + Invocation_rdi_]
  mov rsi, qword ptr [rcx + Invocation_rsi_]
  mov rdx, qword ptr [rcx + Invocation_rdx_]
  # Move rcx at the end, when we're done with it...
  mov r8, qword ptr [rcx + Invocation_r8_]
  mov r9, qword ptr [rcx + Invocation_r9_]
  mov rax, qword ptr [rcx + Invocation_rax_in_]

  movsd xmm0, qword ptr [rcx + Invocation_xmm0_]
  movsd xmm1, qword ptr [rcx + Invocation_xmm1_]
  movsd xmm2, qword ptr [rcx + Invocation_xmm2_]
  movsd xmm3, qword ptr [rcx + Invocation_xmm3_]
  movsd xmm4, qword ptr [rcx + Invocation_xmm4_]
  movsd xmm5, qword ptr [rcx + Invocation_xmm5_]
  movsd xmm6, qword ptr [rcx + Invocation_xmm6_]
  movsd xmm7, qword ptr [rcx + Invocation_xmm7_]

  mov r10, qword ptr [rcx + Invocation_fnptr_]

  mov rcx, qword ptr [rcx + Invocation_rcx_]

  # Call Invocation.fnptr_
  call r10

  mov rcx, [rbp - 16]
  mov [rcx + Invocation_rax_result_], rax

  mov rsp, rbp
  pop rbp
  ret

.global enter_call_x64
enter_call_x64:
  push rbp
  sub rsp, Invocation_sizeof
  mov [rsp + Invocation_rdi_], rdi
  mov [rsp + Invocation_rsi_], rsi
  mov [rsp + Invocation_rdx_], rdx
  mov [rsp + Invocation_rcx_], rcx
  mov [rsp + Invocation_r8_], r8
  mov [rsp + Invocation_r9_], r9
  mov [rsp + Invocation_rax_in_], rax
  movsd [rsp + Invocation_xmm0_], xmm0
  movsd [rsp + Invocation_xmm1_], xmm1
  movsd [rsp + Invocation_xmm2_], xmm2
  movsd [rsp + Invocation_xmm3_], xmm3
  movsd [rsp + Invocation_xmm4_], xmm4
  movsd [rsp + Invocation_xmm5_], xmm5
  movsd [rsp + Invocation_xmm6_], xmm6
  movsd [rsp + Invocation_xmm7_], xmm7
  # First argument of invoke_enter_call() = struct invoke_context *ic
  mov rdi, rsp
  # Second argument of invoke_enter_call() = void *rsp
  lea rsi, [rsp + Invocation_sizeof]
  # Third argument of invoke_enter_call() = struct func_def *fd
  mov rdx, r10
  # Final argument of invoke_enter_call() = struct c_compiler *cc
  mov rcx, r11
  call invoke_enter_call
  mov rax, [rsp + Invocation_rax_result_]
  add rsp, Invocation_sizeof
  pop rbp
  ret

