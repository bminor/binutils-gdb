#source: unknown2.s
#ld: -shared
#readelf: -S
#target: *-*-linux* *-*-gnu* arm*-*-uclinuxfdpiceabi
#xfail: h8300-*-*

#...
  \[[ 0-9]+\] \.note.foo[ \t]+NOTE[ \t]+.*
#pass
