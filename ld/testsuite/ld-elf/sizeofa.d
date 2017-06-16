#source: sizeof.s
#ld: -Ur
#readelf: -sW
#xfail: bfin-*-* cr16-*-* cr16c-*-* cris*-*-elf crx-*-* epiphany-*-*
#xfail: h8300-*-* ip2k-*-* mn10200-*-* mn10300-*-* m32c-*-* metag-*-*
#xfail: rl78-*-* rx-*-* sh*-*-elf* sh*-*-kaos* sh*-*-nto* sh-*-rtems*
#xfail: sh*-*-symbianelf* sh*-*-vxworks v850*-*-*
# Targets with a leading char will fail this test.

Symbol table '\.symtab' contains [0-9]+ entries:
 +Num: +Value +Size Type +Bind +Vis +Ndx Name
 +0: 0+ +0 +NOTYPE +LOCAL +DEFAULT +UND +
#...
 +[0-9]+: +[a-f0-9]+ +0 +NOTYPE +(LOC|GLOB)AL +DEFAULT +[0-9]+ +__stop_scnfoo
#...
 +[0-9]+: +[a-f0-9]+ +0 +NOTYPE +GLOBAL +DEFAULT +UND +.sizeof.scnfoo
#pass
