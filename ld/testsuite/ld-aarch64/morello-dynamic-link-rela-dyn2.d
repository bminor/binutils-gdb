# This testcase ensures that we have the expected number and type of
# relocations in our resultant binary.
#source: morello-dynamic-link-rela-dyn.s
#as: -march=morello+c64
#ld: tmpdir/morello-dynamic-relocs.so
#readelf: --relocs

Relocation section '\.rela\.dyn' at offset .* contains 5 entries:
  Offset          Info           Type           Sym\. Value    Sym\. Name \+ Addend
.*  00000000e803 R_MORELLO_RELATIV                    0
.*  00000000e803 R_MORELLO_RELATIV                    0
.*  00000000e803 R_MORELLO_RELATIV                    0
.*  00000000e803 R_MORELLO_RELATIV                    0
.*  00010000e801 R_MORELLO_GLOB_DA 0000000000000000 var \+ 0
