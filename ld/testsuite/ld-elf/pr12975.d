#ld: --gc-sections -shared -version-script pr12975.t
#readelf: -s --wide
#target: *-*-linux* *-*-gnu* arm*-*-uclinuxfdpiceabi
#xfail: d30v-*-* dlx-*-* hppa64-*-* mep-*-* mn10200-*-* pj*-*-* xgate-*-*
# generic linker targets don't support --gc-sections, nor do a bunch of others

#failif
#...
 +[0-9]+: +[0-9a-f]+ +[0-9]+ +FUNC +LOCAL +DEFAULT +[1-9]+ bar
#...
