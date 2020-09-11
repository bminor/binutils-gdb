#source: ifunc-4.s
#as: -march=morello+c64
#ld:
#readelf: -r --wide
#target: aarch64*-*-*

#...
[0-9a-f]+[ ]+[0-9a-f]+[ ]+R_MORELLO_IRELATIVE[ ]*[0-9a-f]*
#pass
