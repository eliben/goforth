: '\n' 10 ;
: bl 32 ;

\ CR prints a carriage return
: cr '\n' emit ;

\ SPACE prints a space
: space bl emit ;

\ Implement / and mod in terms of /mod
: / /mod swap drop ;
: mod /mod drop ;

\ Compute number of bytes in n cells
: cells ( n -- n )
  8 * ;

\ Compute number of bytes in n chars
: chars ( n -- n )
  ;

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
