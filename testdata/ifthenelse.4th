: cr 10 emit ;

\ All IF statements are in a word, because in standard Forth they aren't
\ supposed to work in interpreter mode (gforth gives a "interpreting a
\ compile-only word" warning).

: ifthenelse

  \ basic IF
  1 if ." 1 is true " then ." and after 1" cr

  \ basic IF with ELSE that's not executed
  1 if ." 1 is the only " else ." maybe not " then cr

  \ IF that's not executed
  0 if ." 0 is true" then ." and after 0" cr

  \ IF with an ELSE that's executed
  0 if ." 0 is true" else ." nah it aint " then -1 . cr ;

ifthenelse

\ using IF in a small function
: sayparity 2 mod 0 = if ." even" else ." odd" then ;

3 sayparity cr
4 sayparity cr

: ifstr
  ." empty if clause" cr
  ." one: " 1 if else 100 . then cr
  ." zero: " 0 if else 100 . then cr

  ." empty else clause" cr
  ." one: " 1 if 100 . else then cr
  ." zero: " 0 if 100 . else then cr
  ;

ifstr
bye

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
