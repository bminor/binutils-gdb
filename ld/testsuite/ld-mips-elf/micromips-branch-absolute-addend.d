#objdump: -dr --prefix-addresses --show-raw-insn
#name: microMIPS link branch to absolute expression with addend
#source: ../../../gas/testsuite/gas/mips/micromips-branch-absolute-addend.s
#as: -EB -32
#ld: -EB -Ttext 0x12340000 -e foo

.*: +file format .*mips.*

Disassembly of section \.text:
	\.\.\.
[0-9a-f]+ <[^>]*> 9400 2c54 	b	0*123468ac <bar\+0x1233>
[0-9a-f]+ <[^>]*> 0c00      	nop
[0-9a-f]+ <[^>]*> 4060 2c51 	bal	0*123468ac <bar\+0x1233>
[0-9a-f]+ <[^>]*> 0000 0000 	nop
[0-9a-f]+ <[^>]*> 4020 2c4d 	bltzal	zero,0*123468ac <bar\+0x1233>
[0-9a-f]+ <[^>]*> 0000 0000 	nop
[0-9a-f]+ <[^>]*> 9402 2c49 	beqz	v0,0*123468ac <bar\+0x1233>
[0-9a-f]+ <[^>]*> 0c00      	nop
[0-9a-f]+ <[^>]*> b402 2c46 	bnez	v0,0*123468ac <bar\+0x1233>
[0-9a-f]+ <[^>]*> 0c00      	nop
[0-9a-f]+ <[^>]*> 0c00      	nop
	\.\.\.
