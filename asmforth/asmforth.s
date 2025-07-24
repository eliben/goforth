# Goal: port http://git.annexia.org/?p=jonesforth.git;a=blob_plain;f=jonesforth.S;hb=HEAD
# to x86_64 Linux assembly.

# as --64 asmforth.s -o asmforth.o
# ld -o asmforth.out asmforth.o

# ---- CONSTANTS ----
.set EXIT_SYSCALL, 60

    .globl _start
    .text
	.align 8

	# NEXT is at the end of Forth primitives (written in asm);
	# it's our "return".
.macro NEXT
	# Load (%rsi) into %rax, and then increment %rsi += 8
    lodsq
	# Jump to the the value of the memory slot indicated by %rax; this is
	# an indirect jump (indirect threaded code).
    jmp *(%rax)
.endm

	# Push reg onto return stack
.macro PUSHRSP reg
	lea -8(%rbp), %rbp
	movq \reg, %rbp
.endm

	# Pop top of return stack to reg
.macro POPRSP reg
	mov (%rbp), \reg
	lea 8(%rbp), %rbp
.endm

	# DOCOL is the interpreter for Forth-implemented words.
	# It saves the current rsi on the return stack, then sets rsi to rax+8,
	# pointing at the first data word. Then NEXT is ready to jump to that
	# code.
	# TODO: rax is set up by NEXT, yes?
DOCOL:
	PUSHRSP %rsi
	addq $8, %rax
	movq %rax, %rsi
	NEXT

.set F_IMMED, 0x80
.set F_HIDDEN, 0x28
.set F_LENMASK, 0x1f

	// Chain of links
.set link, 0

.macro defword name, namelen, flags=0, label
	.section .rodata
	.align 8
	.globl name_\label
name_\label :
	.quad link
	.set link, name_\label
	.byte \flags+\namelen
	.ascii "\name"
	.align 8
	.globl \label
\label :
	.quad DOCOL
	# list of word pointers follow
.endm

.macro defcode name, namelen, flags=0, label
	.section .rodata
	.align 8
	.globl name_\label
name_\label :
	.quad link
	.set link, name_\label
	.byte \flags+\namelen
	.ascii "\name"
	.align 8
	.globl \label
\label :
	# Assembler code follows
.endm

	#
	# Builtin FORTH primitives implemented directly in assembly.
	#

	defcode "DROP",4,,DROP
	pop %rbx
	NEXT

	defcode "SWAP",4,,SWAP
	pop %rax
	pop %rbx
	push %rax
	push %rbx
	NEXT

	defcode "DUP",3,,DUP
	mov (%rsp), %rax
	push %rax
	NEXT

	defcode "OVER",4,,OVER
	mov 8(%rsp), %rax
	push %rax
	NEXT

	defcode "ROT",3,,ROT
	pop %rax
	pop %rbx
	pop %rcx
	push %rbx
	push %rax
	push %rcx
	NEXT

	defcode "-ROT",4,,NROT
	pop %rax
	pop %rbx
	pop %rcx
	push %rax
	push %rcx
	push %rbx
	NEXT

	defcode "2DROP",5,,TWODROP
	pop %rax
	pop %rax
	NEXT

	defcode "2DUP",5,,TWODUP
	mov (%rsp), %rax
	mov 8(%rsp), %rbx
	push %rbx
	push %rax
	NEXT

	defcode "2SWAP",5,,TWOSWAP
	pop %rax
	pop %rbx
	pop %rcx
	pop %rdx
	push %rbx
	push %rax
	push %rdx
	push %rcx
	NEXT

	defcode "?DUP",4,,QDUP
	movq (%rsp), %rax
	test %rax, %rax
	jz 1f
	push %rax
1:	NEXT

	defcode "1+",2,,INCR
	incq (%rsp)
	NEXT

	defcode "1+",2,,DECR
	decq (%rsp)
	NEXT

	defcode "8+",2,,INCR8
	addq $8, (%rsp)
	NEXT

	defcode "8-",2,,DECR8
	subq $8, (%rsp)
	NEXT
	
	defcode "+",1,,ADD
	pop %rax
	addq %rax, (%rsp)
	NEXT
	
	defcode "-",1,,SUB
	pop %rax
	subq %rax, (%rsp)
	NEXT

	defcode "*",1,,MUL
	pop %rax
	pop %rbx
	imulq %rbx, %rax
	push %rax
	NEXT

	defcode "=",1,,EQU
	pop %rax
	pop %rbx
	cmp %rax, %rbx
	sete %al
	movzbq %al, %rax
	pushq %rax
	NEXT

	# TODO: will skip many builtins for now, until the whole thing is working.

_start:
	cld
	mov %rsp, var_S0
	mov $return_stack_top, %rbp
	call set_up_data_segment

	# Initialize interpreter and run it
	mov $cold_start, %rsi
	NEXT

.section .rodata

	# High-level code without a codeword.
cold_start:
	.quad QUIT

    # exit(0)
    mov $0, %rdi
    mov $EXIT_SYSCALL, %rax
    syscall
    ret
