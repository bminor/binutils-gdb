#objdump: -drw
#name: i386 gpoff

.*: +file format .*

Disassembly of section .text:

0+ <_start>:
 +[a-f0-9]+:	8d 05 00 00 00 00    	lea    0x0,%eax	2: R_386_GPOFF	foo
 +[a-f0-9]+:	64 a1 00 00 00 00    	mov    %fs:0x0,%eax	8: R_386_GPOFF	foo
#pass
