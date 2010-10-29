#source: compress1.s
#as: --compress-debug-sections
#ld: -r
#readelf: -S --wide

#failif
#...
  \[[ 0-9]+\] \.zdebug_.*[ 	]+PROGBITS[ 	0-9a-z]+ .*
#...
