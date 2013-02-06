#objdump: -dr --prefix-addresses 
#name: NIOS2 complex

# Test complex expression parsing

.*: +file format elf32-littlenios2

Disassembly of section .text:
0+0000 <[^>]*> ldw	r2,-1\(r3\)
0+0004 <[^>]*> ldw	r2,1\(r3\)
0+0008 <[^>]*> ldw	r2,0\(r3\)
			8: R_NIOS2_S16	stack_top-0x1
