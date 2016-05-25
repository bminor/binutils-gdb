#name: MIPS JALX to unaligned symbol 2
#source: unaligned-jalx-2.s
#source: unaligned-insn.s -mips16
#as: -EB -32
#ld: -EB -Ttext 0x1c000000 -e 0x1c000000
#objdump: -dr --prefix-addresses --show-raw-insn

.*: +file format .*mips.*

Disassembly of section \.text:
[0-9a-f]+ <[^>]*> 77000009 	jalx	1c000024 <bar2>
[0-9a-f]+ <[^>]*> 00000000 	nop
	\.\.\.
	\.\.\.
	\.\.\.
	\.\.\.
	\.\.\.
	\.\.\.
	\.\.\.
	\.\.\.
	\.\.\.
