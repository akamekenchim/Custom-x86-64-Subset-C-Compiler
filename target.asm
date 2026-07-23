.intel_syntax noprefix
.global main
main:
	push rbp
	mov rbp, rsp
	sub rsp, 64
.section .rodata
fmt_int:
	.asciz "%d\n"
.text

.section .rodata
.L_str_0:
	.asciz "=== EXPLICIT TYPE INT INTEGRATION TEST ===\n"
.section .text
	lea rax, [rip + .L_str_0]
	mov rcx, rax
	sub rsp, 32
	call printf
	add rsp, 32
	mov rax, 15
	mov [rbp - 8], rax
	mov rax, 20
	mov [rbp - 16], rax
	mov rax, 15
	mov [rbp - 24], rax
	mov rax, [rbp - 24]
	push rax
	mov rax, [rbp - 8]
	pop rbx
	cmp rax, rbx
	jne .L_else_0

.section .rodata
.L_str_1:
	.asciz "Test 1 (==): PASSED\n"
.section .text
	lea rax, [rip + .L_str_1]
	mov rcx, rax
	sub rsp, 32
	call printf
	add rsp, 32
	jmp .L_end_0
.L_else_0:

.section .rodata
.L_str_2:
	.asciz "Test 1 (==): FAILED\n"
.section .text
	lea rax, [rip + .L_str_2]
	mov rcx, rax
	sub rsp, 32
	call printf
	add rsp, 32
.L_end_0:
	mov rax, [rbp - 16]
	push rax
	mov rax, [rbp - 8]
	pop rbx
	cmp rax, rbx
	je .L_else_1

.section .rodata
.L_str_3:
	.asciz "Test 2 (!=): PASSED\n"
.section .text
	lea rax, [rip + .L_str_3]
	mov rcx, rax
	sub rsp, 32
	call printf
	add rsp, 32
	jmp .L_end_1
.L_else_1:

.section .rodata
.L_str_4:
	.asciz "Test 2 (!=): FAILED\n"
.section .text
	lea rax, [rip + .L_str_4]
	mov rcx, rax
	sub rsp, 32
	call printf
	add rsp, 32
.L_end_1:
	mov rax, [rbp - 16]
	push rax
	mov rax, [rbp - 8]
	pop rbx
	cmp rax, rbx
	jge .L_else_2

.section .rodata
.L_str_5:
	.asciz "Test 3 (<): PASSED\n"
.section .text
	lea rax, [rip + .L_str_5]
	mov rcx, rax
	sub rsp, 32
	call printf
	add rsp, 32
	jmp .L_end_2
.L_else_2:

.section .rodata
.L_str_6:
	.asciz "Test 3 (<): FAILED\n"
.section .text
	lea rax, [rip + .L_str_6]
	mov rcx, rax
	sub rsp, 32
	call printf
	add rsp, 32
.L_end_2:
	mov rax, [rbp - 8]
	push rax
	mov rax, [rbp - 16]
	pop rbx
	cmp rax, rbx
	jle .L_else_3

.section .rodata
.L_str_7:
	.asciz "Test 4 (>): PASSED\n"
.section .text
	lea rax, [rip + .L_str_7]
	mov rcx, rax
	sub rsp, 32
	call printf
	add rsp, 32
	jmp .L_end_3
.L_else_3:

.section .rodata
.L_str_8:
	.asciz "Test 4 (>): FAILED\n"
.section .text
	lea rax, [rip + .L_str_8]
	mov rcx, rax
	sub rsp, 32
	call printf
	add rsp, 32
.L_end_3:
	mov rax, [rbp - 24]
	push rax
	mov rax, [rbp - 8]
	pop rbx
	cmp rax, rbx
	jg .L_else_4

.section .rodata
.L_str_9:
	.asciz "Test 5 (<=): PASSED\n"
.section .text
	lea rax, [rip + .L_str_9]
	mov rcx, rax
	sub rsp, 32
	call printf
	add rsp, 32
	jmp .L_end_4
.L_else_4:

.section .rodata
.L_str_10:
	.asciz "Test 5 (<=): FAILED\n"
.section .text
	lea rax, [rip + .L_str_10]
	mov rcx, rax
	sub rsp, 32
	call printf
	add rsp, 32
.L_end_4:
	mov rax, [rbp - 8]
	push rax
	mov rax, [rbp - 16]
	pop rbx
	cmp rax, rbx
	jl .L_else_5

