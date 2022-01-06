#source: dt-relr-2.s
#ld: -r $DT_RELR_LDFLAGS
#readelf: -rW
#target: x86_64-*-linux* i?86-*-linux-gnu i?86-*-gnu*

#...
Relocation section '\.rel(a|)\.data' at offset 0x[0-9a-f]+ contains 5 entries:
#pass
