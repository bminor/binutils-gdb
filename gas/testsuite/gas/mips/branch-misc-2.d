#objdump: -dr --prefix-addresses --show-raw-insn
#name: MIPS branch-misc-2

# Test the branches to global symbols in current file.

.*: +file format .*mips.*

Disassembly of section .text:
	\.\.\.
	\.\.\.
	\.\.\.
0+003c <[^>]*> 0411fff0 	bal	00000000 <g1>
0+0040 <[^>]*> 00000000 	nop
0+0044 <[^>]*> 0411fff3 	bal	00000014 <g2>
0+0048 <[^>]*> 00000000 	nop
0+004c <[^>]*> 0411fff6 	bal	00000028 <g3>
0+0050 <[^>]*> 00000000 	nop
0+0054 <[^>]*> 0411000a 	bal	00000080 <g4>
0+0058 <[^>]*> 00000000 	nop
0+005c <[^>]*> 0411000d 	bal	00000094 <g5>
0+0060 <[^>]*> 00000000 	nop
0+0064 <[^>]*> 04110010 	bal	000000a8 <g6>
0+0068 <[^>]*> 00000000 	nop
	\.\.\.
	\.\.\.
	\.\.\.
	\.\.\.
