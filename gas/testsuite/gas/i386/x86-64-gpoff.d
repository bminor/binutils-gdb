#objdump: -drw
#name: x86-64 gpoff

.*: +file format .*

Disassembly of section .text:

0+ <_start>:
 +[a-f0-9]+:	8d 04 25 00 00 00 00 	lea    0x0,%eax	3: R_X86_64_GPOFF	foo
 +[a-f0-9]+:	65 8b 04 25 00 00 00 00 	mov    %gs:0x0,%eax	b: R_X86_64_GPOFF	foo
#pass
