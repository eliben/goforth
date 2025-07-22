: cr 10 emit ;

: space 32 emit ;

: ?dup ( x -- 0 | x x )
  dup
  if dup then ;

: MANY ( n) ?DUP IF . ELSE ." No more " THEN ; ( the number)
: BOTTLES ( n) ." bottle" 1 - IF ." s" THEN ; ( handle plural)

: BEER ( n) CR DUP MANY BOTTLES SPACE ." of beer" ;
: WALL SPACE ." on the wall" ;
: DRINK CR ." Take one down and pass it around." ;
: BUY CR ." Go to the store and buy some more." ;

: ANOTHER ( n-n) ?DUP IF DRINK 1- ELSE BUY 99 THEN ;

: VERSE ( n) DUP BEER WALL DUP BEER ANOTHER BEER WALL CR ;
: VERSES ( n) 0 SWAP DO I VERSE 1 -LOOP ;

3 VERSES

\ ---- OUT ----

3 bottles of beer on the wall
3 bottles of beer
Take one down and pass it around.
2 bottles of beer on the wall

2 bottles of beer on the wall
2 bottles of beer
Take one down and pass it around.
1 bottle of beer on the wall

1 bottle of beer on the wall
1 bottle of beer
Take one down and pass it around.
No more bottles of beer on the wall

