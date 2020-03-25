#ld: -shared -T pr16498a.t
#readelf: -l --wide
#target: *-*-linux* *-*-gnu* *-*-nacl* arm*-*-uclinuxfdpiceabi
#xfail: h8300-*-*

#...
  TLS .*
#...
[ ]+[0-9]+[ ]+.tdata .tbss[ ]*
#pass
