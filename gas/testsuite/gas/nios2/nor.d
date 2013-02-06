#objdump: -dr --prefix-addresses
#name: NIOS2 nor

# Test the nor instruction

.*: +file format elf32-littlenios2

Disassembly of section .text:
0+0000 <[^>]*> nor	r6,r8,r10
