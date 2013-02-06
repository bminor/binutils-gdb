#objdump: -dr --prefix-addresses
#name: NIOS2 mul

# Test the mul macro.

.*: +file format elf32-littlenios2

Disassembly of section .text:
0+0000 <[^>]*> mul	r4,r5,r6
0+0004 <[^>]*> muli	r4,r5,0
0+0008 <[^>]*> muli	r4,r5,1
0+000c <[^>]*> muli	r4,r5,-32768
0+0010 <[^>]*> muli	r4,r5,32767
0+0014 <[^>]*> muli	r4,r5,0
[	]*14: R_NIOS2_S16	undefined_symbol
0+0018 <[^>]*> muli	r4,r5,16448
0+001c <[^>]*> mulxss	r4,r5,r6
0+0020 <[^>]*> mulxsu	r4,r5,r6
0+0024 <[^>]*> mulxuu	r4,r5,r6
