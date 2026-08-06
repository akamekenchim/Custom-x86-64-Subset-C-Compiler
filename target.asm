.intel_syntax noprefix
.section .rodata
fmt_int:
	.asciz "%d\n"
.text

.global add
add:
	push rbp
	mov rbp, rsp
	sub rsp, 48
	mov [rbp - 8], rcx
	mov [rbp - 16], rdx
	mov rax, [rbp - 16]
	push rax
	mov rax, [rbp - 8]
	pop rbx
	add rax, rbx
	jmp .L_ret_add
.L_ret_add:
	mov rsp, rbp
	pop rbp
	ret

.global sub
sub:
	push rbp
	mov rbp, rsp
	sub rsp, 48
	mov [rbp - 8], rcx
	mov [rbp - 16], rdx
	mov rax, [rbp - 16]
	push rax
	mov rax, [rbp - 8]
	pop rbx
	sub rax, rbx
	jmp .L_ret_sub
.L_ret_sub:
	mov rsp, rbp
	pop rbp
	ret

.global fibonacci
fibonacci:
	push rbp
	mov rbp, rsp
	sub rsp, 48
	mov [rbp - 8], rcx
	mov rax, 0
	push rax
	mov rax, [rbp - 8]
	pop rbx
	cmp rax, rbx
	jg .L_else_0
	mov rax, 0
	jmp .L_ret_fibonacci
.L_else_0:
	mov rax, 1
	push rax
	mov rax, [rbp - 8]
	pop rbx
	cmp rax, rbx
	jne .L_else_1
	mov rax, 1
	jmp .L_ret_fibonacci
.L_else_1:
	mov rax, 2
	push rax
	mov rax, [rbp - 8]
	pop rbx
	sub rax, rbx
	push rax
	mov rax, 2
	push rax
	pop rdx
	pop rcx
	sub rsp, 32
	call fibonacci
	add rsp, 32
	push rax
	mov rax, 1
	push rax
	mov rax, [rbp - 8]
	pop rbx
	sub rax, rbx
	push rax
	mov rax, 1
	push rax
	pop rdx
	pop rcx
	sub rsp, 32
	call fibonacci
	add rsp, 32
	pop rbx
	add rax, rbx
	jmp .L_ret_fibonacci
.L_ret_fibonacci:
	mov rsp, rbp
	pop rbp
	ret

.global mutate_array
mutate_array:
	push rbp
	mov rbp, rsp
	sub rsp, 64
	mov [rbp - 8], rcx
	mov [rbp - 16], rdx
	mov rax, 0
	mov [rbp - 24], rax
.L_start_for_2:
	mov rax, [rbp - 16]
	push rax
	mov rax, [rbp - 24]
	pop rbx
	cmp rax, rbx
	jge .L_end_for_2
	mov rax, 2
	push rax
	mov rax, [rbp - 24]
	imul rax, 8
	push rax
	mov rax, [rbp - 8]
	pop rbx
	add rax, rbx
	mov rax, [rax]
	neg rax
	pop rbx
	imul rax, rbx
	push rax
	mov rax, [rbp - 24]
	imul rax, 8
	push rax
	mov rax, [rbp - 8]
	pop rbx
	add rax, rbx
	pop rbx
	mov [rax], rbx
.L_inc_for_2:
	mov rax, 1
	push rax
	mov rax, [rbp - 24]
	pop rbx
	add rax, rbx
	mov [rbp - 24], rax
	jmp .L_start_for_2
.L_end_for_2:
	mov rax, 0
	jmp .L_ret_mutate_array
.L_ret_mutate_array:
	mov rsp, rbp
	pop rbp
	ret

.global main
main:
	push rbp
	mov rbp, rsp
	sub rsp, 160
	mov rax, 0
	mov [rbp - 8], rax

.section .rodata
.L_str_0:
	.asciz "==================================================\n"
.section .text
	lea rax, [rip + .L_str_0]
	mov rcx, rax
	sub rsp, 32
	call printf
	add rsp, 32

.section .rodata
.L_str_1:
	.asciz "   COMPILER LIMIT & STRESS TEST SUITE (EXTREME)  \n"
