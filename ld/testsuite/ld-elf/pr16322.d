#ld: -shared -z relro -z noseparate-code
#readelf: -l --wide
#target: *-*-linux-gnu *-*-gnu* *-*-nacl*

#...
  GNU_RELRO .*
#pass
