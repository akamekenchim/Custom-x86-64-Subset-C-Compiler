.intel_syntax noprefix
.global main
main:
	push rbp
	mov rbp, rsp
	sub rsp, 1024
.section .rodata
fmt_int:
	.asciz "%d\n"
.text

.section .rodata
.L_str_0:
	.asciz "====================================================\n"
.section .text
	lea rax, [rip + .L_str_0]
	mov rcx, rax
	sub rsp, 32
	call printf
	add rsp, 32

.section .rodata
.L_str_1:
	.asciz "     C COMPILER HARDCORE EDGE-CASE TEST SUITE       \n"
.section .text
	lea rax, [rip + .L_str_1]
	mov rcx, rax
	sub rsp, 32
	call printf
	add rsp, 32

.section .rodata
.L_str_2:
	.asciz "====================================================\n"
.section .text
	lea rax, [rip + .L_str_2]
	mov rcx, rax
	sub rsp, 32
	call printf
	add rsp, 32
	mov rax, 0
	mov [rbp - 8], rax
	mov rax, 0
	mov [rbp - 16], rax
	lea rax, [rbp - 88]
	mov rax, 0
	mov [rbp - 96], rax
.L_start_while_0:
	mov rax, 3
	push rax
	mov rax, [rbp - 96]
	pop rbx
	cmp rax, rbx
	jge .L_end_while_0
	mov rax, 0
	mov [rbp - 104], rax
	mov rax, 0
	mov [rbp - 104], rax
.L_start_for_1:
	mov rax, 3
	push rax
	mov rax, [rbp - 104]
	pop rbx
	cmp rax, rbx
	jge .L_end_for_1
	mov rax, 1
	push rax
	mov rax, [rbp - 104]
	pop rbx
	add rax, rbx
	push rax
	mov rax, 10
	push rax
	mov rax, 1
	push rax
	mov rax, [rbp - 96]
	pop rbx
	add rax, rbx
	pop rbx
	imul rax, rbx
	pop rbx
	add rax, rbx
	push rax
	mov rax, [rbp - 104]
	push rax
	mov rax, 3
	push rax
	mov rax, [rbp - 96]
	pop rbx
	imul rax, rbx
	pop rbx
	add rax, rbx
	imul rax, 8
	lea rbx, [rbp - 88]
	add rax, rbx
	pop rbx
	mov [rax], rbx
.L_inc_for_1:
	mov rax, 1
	push rax
	mov rax, [rbp - 104]
	pop rbx
	add rax, rbx
	mov [rbp - 104], rax
	jmp .L_start_for_1
.L_end_for_1:
	mov rax, 1
	push rax
	mov rax, [rbp - 96]
	pop rbx
	add rax, rbx
	mov [rbp - 96], rax
	jmp .L_start_while_0
.L_end_while_0:
	mov rax, 0
	mov [rbp - 112], rax
	mov rax, 0
	mov [rbp - 120], rax
	mov rax, 0
	mov [rbp - 120], rax
.L_start_for_2:
	mov rax, 3
	push rax
	mov rax, [rbp - 120]
	pop rbx
	cmp rax, rbx
	jge .L_end_for_2
	mov rax, [rbp - 120]
	push rax
	mov rax, 3
	push rax
	mov rax, [rbp - 120]
	pop rbx
	imul rax, rbx
	pop rbx
	add rax, rbx
	imul rax, 8
	lea rbx, [rbp - 88]
	add rax, rbx
	mov rax, [rax]
	push rax
	mov rax, [rbp - 112]
	pop rbx
	add rax, rbx
	mov [rbp - 112], rax
.L_inc_for_2:
	mov rax, 1
	push rax
	mov rax, [rbp - 120]
	pop rbx
	add rax, rbx
	mov [rbp - 120], rax
	jmp .L_start_for_2
.L_end_for_2:
	mov rax, 11
	push rax
	mov rax, 0
	imul rax, 8
	lea rbx, [rbp - 88]
	add rax, rbx
	mov rax, [rax]
	pop rbx
	cmp rax, rbx
	jne .L_else_3
	mov rax, 22
	push rax
	mov rax, 4
	imul rax, 8
	lea rbx, [rbp - 88]
	add rax, rbx
	mov rax, [rax]
	pop rbx
	cmp rax, rbx
	jne .L_else_4
	mov rax, 33
	push rax
	mov rax, 8
	imul rax, 8
	lea rbx, [rbp - 88]
	add rax, rbx
	mov rax, [rax]
	pop rbx
	cmp rax, rbx
	jne .L_else_5
	mov rax, 66
	push rax
	mov rax, [rbp - 112]
	pop rbx
	cmp rax, rbx
	jne .L_else_6

.section .rodata
.L_str_3:
	.asciz "[1] 1D Grid & Scaled Indexing: PASSED\n"
