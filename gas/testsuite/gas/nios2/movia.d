#objdump: -dr --prefix-addresses 
#name: NIOS2 movia

# Test the movia instruction

.*: +file format elf32-littlenios2

Disassembly of section .text:
0+0000 <[^>]*> movhi	r2,32897
0+0004 <[^>]*> addi	r2,r2,-32640
0+0008 <[^>]*> movhi	r3,0
			8: R_NIOS2_HIADJ16	sym-0x80000000
0+000c <[^>]*> addi	r3,r3,0
			c: R_NIOS2_LO16	sym-0x80000000
0+0010 <[^>]*> movhi	r4,0
			10: R_NIOS2_HIADJ16	sym-0x7fffffff
0+0014 <[^>]*> addi	r4,r4,0
			14: R_NIOS2_LO16	sym-0x7fffffff
