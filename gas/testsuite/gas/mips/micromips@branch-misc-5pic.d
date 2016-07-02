#objdump: -dr --prefix-addresses --show-raw-insn
#name: MIPS branch-misc-5pic
#source: branch-misc-5.s
#as: -32 -call_shared

# Test branches to undefined symbols and a defined local symbol
# in another section (microMIPS).

.*: +file format .*mips.*

Disassembly of section \.text:
[0-9a-f]+ <[^>]*> 9400 fffe 	b	0+0000 <g6>
			0: R_MICROMIPS_PC16_S1	x1
[0-9a-f]+ <[^>]*> 0c00      	nop
[0-9a-f]+ <[^>]*> 9400 fffe 	b	0+0006 <g6\+0x6>
			6: R_MICROMIPS_PC16_S1	x2
[0-9a-f]+ <[^>]*> 0c00      	nop
[0-9a-f]+ <[^>]*> 9400 fffe 	b	0+000c <g6\+0xc>
			c: R_MICROMIPS_PC16_S1	\.Ldata
[0-9a-f]+ <[^>]*> 0c00      	nop
[0-9a-f]+ <[^>]*> 0c00      	nop
	\.\.\.
