# Exercise handling morello relocations against weak undef symbols.
#
# What we want to check is:
#   1) The adrp/ldr instructions point to entries in the GOT.
#   2) There are no relocations in the GOT.
#   3) The GOT entries for weak undefined symbols are zero.
#   4) The values in .data are zero.
#
# The testcase morello-undefweak-relocs-static.d checks all except that there
# are no relocations in the file.  This testcase checks that last part.
#source: morello-undefweak-relocs.s
#as: -march=morello+c64
#ld: -static
#readelf: --relocs

There are no relocations in this file.
