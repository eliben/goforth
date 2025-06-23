: two 2 ;
: five 5 ;

two . 10 emit
two five . . 10 emit
two 8 five 9 . . . . 10 emit

\ multiple words inside a definition
: two-dup dup dup ;

7 two-dup . . . 10 emit

\ comments inside word
: pushpie ( some comment)
  3 1 ( commend inline ) 4 \ line comment
  ;

pushpie . . . 10 emit

\ nested definitions

: four-dup two-dup two-dup ;

8 four-dup . . . . . 10 emit

: ; \ test that an empty definition works

." fin"

\ ---- OUT ----
2 
5 2 
9 5 8 2 
7 7 7 
4 1 3 
8 8 8 8 8 
fin
