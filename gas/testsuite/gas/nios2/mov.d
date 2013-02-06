#objdump: -dr --prefix-addresses
#name: NIOS2 mov

# Test the mov instruction

.*: +file format elf32-littlenios2

Disassembly of section .text:
0+0000 <[^>]*> movz	\$4,\$5,\$6
0+0004 <[^>]*> movnz	\$4,\$5,\$6

