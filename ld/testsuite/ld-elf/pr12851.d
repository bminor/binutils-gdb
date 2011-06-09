#source: pr12851.s
#source: start.s
#ld: --gc-sections
#readelf: -s --wide
#notarget: arc-*-* d30v-*-* dlx-*-* i960-*-* or32-*-* pj*-*-*
#notarget: alpha-*-* hppa64-*-* i370-*-* i860-*-* ia64-*-* mep-*-* mn10200-*-*
#xfail: cr16-*-* crx-*-*
# generic linker targets don't support --gc-sections, nor do a bunch of others
# cr16 and crx use non-standard scripts with memory regions, which don't play
# well with unique group sections under ld -r.

#...
 +.* _.stapsdt.base
#pass