.section .text
	lea rax, [rip + .L_str_3]
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
	jmp .L_end_6
.L_else_6:

.section .rodata
.L_str_4:
	.asciz "[1] 1D Grid & Scaled Indexing: FAILED\n"
.section .text
	lea rax, [rip + .L_str_4]
	mov rcx, rax
	sub rsp, 32
	call printf
	add rsp, 32
	mov rax, 1
	push rax
	mov rax, [rbp - 16]
	pop rbx
	add rax, rbx
	mov [rbp - 16], rax
.L_end_6:
	jmp .L_end_5
.L_else_5:

.section .rodata
.L_str_5:
	.asciz "[1] 1D Grid & Scaled Indexing: FAILED\n"
.section .text
	lea rax, [rip + .L_str_5]
	mov rcx, rax
	sub rsp, 32
	call printf
	add rsp, 32
	mov rax, 1
	push rax
	mov rax, [rbp - 16]
	pop rbx
	add rax, rbx
	mov [rbp - 16], rax
.L_end_5:
	jmp .L_end_4
.L_else_4:

.section .rodata
.L_str_6:
	.asciz "[1] 1D Grid & Scaled Indexing: FAILED\n"
.section .text
	lea rax, [rip + .L_str_6]
	mov rcx, rax
	sub rsp, 32
	call printf
	add rsp, 32
	mov rax, 1
	push rax
	mov rax, [rbp - 16]
	pop rbx
	add rax, rbx
	mov [rbp - 16], rax
.L_end_4:
	jmp .L_end_3
.L_else_3:

.section .rodata
.L_str_7:
	.asciz "[1] 1D Grid & Scaled Indexing: FAILED\n"
.section .text
	lea rax, [rip + .L_str_7]
	mov rcx, rax
	sub rsp, 32
	call printf
	add rsp, 32
	mov rax, 1
	push rax
	mov rax, [rbp - 16]
	pop rbx
	add rax, rbx
	mov [rbp - 16], rax
.L_end_3:
	lea rax, [rbp - 168]
	mov rax, 97
	push rax
	mov rax, 0
	imul rax, 1
	lea rbx, [rbp - 168]
	add rax, rbx
	pop rbx
	mov byte ptr [rax], bl
	mov rax, 98
	push rax
	mov rax, 1
	imul rax, 1
	lea rbx, [rbp - 168]
	add rax, rbx
	pop rbx
	mov byte ptr [rax], bl
	mov rax, 99
	push rax
	mov rax, 2
	imul rax, 1
	lea rbx, [rbp - 168]
	add rax, rbx
	pop rbx
	mov byte ptr [rax], bl
	mov rax, 100
	push rax
	mov rax, 3
	imul rax, 1
	lea rbx, [rbp - 168]
	add rax, rbx
	pop rbx
	mov byte ptr [rax], bl
	mov rax, 101
	push rax
	mov rax, 4
	imul rax, 1
	lea rbx, [rbp - 168]
	add rax, rbx
	pop rbx
	mov byte ptr [rax], bl
	mov rax, 0
	push rax
	mov rax, 5
	imul rax, 1
	lea rbx, [rbp - 168]
	add rax, rbx
	pop rbx
	mov byte ptr [rax], bl
	lea rax, [rbp - 168]
	mov [rbp - 176], rax
	mov rax, 0
	mov [rbp - 184], rax
	mov rax, 4
	mov [rbp - 192], rax
.L_start_while_7:
	mov rax, [rbp - 192]
	push rax
	mov rax, [rbp - 184]
	pop rbx
	cmp rax, rbx
	jge .L_end_while_7
	mov rax, [rbp - 184]
	imul rax, 1
	mov rbx, [rbp - 176]
	add rax, rbx
	movzx rax, byte ptr [rax]
	mov byte ptr [rbp - 200], al
	mov rax, [rbp - 192]
	imul rax, 1
	mov rbx, [rbp - 176]
	add rax, rbx
	movzx rax, byte ptr [rax]
	push rax
	mov rax, [rbp - 184]
	imul rax, 1
	mov rbx, [rbp - 176]
	add rax, rbx
	pop rbx
	mov byte ptr [rax], bl
	movzx rax, byte ptr [rbp - 200]
	push rax
	mov rax, [rbp - 192]
	imul rax, 1
	mov rbx, [rbp - 176]
	add rax, rbx
	pop rbx
	mov byte ptr [rax], bl
	mov rax, 1
	push rax
	mov rax, [rbp - 184]
	pop rbx
	add rax, rbx
	mov [rbp - 184], rax
	mov rax, 1
	push rax
	mov rax, [rbp - 192]
	pop rbx
	sub rax, rbx
	mov [rbp - 192], rax
	jmp .L_start_while_7
