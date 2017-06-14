#source: sizeof.s
#ld: -Ur
#readelf: -sW
#notarget: bfin-*-* cr16-*-* cr16c-*-* cris*-*-* crx-*-* epiphany-*-*
#notarget: h8300-*-* ip2k-*-* m10200-*-* m10300-*-* m32c-*-* metag-*-*
#notarget: metag-*-* rl78-*-* rx-*-* sh*-*-* v850*-*-*
# Targets with a leading char will faill this test.

Symbol table '\.symtab' contains [0-9]+ entries:
 +Num: +Value +Size Type +Bind +Vis +Ndx Name
 +0: 0+ +0 +NOTYPE +LOCAL +DEFAULT +UND +
#...
 +[0-9]+: +[a-f0-9]+ +0 +NOTYPE +GLOBAL +DEFAULT +[0-9]+ +__stop_scnfoo
#...
 +[0-9]+: +[a-f0-9]+ +0 +NOTYPE +GLOBAL +DEFAULT +UND +.sizeof.scnfoo
#pass
