: cr 10 emit ;

: tuck ( a b -- b a b)
  swap over ;

: ?dup
  dup 0 <> if dup then ;

: gcd ( a b -- gcd)
  ?dup if tuck mod gcd then ; 

." testing ?dup" cr
4 ?dup .s cr
clearstack
0 ?dup .s cr

." testing gcd" cr
39 18 gcd . cr
109 28 gcd . cr
105 28 gcd . cr

\ ---- OUT ----
testing ?dup
<2> 4 4 
<1> 0 
testing gcd
3 
1 
7 
