# Want to double-check that when creating a PDE which loads a data pointer
# in an external library using an adrp/ldr code sequence and also via a CAPINIT
# relocation, we generate both a COPY relocation and a CAPINIT relocation.
#
# We want to make sure the COPY relocation is used by the text.  We can not
# check that the dynamic loader would initialize the CAPINIT relocation with
# the address of that COPY relocation, so that's left.
#as: -march=morello+c64
#ld: tmpdir/morello-dynamic-relocs.so
#objdump: -dR -j .data -j .bss -j .text

.*:     file format .*


Disassembly of section \.text:

.* <_start>:
#record: PAGE
.*:	.* 	adrp	c0, ([0-9a-f]*).*
#record: OFFSET
.*:	.* 	add	c0, c0, #0x([0-9a-f]*)
.*:	.* 	ldr	w0, \[c0\]
.*:	.* 	ret	c30

Disassembly of section \.data:
#...
			.*: R_MORELLO_CAPINIT	var

Disassembly of section \.bss:

#check: VAR_LOCATION aarch64_page_plus_offset $PAGE $OFFSET
0*VAR_LOCATION <var>:
	\.\.\.
			VAR_LOCATION: R_AARCH64_COPY	var

