; Jump to a location if TOS = 0. Location is calculated in a similar way
; 0branch is a compile-only word.
native '0branch', branch0
  pop rax
  test rax, rax
  jnz .skip
  mov pc, [pc]
  jmp next
  .skip:
  add pc, 8
  jmp next

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

; Saves PC when the colon word starts
native 'docol', docol
  sub rstack, 8
  mov [rstack], pc
  add w, 8
  mov pc, w
  jmp next

; Returns from the colon word
native 'exit', exit
  mov pc, [rstack]
  add rstack, 8
  jmp next

; Takes a buffer pointer from stack
; Reads a word from input and stores it
; starting in the given buffer
native 'word', word
  pop rdi
  call read_word
  push rdx
  jmp next

; Takes a pointer to a string from the stack
; and prints it
native 'prints', prints
  pop rdi
  call print_string
  jmp next

; Exits program
native 'bye', bye
  mov rax, 60
  xor rdi, rdi
  syscall

; Loads the predefined buffer address
native 'inbuf', inbuf
  push qword input_buf
  jmp next
