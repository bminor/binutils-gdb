#objdump: -dr --prefix-addresses
#name: NIOS2 nios2-reloc-r-nios2-call26

# Test the branch instructions.
.*: +file format elf32-littlenios2

Disassembly of section .text:
[	]*\.\.\.
[	]*0: R_NIOS2_CALL26	.text\+0x100
[	]*4: R_NIOS2_CALL26	globalfunc
0+0008 <[^>]*> nop
0+000c <[^>]*> nop
0+0010 <[^>]*> nop
0+0014 <[^>]*> nop
0+0018 <[^>]*> nop
0+001c <[^>]*> nop
0+0020 <[^>]*> nop
0+0024 <[^>]*> nop
0+0028 <[^>]*> nop
0+002c <[^>]*> nop
0+0030 <[^>]*> nop
0+0034 <[^>]*> nop
0+0038 <[^>]*> nop
0+003c <[^>]*> nop
0+0040 <[^>]*> nop
0+0044 <[^>]*> nop
0+0048 <[^>]*> nop
0+004c <[^>]*> nop
0+0050 <[^>]*> nop
0+0054 <[^>]*> nop
0+0058 <[^>]*> nop
0+005c <[^>]*> nop
0+0060 <[^>]*> nop
0+0064 <[^>]*> nop
0+0068 <[^>]*> nop
0+006c <[^>]*> nop
0+0070 <[^>]*> nop
0+0074 <[^>]*> nop
0+0078 <[^>]*> nop
0+007c <[^>]*> nop
0+0080 <[^>]*> nop
0+0084 <[^>]*> nop
0+0088 <[^>]*> nop
0+008c <[^>]*> nop
0+0090 <[^>]*> nop
0+0094 <[^>]*> nop
0+0098 <[^>]*> nop
0+009c <[^>]*> nop
0+00a0 <[^>]*> nop
0+00a4 <[^>]*> nop
0+00a8 <[^>]*> nop
0+00ac <[^>]*> nop
0+00b0 <[^>]*> nop
0+00b4 <[^>]*> nop
0+00b8 <[^>]*> nop
0+00bc <[^>]*> nop
0+00c0 <[^>]*> nop
0+00c4 <[^>]*> nop
0+00c8 <[^>]*> nop
0+00cc <[^>]*> nop
0+00d0 <[^>]*> nop
0+00d4 <[^>]*> nop
0+00d8 <[^>]*> nop
0+00dc <[^>]*> nop
0+00e0 <[^>]*> nop
0+00e4 <[^>]*> nop
0+00e8 <[^>]*> nop
0+00ec <[^>]*> nop
0+00f0 <[^>]*> nop
0+00f4 <[^>]*> nop
0+00f8 <[^>]*> nop
0+00fc <[^>]*> nop
0+0100 <[^>]*> nop
	...


