#source: dt-relr-2.s
#ld: -e _start -shared $NO_DT_RELR_LDFLAGS
#readelf: -rW -d
#target: x86_64-*-linux* i?86-*-linux-gnu i?86-*-gnu*

#failif
#...
Relocation section '\.relr\.dyn' at offset .*
#pass
