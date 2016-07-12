#objdump: -dr --prefix-addresses --show-raw-insn
#name: microMIPS branch to absolute expression (n64)
#as: -64 -march=from-abi
#source: micromips-branch-absolute.s

.*: +file format .*mips.*

Disassembly of section \.text:
	\.\.\.
[0-9a-f]+ <[^>]*> 9400 0000 	b	0000000000001004 <foo\+0x4>
[ 	]*[0-9a-f]+: R_MICROMIPS_PC16_S1	bar-0x4
[ 	]*[0-9a-f]+: R_MIPS_NONE	\*ABS\*-0x4
[ 	]*[0-9a-f]+: R_MIPS_NONE	\*ABS\*-0x4
[0-9a-f]+ <[^>]*> 0c00      	nop
[0-9a-f]+ <[^>]*> 4060 0000 	bal	000000000000100a <foo\+0xa>
[ 	]*[0-9a-f]+: R_MICROMIPS_PC16_S1	bar-0x4
[ 	]*[0-9a-f]+: R_MIPS_NONE	\*ABS\*-0x4
[ 	]*[0-9a-f]+: R_MIPS_NONE	\*ABS\*-0x4
[0-9a-f]+ <[^>]*> 0000 0000 	nop
[0-9a-f]+ <[^>]*> 4020 0000 	bltzal	zero,0000000000001012 <foo\+0x12>
[ 	]*[0-9a-f]+: R_MICROMIPS_PC16_S1	bar-0x4
[ 	]*[0-9a-f]+: R_MIPS_NONE	\*ABS\*-0x4
[ 	]*[0-9a-f]+: R_MIPS_NONE	\*ABS\*-0x4
[0-9a-f]+ <[^>]*> 0000 0000 	nop
[0-9a-f]+ <[^>]*> 9402 0000 	beqz	v0,000000000000101a <foo\+0x1a>
[ 	]*[0-9a-f]+: R_MICROMIPS_PC16_S1	bar-0x4
[ 	]*[0-9a-f]+: R_MIPS_NONE	\*ABS\*-0x4
[ 	]*[0-9a-f]+: R_MIPS_NONE	\*ABS\*-0x4
[0-9a-f]+ <[^>]*> 0c00      	nop
[0-9a-f]+ <[^>]*> b402 0000 	bnez	v0,0000000000001020 <foo\+0x20>
[ 	]*[0-9a-f]+: R_MICROMIPS_PC16_S1	bar-0x4
[ 	]*[0-9a-f]+: R_MIPS_NONE	\*ABS\*-0x4
[ 	]*[0-9a-f]+: R_MIPS_NONE	\*ABS\*-0x4
[0-9a-f]+ <[^>]*> 0c00      	nop
[0-9a-f]+ <[^>]*> 0c00      	nop
	\.\.\.
