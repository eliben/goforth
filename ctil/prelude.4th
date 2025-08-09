: '\n' 10 ;
: bl 32 ;

\ CR prints a carriage return
: cr '\n' emit ;

\ SPACE prints a space
: space bl emit ;

\ VARIABLE expects a name following it in the input
\ ( "name" -- a-addr )
: variable create 1 cells allot ;

: tuck ( x y -- y x y )
  swap over ;

: +! ( addend addr -- )
  TUCK      ( addr addend addr )
  @         ( addr addend value-at-addr )
  +         ( addr updated-value )
  swap      ( updated-value addr )
  ! ;
