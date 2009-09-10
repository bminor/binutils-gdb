#as: -EL -mARC700
#objdump: -dr -EL

.*: +file format elf32-.*arc

Disassembly of section .text:

00000000 <main>:
   0:	00 21 80 00             	add        r0,r1,r2
   4:	6f 22 3f 00             	swi        
   8:	02 24 43 01             	sub        r3,r4,r5
