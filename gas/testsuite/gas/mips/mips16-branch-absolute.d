#objdump: -dr --prefix-addresses --show-raw-insn
#name: MIPS16 branch to absolute expression
#as: -32

.*: +file format .*mips.*

Disassembly of section \.text:
	\.\.\.
[0-9a-f]+ <[^>]*> f101 1018 	b	00002234 <bar\+0x1000>
[ 	]*[0-9a-f]+: R_MIPS16_PC16_S1	\*ABS\*
[0-9a-f]+ <[^>]*> f101 6018 	bteqz	00002238 <bar\+0x1004>
[ 	]*[0-9a-f]+: R_MIPS16_PC16_S1	\*ABS\*
[0-9a-f]+ <[^>]*> f101 6118 	btnez	0000223c <bar\+0x1008>
[ 	]*[0-9a-f]+: R_MIPS16_PC16_S1	\*ABS\*
[0-9a-f]+ <[^>]*> f101 2218 	beqz	v0,00002240 <bar\+0x100c>
[ 	]*[0-9a-f]+: R_MIPS16_PC16_S1	\*ABS\*
[0-9a-f]+ <[^>]*> f101 2a18 	bnez	v0,00002244 <bar\+0x1010>
[ 	]*[0-9a-f]+: R_MIPS16_PC16_S1	\*ABS\*
[0-9a-f]+ <[^>]*> 6500      	nop
	\.\.\.
