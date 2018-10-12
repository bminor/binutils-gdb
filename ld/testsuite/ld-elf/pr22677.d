#ld: -r  --gc-sections -u foo
#readelf: -S --wide
# generic linker targets don't support --gc-sections, nor do a bunch of
# others.
#xfail: d30v-*-* dlx-*-* hppa64-*-*
#xfail: mep-*-* mn10200-*-* pj*-*-* pru-*-* xgate-*-*

#...
  \[[ 0-9]+\] \.preinit_array\.01000[ \t]+PREINIT_ARRAY[ \t0-9a-f]+WA?.*
#...
  \[[ 0-9]+\] \.init_array\.01000[ \t]+INIT_ARRAY[ \t0-9a-f]+WA?.*
#...
  \[[ 0-9]+\] \.fini_array\.01000[ \t]+FINI_ARRAY[ \t0-9a-f]+WA?.*
#pass
