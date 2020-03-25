#source: exclude3.s
#ld: --shared
#readelf: -S --wide
#target: *-*-linux* *-*-gnu* arm*-*-uclinuxfdpiceabi
#xfail: h8300-*-*

#failif
#...
[ 	]*\[.*\][ 	]+\.foo1[ 	]+.*
#...
