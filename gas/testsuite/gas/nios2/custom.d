#objdump: -dr --prefix-addresses 
#name: NIOS2 custom

# Test the custom instruction

.*: +file format elf32-littlenios2

Disassembly of section .text:
0+0000 <[^>]*> custom	0,r11,r2,r3
0+0004 <[^>]*> custom	255,r11,r2,r3
0+0008 <[^>]*> custom	150,c1,r2,r3
0+000c <[^>]*> custom	24,c1,c2,r3
0+0010 <[^>]*> custom	56,c1,c2,c3
