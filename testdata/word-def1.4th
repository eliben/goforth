\ Basic definition without numbers
: sq dup * ;

32 sq . 10 emit
10 sq 20 sq + . 10 emit

\ Definition that uses other definitions
: sumsq sq swap sq + ;

3 4 sumsq . 10 emit

: dbl 2 * ;

99 dbl . 10 emit

: trpl dup dbl + ;

14 trpl . 10 emit

\ ---- OUT ----
1024 
500 
25 
198 
42 
