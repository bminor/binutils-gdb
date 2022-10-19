#as:
#objdump: -d

.*:     file format pe-aarch64-little


Disassembly of section .text:

0000000000000000 <_start>:
   0:	d2800281 	mov	x1, #0x14                  	// #20
   4:	14000001 	b	8 <foo>

0000000000000008 <foo>:
   8:	d65f03c0 	ret
