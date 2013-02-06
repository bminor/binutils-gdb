#objdump: -dr --prefix-addresses 
#name: NIOS2 cache

.*: +file format elf32-littlenios2

Disassembly of section .text:
0+0000 <[^>]*> flushd	-32768\(r6\)
0+0004 <[^>]*> flushd	32767\(r6\)
0+0008 <[^>]*> flushd	0\(r6\)
0+000c <[^>]*> flushd	-1\(r6\)
0+0010 <[^>]*> flushd	0\(r6\)
[	]*10: R_NIOS2_S16	.text
0+0014 <[^>]*> flushd	0\(r6\)
[	]*14: R_NIOS2_S16	external
0+0018 <[^>]*> flushi	r2
0+001c <[^>]*> flushp

