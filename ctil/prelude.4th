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

: cell
  1 cells ;

: cell+
  1 cells + ;

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
\ IF ... THEN compiles to:
\   0BRANCH OFFSET true-part rest
\ where OFFSET is the offset of rest
\
\ IF ... ELSE ... THEN compiles to :
\   0BRANCH OFFSET true-part BRANCH OFFSET2 false-part rest
\ where OFFSET is the offset of false-part and OFFSET2 is the offset of rest
: if immediate
  ' 0branch ,   \ place 0BRANCH
  here          \ save HERE address on stack
  0 ,           \ place dummy offset in memory
  ;

: then immediate
  dup           ( [saved OFFSET ] [saved OFFSET -- )
  here swap -   ( [saved OFFSET ] [HERE - saved OFFSET ] -- )
  swap          ( [HERE - saved OFFSET ] [saved OFFSET ] -- )
  ! ;

: else immediate
  ' branch ,    \ place BRANCH
  here          \ save HERE address on stack, now [ saved OFFSET ] [ OFFSET2 ] --
  0 ,           \ place dummy offset in memory
  swap          \ [ OFFSET2 ] [ saved OFFSET ] --
                \ From here, do what THEN does to patch the 0BRANCH offset
  dup           \ [ OFFSET2 ] [ saved OFFSET ] [ saved OFFSET ] --
  here swap -   \ [ OFFSET2 ] [ saved OFFSET ] [ HERE - saved OFFSET ] --
  swap          \ [ OFFSET2 ] [ HERE - saved OFFSET] [ saved OFFSET ] --
  ! ;
  
