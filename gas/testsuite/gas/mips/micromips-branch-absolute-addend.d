#objdump: -dr --prefix-addresses --show-raw-insn
#name: microMIPS branch to absolute expression with addend
#as: -32

.*: +file format .*mips.*

Disassembly of section \.text:
	\.\.\.
[0-9a-f]+ <[^>]*> 9400 0918 	b	00002234 <foo\+0x1234>
[ 	]*[0-9a-f]+: R_MICROMIPS_PC16_S1	bar
[0-9a-f]+ <[^>]*> 0c00      	nop
[0-9a-f]+ <[^>]*> 4060 0918 	bal	0000223a <foo\+0x123a>
[ 	]*[0-9a-f]+: R_MICROMIPS_PC16_S1	bar
[0-9a-f]+ <[^>]*> 0000 0000 	nop
[0-9a-f]+ <[^>]*> 4020 0918 	bltzal	zero,00002242 <foo\+0x1242>
[ 	]*[0-9a-f]+: R_MICROMIPS_PC16_S1	bar
[0-9a-f]+ <[^>]*> 0000 0000 	nop
[0-9a-f]+ <[^>]*> 9402 0918 	beqz	v0,0000224a <foo\+0x124a>
[ 	]*[0-9a-f]+: R_MICROMIPS_PC16_S1	bar
[0-9a-f]+ <[^>]*> 0c00      	nop
[0-9a-f]+ <[^>]*> b402 0918 	bnez	v0,00002250 <foo\+0x1250>
[ 	]*[0-9a-f]+: R_MICROMIPS_PC16_S1	bar
[0-9a-f]+ <[^>]*> 0c00      	nop
[0-9a-f]+ <[^>]*> 0c00      	nop
	\.\.\.
