#objdump: -dr --prefix-addresses 
#name: NIOS2 add

# Test the add instruction

.*: +file format elf32-littlenios2

Disassembly of section .text:
0+0000 <[^>]*> add	r4,r4,r4
0+0004 <[^>]*> addi	r4,r4,32767
0+0008 <[^>]*> addi	r4,r4,-32768
0+000c <[^>]*> addi	r4,r4,0
0+0010 <[^>]*> addi	r4,r4,-1
0+0014 <[^>]*> addi	r4,r4,-1
0+0018 <[^>]*> addi	r4,r4,13398
0+001c <[^>]*> nop
