\ Due to the way goforth is implemented, we support
\ recursion naturally without any special 'recurse'
\ words. By the time the interpreter sees the 'fact'
\ call in the word body below, the 'fact' word was
\ already added to the dict.
: fact ( n -- result )
  dup 0 > if dup 1 - fact * else drop 1 then ; 

: cr 10 emit ;
4 fact . cr
8 fact . cr

: fib-rec ( n -- f )
  dup 2 < if else 1- dup fib-rec swap 1- fib-rec + then ;

: dorec
  10 1 do
    i fib-rec .
  loop cr
  ;

dorec
bye

\ ---- OUT ----
24 
40320 
1 1 2 3 5 8 13 21 34 
