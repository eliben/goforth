: star [char] * emit ;
: cr 10 emit ;

star cr
char ? emit cr

: alphabet
  8 0 do
    i [char] A + emit \ 'A' + loop index
  loop ;

alphabet cr
bye

\ ---- OUT ----
*
?
ABCDEFGH
