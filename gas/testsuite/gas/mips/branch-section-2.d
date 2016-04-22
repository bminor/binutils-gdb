#objdump: -dr --prefix-addresses --show-raw-insn
#name: microMIPS branch to a different section
#as: -32 -mmicromips
#source: branch-section.s

.*: +file format .*mips.*

Disassembly of section \.text:
[0-9a-f]+ <[^>]*> 9400 fffe 	b	00000000 <foo>
[ 	]*[0-9a-f]+: R_MICROMIPS_PC16_S1	bar
[0-9a-f]+ <[^>]*> 0c00      	nop
	\.\.\.

Disassembly of section \.init:
[0-9a-f]+ <[^>]*> 459f      	jr	ra
[0-9a-f]+ <[^>]*> 0c00      	nop
	\.\.\.
