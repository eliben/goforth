\ Using tick and comma to "implement" foo with +
: foo immediate
  ' + , ;

: bar 10 20 foo ;

bar . cr

\ ---- OUT ----
30 