.section .text
	lea rax, [rip + .L_str_1]
	mov rcx, rax
	sub rsp, 32
	call printf
	add rsp, 32

.section .rodata
.L_str_2:
	.asciz "==================================================\n"
.section .text
	lea rax, [rip + .L_str_2]
	mov rcx, rax
	sub rsp, 32
	call printf
	add rsp, 32

.section .rodata
.L_str_3:
	.asciz "TEST 1: Unary Minus & Precedence Logic\n"
.section .text
	lea rax, [rip + .L_str_3]
	mov rcx, rax
	sub rsp, 32
	call printf
	add rsp, 32
	mov rax, 5
	neg rax
	mov [rbp - 16], rax
	mov rax, 10
	neg rax
	neg rax
	mov [rbp - 24], rax
	mov rax, 4
	push rax
	mov rax, 20
	neg rax
	pop rbx
	cqo
	idiv rbx
	push rax
	mov rax, [rbp - 24]
	neg rax
	push rax
	mov rax, [rbp - 16]
	neg rax
	pop rbx
	imul rax, rbx
	pop rbx
	add rax, rbx
	mov [rbp - 32], rax
	mov rax, 55
	neg rax
	push rax
	mov rax, [rbp - 32]
	pop rbx
	cmp rax, rbx
	jne .L_else_3

.section .rodata
.L_str_4:
	.asciz "-> TEST 1: PASS\n"
.section .text
	lea rax, [rip + .L_str_4]
	mov rcx, rax
	sub rsp, 32
	call printf
	add rsp, 32
	mov rax, 1
	push rax
	mov rax, [rbp - 8]
	pop rbx
	add rax, rbx
	mov [rbp - 8], rax
	jmp .L_end_3
.L_else_3:

.section .rodata
.L_str_5:
	.asciz "-> TEST 1: FAIL (Expected -55)\n"
.section .text
	lea rax, [rip + .L_str_5]
	mov rcx, rax
	sub rsp, 32
	call printf
	add rsp, 32
.L_end_3:

.section .rodata
.L_str_6:
	.asciz "TEST 2: Pointer Arithmetic & Array Mutation\n"
.section .text
	lea rax, [rip + .L_str_6]
	mov rcx, rax
	sub rsp, 32
	call printf
	add rsp, 32
	mov rax, 10
	push rax
	mov rax, 0
	imul rax, 8
	lea rbx, [rbp - 72]
	add rax, rbx
	pop rbx
	mov [rax], rbx
	mov rax, 20
	neg rax
	push rax
	mov rax, 1
	imul rax, 8
	lea rbx, [rbp - 72]
	add rax, rbx
	pop rbx
	mov [rax], rbx
	mov rax, 30
	push rax
	mov rax, 2
	imul rax, 8
	lea rbx, [rbp - 72]
	add rax, rbx
	pop rbx
	mov [rax], rbx
	mov rax, 40
	neg rax
	push rax
	mov rax, 3
	imul rax, 8
	lea rbx, [rbp - 72]
	add rax, rbx
	pop rbx
	mov [rax], rbx
	mov rax, 50
	push rax
	mov rax, 4
	imul rax, 8
	lea rbx, [rbp - 72]
	add rax, rbx
	pop rbx
	mov [rax], rbx
	lea rax, [rbp - 72]
	mov [rbp - 80], rax
	mov rax, [rbp - 80]
	push rax
	mov rax, 5
	push rax
	pop rdx
	pop rcx
	sub rsp, 32
	call mutate_array
	add rsp, 32
	mov rax, 3
	imul rax, 8
	push rax
	mov rax, [rbp - 80]
	pop rbx
	add rax, rbx
	mov rax, [rax]
	push rax
	mov rax, 1
	imul rax, 8
	push rax
	mov rax, [rbp - 80]
	pop rbx
	add rax, rbx
	mov rax, [rax]
	pop rbx
	add rax, rbx
	mov [rbp - 88], rax
	mov rax, 120
	push rax
	mov rax, [rbp - 88]
	pop rbx
	cmp rax, rbx
	jne .L_else_4

