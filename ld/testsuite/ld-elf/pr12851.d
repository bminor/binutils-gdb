#source: pr12851.s
#source: start.s
#ld: --gc-sections
#readelf: -s --wide
#xfail: d30v-*-* dlx-*-* hppa64-*-* mep-*-* mn10200-*-*
#xfail: pj*-*-* pru-*-* xgate-*-*
# generic linker targets don't support --gc-sections, nor do a bunch of others

#...
 +.* _.stapsdt.base
#pass
