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

\ Add one char length to value on TOS
: char+ ( n -- n )
  1 chars + ;

\ VARIABLE expects a name following it in the input
\ ( "name" -- a-addr )
: variable create 1 cells allot ;

: tuck ( x y -- y x y )
  swap over ;

: +! ( addend addr -- )
  tuck      ( addr addend addr )
  @         ( addr addend value-at-addr )
  +         ( addr updated-value )
  swap      ( updated-value addr )
  ! ;

\ IF, ELSE, THEN work together to compile to lower-level branches.
\
\ IF compiles to:
\   0BRANCH OFFSET true-part rest
\ where OFFSET is the offset of rest
: if immediate
  ' 0branch ,   \ place 0BRANCH
  here          \ save HERE address on stack
  0 ,           \ place dummy offset in memory
  ;

: then immediate
  dup           ( [saved HERE] [saved HERE] -- )
  here swap -   ( [saved HERE] [HERE - saved HERE] -- )
  swap          ( [HERE - saved HERE] [saved HERE] -- )
  ! ;

