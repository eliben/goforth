1 .

\ We add 'bye' at the end of tests not because it's needed (by default the
\ interpreters will exit at the end of input anyway), but because it makes it
\ nicer to run test files directly as interpreter inputs without the test
\ harness (otherwise they would choke on the expected output, which may consist
\ of invalid Forth words)
bye

\ ---- OUT ----
1 
