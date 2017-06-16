#ld: -shared -z defs --gc-sections
#readelf: -s -S --wide
#target: *-*-linux* *-*-gnu*
#xfail: d30v-*-* dlx-*-* i960-*-* pj*-*-*
#xfail: hppa64-*-* i370-*-* i860-*-* ia64-*-* mep-*-* mn10200-*-*
#xfail: bfin-*-* cr16-*-* cr16c-*-* crx-*-* epiphany-*-*
#xfail: h8300-*-* ip2k-*-* mn10200-*-* mn10300-*-* m32c-*-* metag-*-*
#xfail: rl78-*-* rx-*-* v850*-*-*
# Targets with a leading char will fail this test.
# generic linker targets don't support --gc-sections, nor do a bunch of others

#...
  \[[ 0-9]+\] scnfoo[ \t]+PROGBITS[ \t]+[0-9a-f]+ +[0-9a-f]+ +0*10[ \t]+.*
#...
 +[0-9]+: +[0-9a-f]+ +[0-9a-f]+ +NOTYPE +LOCAL +DEFAULT +[0-9]+ +__start_scnfoo
#pass
