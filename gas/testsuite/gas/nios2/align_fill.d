#objdump: -dr --prefix-addresses
#name: NIOS2 align_fill

# Test the and macro.

.*: +file format elf32-littlenios2

Disassembly of section .text:
0+0000 <[^>]*> addi	sp,sp,-8
0+0004 <[^>]*> stw	fp,4\(sp\)
0+0008 <[^>]*> mov	fp,sp
0+000c <[^>]*> mov	r3,zero
0+0010 <[^>]*> nop
0+0014 <[^>]*> nop
0+0018 <[^>]*> nop
0+001c <[^>]*> nop
0+0020 <[^>]*> addi	r3,r3,1
0+0024 <[^>]*> cmplti	r2,r3,100
0+0028 <[^>]*> bne	r2,zero,0+0020 <[^>*]*>
0+002c <[^>]*> ldw	fp,4\(sp\)
0+0030 <[^>]*> addi	sp,sp,8
0+0034 <[^>]*> ret
	...
