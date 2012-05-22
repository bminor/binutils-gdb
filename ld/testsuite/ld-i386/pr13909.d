#name: PR ld/13909
#source: dummy.s
#as: --32
#ld: -melf_i386 -shared
#readelf: -S --wide

#failif
#...
 +\[ [0-9]\] .eh_frame +PROGBITS +[0-9a-f]+ +[0-9a-f]+ 0+ +.*
#...
