#as: -EL -marc8
#objdump: -dr -EL

.*: +file format elf32-.*arc

Disassembly of section .text:

00000000 <condcodeTest>:
   0:	12 02 00 40 	40000212     add.isbusy r0,r0,r1
