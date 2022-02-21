# Checking that we include READONLY ALLOC sections in PCC bounds.
#
# Check is done using the fragment values to show what the size of the PCC
# bounds is given as, and we reorder sections so that the READONLY ALLOC
# section is after all executable sections and RELRO sections.
#
# Test is implemented by ensuring that .othersection is outside of RELRO and is
# the last section that should be included in PCC bounds, and that .data is
# directly after it.  This is done with a linker script.  If we don't
# include READONLY ALLOC sections in our PCC bounds then .othersection will not
# be within the range of the `obj` capability.
#
#source: morello-pcc-bounds-include-readonly.s
#as: -march=morello+c64
#ld: -static -pie -T morello-pcc-bounds-include-readonly.ld
#objdump: -j .data -j .text -j .othersection -DR

.*:     file format .*


Disassembly of section \.text:

00000000000001a0 <obj>:
#...

Disassembly of section .othersection:

0000000000012000 <othersection_start>:
#...

Disassembly of section .data:

0000000000014000 <data_start>:
   [0-9a-f]+:	000001a0 	udf	#416
			14000: R_MORELLO_RELATIVE	\*ABS\*
   [0-9a-f]+:	00000000 	udf	#0
   [0-9a-f]+:	00011f00 	\.inst	0x00011f00 ; undefined
   [0-9a-f]+:	04000000 	add	z0\.b, p0/m, z0\.b, z0\.b

