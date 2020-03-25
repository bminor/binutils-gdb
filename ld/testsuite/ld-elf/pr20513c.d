#source: pr20513a.s
#source: pr20513b.s
#ld: -shared
#readelf: -S --wide
#target: *-*-linux* *-*-gnu* arm*-*-uclinuxfdpiceabi
#xfail: h8300-*-*

#failif
#...
[ 	]*\[.*\][ 	]+\..text\.exclude[ 	]+.*
#...
