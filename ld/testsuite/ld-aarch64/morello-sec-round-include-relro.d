# Checking that we include RELRO sections in PCC bounds.
#
# Check is done using the fragment values to show what the size of the PCC
# bounds is given as, and we reorder sections so that at least one RELRO
# section is ater all READONLY ALLOC section.
#
# Test only works if we have relro, so is unsupported bare-metal.
#
# Test is implemented by ensuring that the .data.rel.ro is the last section in
# the RELRO segment, and that .data is directly after it.  This is ensured with
# a linker script.  If we don't include RELRO in our PCC bounds then
# .data.rel.ro will not be within the range of the `obj` capability (since the
# .data.rel.ro section itself is marked as being writeable).
#
#target: [check_relro_support]
#source: morello-sec-round-include-relro.s
#as: -march=morello+c64
#ld: -static -pie -z relro -z now -T morello-sec-round-include-relro.ld
#objdump: -j .data -j .text -j .data.rel.ro -DR

.*:     file format .*


Disassembly of section \.text:

00000000000001a0 <obj>:
#...

Disassembly of section .data.rel.ro:

0000000000010ff0 <data_relro_start>:
#...

Disassembly of section .data:

0000000000011000 <data_start>:
   11000:	000001a0 	udf	#416
			11000: R_MORELLO_RELATIVE	\*ABS\*
   11004:	00000000 	udf	#0
   11008:	00010e60 	\.inst	0x00010e60 ; undefined
   1100c:	04000000 	add	z0\.b, p0/m, z0\.b, z0\.b
