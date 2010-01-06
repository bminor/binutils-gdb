# name: Data disassembler test
# skip: *-*-*coff *-*-pe *-*-wince *-*-*aout* *-*-netbsd *-*-riscix*
# objdump: -dr --prefix-addresses --show-raw-insn

.*: +file format .*arm.*

Disassembly of section \.text:
0x00000000 20010000 	.word	0x20010000
0x00000004 000000f9 	.word	0x000000f9
0x00000008 00004cd5 	.word	0x00004cd5
