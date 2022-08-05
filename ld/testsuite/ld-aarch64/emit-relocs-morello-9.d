# Checking that the LSB is set on all relocations to a function symbol where
# the relocations are there to load an address into a register (rather than
# jump to a relative address).
#source: emit-relocs-morello-9.s
#as: -march=morello+c64
#ld: -static -Ttext-segment 0x0
#objdump: -d -j .data -j .text

.*:     file format .*


Disassembly of section \.text:

#record: TEXT_START
([0-9a-f]+) <_start>:
#check: LOBITS format %x [expr "0x$TEXT_START + 1"]
.*:	........ 	movk	x0, #0xLOBITS
.*:	........ 	movk	x0, #0xLOBITS
.*:	........ 	adr	c0, LOBITS <_start\+0x1>
.*:	........ 	adr	c0, LOBITS <_start\+0x1>
.*:	........ 	add	c0, c0, #0xLOBITS
.*:	........ 	add	c0, c0, #0xLOBITS
.*:	........ 	mov	x0, #0xLOBITS .*
.*:	........ 	mov	x0, #0xLOBITS .*
.*:	........ 	movk	x0, #0xLOBITS
.*:	........ 	movk	x0, #0xLOBITS

Disassembly of section \.data:

#check: LO8BITS aarch64_8digit_addr $TEXT_START 1
.* <val>:
   .*:	LO8BITS 	.word	0xLO8BITS
   .*:	LO8BITS 	.word	0xLO8BITS
   .*:	LO8BITS 	.word	0xLO8BITS
   .*:	00000000 	.word	0x00000000
   .*:	LO8BITS 	.word	0xLO8BITS
   .*:	LO8BITS 	.word	0xLO8BITS
   .*:	LO8BITS 	.word	0xLO8BITS
   .*:	00000000 	.word	0x00000000
