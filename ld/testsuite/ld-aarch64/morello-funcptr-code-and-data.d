# Want to double-check that when creating a PDE which references a function
# pointer in an external library with a CAPINIT and also references the
# function pointer in code with an adrp/add, we generate a CAPINIT
# entry in that PDE and also generate a PLT entry.
# We check this by ensuring that there is a memcpy@plt entry in the PLT, that
# there is the associated MORELLO jump slot relocation in the .got.plt, and
# there is a CAPINIT relocation.
#as: -march=morello+c64
#ld: tmpdir/morello-dynamic-relocs.so
#objdump: -dR -j .plt -j .data -j .got.plt

.*:     file format .*

Disassembly of section \.plt:
#...
.* <memcpy@plt>:
#...
Disassembly of section \.got\.plt:
#...
.*: R_MORELLO_JUMP_SLOT	memcpy
#...
Disassembly of section \.data:

.* <p>:
	\.\.\.
	.*: R_MORELLO_CAPINIT	memcpy
