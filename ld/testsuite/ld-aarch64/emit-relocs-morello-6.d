# Check handling relocations into the got that require a GOT entry.
# This case handles PIE binaries.
#
# This testcase uses exact values in order to check that of the two GOT entries
# created, the one that is referenced by the first instruction in _start is
# the one which has the LSB set in its value.
#
# Have tried to mitigate the flakyness of the test with a linker script that
# aligns .text and the GOT to 12 bits so that the ldr addends are just offsets
# into the GOT and we are likely to get .text at 0x1000.
#source: emit-relocs-morello-6.s
#as: -march=morello+c64
#ld: -Ttext-segment 0x100000 -T emit-relocs-morello-7.ld -pie -static
#objdump: -DR -j .got -j .text -j .data


.*:     file format .*


Disassembly of section \.text:

0000000000100100 <_start>:
  100100:	c2400c00 	ldr	c0, \[c0, #48\]
  100104:	c2400800 	ldr	c0, \[c0, #32\]
  100108:	c2401000 	ldr	c0, \[c0, #64\]
  10010c:	c2400400 	ldr	c0, \[c0, #16\]

Disassembly of section \.data:

00000000001001e8 <data_obj>:
  1001e8:	0000000a 	.*

00000000001001ec <data_func>:
  1001ec:	0000000a 	.*

Disassembly of section \.got:

0000000000101000 <\.got>:
  101000:	00100000 	.*
	\.\.\.
  101010:	001001ed 	.*
			101010: R_MORELLO_RELATIVE	\*ABS\*
  101014:	00000000 	.*
  101018:	00000004 	.*
  10101c:	02000000 	.*
  101020:	00100100 	.*
			101020: R_MORELLO_RELATIVE	\*ABS\*
  101024:	00000000 	.*
  101028:	00000f50 	.*
  10102c:	04000000 	.*
  101030:	00100100 	.*
			101030: R_MORELLO_RELATIVE	\*ABS\*\+0x1
  101034:	00000000 	.*
  101038:	00000f50 	.*
  10103c:	04000000 	.*
  101040:	001001e8 	.*
			101040: R_MORELLO_RELATIVE	\*ABS\*
  101044:	00000000 	.*
  101048:	00000004 	.*
  10104c:	02000000 	.*
