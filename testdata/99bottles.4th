: cr 10 emit ;

: space 32 emit ;

: ?dup ( x -- 0 | x x )
  dup
  if dup then ;

: many ( n )
  ?dup if . else ." no more " then ;

: bottles ( n )
  ." bottle" 1 - if ." s" then ;

: beer ( n )
  cr
  dup many bottles space ." of beer" ;

: wall
  space ." on the wall" ;

: drink
  cr
  ." take one down and pass it around" ;

: another
  ?dup if drink 1- then ;

: verse
  dup beer wall dup beer another beer wall cr ;

: verses
  0 swap do i verse 1 -loop ;

3 verses

\ ---- OUT ----

3 bottles of beer on the wall
3 bottles of beer
take one down and pass it around
2 bottles of beer on the wall

2 bottles of beer on the wall
2 bottles of beer
take one down and pass it around
1 bottle of beer on the wall

1 bottle of beer on the wall
1 bottle of beer
take one down and pass it around
no more bottles of beer on the wall

