#source: pr21562b.s
#ld: -shared -z defs -T pr21562a.t
#readelf: -s -S --wide
#target: *-*-linux* *-*-gnu*
#notarget: bfin-*-* cr16-*-* cr16c-*-* cris*-*-* crx-*-* epiphany-*-*
#notarget: h8300-*-* ip2k-*-* m10200-*-* m10300-*-* m32c-*-* metag-*-*
#notarget: metag-*-* rl78-*-* rx-*-* sh*-*-* v850*-*-*
# Targets with a leading char will faill this test.

#...
  \[[ 0-9]+\] scnfoo[ \t]+PROGBITS[ \t]+[0-9a-f]+ +[0-9a-f]+ +0*10[ \t]+.*
#...
 +[0-9]+: +[0-9a-f]+ +[0-9a-f]+ +NOTYPE +LOCAL +DEFAULT +[0-9]+ +__stop_scnfoo
#pass
