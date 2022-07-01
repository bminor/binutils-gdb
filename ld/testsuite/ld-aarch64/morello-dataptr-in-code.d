# Want to double-check that when creating a PDE which references a data pointer
# in an external library in code with an adrp/add, we generate a COPY
# relocation and access that the data said relocation will initialize.
#
# We check this by ensuring that there is a new `var` symbol in the .bss of the
# PDE, that the adrp/add addresses point to that symbol, and that there is a
# COPY relocation at that symbol for its initialisation.
#as: -march=morello+c64
#ld: tmpdir/morello-dynamic-relocs.so
#objdump: -dR -j .plt -j .bss -j .text

.*:     file format .*


Disassembly of section \.text:

.* <_start>:
#record: PAGE
.*:	.* 	adrp	c0, ([0-9a-f]*).*
#record: OFFSET
.*:	.* 	add	c0, c0, #0x([0-9a-f]*)
.*:	.* 	ldr	w0, \[c0\]
.*:	.* 	ret	c30

Disassembly of section \.bss:

#check: VAR_LOCATION aarch64_page_plus_offset $PAGE $OFFSET
0*VAR_LOCATION <var>:
	\.\.\.
			VAR_LOCATION: R_AARCH64_COPY	var

