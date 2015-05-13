#ld: -shared -melf_i386
#readelf: -S --wide
#as: --32

#failif
#...
 +\[ [0-9]+\] \.plt +PROGBITS +.*
#...
