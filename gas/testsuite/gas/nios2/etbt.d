#objdump: -dr --prefix-addresses 
#name: NIOS2 etbt

# Test the et, bt registers

.*: +file format elf32-littlenios2

Disassembly of section .text:
0+0000 <[^>]*> add	et,bt,r6
0+0004 <[^>]*> add	et,bt,r6
