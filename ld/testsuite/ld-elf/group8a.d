#source: group8.s
#ld: -r --gc-sections --entry foo
#readelf: -g --wide
# generic linker targets don't support --gc-sections, nor do a bunch of others
#xfail: d30v-*-* dlx-*-* hppa64-*-*
#xfail: mep-*-* mn10200-*-* pj*-*-* pru-*-* xgate-*-*

COMDAT group section \[[ 0-9]+\] `.group' \[foo\] contains . sections:
   \[Index\]    Name
   \[[ 0-9]+\]   .text.foo
#pass
