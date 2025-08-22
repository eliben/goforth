: cr 10 emit ;

: fizz?  3 mod 0 = dup if ." Fizz" then ;
: buzz?  5 mod 0 = dup if ." Buzz" then ;
: fizz-buzz?  dup fizz? swap buzz? or invert ;
: do-fizz-buzz  16 1 do cr i fizz-buzz? if i . then loop ;
do-fizz-buzz
bye

\ ---- OUT ----
1 
2 
Fizz
4 
Buzz
Fizz
7 
8 
Fizz
Buzz
11 
Fizz
13 
14 
FizzBuzz