.L_end_while_7:
	mov rax, 101
	push rax
	mov rax, 0
	imul rax, 1
	lea rbx, [rbp - 168]
	add rax, rbx
	movzx rax, byte ptr [rax]
	pop rbx
	cmp rax, rbx
	jne .L_else_8
	mov rax, 100
	push rax
	mov rax, 1
	imul rax, 1
	lea rbx, [rbp - 168]
	add rax, rbx
	movzx rax, byte ptr [rax]
	pop rbx
	cmp rax, rbx
	jne .L_else_9
	mov rax, 99
	push rax
	mov rax, 2
	imul rax, 1
	lea rbx, [rbp - 168]
	add rax, rbx
	movzx rax, byte ptr [rax]
	pop rbx
	cmp rax, rbx
	jne .L_else_10
	mov rax, 97
	push rax
	mov rax, 4
	imul rax, 1
	lea rbx, [rbp - 168]
	add rax, rbx
	movzx rax, byte ptr [rax]
	pop rbx
	cmp rax, rbx
	jne .L_else_11

.section .rodata
.L_str_8:
	.asciz "[2] Two-Pointer Char In-Place Swap: PASSED\n"
.section .text
	lea rax, [rip + .L_str_8]
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
.L_str_9:
	.asciz "[2] Two-Pointer Char In-Place Swap: FAILED\n"
.section .text
	lea rax, [rip + .L_str_9]
	mov rcx, rax
	sub rsp, 32
	call printf
	add rsp, 32
	mov rax, 1
	push rax
	mov rax, [rbp - 16]
	pop rbx
	add rax, rbx
	mov [rbp - 16], rax
.L_end_11:
	jmp .L_end_10
.L_else_10:

.section .rodata
.L_str_10:
	.asciz "[2] Two-Pointer Char In-Place Swap: FAILED\n"
.section .text
	lea rax, [rip + .L_str_10]
	mov rcx, rax
	sub rsp, 32
	call printf
	add rsp, 32
	mov rax, 1
	push rax
	mov rax, [rbp - 16]
	pop rbx
	add rax, rbx
	mov [rbp - 16], rax
.L_end_10:
	jmp .L_end_9
.L_else_9:

.section .rodata
.L_str_11:
	.asciz "[2] Two-Pointer Char In-Place Swap: FAILED\n"
.section .text
	lea rax, [rip + .L_str_11]
	mov rcx, rax
	sub rsp, 32
	call printf
	add rsp, 32
	mov rax, 1
	push rax
	mov rax, [rbp - 16]
	pop rbx
	add rax, rbx
	mov [rbp - 16], rax
.L_end_9:
	jmp .L_end_8
.L_else_8:

.section .rodata
.L_str_12:
	.asciz "[2] Two-Pointer Char In-Place Swap: FAILED\n"
.section .text
	lea rax, [rip + .L_str_12]
	mov rcx, rax
	sub rsp, 32
	call printf
	add rsp, 32
	mov rax, 1
	push rax
	mov rax, [rbp - 16]
	pop rbx
	add rax, rbx
	mov [rbp - 16], rax
.L_end_8:
	mov rax, 0
	mov [rbp - 208], rax
	mov rax, 0
	mov [rbp - 216], rax
.L_start_while_12:
	mov rax, 4
	push rax
	mov rax, [rbp - 216]
	pop rbx
	cmp rax, rbx
	jge .L_end_while_12
	mov rax, 1
	push rax
	mov rax, [rbp - 216]
	pop rbx
	add rax, rbx
	mov [rbp - 216], rax
	mov rax, 0
	mov [rbp - 224], rax
	mov rax, 1
	mov [rbp - 224], rax
.L_start_for_13:
	mov rax, 5
	push rax
	mov rax, [rbp - 224]
	pop rbx
	cmp rax, rbx
	jg .L_end_for_13
	mov rax, 0
	push rax
	mov rax, 2
	push rax
	mov rax, [rbp - 224]
	pop rbx
	cqo
	idiv rbx
	mov rax, rdx
	pop rbx
	cmp rax, rbx
	jne .L_else_14
	jmp .L_inc_for_13
.L_else_14:
	mov rax, 3
	push rax
	mov rax, [rbp - 224]
	pop rbx
	cmp rax, rbx
	jle .L_else_15
	jmp .L_end_for_13
.L_else_15:
	mov rax, [rbp - 224]
	push rax
	mov rax, [rbp - 208]
	pop rbx
	add rax, rbx
	mov [rbp - 208], rax
.L_inc_for_13:
	mov rax, 1
	push rax
	mov rax, [rbp - 224]
	pop rbx
	add rax, rbx
	mov [rbp - 224], rax
	jmp .L_start_for_13
.L_end_for_13:
	jmp .L_start_while_12
.L_end_while_12:
	mov rax, 16
	push rax
	mov rax, [rbp - 208]
	pop rbx
	cmp rax, rbx
	jne .L_else_16

