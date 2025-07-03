: cr 10 emit ;

4 1 do ." hop " loop cr

: linf 4 1 do ." i=" i . loop cr ;
linf

." nested test" cr
2 0 do
  43 40 do
    j . i . ." -- "
  loop
loop cr

." using leave" cr
8 1 do
  i .
  i 5 = if leave then
  i 2 * .
loop cr

." using leave in nested loop" cr
4 1 do
  i . ." -> "
  6 1 do
    i 3 = if leave then
    i .
  loop cr
loop

\ ---- OUT ----
hop hop hop 
i=1 i=2 i=3 
nested test
0 40 -- 0 41 -- 0 42 -- 1 40 -- 1 41 -- 1 42 -- 
using leave
1 2 2 4 3 6 4 8 5 
using leave in nested loop
1 -> 1 2 
2 -> 1 2 
3 -> 1 2 
