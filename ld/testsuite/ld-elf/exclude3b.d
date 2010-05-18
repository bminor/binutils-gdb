#source: exclude3.s
#ld: --shared
#readelf: -S --wide
#target: x86_64-*-* i?86-*-* ia64-*-*

#...
[ 	]*\[.*\][ 	]+\.got.*[ 	]+PROGBITS.*
[ 	]*\[.*\][ 	]+.*STRTAB.*
#pass
