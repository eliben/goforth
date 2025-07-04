: fact ( n -- result )
  dup 0 > if dup 1 - fact * else drop 1 then ; 

: cr 10 emit ;
4 fact . cr
8 fact . cr

\ ---- OUT ----
24 
40320 
