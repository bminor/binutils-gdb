#objdump: -dr --prefix-addresses 
#name: NIOS2 jmp

# Test the jmp instruction.

.*: +file format elf32-littlenios2

Disassembly of section .text:
0+0000 <[^>]*> jmp	bt

