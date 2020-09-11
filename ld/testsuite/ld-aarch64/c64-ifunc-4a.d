#as: -march=morello+c64
#ld: -s
#readelf: -r --wide
#target: aarch64*-*-*
#source: ifunc-4.s

#...
[0-9a-f]+[ ]+[0-9a-f]+[ ]+R_MORELLO_IRELATIVE[ ]*[0-9a-f]*
#pass
