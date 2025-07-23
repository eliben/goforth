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
    lodsq
    jmp *($rax)
.endm

