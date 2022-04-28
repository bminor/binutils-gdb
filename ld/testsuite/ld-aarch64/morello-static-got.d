# This testcase is just to exercise some code rather than to test for an
# observable.  We may as well check that the __rela_dyn_start symbol exists.
#as: -march=morello+c64
#ld: -static
#readelf: --symbols --relocs

Relocation section '\.rela\.dyn' at offset .* contains 1 entry:
  Offset          Info           Type           Sym\. Value    Sym\. Name \+ Addend
.*  00000000e803 R_MORELLO_RELATIV                    0

#...
.* __rela_dyn_start
#...