.section .rodata
.L_str_13:
	.asciz "[3] Interleaved Flow Control: PASSED\n"
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
	jmp .L_end_16
.L_else_16:

.section .rodata
.L_str_14:
	.asciz "[3] Interleaved Flow Control: FAILED\n"
.section .text
	lea rax, [rip + .L_str_14]
	mov rcx, rax
	sub rsp, 32
	call printf
	add rsp, 32
	mov rax, 1
	push rax
	mov rax, [rbp - 16]
	pop rbx
	add rax, rbx
	mov [rbp - 16], rax
.L_end_16:
	mov rax, 50
	mov [rbp - 232], rax
	mov rax, 100
	mov [rbp - 240], rax
	lea rax, [rbp - 232]
	mov [rbp - 248], rax
	lea rax, [rbp - 240]
	mov [rbp - 256], rax
	mov rax, 25
	push rax
	mov rax, [rbp - 248]
	mov rax, [rax]
	pop rbx
	add rax, rbx
	push rax
	mov rax, [rbp - 248]
	pop rbx
	mov [rax], rbx
	mov rax, [rbp - 248]
	mov rax, [rax]
	push rax
	mov rax, [rbp - 256]
	mov rax, [rax]
	pop rbx
	sub rax, rbx
	push rax
	mov rax, [rbp - 256]
	pop rbx
	mov [rax], rbx
	mov rax, [rbp - 256]
	mov [rbp - 248], rax
	mov rax, 2
	push rax
	mov rax, [rbp - 248]
	mov rax, [rax]
	pop rbx
	imul rax, rbx
	push rax
	mov rax, [rbp - 248]
	pop rbx
	mov [rax], rbx
	mov rax, 75
	push rax
	mov rax, [rbp - 232]
	pop rbx
	cmp rax, rbx
	jne .L_else_17
	mov rax, 50
	push rax
	mov rax, [rbp - 240]
	pop rbx
	cmp rax, rbx
	jne .L_else_18

.section .rodata
.L_str_15:
	.asciz "[4] Pointer Deref & Re-assignment: PASSED\n"
.section .text
	lea rax, [rip + .L_str_15]
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
	jmp .L_end_18
.L_else_18:

.section .rodata
.L_str_16:
	.asciz "[4] Pointer Deref & Re-assignment: FAILED\n"
.section .text
	lea rax, [rip + .L_str_16]
	mov rcx, rax
	sub rsp, 32
	call printf
	add rsp, 32
	mov rax, 1
	push rax
	mov rax, [rbp - 16]
	pop rbx
	add rax, rbx
	mov [rbp - 16], rax
.L_end_18:
	jmp .L_end_17
.L_else_17:

.section .rodata
.L_str_17:
	.asciz "[4] Pointer Deref & Re-assignment: FAILED\n"
.section .text
	lea rax, [rip + .L_str_17]
	mov rcx, rax
	sub rsp, 32
	call printf
	add rsp, 32
	mov rax, 1
	push rax
	mov rax, [rbp - 16]
	pop rbx
	add rax, rbx
	mov [rbp - 16], rax
.L_end_17:

.section .rodata
.L_str_18:
	.asciz "====================================================\n"
.section .text
	lea rax, [rip + .L_str_18]
	mov rcx, rax
	sub rsp, 32
	call printf
	add rsp, 32

.section .rodata
.L_str_19:
	.asciz "Passed Modules: "
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
	.asciz "Failed Modules: "
.section .text
	lea rax, [rip + .L_str_20]
	mov rcx, rax
	sub rsp, 32
	call printf
	add rsp, 32
	mov rax, [rbp - 16]
	mov rdx, rax
	lea rcx, [rip + fmt_int]
	sub rsp, 32
	call printf
	add rsp, 32
	mov rax, 0
	push rax
	mov rax, [rbp - 16]
	pop rbx
	cmp rax, rbx
	jne .L_else_19

.section .rodata
.L_str_21:
	.asciz ">>> HARDCORE EDGE-CASE TESTS PASSED! <<<\n"
.section .text
	lea rax, [rip + .L_str_21]
	mov rcx, rax
	sub rsp, 32
	call printf
	add rsp, 32
	jmp .L_end_19
.L_else_19:

.section .rodata
.L_str_22:
	.asciz ">>> HARDCORE EDGE-CASE TESTS FAILED! <<<\n"
.section .text
	lea rax, [rip + .L_str_22]
	mov rcx, rax
	sub rsp, 32
	call printf
	add rsp, 32
.L_end_19:

.section .rodata
.L_str_23:
	.asciz "====================================================\n"
.section .text
	lea rax, [rip + .L_str_23]
	mov rcx, rax
	sub rsp, 32
	call printf
	add rsp, 32
	mov rax, 0
	leave
	ret
