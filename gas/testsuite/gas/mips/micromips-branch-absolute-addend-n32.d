#objdump: -dr --prefix-addresses --show-raw-insn
#name: microMIPS branch to absolute expression with addend (n32)
#as: -n32 -march=from-abi
#source: micromips-branch-absolute-addend.s

.*: +file format .*mips.*

Disassembly of section \.text:
	\.\.\.
[0-9a-f]+ <[^>]*> 9400 0000 	b	00001004 <foo\+0x4>
[ 	]*[0-9a-f]+: R_MICROMIPS_PC16_S1	bar\+0x1230
[0-9a-f]+ <[^>]*> 0c00      	nop
[0-9a-f]+ <[^>]*> 4060 0000 	bal	0000100a <foo\+0xa>
[ 	]*[0-9a-f]+: R_MICROMIPS_PC16_S1	bar\+0x1230
[0-9a-f]+ <[^>]*> 0000 0000 	nop
[0-9a-f]+ <[^>]*> 4020 0000 	bltzal	zero,00001012 <foo\+0x12>
[ 	]*[0-9a-f]+: R_MICROMIPS_PC16_S1	bar\+0x1230
[0-9a-f]+ <[^>]*> 0000 0000 	nop
[0-9a-f]+ <[^>]*> 9402 0000 	beqz	v0,0000101a <foo\+0x1a>
[ 	]*[0-9a-f]+: R_MICROMIPS_PC16_S1	bar\+0x1230
[0-9a-f]+ <[^>]*> 0c00      	nop
[0-9a-f]+ <[^>]*> b402 0000 	bnez	v0,00001020 <foo\+0x20>
[ 	]*[0-9a-f]+: R_MICROMIPS_PC16_S1	bar\+0x1230
[0-9a-f]+ <[^>]*> 0c00      	nop
[0-9a-f]+ <[^>]*> 0c00      	nop
	\.\.\.
