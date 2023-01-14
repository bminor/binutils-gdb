#source: start.s
#ld: -shared -z global
#readelf: -d
#target: *-*-linux* *-*-gnu* arm*-*-uclinuxfdpiceabi
#xfail: h8300-*-*

#...
 0x0*6ffffffb \(FLAGS_1\) *Flags: GLOBAL
#pass
