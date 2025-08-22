: cr 10 emit ;

: rectangle
  24 0 do
    i 8 mod 0= if cr then ." *" 
  loop ;
    
rectangle
bye

\ ---- OUT ----
********
********
********
