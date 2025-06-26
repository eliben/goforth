\ 30 is moved to the return stack
10 20 30 >r .s 10 emit

\ 10*20 on top of stack
* dup . 10 emit

\ get 30 back and add it to the result
r> + . 10 emit

\ 5 remains on TOS, 8 moves to RS
5 8 >r
r@ r@ r@ ( 5 8 8 8 )
.s 10 emit
rdrop
clearstack

\ Test that rdrop works
5 6 7 >r >r rdrop ( only 7 remains on RS ... )
. r> . 10 emit

\ ---- OUT ----
<2> 10 20 
200 
230 
<4> 5 8 8 8 
5 7 
