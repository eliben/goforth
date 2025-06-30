: cr 10 emit ;

\ basic IF
1 if ." 1 is true " then ." and after 1" cr

\ basic IF with ELSE that's not executed
1 if ." 1 is the only " else ." maybe not " then cr

\ IF that's not executed
0 if ." 0 is true" then ." and after 0" cr

\ IF with an ELSE that's executed
0 if ." 0 is true" else ." nah it aint " then -1 . cr

\ ---- OUT ----
1 is true and after 1
1 is the only 
and after 0
nah it aint -1