.section .rodata
.L_str_7:
	.asciz "-> TEST 2: PASS\n"
.section .text
	lea rax, [rip + .L_str_7]
	mov rcx, rax
	sub rsp, 32
	call printf
	add rsp, 32
	mov rax, 1
	push rax
	mov rax, [rbp - 8]
	pop rbx
	add rax, rbx
	mov [rbp - 8], rax
	jmp .L_end_4
.L_else_4:

.section .rodata
.L_str_8:
	.asciz "-> TEST 2: FAIL (Expected 120)\n"
.section .text
	lea rax, [rip + .L_str_8]
	mov rcx, rax
	sub rsp, 32
	call printf
	add rsp, 32
.L_end_4:

.section .rodata
.L_str_9:
	.asciz "TEST 3: Dual Recursion & ABI Register Protection\n"
.section .text
	lea rax, [rip + .L_str_9]
	mov rcx, rax
	sub rsp, 32
	call printf
	add rsp, 32
	mov rax, 5
	push rax
	pop rcx
	sub rsp, 32
	call fibonacci
	add rsp, 32
	neg rax
	push rax
	mov rax, 7
	push rax
	pop rcx
	sub rsp, 32
	call fibonacci
	add rsp, 32
	pop rbx
	add rax, rbx
	mov [rbp - 96], rax
	mov rax, 8
	push rax
	mov rax, [rbp - 96]
	pop rbx
	cmp rax, rbx
	jne .L_else_5

.section .rodata
.L_str_10:
	.asciz "-> TEST 3: PASS\n"
.section .text
	lea rax, [rip + .L_str_10]
	mov rcx, rax
	sub rsp, 32
	call printf
	add rsp, 32
	mov rax, 1
	push rax
	mov rax, [rbp - 8]
	pop rbx
	add rax, rbx
	mov [rbp - 8], rax
	jmp .L_end_5
.L_else_5:

.section .rodata
.L_str_11:
	.asciz "-> TEST 3: FAIL (Expected 8)\n"
.section .text
	lea rax, [rip + .L_str_11]
	mov rcx, rax
	sub rsp, 32
	call printf
	add rsp, 32
.L_end_5:

.section .rodata
.L_str_12:
	.asciz "TEST 4: Complex Loop Control & Bitwise Flags\n"
.section .text
	lea rax, [rip + .L_str_12]
	mov rcx, rax
	sub rsp, 32
	call printf
	add rsp, 32
	mov rax, 0
	mov [rbp - 104], rax
	mov rax, 0
	mov [rbp - 112], rax
.L_start_while_6:
	mov rax, 10
	push rax
	mov rax, [rbp - 112]
	pop rbx
	cmp rax, rbx
	jge .L_end_while_6
	mov rax, 1
	push rax
	mov rax, [rbp - 112]
	pop rbx
	add rax, rbx
	mov [rbp - 112], rax
	mov rax, 0
	push rax
	mov rax, 1
	push rax
	mov rax, [rbp - 112]
	pop rbx
	and rax, rbx
	pop rbx
	cmp rax, rbx
	je .L_else_7
	jmp .L_start_while_6
.L_else_7:
	mov rax, 6
	push rax
	mov rax, [rbp - 112]
	pop rbx
	cmp rax, rbx
	jne .L_else_8
	jmp .L_start_while_6
.L_else_8:
	mov rax, 10
	push rax
	mov rax, [rbp - 112]
	pop rbx
	cmp rax, rbx
	jl .L_else_9
	jmp .L_end_while_6
.L_else_9:
	mov rax, [rbp - 112]
	push rax
	mov rax, [rbp - 104]
	pop rbx
	add rax, rbx
	mov [rbp - 104], rax
	jmp .L_start_while_6
.L_end_while_6:
	mov rax, 14
	push rax
	mov rax, [rbp - 104]
	pop rbx
	cmp rax, rbx
	jne .L_else_10

.section .rodata
.L_str_13:
	.asciz "-> TEST 4: PASS\n"
