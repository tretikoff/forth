; (  -- buf )
; loads the predefined buffer address
native 'buf', buf
  push qword user_buf
  jmp next

; ( buf -- len )
; reads word to buffer
native 'read', read
  pop rdi
  call read_word
  push rdx
  jmp next

; ( a --  )
; drops the topmost element from the stack
native 'drop', drop
  add rsp, 8
  jmp next

; ( addr --  )
; prints string
native 'prints', prints
  pop rdi
  call print_string
  jmp next

; prints new line
native 'printnl', printnl
  call print_newline
  jmp next

; ( a --  )
; prints integer number
native '.', dot
  pop rdi
  call print_int
  jmp next

; ( arg2 arg1 -- answer )
; check strings to equality
native 'compare', compare
  pop rdi
  pop rsi
  call string_equals
  push rax
  jmp next

; ( a -- a a )
native 'dup', dup
  push qword [rsp]
  jmp next

; ( b a -- [ b + a ] )
native '+', plus
  pop rdx
  pop rax
  add rax, rdx
  push rax
  jmp next

; ( b a -- [ b - a ] )
native '-', minus
  pop rdx
  pop rax
  sub rax, rdx
  push rax
  jmp next

; ( b a -- [ b * a ] )
native '*', mul
  pop rdx
  pop rax
  imul rdx
  push rax
  jmp next

; ( b a -- [ b / a ] )
native '/', div
  pop rcx
  pop rax
  cqo
  idiv rcx
  push rax
  jmp next

; ( b a -- [a < b] )
native '<', less
  pop rdx
  pop rax
  cmp rax, rdx
  setl al
  movzx rax, al
  push rax
  jmp next

; ( a b -- b a )
; swap two cells on top of the stack
native 'swap', swap
  pop rdx
  pop rax
  push rdx
  push rax
  jmp next

; print all stack elements
native '.S', show_stack
  mov rcx, rsp
  .loop:
    cmp rcx, [stack_start]
    jae next
    mov rdi, [rcx]
    push rcx
    call print_int
    call print_newline
    pop rcx
    add rcx, 8
    jmp .loop

; ( b a -- [ b = a ] )
native '=', equals
  pop rdx
  pop rax
  cmp rax, rdx
  jne .nequals
  xor rax, rax
  mov rax, 1
  push rax
  jmp next
  .nequals:
    xor rax, rax
    push rax
    jmp next

; ( b a -- [ b mod a ] )
native "%", mod
  pop rcx
  pop rax
  cqo
  idiv rcx
  push rdx
  jmp next

; ( b a -- [ b & a ] )
; bitwise and
native 'and', and
  pop rdx
  pop rax
  and rax, rdx
  push rax
  jmp next

; ( b a -- [ b | a ] )
; bitwise or
native 'or', or
  pop rax
  or [rsp], rax
  jmp next

; ( a -- !a )
native 'not', not
  pop rax
  test rax, rax
  jz .setone
  xor rax, rax
  push rax
  jmp next
  .setone:
    xor rax, rax
    mov rax, 1
    push rax
    jmp next

; ( c b a -- b a c )
native 'rot', rot
  pop rax
  pop rdx
  pop rcx
  push rdx
  push rax
  push rcx
  jmp next

; ( -- c )
; read a single character from stdin
native 'key', key
  call read_char
  push rax
  jmp next

; ( c -- )
; output a single character to stdout
native 'emit', emit
  pop rdi
  call print_char
  jmp next

; ( val addr -- )
; store value by address
native '!', write
  pop rax
  pop rdx
  mov [rax], rdx
  jmp next

; ( addr -- value )
; fetch value from address
native '@', fetch
  pop rax
  push qword[rax]
  jmp next

; ( val addr -- )
; store value by address
native 'c!', write_char
  pop rax
  pop rdx
  mov [rax], dl
  jmp next

; ( addr -- char )
; read one byte starting at addr
native 'c@', fetch_char
  pop rax
  movzx rax, byte [rax]
  push rax
  jmp next

native '>r', to_r
  pop rax
  rpush rax
  jmp next

native 'r>', from_r
  rpop rax
  push rax
  jmp next

native "r@", r_fetch
  push qword [rstack]
  jmp next

; logic or
native 'lor', lor
  pop rax
  pop rdx
  test rax, rax
  jnz .yes
  push rdx
  jmp next
  .yes:
    push rax
    jmp next

; logic and
native "land", land
  pop rax
  pop rdx
  test rax, rax
  jz .no
  push rdx
  jmp next
  .no:
    push rax
    jmp next

; ( user_word_addr -- dict_word_addr )
; find word in dictionary
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

; ( xt_addr --  )
native 'initcmd', initcmd
  pop rax
  mov w, rax
  jmp [w]

; terminate the program
native 'bye', bye
  mov rax, 60
  xor rdi, rdi
  syscall

native 'warn', warn
  mov  rdi, warning_message
  call print_string
  call print_newline
  jmp next

; ( xt_addr -- xt_addr answer )
native 'isimmediate', isimmediate
  pop rax
  push rax
  xor rdx, rdx
  mov dl, byte[rax - 1]
  push rdx
  jmp next

; ( addr -- num len )
native 'parsei', parsei
  pop rdi
  call parse_int          ; rax - number ; rdx - length of number
  push rax                ; save number to stack
  push rdx
  jmp next

