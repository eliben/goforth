: cr 10 emit ;

\ nested IF that executes in IF
1 if 
  ." outer if ok "
  9 if
    ." inner if ok"
  then
then cr

\ nested ELSE that executes in IF
1 if
  ." outer if ok "
  0 if
    ." inner if ok"
  else
    ." inner else"
  then
then cr

\ nested IF that executes in ELSE
0 if
  ." outer if ok "
else
  ." outer else "
  -1 if ." inner if ok" then
then cr

\ Nested IF that doesn't execute in IF
0 if
  ." outer if ok "
  1 if ." and inner if too " then
else
  ." outer else 4 "
then cr

\ Nested IF that doesn't execute in ELSE
1 if
  ." outer if ok 5 "
  1 if ." and inner if too " then
else
  ." outer else 5 "
then cr

\ ---- OUT ----
outer if ok inner if ok
outer if ok inner else
outer else inner if ok
outer else 4 
outer if ok 5 and inner if too 
