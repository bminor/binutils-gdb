#source: pr21562a.s
#ld: -shared -z defs
#readelf: -s -S --wide
#target: *-*-linux* *-*-gnu*
#xfail: bfin-*-* cr16-*-* cr16c-*-* crx-*-* epiphany-*-*
#xfail: h8300-*-* ip2k-*-* mn10200-*-* mn10300-*-* m32c-*-* metag-*-*
#xfail: rl78-*-* rx-*-* v850*-*-*
# Targets with a leading char will fail this test.

#...
  \[[ 0-9]+\] scnfoo[ \t]+PROGBITS[ \t]+[0-9a-f]+ +[0-9a-f]+ +0*10[ \t]+.*
#...
 +[0-9]+: +[0-9a-f]+ +[0-9a-f]+ +NOTYPE +LOCAL +DEFAULT +[0-9]+ +__start_scnfoo
#pass