.section .rodata
.L_str_11:
	.asciz "Test 6 (>=): PASSED\n"
.section .text
	lea rax, [rip + .L_str_11]
	mov rcx, rax
	sub rsp, 32
	call printf
	add rsp, 32
	jmp .L_end_5
.L_else_5:

.section .rodata
.L_str_12:
	.asciz "Test 6 (>=): FAILED\n"
.section .text
	lea rax, [rip + .L_str_12]
	mov rcx, rax
	sub rsp, 32
	call printf
	add rsp, 32
.L_end_5:
	mov rax, 3
	push rax
	mov rax, 10
	push rax
	mov rax, 2
	push rax
	mov rax, 5
	push rax
	mov rax, [rbp - 8]
	pop rbx
	add rax, rbx
	pop rbx
	imul rax, rbx
	pop rbx
	sub rax, rbx
	pop rbx
	cqo
	idiv rbx
	mov [rbp - 32], rax
	mov rax, 1
	push rax
	mov rax, 31
	push rax
	mov rax, 2
	push rax
	mov rax, [rbp - 32]
	pop rcx
	sal rax, cl
	pop rbx
	and rax, rbx
	pop rbx
	or rax, rbx
	mov [rbp - 40], rax

.section .rodata
.L_str_13:
	.asciz "Calculated bit_calc: "
.section .text
	lea rax, [rip + .L_str_13]
	mov rcx, rax
	sub rsp, 32
	call printf
	add rsp, 32
	mov rax, [rbp - 40]
	mov rdx, rax
	lea rcx, [rip + fmt_int]
	sub rsp, 32
	call printf
	add rsp, 32
	mov rax, 0
	mov [rbp - 48], rax
	mov rax, [rbp - 56]
	mov rax, 0
	mov [rbp - 56], rax
.L_start_while_6:
	mov rax, 6
	push rax
	mov rax, [rbp - 48]
	pop rbx
	cmp rax, rbx
	je .L_end_while_6
	mov rax, 0
	push rax
	mov rax, 2
	push rax
	mov rax, [rbp - 48]
	pop rbx
	cqo
	idiv rbx
	mov rax, rdx
	pop rbx
	cmp rax, rbx
	jne .L_else_7
	mov rax, 3
	push rax
	mov rax, [rbp - 48]
	pop rbx
	imul rax, rbx
	push rax
	mov rax, [rbp - 56]
	pop rbx
	add rax, rbx
	mov [rbp - 56], rax
	jmp .L_end_7
.L_else_7:
	mov rax, 1
	push rax
	mov rax, [rbp - 48]
	pop rcx
	sal rax, cl
	push rax
	mov rax, [rbp - 56]
	pop rbx
	add rax, rbx
	mov [rbp - 56], rax
.L_end_7:
	mov rax, 1
	push rax
	mov rax, [rbp - 48]
	pop rbx
	add rax, rbx
	mov [rbp - 48], rax
	jmp .L_start_while_6
.L_end_while_6:

.section .rodata
.L_str_14:
	.asciz "Loop Accumulator Result: "
.section .text
	lea rax, [rip + .L_str_14]
	mov rcx, rax
	sub rsp, 32
	call printf
	add rsp, 32
	mov rax, [rbp - 56]
	mov rdx, rax
	lea rcx, [rip + fmt_int]
	sub rsp, 32
	call printf
	add rsp, 32
	mov rax, 36
	push rax
	mov rax, [rbp - 56]
	pop rbx
	cmp rax, rbx
	jne .L_else_8

.section .rodata
.L_str_15:
	.asciz "Final Check: PASSED\n"
.section .text
	lea rax, [rip + .L_str_15]
	mov rcx, rax
	sub rsp, 32
	call printf
	add rsp, 32
	jmp .L_end_8
.L_else_8:

.section .rodata
.L_str_16:
	.asciz "Final Check: FAILED\n"
.section .text
	lea rax, [rip + .L_str_16]
	mov rcx, rax
	sub rsp, 32
	call printf
	add rsp, 32
.L_end_8:

.section .rodata
.L_str_17:
	.asciz "=== ALL TESTS COMPLETED ===\n"
.section .text
	lea rax, [rip + .L_str_17]
	mov rcx, rax
	sub rsp, 32
	call printf
	add rsp, 32
	mov rax, 0
	leave
	ret
