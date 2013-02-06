#objdump: -dr --prefix-addresses
#name: NIOS2 rotate

# Test the and macro.

.*: +file format elf32-littlenios2

Disassembly of section .text:
0+0000 <[^>]*> rol	r4,r4,r4
0+0004 <[^>]*> roli	r4,r4,31
0+0008 <[^>]*> ror	r4,r4,r4
0+000c <[^>]*> sll	r4,r4,r4
0+0010 <[^>]*> slli	r4,r4,24
0+0014 <[^>]*> sra	r4,r4,r4
0+0018 <[^>]*> srai	r4,r4,10
0+001c <[^>]*> srl	r4,r4,r4
0+0020 <[^>]*> srli	r4,r4,5
