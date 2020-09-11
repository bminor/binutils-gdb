#source: ifunc-3.s
#target: [check_shared_lib_support]
#as: -march=morello+c64
#ld: -shared
#readelf: -r --wide

#...
[0-9a-f]+[ ]+[0-9a-f]+[ ]+R_MORELLO_IRELATIVE[ ]*[0-9a-f]*
#pass
