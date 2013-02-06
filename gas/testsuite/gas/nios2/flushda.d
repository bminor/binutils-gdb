#objdump: -dr --prefix-addresses 
#name: NIOS2 flushda

# Test the jmp instruction.

.*: +file format elf32-littlenios2

Disassembly of section .text:
0+0000 <[^>]*> flushda	12\(r2\)

