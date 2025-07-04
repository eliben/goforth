: cr 10 emit ;

\ basic IF
1 if ." 1 is true " then ." and after 1" cr

\ basic IF with ELSE that's not executed
1 if ." 1 is the only " else ." maybe not " then cr

\ IF that's not executed
0 if ." 0 is true" then ." and after 0" cr

\ IF with an ELSE that's executed
0 if ." 0 is true" else ." nah it aint " then -1 . cr

\ using IF in a function
: sayparity 2 mod 0 = if ." even" else ." odd" then ;

3 sayparity cr
4 sayparity cr

." empty if clause" cr
." one: " 1 if else 100 . then cr
." zero: " 0 if else 100 . then cr

." empty else clause" cr
." one: " 1 if 100 . else then cr
." zero: " 0 if 100 . else then cr

\ ---- OUT ----
1 is true and after 1
1 is the only 
and after 0
nah it aint -1 
odd
even
empty if clause
one: 
zero: 100 
empty else clause
one: 100 
zero: 
