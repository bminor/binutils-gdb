#as: -a64 -mfuture
#ld: -melf64ppc -shared --plt-align=0 --hash-style=gnu
#objdump: -dr -Mfuture

.*

Disassembly of section \.text:

.*\.plt_call\.f1>:
.*:	(f8 41 00 18|18 00 41 f8) 	std     r2,24\(r1\)
.*:	(60 00 00 00|00 00 00 60) 	nop
.*:	(04 10 00 01|01 00 10 04) 	pld     r12,.*
.*:	(e5 80 .. ..|.. .. 80 e5) 
.*:	(7d 89 03 a6|a6 03 89 7d) 	mtctr   r12
.*:	(4e 80 04 20|20 04 80 4e) 	bctr

.*\.plt_call\.f2>:
.*:	(04 10 00 01|01 00 10 04) 	pld     r12,.*
.*:	(e5 80 .. ..|.. .. 80 e5) 
.*:	(7d 89 03 a6|a6 03 89 7d) 	mtctr   r12
.*:	(4e 80 04 20|20 04 80 4e) 	bctr
#...
.*:	(4b ff ff c1|c1 ff ff 4b) 	bl      .*\.plt_call\.f1>
.*:	(e8 41 00 18|18 00 41 e8) 	ld      r2,24\(r1\)
.*:	(4b ff ff bd|bd ff ff 4b) 	bl      .*\.plt_call\.f1\+0x4>
.*:	(4b ff ff cd|cd ff ff 4b) 	bl      .*\.plt_call\.f2>
.*:	(04 10 00 01|01 00 10 04) 	pld     r3,.*
.*:	(e4 60 .. ..|.. .. 60 e4) 
#pass
