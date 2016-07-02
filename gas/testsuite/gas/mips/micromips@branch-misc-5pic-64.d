#objdump: -dr --prefix-addresses --show-raw-insn
#name: MIPS branch-misc-5pic-64
#source: branch-misc-5.s
#as: -64 -call_shared

# Test branches to undefined symbols and a defined local symbol
# in another section (microMIPS).

.*: +file format .*mips.*

Disassembly of section \.text:
[0-9a-f]+ <[^>]*> 9400 0000 	b	0+0004 <g6\+0x4>
			0: R_MICROMIPS_PC16_S1	x1\-0x4
			0: R_MIPS_NONE	\*ABS\*\-0x4
			0: R_MIPS_NONE	\*ABS\*\-0x4
[0-9a-f]+ <[^>]*> 0c00      	nop
[0-9a-f]+ <[^>]*> 9400 0000 	b	0+000a <g6\+0xa>
			6: R_MICROMIPS_PC16_S1	x2\-0x4
			6: R_MIPS_NONE	\*ABS\*\-0x4
			6: R_MIPS_NONE	\*ABS\*\-0x4
[0-9a-f]+ <[^>]*> 0c00      	nop
[0-9a-f]+ <[^>]*> 9400 0000 	b	0+0010 <g6\+0x10>
			c: R_MICROMIPS_PC16_S1	\.data\-0x4
			c: R_MIPS_NONE	\*ABS\*\-0x4
			c: R_MIPS_NONE	\*ABS\*\-0x4
[0-9a-f]+ <[^>]*> 0c00      	nop
[0-9a-f]+ <[^>]*> 0c00      	nop
	\.\.\.
