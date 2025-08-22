\ Allocate array with 3 cells
create nums 3 cells allot

\ Store 10 into nums[0], by adding offset 0 to nums (storage location)
10 nums 0 cells + !

\ Simplify by using a word
: num@  ( offset -- addr )  cells nums + ;

30 1 num@ !
40 2 num@ !

0 num@ ?
1 num@ ?
2 num@ ?
10 emit

\ We can also allocate arrays and initialize them with a comma
create arr2 100 , 200 , 300 , 400 , 500 ,

1 cells arr2 + ?
2 cells arr2 + ?
4 cells arr2 + ?
bye

\ ---- OUT ----
10 30 40 
200 300 500 