.section .text
	lea rax, [rip + .L_str_13]
	mov rcx, rax
	sub rsp, 32
	call printf
	add rsp, 32
	mov rax, 1
	push rax
	mov rax, [rbp - 8]
	pop rbx
	add rax, rbx
	mov [rbp - 8], rax
	jmp .L_end_10
.L_else_10:

.section .rodata
.L_str_14:
	.asciz "-> TEST 4: FAIL (Expected 14)\n"
.section .text
	lea rax, [rip + .L_str_14]
	mov rcx, rax
	sub rsp, 32
	call printf
	add rsp, 32
.L_end_10:

.section .rodata
.L_str_15:
	.asciz "TEST 5: Deeply Nested Function Arguments\n"
.section .text
	lea rax, [rip + .L_str_15]
	mov rcx, rax
	sub rsp, 32
	call printf
	add rsp, 32
	mov rax, 10
	neg rax
	push rax
	mov rax, 20
	push rax
	pop rdx
	pop rcx
	sub rsp, 32
	call add
	add rsp, 32
	push rax
	mov rax, 5
	push rax
	mov rax, 15
	neg rax
	push rax
	pop rdx
	pop rcx
	sub rsp, 32
	call sub
	add rsp, 32
	push rax
	pop rdx
	pop rcx
	sub rsp, 32
	call add
	add rsp, 32
	mov [rbp - 120], rax
	mov rax, 30
	push rax
	mov rax, [rbp - 120]
	pop rbx
	cmp rax, rbx
	jne .L_else_11

.section .rodata
.L_str_16:
	.asciz "-> TEST 5: PASS\n"
.section .text
	lea rax, [rip + .L_str_16]
	mov rcx, rax
	sub rsp, 32
	call printf
	add rsp, 32
	mov rax, 1
	push rax
	mov rax, [rbp - 8]
	pop rbx
	add rax, rbx
	mov [rbp - 8], rax
	jmp .L_end_11
.L_else_11:

.section .rodata
.L_str_17:
	.asciz "-> TEST 5: FAIL (Expected 30)\n"
.section .text
	lea rax, [rip + .L_str_17]
	mov rcx, rax
	sub rsp, 32
	call printf
	add rsp, 32
.L_end_11:

.section .rodata
.L_str_18:
	.asciz "==================================================\n"
.section .text
	lea rax, [rip + .L_str_18]
	mov rcx, rax
	sub rsp, 32
	call printf
	add rsp, 32

.section .rodata
.L_str_19:
	.asciz "TOTAL PASSED TESTS (OUT OF 5):\n"
.section .text
	lea rax, [rip + .L_str_19]
	mov rcx, rax
	sub rsp, 32
	call printf
	add rsp, 32
	mov rax, [rbp - 8]
	mov rdx, rax
	lea rcx, [rip + fmt_int]
	sub rsp, 32
	call printf
	add rsp, 32

.section .rodata
.L_str_20:
	.asciz "\n"
.section .text
	lea rax, [rip + .L_str_20]
	mov rcx, rax
	sub rsp, 32
	call printf
	add rsp, 32
	mov rax, 5
	push rax
	mov rax, [rbp - 8]
	pop rbx
	cmp rax, rbx
	jne .L_else_12

.section .rodata
.L_str_21:
	.asciz ">>> CONGRATULATIONS! ALL LIMIT TESTS PASSED! <<<\n"
.section .text
	lea rax, [rip + .L_str_21]
	mov rcx, rax
	sub rsp, 32
	call printf
	add rsp, 32
	jmp .L_end_12
.L_else_12:

.section .rodata
.L_str_22:
	.asciz ">>> STRESS TEST DETECTED BUGS IN COMPILER! <<<\n"
.section .text
	lea rax, [rip + .L_str_22]
	mov rcx, rax
	sub rsp, 32
	call printf
	add rsp, 32
.L_end_12:

.section .rodata
.L_str_23:
	.asciz "==================================================\n"
.section .text
	lea rax, [rip + .L_str_23]
	mov rcx, rax
	sub rsp, 32
	call printf
	add rsp, 32
	mov rax, 0
	jmp .L_ret_main
.L_ret_main:
	mov rsp, rbp
	pop rbp
	ret
