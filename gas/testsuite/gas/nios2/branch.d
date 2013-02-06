#objdump: -dr --prefix-addresses
#name: NIOS2 branch

# Test the branch instructions.
dump.o:     file format elf32-littlenios2

Disassembly of section .text:
0+0000 <[^>]*> beq	r4,r5,00000004 <text_label\+0x4>
[	]*0: R_NIOS2_PCREL16	text_label
0+0004 <[^>]*> bge	r4,r5,00000008 <text_label\+0x8>
[	]*4: R_NIOS2_PCREL16	text_label
0+0008 <[^>]*> bgeu	r4,r5,0000000c <text_label\+0xc>
[	]*8: R_NIOS2_PCREL16	text_label
0+000c <[^>]*> blt	r4,r5,00000010 <text_label\+0x10>
[	]*c: R_NIOS2_PCREL16	text_label
0+0010 <[^>]*> bltu	r4,r5,00000014 <text_label\+0x14>
[	]*10: R_NIOS2_PCREL16	text_label
0+0014 <[^>]*> bne	r4,r5,00000018 <text_label\+0x18>
[	]*14: R_NIOS2_PCREL16	text_label
0+0018 <[^>]*> br	0000001c <text_label\+0x1c>
[	]*18: R_NIOS2_PCREL16	external_label
