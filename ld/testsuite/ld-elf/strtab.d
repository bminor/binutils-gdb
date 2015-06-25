#ld: -shared
#readelf: -W -x .strtab

#failif
#...
 +0x[0-9 ]+.*\.xxxx\..*
#...
