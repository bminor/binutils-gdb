#name: MIPS LDPC from unaligned symbol 4
#source: unaligned-ldpc-4.s
#source: unaligned-syms.s
#as: -EB -32 -mips64r6
#ld: -EB -Ttext 0x1c000000 -Tdata 0x1c080000 -e 0x1c000000
#objdump: -dr --prefix-addresses --show-raw-insn

.*: +file format .*mips.*

Disassembly of section \.text:
[0-9a-f]+ <[^>]*> ec590005 	ldpc	v0,1c080028 <bar8>
	\.\.\.
