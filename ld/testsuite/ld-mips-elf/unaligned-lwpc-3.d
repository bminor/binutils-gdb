#name: MIPS LWPC from unaligned symbol 3
#source: unaligned-lwpc-3.s
#source: unaligned-syms.s
#as: -EB -32 -mips32r6
#ld: -EB -Ttext 0x1c000000 -Tdata 0x1c080000 -e 0x1c000000
#objdump: -dr --prefix-addresses --show-raw-insn

.*: +file format .*mips.*

Disassembly of section \.text:
[0-9a-f]+ <[^>]*> ec4a0009 	lwpc	v0,1c080024 <bar4>
	\.\.\.
