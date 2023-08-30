#source: pr30791c.s
#source: pr30791d.s
#ld: -r
#readelf: -S --wide

#failif
#...
  \[[ 0-9]+\] __patchable_function_entries[ \t]+PROGBITS[ \t0-9a-f]+WAL.*
#...
  \[[ 0-9]+\] __patchable_function_entries[ \t]+PROGBITS[ \t0-9a-f]+WAL.*
#...
