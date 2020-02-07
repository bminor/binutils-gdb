#ld: -T pr25022.t
#readelf: -SW
#xfail: msp*-*
# msp* doesn't use ldelf_before_place_orphans.

#failif
#...
 +\[ *[0-9]+\] \.(bar|moo|zed) +.*
#...
