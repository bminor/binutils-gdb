#objdump: -dr --prefix-addresses --show-raw-insn
#name: microMIPS link branch to absolute expression
#source: ../../../gas/testsuite/gas/mips/micromips-branch-absolute.s
#as: -EB -32
#ld: -EB -Ttext 0 -e foo

.*: +file format .*mips.*

Disassembly of section \.text:
	\.\.\.
[0-9a-f]+ <[^>]*> 9400 0118 	b	0+001234 <foo\+0x234>
[0-9a-f]+ <[^>]*> 0c00      	nop
[0-9a-f]+ <[^>]*> 4060 0115 	bal	0+001234 <foo\+0x234>
[0-9a-f]+ <[^>]*> 0000 0000 	nop
[0-9a-f]+ <[^>]*> 4020 0111 	bltzal	zero,0+001234 <foo\+0x234>
[0-9a-f]+ <[^>]*> 0000 0000 	nop
[0-9a-f]+ <[^>]*> 9402 010d 	beqz	v0,0+001234 <foo\+0x234>
[0-9a-f]+ <[^>]*> 0c00      	nop
[0-9a-f]+ <[^>]*> b402 010a 	bnez	v0,0+001234 <foo\+0x234>
[0-9a-f]+ <[^>]*> 0c00      	nop
[0-9a-f]+ <[^>]*> 0c00      	nop
	\.\.\.
