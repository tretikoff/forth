%include 'io_lib.inc'
%include 'macro.inc'
%include 'util.inc'
%include 'dict.inc'

global _start

section .data
  last_word: dq link

section .text
_start:
  jmp init_impl

run:
  dq docol_impl
interpreter_loop:
  dq xt_buffer
  dq xt_read                ; read the word
  branchif0 .exit           ; word read error or empty string

  dq xt_buffer
  dq xt_find
  dq xt_dup
  branchif0 .number

  dq xt_cfa                 ; rax - execution address
  dq xt_ps

  .number:
    dq xt_drop
    dq xt_parsei
    branchif0 warning
    branch interpreter_loop

  .exit:
    dq xt_bye

warning:
  dq xt_drop
	dq xt_warn
