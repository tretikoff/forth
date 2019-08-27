%define BUFFER_SIZE 32
section .data
	buf : times 256 db 0

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
    call string_length 				; rax - length
    mov rsi, rdi 							; rsi - string address
    mov rdi, 1 								; rdi - file descriptor - stdout(1)
    mov rdx, rax 							; rdx - length
    mov rax, 1 								; rax - command number - sys_write(1)
    syscall										; sys_write rsi rdi rdx rax
    ret

; rdi - char address
print_char:
    push rdi 									; callee-saved - save char address
    mov rsi, rsp 							; rsi - char address
    mov rdi, 1 								; rdi - file descriptor - stdout(1)
		mov rdx, 1 								; rdx - length
    mov rax, 1 								; rax - command number - sys_write(1)
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
  jns  .unsigned 							; SF == 0 ? print_uint
  push rdi 										; save rdi
  mov  rdi, '-'
  call print_char 						; print '-'
  pop  rdi 										; return rdi
  neg  rdi 										; not and inc
  .unsigned:
  jmp  print_uint

; rdi - first string address
; rsi - second string address
string_equals:
  call string_length					; rax - length
  mov  rcx, rax 							; rcx - length of the first
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
  dec rsp 										; BUFFER the byte for char
  mov rax, 0 									; rax - command number - sys_read(0)
  mov rdi, 0 									; rdi - file descriptor - stdin(0)
  mov rsi, rsp 								; rsi - address to paste
  mov rdx, 1 									; rdx - length
  syscall
  mov rax, [rsp] 							; rax - char
  inc rsp 										; free BUFFER
  ret

; rsi - BUFFER size
; rdi - BUFFER address
; rdx <= length of word
; rax <= address of word
read_word:
  push rbx 										; callee-saved - rbx
	mov r8, rsi 								; r8 - BUFFER size
	mov r9, rdi 								; r9 - BUFFER address
  xor  rbx, rbx 							; rbx = 0 - length counter
  xor  rdi, rdi 							; rdi - file descriptor - stdin(0)
  mov  rdx, 1 								; rdx - length
  .skip:
    xor rax, rax 							; rax - command number - sys_read(0)
    mov rsi, buf 							; rsi - address to paste
    syscall
      cmp al, 0 							; compare one symbol for null-termination
      je  .finally
    cmp byte[buf], 0x21 			; check to bad symbols below 0x21
    jb  .skip
  inc rbx
  .read:
    xor rax, rax 							; rax - command number - sys_read(0)
    lea rsi, [buf + rbx] 			; rsi - address to paste
    syscall
    cmp byte [buf + rbx], 0x21; check to bad symbols below 0x21
    jb  .finally
		cmp r8, rbx 							; check the size of BUFFER
		jbe .exit 								; below or equal ? exit
    inc rbx
    jmp .read
  .finally:
  mov byte[buf + rbx], 0 			; add null-terminator to end
  mov rdx, rbx 								; rdx - length
	mov rax, buf 								; rax - BUFFER address
  pop rbx 										; callee-saved
  ret
	.exit:
	xor rdx, r8 								; rdx - length of buffer
	xor rax, rax 								; rax - address is 0
	pop rbx 										; callee-saved
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
