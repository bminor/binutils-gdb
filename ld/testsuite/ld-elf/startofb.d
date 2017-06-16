#source: startof.s
#ld: -shared
#readelf: -sW
#target: *-*-linux* *-*-gnu*
#xfail: bfin-*-* cr16-*-* cr16c-*-* crx-*-* epiphany-*-*
#xfail: h8300-*-* ip2k-*-* mn10200-*-* mn10300-*-* m32c-*-* metag-*-*
#xfail: rl78-*-* rx-*-* v850*-*-*
# Targets with a leading char will fail this test.

Symbol table '\.dynsym' contains [0-9]+ entries:
 +Num: +Value +Size Type +Bind +Vis +Ndx Name
 +0: 0+ +0 +NOTYPE +LOCAL +DEFAULT +UND +
#...
 +[0-9]+: +[a-f0-9]+ +0 +NOTYPE +LOCAL +DEFAULT +[0-9]+ +.startof.scnfoo
#...
 +[0-9]+: +[a-f0-9]+ +0 +NOTYPE +LOCAL +DEFAULT +[0-9]+ +__start_scnfoo
#pass
