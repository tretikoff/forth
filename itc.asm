%include 'io_lib.inc'

global _start

%define pc r15
%define w r14
%define rstack r13
%define link 0

%assign link_size 8

%macro create_link 0
  %%link: dq link        ; %% creates new local (example: @1234.link)
  %define link %%link
%endmacro

%macro native 2
  section .data
    w_ %+ %2:            ; w_COMMAND label
	    create_link        ; create link for dictionary search
	    db %1, 0           ; put name of COMMAND with null-terminator
	  xt_ %+ %2:           ; xt_COMMAND label
      dq %2 %+ _impl     ; COMMAND_impl address

  section .text
	 %2 %+ _impl:           ; COMMAND_impl label
%endmacro

%macro colon 2
  section .data
	 w_ %+ %2:
	  create_link
	  db %1, 0

  section .text
	 xt_ %+ %2:
	  dq i_docol             ; The `docol` address −− one level of indirection
%endmacro

section .data
  program_stub: dq 0
  xt_interpreter: dq interpreter_loop
  last_word: dq link
  warning_message: db "Warning: we do not have such word", 0

section .bss
  resq 1023
  rstack_start: resq 1
  input_buf: resb 1024

; DICTIONARY
; drop, +, dup, double

; Drops the topmost element from the stack
native 'drop', drop
  add rsp, 8
  jmp next

native '+', plus
  pop rax
  add rax, [rsp]
  mov [rsp], rax
  jmp next

native 'dup', dup
  push qword [rsp]
  jmp next

colon 'double', double
  dq xt_dup                 ; The words consisting `dup` start here.
  dq xt_plus
  dq xt_exit


; EXECUTION

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
  dq i_docol

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
  dq i_docol

  dq xt_inbuf
  dq xt_word
  dq xt_drop

  dq xt_inbuf
  dq xt_prints

  dq xt_bye


; UTILS
; docol, exit, word, prints, bye, inbuf, next

; Saves PC when the colon word starts
xt_docol:
  dq i_docol
i_docol:
  sub rstack, 8
  mov [rstack], pc
  add w, 8
  mov pc, w
  jmp next

; Returns from the colon word
xt_exit:
  dq i_exit
i_exit:
  mov pc, [rstack]
  add rstack, 8
  jmp next

; Takes a buffer pointer from stack
; Reads a word from input and stores it
; starting in the given buffer
xt_word:
  dq i_word
i_word:
  pop rdi
  call read_word
  push rdx
  jmp next

; Searches word in dictionary
; rax <= address of found word or zero(0)
find_word:
  xor eax, eax            ; rax - answer: true or false
  pop rdi                 ; rdi - user's word
  mov rsi, [last_word]    ; rsi - pointer to the last word

  .loop:
    push rdi                ; caller-saved
    push rsi                ; caller-saved
    add rsi, link_size      ; rsi - jump over the link to word
    call string_equals      ; rax - find succeed? true(1) : false(0)
    pop rdi                 ; caller-saved
    pop rsi                 ; caller-saved

    test rax, rax
    jnz .found              ; check to successful search
    mov rsi, [rsi]          ; rsi - pointer to the next word
    test rsi, rsi           ; is it the last one?
    jnz .loop

    xor eax, eax
    ret

  .found:
    mov rax, rsi
    ret

; code from address - jump pointer to execution_point place
; rdi - command address
; rax - address of xt_COMMAND
cfa:
	xor eax, eax
  pop rdi
	add rdi, link_size
  push rdi
  call string_length        ; rax - length of string
  pop rdi
  add rax, 1                ; rax - length with null-terminator
  add rdi, rax
	mov rax, rdi
  ret

; Takes a pointer to a string from the stack
; and prints it
xt_prints:
  dq i_prints
i_prints:
  pop rdi
  call print_string
  jmp next

; Exits program
xt_bye:
  dq i_bye
i_bye:
  mov rax, 60
  xor rdi, rdi
  syscall

; Loads the predefined buffer address
xt_inbuf:
  dq i_inbuf
i_inbuf:
  push qword input_buf
  jmp next

; The inner interpreter. These three lines
; fetch the next instruction and start its
; execution
next:
  mov w, [pc]
  add pc, 8
  jmp [w]
