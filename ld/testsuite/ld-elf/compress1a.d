#source: compress1.s
#as: --compress-debug-sections
#ld: -e func_cu2
#readelf: -S --wide

#failif
#...
  \[[ 0-9]+\] \.zdebug_.*[ 	]+PROGBITS[ 	0-9a-z]+ .*
#...
