#source: tlstpoff1.s
#source: tlstpoff2.s
#as: -little
#ld: -EL -e foo
#readelf: -r
#target: sh*-*-linux* sh*-*-netbsd*

Relocation section '\.rela\.dyn' at offset 0x[0-9a-f]+ contains 2 entries:
 Offset +Info +Type +Sym\.Value +Sym\. Name \+ Addend
0+[0-9a-f]+  00000097 R_SH_TLS_TPOFF32 +0+04
0+[0-9a-f]+  00000097 R_SH_TLS_TPOFF32 +0+04

