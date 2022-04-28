# This testcase is written mostly to ensure a particular code path is
# exercised.  That is the purpose of the local `x` symbol that we have a GOT
# relocation to.
#
# There is no particular observable for this code path -- a relocation is put
# in one input section rather than another (though both input sections have the
# same output section).  May as well check that the __rela_dyn_* symbols are
# not emitted, since this is a dynamic symbol.
#as: -march=morello+crc+c64
#ld: tmpdir/morello-dynamic-relocs.so
#readelf: --symbols

#failif
#...
.* __rela_dyn_start
#...
