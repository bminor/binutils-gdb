# objdump: -dr --prefix-addresses
#name: NIOS2 call

.*: +file format elf32-littlenios2

Disassembly of section .text:
0+0000 <[^>]*> call	00000000 <foo>
[	]*0: R_NIOS2_CALL26	.text\+0xc
0+0004 <[^>]*> callr	r10
0+0008 <[^>]*> call	00000000 <foo>
[	]*8: R_NIOS2_CALL26	external
