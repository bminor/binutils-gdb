#objdump: -dr --prefix-addresses --show-raw-insn
#name: microMIPS branch to absolute expression
#as: -32

.*: +file format .*mips.*

Disassembly of section \.text:
	\.\.\.
[0-9a-f]+ <[^>]*> 9400 fffe 	b	00001000 <foo>
[ 	]*[0-9a-f]+: R_MICROMIPS_PC16_S1	bar
[0-9a-f]+ <[^>]*> 0c00      	nop
[0-9a-f]+ <[^>]*> 4060 fffe 	bal	00001006 <foo\+0x6>
[ 	]*[0-9a-f]+: R_MICROMIPS_PC16_S1	bar
[0-9a-f]+ <[^>]*> 0000 0000 	nop
[0-9a-f]+ <[^>]*> 4020 fffe 	bltzal	zero,0000100e <foo\+0xe>
[ 	]*[0-9a-f]+: R_MICROMIPS_PC16_S1	bar
[0-9a-f]+ <[^>]*> 0000 0000 	nop
[0-9a-f]+ <[^>]*> 9402 fffe 	beqz	v0,00001016 <foo\+0x16>
[ 	]*[0-9a-f]+: R_MICROMIPS_PC16_S1	bar
[0-9a-f]+ <[^>]*> 0c00      	nop
[0-9a-f]+ <[^>]*> b402 fffe 	bnez	v0,0000101c <foo\+0x1c>
[ 	]*[0-9a-f]+: R_MICROMIPS_PC16_S1	bar
[0-9a-f]+ <[^>]*> 0c00      	nop
[0-9a-f]+ <[^>]*> 0c00      	nop
	\.\.\.
