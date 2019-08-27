; IO_LIB WORDS

; ( addr --  )
; print string
native 'prints', prints
  pop rdi
  call print_string
  jmp next

; print new line
native 'printnl', printnl
  call print_newline
  jmp next

; ( num --  )
; print integer number
native 'printi', printi
  pop rdi
  call print_int
  jmp next

; ( arg2_addr arg1_addr -- answer )
; check strings to equality
native 'equals', equals
  pop rdi
  pop rsi
  call string_equals
  push rax
  jmp next

; ( buf_addr -- len )
; read word to buffer
native 'read', read
  pop rdi
  call read_word
  push rdx
  jmp next

; (  -- num len )
native 'parsei', parsei
  call parse_int          ; rax - number ; rdx - length of number
  push rax                ; save number to stack
  push rdx
  jmp next


; UTIL WORDS

; ( user_word_addr -- dict_word_addr )
; Find word in dictionary
native 'find', find
  pop rdi                ; rdi - user's word
  call find_word
  push rax
  jmp next

; ( addr -- xt_addr )
; jump pointer to execution_point place
native 'cfa', cfa
  pop rdi
	call call_from_address
  push rax
  jmp next


; COMPILE-ONLY WORDS

; ( answer --  )
; Jump to a location if answer is 0
native 'branchifz', branchifz
  pop rax
  test rax, rax
  jnz .skip
  mov pc, [pc]
  jmp next
  .skip:
    add pc, 8
    jmp next

; (  --  )
; Jump to a location
native "branch", branch
    mov pc, [pc]
    jmp next

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

; Exits program
native 'bye', bye
  mov rax, 60
  xor rdi, rdi
  syscall

; (  -- buf_addr )
; Loads the predefined buffer address
native 'buffer', buffer
  push qword input_buf
  jmp next

; ( answer --  )
native 'ps', ps
  pop rax
  mov [program_stub], rax
  mov pc, program_stub
  jmp next

native 'warn', warn
  mov  rdi, warning_message
  call print_string
  call print_newline
  mov  pc, xt_interpreter
  jmp next

; Initializes registers
native 'init', init
  mov rstack, rstack_start
  ; interpreter mode
  mov pc, xt_run    ; entry_point - other way to work with
  jmp next


; GENERAL WORDS

; ( a --  )
; Drops the topmost element from the stack
native 'drop', drop
  add rsp, 8
  jmp next

; ( b a -- [ b + a ] )
native '+', plus
  pop rax
  add rax, [rsp]
  mov [rsp], rax
  jmp next

; ( a -- a a )
native 'dup', dup
  push qword [rsp]
  jmp next

; ( a -- [ a + a ] )
colon 'double', double
  dq xt_dup                 ; The words consisting `dup` start here.
  dq xt_plus
  dq xt_exit
