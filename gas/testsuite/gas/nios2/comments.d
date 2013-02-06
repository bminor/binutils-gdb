#objdump: -dr --prefix-addresses 
#name: NIOS2 comments

# Test the add instruction

.*: +file format elf32-littlenios2

Disassembly of section .text:
0+0000 <[^>]*> br	0000001c <start>
0+0004 <[^>]*> br	00000008 <abort>
0+0008 <[^>]*> movui	r3,0
0+000c <[^>]*> movui	r2,1
0+0010 <[^>]*> movui	r3,0
0+0014 <[^>]*> movui	r2,0
0+0018 <[^>]*> br	00000044 <exit>
0+001c <[^>]*> addi	r2,r2,-4
0+0020 <[^>]*> movui	r11,1
0+0024 <[^>]*> movui	r5,0
0+0028 <[^>]*> movui	r6,0
0+002c <[^>]*> br	00000030 <ldst>
0+0030 <[^>]*> movui	r2,61452
0+0034 <[^>]*> movui	r20,64206
0+0038 <[^>]*> stw	r20,0\(r2\)
0+003c <[^>]*> ldw	r21,0\(r2\)
0+0040 <[^>]*> br	00000010 <end>
0+0044 <[^>]*> br	00000044 <exit>
