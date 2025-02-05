; Copyright 2023-2025 Kinglet B.V.
;
; Licensed under the Apache License, Version 2.0 (the "License");
; you may not use this file except in compliance with the License.
; You may obtain a copy of the License at
;
; http://www.apache.org/licenses/LICENSE-2.0
;
; Unless required by applicable law or agreed to in writing, software
; distributed under the License is distributed on an "AS IS" BASIS,
; WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
; See the License for the specific language governing permissions and
; limitations under the License.

_DATA SEGMENT
hello_msg db "Hello world", 0
_DATA ENDS

PUBLIC invoke_call_x64
PUBLIC enter_call_tramp_x64
PUBLIC enter_call_x64
EXTERN invoke_enter_call:proto

_TEXT SEGMENT

Invocation STRUCT
rcx_ QWORD ?
rdx_ QWORD ?
r8_  QWORD ?
r9_  QWORD ?
xmm0_ QWORD ?
xmm1_ QWORD ?
xmm2_ QWORD ?
xmm3_ QWORD ?
fnptr_ QWORD ?
rax_result_ QWORD ?
hspace_size_ QWORD ?
hspace_ QWORD ?
Invocation ENDS

invoke_call_x64 PROC FRAME
  push rdi
.pushreg rdi
  push rsi
.pushreg rsi
  push rbp
.pushreg rbp
  push r12
.pushreg r12
  mov r12, rsp
.setframe r12, 0    ; nominate r12 as the frame pointer - this is needed because the stackframe is dynamic in size (hspace_size_ is variable)
  sub rsp, 8 * (2) + 8 * 4 + 8
.allocstack 8 * (2) + 8 * 4 + 8
  lea rbp, [rsp + 8 * 2 + 8 * 4 + 8]

.endprolog
  sub rsp, [rcx.Invocation].hspace_size_

  mov [rbp - 16], rcx

  mov rsi, [rcx.Invocation].hspace_
  mov rcx, [rcx.Invocation].hspace_size_
  mov rdi, rsp
  rep movsb

  mov rcx, [rbp - 16]
  mov r10, qword ptr [rcx.Invocation].fnptr_

  movsd xmm3, qword ptr [rcx.Invocation].xmm3_
  movsd xmm2, qword ptr [rcx.Invocation].xmm2_
  movsd xmm1, qword ptr [rcx.Invocation].xmm1_
  movsd xmm0, qword ptr [rcx.Invocation].xmm0_

  mov r9, qword ptr [rcx.Invocation].r9_
  mov r8, qword ptr [rcx.Invocation].r8_
  mov rdx, qword ptr [rcx.Invocation].rdx_
  mov rcx, qword ptr [rcx.Invocation].rcx_

  ; Variable stack block - rep stos the data onto the stack. For consistency's sake this may just include the 4*8 as well as
  ; that would move a lot of the call logic into C country

  ; jump to [rcx.Invocation].fnptr_
  call r10

  mov rcx, [rbp - 16]
  mov [rcx.Invocation].rax_result_, rax

  add rsp, [rcx.Invocation].hspace_size_
  
  add rsp, 8 * (2) + 8 * 4 + 8
  pop r12
  pop rbp
  pop rsi
  pop rdi
  ret
invoke_call_x64 ENDP

enter_call_tramp_x64 PROC
  mov r11, 123h
  mov r10, 1234123412341234h
  jmp enter_call_x64
enter_call_tramp_x64 ENDP

 db 123

enter_call_x64 PROC FRAME
  push rbp                            ; Odd-aligned on entry, push rbp even-aligns us.
.pushreg rbp
  sub rsp, SIZEOF Invocation + 8*4    ; 8 * 4 is for register parameter stack area on subsequent CALLs inside this function.
.allocstack SIZEOF Invocation + 8*4
.endprolog
  lea rax, [rsp + SIZEOF Invocation + 8 + 8*4]
  mov [8*4 + rsp.Invocation].rcx_, rcx
  mov [8*4 + rsp.Invocation].rdx_, rdx
  mov [8*4 + rsp.Invocation].r8_, r8
  mov [8*4 + rsp.Invocation].r9_, r9
  movsd [8*4 + rsp.Invocation].xmm0_, xmm0
  movsd [8*4 + rsp.Invocation].xmm1_, xmm1
  movsd [8*4 + rsp.Invocation].xmm2_, xmm2
  movsd [8*4 + rsp.Invocation].xmm3_, xmm3
  lea rcx, [8 * 4 + rsp]
  ; mov rcx, rsp
  mov rdx, rax
  mov r8, r10
  mov r9, r11
  call invoke_enter_call
  mov rax, [8 * 4 + rsp.Invocation].rax_result_
  add rsp, SIZEOF Invocation + 8*4 
  pop rbp
  ret
enter_call_x64 ENDP

_TEXT ENDS

END
