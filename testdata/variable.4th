variable bob

99 bob !
." value of bob: " bob ? 10 emit

variable mark
101 mark !

." value of bob: " bob ? 10 emit
." value of mark " mark ? 10 emit

-59 bob !
-404 mark !

." value of bob: " bob ? 10 emit
." value of mark " mark ? 10 emit

200 mark +!

." value of bob: " bob ? 10 emit
." value of mark " mark ? 10 emit
bye

\ ---- OUT ----
value of bob: 99 
value of bob: 99 
value of mark 101 
value of bob: -59 
value of mark -404 
value of bob: -59 
value of mark -204 
