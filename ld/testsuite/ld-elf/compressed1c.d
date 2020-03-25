#source: compress1.s
#as: --compress-debug-sections=zlib-gabi
#ld: -shared --compress-debug-sections=none
#readelf: -t
#target: *-*-linux* *-*-gnu* arm*-*-uclinuxfdpiceabi
#xfail: h8300-*-*

#failif
#...
  .*COMPRESSED.*
#...
