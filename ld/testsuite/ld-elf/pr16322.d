#ld: -shared -z relro -z noseparate-code
#readelf: -l --wide
#target: *-*-linux-gnu *-*-gnu* *-*-nacl* arm*-*-uclinuxfdpiceabi
#xfail: h8300-*-*

#...
  GNU_RELRO .*
#pass
