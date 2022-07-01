# Want to double-check that when creating a PDE which loads a data pointer
# in an external library using a CAPINIT relocation we end up with a
# straight-forward CAPINIT relocation in the final binary requesting the
# dynamic loader to provide a capability pointing to that bit of data.
#as: -march=morello+c64
#ld: tmpdir/morello-dynamic-relocs.so
#readelf: --relocs

Relocation section '\.rela\.dyn' at offset .* contains 1 entry:
  Offset          Info           Type           Sym\. Value    Sym\. Name \+ Addend
.*  .* R_MORELLO_CAPINIT .* var \+ 0
