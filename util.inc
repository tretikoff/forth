section .data
  xt_run: dq run
  xt_loop: dq main_loop
  program_stub: dq 0
  warning_message: db "Warning: no such word", 0
  mode: db 0               ; 0 - interpreter ; 1 - compiler
  was_branch: db 0
  mem: dq forth_mem
  stack_start:


section .bss

  ; return stack - END   ;
  resq 1023
  rstack_start: resq 1
  ; return stack - START ;
  forth_mem: resq 65536
  input_buf: resb 1024

section .text
; Searches word in dictionary
; rax <= address of found word or zero(0)
find_word:
  xor eax, eax            ; rax - answer: true or false
  mov rsi, [last_word]    ; rsi - pointer to the last word

  .loop:
    push rdi                ; caller-saved
    push rsi                ; caller-saved
    add rsi, link_size      ; rsi - jump over the link to word
    call string_equals      ; rax - find succeed? true(1) : false(0)
    pop rsi                 ; caller-saved
    pop rdi                 ; caller-saved

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

call_from_address:
  xor eax, eax
  add rdi, link_size
  push rdi
  call string_length        ; rax - length of string
  pop rdi
  add rax, 1                ; rax - length with null-terminator
  add rdi, rax
  mov rax, rdi
  ret

; The inner interpreter. These three lines
; fetch the next instruction and start its
; execution
next:
  mov w, [pc]
  add pc, 8
  jmp [w]
