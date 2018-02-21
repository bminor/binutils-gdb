#source: pr12851.s
#source: start.s
#ld: --gc-sections
#readelf: -s --wide
#xfail: d30v-*-* dlx-*-* i960-*-* pj*-*-* pru-*-*
#xfail: hppa64-*-* i370-*-* i860-*-* mep-*-* mn10200-*-*
# generic linker targets don't support --gc-sections, nor do a bunch of others

#...
 +.* _.stapsdt.base
#pass
