#objdump: -dr --prefix-addresses
#name: NIOS2 xor

# Test the nor instruction

.*: +file format elf32-littlenios2

Disassembly of section .text:
0+0000 <[^>]*> xor	r6,r8,r10
0+0004 <[^>]*> xorhi	r6,r7,65535
0+0008 <[^>]*> xori	r6,r7,65535
