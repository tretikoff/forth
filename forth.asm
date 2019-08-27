%include 'io_lib.inc'
%include 'macro.inc'
%include 'util.inc'
%include 'dict.inc'

global _start
; The program starts execution from the init word
_start:
  jmp i_init

; Initializes registers
xt_init:
  dq i_init
i_init:
  mov rstack, rstack_start

  ; interpreter mode
  mov pc, xt_interpreter    ; entry_point - other way to work with
  jmp next

interpreter_loop:
  dq docol_impl

  dq xt_inbuf
  dq xt_word                ; read the word
  push rdx                  ; rdx - count of symbols
  test rdx, rdx
  jz .exit                  ; word read error or empty string

  dq xt_inbuf
  call find_word
  test rax, rax
  jz .number

  push rax
  call cfa                 ; rax - execution address
  mov [program_stub], rax
  mov pc, program_stub
  jmp next

  .number:
    call parse_int          ; rax - number ; rdx - length of number
    test rdx, rdx
    jz warning
    push rax                ; save number to stack
    jmp interpreter_loop

  .exit:
    dq xt_bye

warning:
	mov  rdi, warning_message
	call print_string
	call print_newline
	mov  pc, xt_interpreter
	jmp next

; this one cell is the program
entry_point:
  dq xt_main

; This is a colon word, it stores
; execution tokens. Each token
; corresponds to a Forth word to be
; executed
xt_main:
  dq docol_impl

  dq xt_inbuf
  dq xt_word
  dq xt_drop

  dq xt_inbuf
  dq xt_prints

  dq xt_bye
