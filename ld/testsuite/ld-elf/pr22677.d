#ld: -r  --gc-sections -u foo
#readelf: -S --wide
#xfail: cr16-*-* crx-*-* d30v-*-* dlx-*-* hppa64-*-*
#xfail: mep-*-* mn10200-*-* msp430-*-* pj*-*-* pru-*-* xgate-*-*
# msp430 puts the init_array and fini_array inside the .rodata section.
# generic linker targets don't support --gc-sections, nor do a bunch of
# others.
# cr16 and crx use non-standard scripts with memory regions, which don't
# play well with unique group sections under ld -r.

#...
  \[[ 0-9]+\] \.preinit_array\.01000[ \t]+PREINIT_ARRAY[ \t0-9a-f]+WA?.*
#...
  \[[ 0-9]+\] \.init_array\.01000[ \t]+INIT_ARRAY[ \t0-9a-f]+WA?.*
#...
  \[[ 0-9]+\] \.fini_array\.01000[ \t]+FINI_ARRAY[ \t0-9a-f]+WA?.*
#pass