; (  -- mode )
native 'pushmode', pushmode
  xor eax, eax
  mov rax, [mode]
  push rax
  jmp next

; (  -- addr )
native 'pushmode_addr', pushmode_addr
  xor eax, eax
  mov rax, mode
  push rax
  jmp next

; ( a -- )
; add element from stack to the word
native ',', comma
  mov rax, [here]
  pop qword [rax]
  add qword [here], 8
  jmp next

; ( xt_addr -- xt_addr )
native 'isbranch', isbranch
  pop rax
  push rax
  cmp byte[rax - 1], 2
  jne .continue
  mov byte[was_branch], 1
  .continue:
  jmp next

; (  --  )
native 'unsetbranch', unsetbranch
  mov byte[was_branch], 0
  jmp next

; (  -- answer )
native 'wasbranch', wasbranch
  xor rdx, rdx
  mov dl, byte[was_branch]
  push rdx
  jmp next

native 'pushlit', pushlit
  push xt_lit
  jmp next

native 'push_lastword', push_lastword
  push last_word
  jmp next

; push a value immediately following this XT
native 'lit', lit
  push qword [pc]
  add pc, 8
  jmp next

; ( xt_addr --  )
native 'saveword', saveword
  pop rax
  mov  qword[here], rax

  ; add  here, word_size
  xor eax, eax
  mov rax, here
  add rax, word_size
  add qword[here], rax

  jmp next

; ( xt_addr --  )
native 'savenum', savenum
  pop rax
  mov [here], rax

  ; add  here, word_size
  xor eax, eax
  mov rax, here
  add rax, word_size
  mov qword[here], rax

  jmp next

; Initializes registers
native 'init', init
  mov rstack, rstack_start
  mov stack, stack_start

  xor eax, eax
  mov rax, [in_fd]
  rpush rax
  xor eax, eax
  pop rax
  mov [in_fd], rax

  mov qword[mode], 0
  mov pc, xt_run
  cmp qword [stack_start], 0
  je  .first
  mov rsp, [stack_start]
  jmp next
  .first:
  mov [stack_start], rsp
  jmp next

; (  -- buf_addr )
; load the predefined buffer address
native 'buffer', buffer
  push qword input_buf
  jmp next

; save PC when the colon word starts
native 'docol', docol
  sub rstack, 8
  mov qword [rstack], pc
  add w, 8
  mov pc, w
  jmp next

; return from the colon word
native 'exit', exit
  mov pc, [rstack]
  add rstack, 8
  jmp next

; ( answer --  )
; jump to a location if answer is 0
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
; jump to a location
native 'branch', branch
  mov pc, [pc]
  jmp next

; ( flags name --  )
; create an entry in the dictionary
native 'create', create
  ; link
  mov rcx, [last_word]
  mov rsi, [here]
  mov [rsi], rcx
  mov [last_word], rsi
  add rsi, 8

  ; name
  pop rdi
  push rsi
  call string_copy
  pop rsi
  push rsi
  mov rdi, rsi
  call string_length
  pop rsi
  add rsi, rax
  ; flags
  inc rsi               ; null-terminator
  pop rax
  mov [rsi], al
  inc rsi

  mov [here], rsi
  jmp next

const dp, dp
const here, [here]
const in_fd, in_fd
const state, state
const mem, user_mem

native 'comp_m', comp_m
  mov  rdi, cmode_message
  call print_string
  call print_newline
  jmp next

native 'inte_m', inte_m
  mov  rdi, imode_message
  call print_string
  call print_newline
  jmp next

; ( call_num a1 a2 a3 a4 a5 a6 -- new_rax new_rdx )
; execute syscall
native "syscall", syscall
  pop r9
  pop r8
  pop r10
  pop rdx
  pop rsi
  pop rdi
  pop rax
  syscall
  push rax
  push rdx
  jmp next


; ( a -- [ a + a ] )
colon 'double', double
  dq xt_dup
  dq xt_plus
  dq xt_exit

; ( b a -- [b>a] )
colon '>', greater
  dq xt_swap
  dq xt_less
  dq xt_exit

; Read word from stdin and define it
colon ':', colon
  .restart:
    dq xt_buffer
    dq xt_read
    branchif0 .restart

    dq xt_lit, 0          ; interpreter mode flag
    dq xt_buffer
    dq xt_create

    dq xt_lit, 1, xt_pushmode_addr
    dq xt_write

    dq xt_lit, docol_impl
    dq xt_comma

    dq xt_exit

colon ';', semicolon, 1
  dq xt_lit, 0, xt_pushmode_addr
  dq xt_write

  dq xt_lit, xt_exit
  dq xt_comma
  dq xt_exit

; ( -- xt_addr )
; read word and place its xt_address on the stack
colon "'", tick, 1
  dq xt_buffer
  dq xt_read
  dq xt_drop
  dq xt_buffer
  dq xt_find, xt_dup
  branchif0 .no_such_word
  dq xt_cfa
  dq xt_pushmode
  branchif0 .interpret
  dq xt_lit, xt_lit
  dq xt_comma
  dq xt_comma
  .interpret:
    dq xt_exit
    .no_such_word:
      dq xt_warn
      dq xt_exit
