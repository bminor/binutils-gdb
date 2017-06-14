#source: pr21562a.s
#ld: -shared -z defs --gc-sections -T pr21562c.t
#readelf: -s -S --wide
#target: *-*-linux* *-*-gnu*
#notarget: d30v-*-* dlx-*-* i960-*-* pj*-*-*
#notarget: hppa64-*-* i370-*-* i860-*-* ia64-*-* mep-*-* mn10200-*-*
#notarget: bfin-*-* cr16-*-* cr16c-*-* cris*-*-* crx-*-* epiphany-*-*
#notarget: h8300-*-* ip2k-*-* m10200-*-* m10300-*-* m32c-*-* metag-*-*
#notarget: metag-*-* rl78-*-* rx-*-* sh*-*-* v850*-*-*
# Targets with a leading char will faill this test.
# generic linker targets don't support --gc-sections, nor do a bunch of others

#...
  \[[ 0-9]+\] \.foo[ \t]+PROGBITS[ \t]+[0-9a-f]+ +[0-9a-f]+ +0*10[ \t]+.*
#...
 +[0-9]+: +[0-9a-f]+ +[0-9a-f]+ +NOTYPE +LOCAL +DEFAULT +[0-9]+ +__start_scnfoo
#pass
