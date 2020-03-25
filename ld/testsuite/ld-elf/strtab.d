#ld: -shared
#readelf: -W -x .strtab
#target: *-*-linux* *-*-gnu* arm*-*-uclinuxfdpiceabi
#xfail: h8300-*-*

#failif
#...
 +0x[0-9 ]+.*\.xxxx\..*
#...
