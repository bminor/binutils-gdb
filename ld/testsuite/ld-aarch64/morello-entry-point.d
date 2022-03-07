# Checking that the entry point address of a binary with a c64 function symbol
# as the entry address is odd (i.e. has the LSB set).
#source: emit-relocs-morello-2.s
#as: -march=morello+c64
#ld: -static
#readelf: --file-header

#...
  Entry point address:               0x.*[13579]
#pass
