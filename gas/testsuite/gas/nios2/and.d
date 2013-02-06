#objdump: -dr --prefix-addresses
#name: NIOS2 and

# Test the and macro.

.*: +file format elf32-littlenios2

Disassembly of section .text:
0+0000 <[^>]*> and	r4,r4,r4
0+0004 <[^>]*> andi	r4,r4,32767
0+0008 <[^>]*> andi	r4,r4,32768
0+000c <[^>]*> andi	r4,r4,65535
0+0010 <[^>]*> andi	r4,r4,0
0+0014 <[^>]*> andhi	r4,r4,32767
0+0018 <[^>]*> andhi	r4,r4,32768
0+001c <[^>]*> andhi	r4,r4,65535
0+0020 <[^>]*> andhi	r4,r4,0
