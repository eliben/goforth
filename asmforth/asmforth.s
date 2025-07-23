# Goal: port http://git.annexia.org/?p=jonesforth.git;a=blob_plain;f=jonesforth.S;hb=HEAD
# to x86_64 Linux assembly.

# as --64 asmforth.s -o asmforth.o
# ld -o asmforth.out asmforth.o

# ---- CONSTANTS ----
.set EXIT_SYSCALL, 60

    .globl _start
    .text

_start:
    # exit(0)
    mov $0, %rdi
    mov $EXIT_SYSCALL, %rax
    syscall
    ret

.macro NEXT
	# Load (%rsi) into %rax, and then increment %rsi += 8
    lodsq
	# Jump to the the value of the memory slot indicated by %rax; this is
	# an indirect jump (indirect threaded code).
    jmp *(%rax)
.endm

