#source: got-weak.s
#as:
#ld: -Bstatic
#objdump: -d

[^:]*:     file format elf32-.*arc


Disassembly of section \.text:

00000100 <.*>:
 100:	2730 7f80 0000 2014 	ld	r0,\[pcl,0x2014\].*
