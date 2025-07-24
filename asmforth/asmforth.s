# Goal: port http://git.annexia.org/?p=jonesforth.git;a=blob_plain;f=jonesforth.S;hb=HEAD
# to x86_64 Linux assembly.

# as --64 asmforth.s -o asmforth.o
# ld -o asmforth.out asmforth.o

# ---- CONSTANTS ----
.set EXIT_SYSCALL, 60

    .globl _start
    .text
	.align 8

_start:
    # exit(0)
    mov $0, %rdi
    mov $EXIT_SYSCALL, %rax
    syscall
    ret

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
	# pointing at the first data word.
	# and 
DOCOL:
	PUSHRSP %rsi
	addq $8, %rax
	movq %rax, %rsi
	NEXT


