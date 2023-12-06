#name: disas-adrp-neg-ofs
#source: disas-adrp-neg-ofs.s
#as: -march=morello+c64
#ld: --section-start .text=0x400000
#objdump: -D

.*: +file format .*aarch64.*


Disassembly of section .text:

0000000000400000 <_start>:
  400000:	d503201f 	nop
	...
  401004:	f0ffffe0 	adrp	c0, 400000 <_start>
