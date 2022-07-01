# Want to double-check that when creating a PIE which references a function
# pointer in an external library with a CAPINIT and does not reference the
# function pointer in code with an adrp/add, we generate a CAPINIT
# entry in that PIE and do not generate a PLT entry.
# We check this by ensuring that there is no memcpy@plt entry in the PLT, and
# by checking there is a CAPINIT relocation.
#source: morello-funcptr-through-data.s
#as: -march=morello+c64
#ld: -pie tmpdir/morello-dynamic-relocs.so
#objdump: -dR -j .plt -j .data

.*:     file format .*

Disassembly of section \.data:

.* <p>:
	\.\.\.
	.*: R_MORELLO_CAPINIT	memcpy

