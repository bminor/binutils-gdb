#as: -mpower10
#objdump: -dr -Mpower10
#name: POWER10 tests

.*

Disassembly of section \.text:

0+0 <_start>:
.*:	(7d 40 06 a4|a4 06 40 7d) 	slbiag  r10
.*:	(7d 40 06 a4|a4 06 40 7d) 	slbiag  r10
.*:	(7d 41 06 a4|a4 06 41 7d) 	slbiag  r10,1
.*:	(7c 2a 5f 0d|0d 5f 2a 7c) 	paste\.  r10,r11
.*:	(7c 2a 5f 0d|0d 5f 2a 7c) 	paste\.  r10,r11
.*:	(7c 0a 5f 0d|0d 5f 0a 7c) 	paste\.  r10,r11,0
#pass
