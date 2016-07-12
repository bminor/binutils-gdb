#objdump: -dr --prefix-addresses --show-raw-insn
#name: MIPS branch to absolute expression
#as: -32

.*: +file format .*mips.*

Disassembly of section \.text:
	\.\.\.
[0-9a-f]+ <[^>]*> 1000048c 	b	00002234 <bar\+0x1000>
[ 	]*[0-9a-f]+: R_MIPS_PC16	\*ABS\*
[0-9a-f]+ <[^>]*> 00000000 	nop
[0-9a-f]+ <[^>]*> 0411048c 	bal	0000223c <bar\+0x1008>
[ 	]*[0-9a-f]+: R_MIPS_PC16	\*ABS\*
[0-9a-f]+ <[^>]*> 00000000 	nop
[0-9a-f]+ <[^>]*> 0410048c 	bltzal	zero,00002244 <bar\+0x1010>
[ 	]*[0-9a-f]+: R_MIPS_PC16	\*ABS\*
[0-9a-f]+ <[^>]*> 00000000 	nop
[0-9a-f]+ <[^>]*> 1040048c 	beqz	v0,0000224c <bar\+0x1018>
[ 	]*[0-9a-f]+: R_MIPS_PC16	\*ABS\*
[0-9a-f]+ <[^>]*> 00000000 	nop
[0-9a-f]+ <[^>]*> 1440048c 	bnez	v0,00002254 <bar\+0x1020>
[ 	]*[0-9a-f]+: R_MIPS_PC16	\*ABS\*
[0-9a-f]+ <[^>]*> 00000000 	nop
	\.\.\.
