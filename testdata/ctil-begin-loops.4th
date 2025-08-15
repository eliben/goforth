: beginloops

  \ count down from 5
  5
  begin
    ." counter=" dup .
    1-
    dup 0 =
  until
  cr

  ;

beginloops

\ ---- OUT ----
counter=5 counter=4 counter=3 counter=2 counter=1

