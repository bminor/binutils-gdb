#name: PR ld/13909
#source: dummy.s
#as: --64
#ld: -melf_x86_64 -shared
#readelf: -S --wide

#failif
#...
 +\[ [0-9]\] .eh_frame +PROGBITS +[0-9a-f]+ +[0-9a-f]+ 0+ +.*
#...
