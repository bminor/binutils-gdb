#source: pr23658-1a.s
#source: pr23658-1b.s
#source: pr23658-1c.s
#source: pr23658-1d.s
#source: start.s
#ld: --build-id
#readelf: -l --wide
# Since generic linker targets don't place SHT_NOTE sections as orphan,
# SHT_NOTE sections aren't grouped nor sorted.
#xfail: d30v-* dlx-* fr30-* frv-*-elf ft32-* iq2000-*
#xfail: m68hc12-* mn10200-* moxie-* mt-* msp430-* pj-* xgate-*
# The following targets don't support --build-id.
#xfail: cr16-* crx-* visium-* xc16x-*
# The following targets place .note.gnu.build-id in unusual places.
#xfail: pru-*

#...
 +[0-9]+ +\.note\.4 \.note\.1 +
 +[0-9]+ +\.note.gnu.build-id \.note\.2 .note\.3 +
#pass
