#objdump: -ldr
#name: D30V .align test
#as: 

.*: +file format elf32-d30v

Disassembly of section .text:

00000000 <start>:
   0:	08815a80 00f00000 	abs	r21, r42	||	nop	
   8:	08815a80 00f00000 	abs	r21, r42	||	nop	
  10:	08815a80 00f00000 	abs	r21, r42	||	nop	
	...
  20:	08815a80 88815a80 	abs	r21, r42	->	abs	r21, r42
  28:	08815a80 00f00000 	abs	r21, r42	||	nop	
