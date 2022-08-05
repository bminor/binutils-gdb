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
#objdump: -DR --section-headers

.*:     file format .*

Sections:
Idx Name          Size      VMA               LMA               File off  Algn
.* \.text         00000010  00000000000001a0  00000000000001a0  000001a0  2\*\*5
#record: LAST_RO_SIZE LAST_RO_VMA
#...
 +[0-9]+ \.rela\.dyn +([0-9a-f]+)  ([0-9a-f]+) .*
 + CONTENTS, ALLOC, LOAD, READONLY, DATA
 +[0-9]+ \.data .*
#...

Disassembly of section \.text:

00000000000001a0 <obj>:
#...
Disassembly of section \.data:

#check: PCC_SIZE format %08x [expr "0x$LAST_RO_VMA + 0x$LAST_RO_SIZE - 0x1a0"]
0000000000014000 <data_start>:
   [0-9a-f]+:	000001a0 	.*
			14000: R_MORELLO_RELATIVE	\*ABS\*
   [0-9a-f]+:	00000000 	.*
   [0-9a-f]+:	PCC_SIZE 	.*
   [0-9a-f]+:	04000000 	add	z0\.b, p0/m, z0\.b, z0\.b

