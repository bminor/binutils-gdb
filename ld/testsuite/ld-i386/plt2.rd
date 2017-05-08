#source: plt2.s
#as: --32
#ld: -z now -melf_i386
#readelf: -SW
#target: i?86-*-*

#...
 +\[ *[0-9]+\] \.plt +PROGBITS +[0-9a-f]+ +[0-9a-f]+ +0+20 +.* +AX +0 +0 +16
#pass
