#ld: -shared -melf_x86_64
#readelf: -S --wide
#as: --64

#failif
#...
 +\[ [0-9]+\] \.plt +PROGBITS +.*
#...
