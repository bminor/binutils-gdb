#source: define.s
#source: undef.s
#ld: -r
#readelf: -s

Symbol table '.symtab' contains .* entries:
   Num:    Value[ 	]+Size Type    Bind   Vis      Ndx Name
#...
     .: [0-9a-fA-F]*     0 NOTYPE  GLOBAL INTERNAL    . internal
#pass
