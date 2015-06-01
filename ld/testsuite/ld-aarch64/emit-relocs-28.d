#source: emit-relocs-28.s
#as: -mabi=ilp32
#ld: -m aarch64linux32 --defsym globala=0x11000 --defsym globalb=0x45000 --defsym globalc=0x1234  -e0 --emit-relocs
#objdump: -dr

.*: +file format .*


Disassembly of section .text:

00400074 <\.text>:
  400074:	90000082 	adrp	x2, 410000 <globalb\+0x3cb000>
			400074: R_AARCH64_P32_ADR_PREL_PG_HI21	_GLOBAL_OFFSET_TABLE_
  400078:	f9408c40 	ldr	x0, \[x2,#280\]
			400078: R_AARCH64_P32_LD32_GOTPAGE_LO14	globala
  40007c:	f9409040 	ldr	x0, \[x2,#288\]
			40007c: R_AARCH64_P32_LD32_GOTPAGE_LO14	globalb
  400080:	f9408840 	ldr	x0, \[x2,#272\]
			400080: R_AARCH64_P32_LD32_GOTPAGE_LO14	globalc
