#objdump: -dr --prefix-addresses --show-raw-insn
#name: MIPS branch-misc-2

# Test the branches to global symbols in current file.

.*: +file format .*mips.*

Disassembly of section .text:
	\.\.\.
	\.\.\.
	\.\.\.
0+003c <[^>]*> 0411fffc 	bal	00000030 <g3\+0x8>
[ 	]*3c: R_MIPS_PC16	g1
0+0040 <[^>]*> 00000000 	nop
0+0044 <[^>]*> 0411fffc 	bal	00000038 <g3\+0x10>
[ 	]*44: R_MIPS_PC16	g2
0+0048 <[^>]*> 00000000 	nop
0+004c <[^>]*> 0411fffc 	bal	00000040 <x\+0x4>
[ 	]*4c: R_MIPS_PC16	g3
0+0050 <[^>]*> 00000000 	nop
0+0054 <[^>]*> 0411fffc 	bal	00000048 <x\+0xc>
[ 	]*54: R_MIPS_PC16	g4
0+0058 <[^>]*> 00000000 	nop
0+005c <[^>]*> 0411fffc 	bal	00000050 <x\+0x14>
[ 	]*5c: R_MIPS_PC16	g5
0+0060 <[^>]*> 00000000 	nop
0+0064 <[^>]*> 0411fffc 	bal	00000058 <x\+0x1c>
[ 	]*64: R_MIPS_PC16	g6
0+0068 <[^>]*> 00000000 	nop
	\.\.\.
	\.\.\.
	\.\.\.
	\.\.\.
