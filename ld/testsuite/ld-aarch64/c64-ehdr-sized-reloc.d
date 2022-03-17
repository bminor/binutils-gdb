#as: -march=morello+c64
#ld: -shared
#objdump: -dR -j .data

.*:     file format .*


Disassembly of section \.data:

00000000000[[:xdigit:]]* <val>:
	...
			[[:xdigit:]]*: R_MORELLO_RELATIVE	\*ABS\*
# Want to check that the size is non-zero.
# Check that using a negative line match to a zero size.
# In fact, when this size is zero objdump doesn't even print a line here, but
# that just adds extra robustness to our check.
!   .*:	00000000 	\.word	0x00000000
   .*:	01000000 	.*
