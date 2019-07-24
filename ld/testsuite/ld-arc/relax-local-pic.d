#source: relax-local-pic.s
#as:
#ld: -q -A elf32-arclittle -relax
#objdump: -dr

[^:]+:     file format elf32-.*arc


Disassembly of section \.text:

00000100 <__start>:
 100:	2700 7f84 0000 [0-9a-f]+ 	add	r4,pcl,0x[0-9a-f]+.*
			104: R_ARC_PC32	a_in_other_thread
 108:	1c00 [0-9a-f\s]+	st\s+.*
