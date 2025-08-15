42 constant asterisk

: beginloops

  \ count down from 5
  5
  begin
    ." c=" dup .
    1-
    dup 0 =
  until
  cr
  clearstack

  \ with while...repeat, calculate 6!
  1 7               ( result counter -- )
  begin
    1- dup 0 >      ( result counter cond -- )
  while
    swap over       ( counter result counter -- )
    *               ( counter updated-result -- )
    swap
  repeat
  drop              ( result -- )

  ." result is " . cr
  clearstack

  \ nest an IF in a loop
  5
  begin
    ." c=" dup .
    dup 3 = if ." three!! " then
    1-
    dup 0 =
  until
  cr

  \ nested loops
  5
  begin
    dup
    begin
      asterisk emit
      1-
      dup 0 =
    until
    drop
    cr
    1-
    dup 0 =
  until

  ;

beginloops

\ ---- OUT ----
c=5 c=4 c=3 c=2 c=1 
result is 720 
c=5 c=4 c=3 three!! c=2 c=1 
*****
****
***
**
*

