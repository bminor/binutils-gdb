#as:
#objdump: -ds

.*:     file format .*

Contents of section .text:
 0000 9409d000 6c00d000 001ff800           ....l.......    

Disassembly of section .text:

00000000 <.text>:
   0:	9409d000 	mts	rpid, r9
   4:	6c00d000 	tnput	rfsl0
   8:	001ff800 	add	r0, r31, r31
