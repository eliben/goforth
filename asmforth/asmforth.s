# as --32 asmforth.s -o asmforth.o
# ld -m elf_i386 -o asmforth.out asmforth.o

.macro NEXT
    lodsl
    jmp *($eax)
.endm

