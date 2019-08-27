%define BUFFER_SIZE 32

section .data
buf: times 256 db 0

section .text
; rdi - string address
; rax <= length
string_length:
	xor rax, rax 							; rax = 0
	.loop:
	  cmp  byte[rdi + rax], 0 ; compare one symbol for null-termination
	  je   .exit 							; succeed? exit
	  inc  rax 								; increment rax
	  jmp  .loop
	.exit:
		ret

; rdi - string address
print_string:
	push rdi
	call string_length 				; rax - length
	pop rsi			 							; rsi - string address
	mov rdx, rax 							; rdx - length
	mov rax, 1 								; rax - command number - sys_write(1)
	mov rdi, 1 								; rdi - file descriptor - stdout(1)
	syscall										; sys_write rsi rdi rdx rax
	ret

; rdi - char address
print_char:
  push rdi 									; callee-saved - save char address
  mov rsi, rsp 							; rsi - char address
	mov rdx, 1 								; rdx - length
  mov rax, 1 								; rax - command number - sys_write(1)
	mov rdi, 1 								; rdi - file descriptor - stdout(1)
  syscall
  pop rdi 									; callee-saved
  ret

print_newline:
  mov rdi, 10 							; set \n to rdi
  jmp print_char

; rdi - string address
; rsi - address to paste
string_copy:
  call string_length				; rax - length
  mov  rcx, rax 						; rcx - length
  inc  rcx 									; rcx - length of null-terminated string
  xchg rsi, rdi 						; rdi - place to paste ; rsi - string address
  mov  byte[rdi + rax], 0 	; null-terminate the last symbol of paste place
  rep  movsb 								; move from rsi to rdi with bytes ; rcx - counter
  ret

; rdi - number
print_uint:
  push rbp 									; callee-saved - rbp
  mov  rbp, rsp 						; rbp - stack address - will be pointer
  mov  rax, rdi 						; rax - number
  mov  rdi, 10 							; rdi - devider - 10
  sub  rsp, BUFFER_SIZE 		; create BUFFER for number at stack
  dec  rbp
  mov  byte[rbp], 0 				; rbp - last byte of BUFFER is null-terminator
	.loop:
    dec  rbp 							; rbp - decrement for new byte
    xor  rdx, rdx 				; rdx = 0
    div  rdi 							; rdx = rax % 10 ; rax /= 10
    add  rdx, '0' 				; rdx - ASCII code
    mov  byte[rbp], dl 		; put rdx to stack
    test rax, rax 				; set flags
    jnz  .loop 						; ZF == 0 ? loop
  mov rdi, rbp 							; rdi - pointer to int
  call print_string
  add rsp, BUFFER_SIZE 			; free BUFFER
  pop rbp 									; callee-saved
  ret

; rdi - number
print_int:
  test rdi, rdi 							; set flags
  jns .unsigned 							; SF == 0 ? print_uint
  push rdi 										; save rdi
  mov rdi, '-'
  call print_char 						; print '-'
  pop rdi 										; return rdi
  neg rdi 										; not and inc
  .unsigned:
  	jmp print_uint

; rdi - first string address
; rsi - second string address
string_equals:
  call string_length					; rax - length
  mov rcx, rax	 							; rcx - length of the first
  xchg rdi, rsi 							; exchange rdi - second, rsi - first
  call string_length					; rax - length of the second
  cmp rax, rcx 								; check their lengths
  jne  .not_equals
  repe cmpsb 									; repe - repeat until not equals ; cmpsb - set flags
  jne  .not_equals
  mov rax, 1 									; return 1
  ret
  .not_equals:
    mov rax, 0 								; return 0
    ret

; rax <= char
read_char:
  push 0	 										; BUFFER the byte for char
  mov rsi, rsp 								; rsi - address to paste
  mov rdx, 1 									; rdx - length
	xor rdi, rdi								; rdi - file descriptor - stdin(0)
	xor rax, rax 								; rax - command number - sys_read(0)
  syscall
  pop rax				 							; rax - char
  ret

; rdi - address of word
; r14 <= length of word
read_word:
  push r14										; callee-saved
  push r15										; callee-saved
  xor r14, r14								; r14 - length
  mov r15, rsi
  dec r15

  .read_first:
	  push rdi
	  call read_char
	  pop rdi
	  cmp al, ' '								; ASCII space symbol
	  je .read_first
	  cmp al, 10								; ASCII space symbol
	  je .read_first
	  cmp al, 13								; ASCII space symbol
	  je .read_first
	  cmp al, 9									; ASCII space symbol
	  je .read_first
	  test al, al
	  jz .read_success

  .read_next:
	  mov byte [rdi + r14], al	; save char
	  inc r14

	  push rdi
	  call read_char
	  pop rdi
	  cmp al, ' '								; ASCII space symbol
	  je .read_success
	  cmp al, 10								; ASCII space symbol
	  je .read_success
	  cmp al, 13								; ASCII space symbol
	  je .read_success
	  cmp al, 9									; ASCII space symbol
	  je .read_success
	  test al, al
	  jz .read_success
	  cmp r14, r15
	  je .read_err
	  jmp .read_next

  .read_success:
	  mov byte [rdi + r14], 0
	  mov rax, rdi
	  mov rdx, r14
	  pop r15
	  pop r14
	  ret

  .read_err:
	  xor rax, rax
	  pop r15
	  pop r14
	  ret

; rdi - string address
; rdx <= length
; rax <= number
parse_uint:
  call string_length					; rax - length
  mov rcx, rax 								; rcx - length
  mov rsi, rdi 								; rsi - string address
  xor rdx, rdx 								; rdx = 0 - length of the numbers
  xor rax, rax 								; rax = 0 - the number
  .pars:
    xor  rdi, rdi 						; rdi = 0
    mov  dil, byte[rsi + rdx] ; rdi - next digit in ASCII
    cmp  dil, '0' 						; rdi < '0' ? exit
    jb   .exit
    cmp  dil, '9' 						; rdi > '9' ? exit
    ja   .exit
    sub  dil, '0' 						; rdi - next digit
    imul rax, 10							; rax *= 10
    add  rax, rdi 						; rax += rdi
    inc  rdx
    dec  rcx 									; length != 0 ? pars
    jnz  .pars
  .exit:
  ret

; rdi - string address
; rdx <= length
; rax <= number
parse_int:
  cmp byte[rdi], '-' 					; '-' ? minus
  je .minus
  jmp parse_uint
  .minus:
    inc rdi
    call parse_uint
    test rdx, rdx 						; set flags
    jz .null
    neg rax 									; not and inc
    inc rdx
    ret
  .null:
    xor eax, eax 							; rax = 0
    ret
