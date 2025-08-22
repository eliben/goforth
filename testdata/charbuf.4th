create buf 10 chars allot   \ reserve 10 bytes
buf      CHAR H over c!     \ store 'H' at buf[0]
buf char+ CHAR i over c!    \ store 'i' at buf[1]

buf c@ .          \ prints 72 (ASCII 'H')
buf char+ c@ emit \ prints i

10 emit
bye

\ ---- OUT ----
72 i
