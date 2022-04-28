# Test here is to ensure that we only emit one relocation into the GOT for the
# one use of a GOT entry.  This is already checked by the fact that the last
# linker command would hit an assertion failure if it were not the case.
# We check that there is only one relocation in the resulting binary anway,
# since double-checking is always nice.
#
# N.b. aarch64-elf.exp compiles a shared libary for this test under
# tmpdir/morello-dynamic-relocs.so.  We use that shared library for the test in
# the `ld` command below.
#as: -march=morello+c64
#ld: tmpdir/morello-dynamic-relocs.so
#readelf: --relocs

Relocation section '\.rela\.dyn' at offset 0x[0-9a-f]+ contains 1 entry:
  Offset          Info           Type           Sym\. Value    Sym\. Name \+ Addend
[0-9a-f]+  [0-9a-f]+ R_MORELLO_GLOB_DA 0000000000000000 var \+ 0
