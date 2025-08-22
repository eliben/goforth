\ Check that the IMMEDIATE word works.

: kwa 99 ;
: kwa-imm immediate 99 ;

\ We don't expect this to change the stack because kwa is not immediate
: word1 kwa ;
.s cr

\ Since kwa-imm is immediate, it's executed while word2 is being defined,
\ so this will add 99 to the stack
: word2 kwa-imm ;
.s cr
bye

\ ---- OUT ----
<0> 
<1> 99 
