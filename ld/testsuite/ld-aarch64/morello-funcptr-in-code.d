# Want to double-check that when creating a PDE which loads a function pointer
# in an external library using an adrp/ldr code sequence, we generate a PLT
# entry in that PDE and use that PLT entry as the address that is loaded in the
# code.
# We check this by ensuring that there is a memcpy@plt entry in the PLT, and
# that the .got.plt has a MORELLO jump slot relocation in it.
#as: -march=morello+c64
#ld: tmpdir/morello-dynamic-relocs.so
#objdump: -dR -j .plt -j .got.plt

.*:     file format .*


Disassembly of section \.plt:
#...
.* <memcpy@plt>:
#...
Disassembly of section \.got\.plt:
#...
.*: R_MORELLO_JUMP_SLOT	memcpy
#pass
