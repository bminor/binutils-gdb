#name: bl local instructions for v4t.
#objdump: -drw --prefix-addresses --show-raw-insn
#skip: *-*-*coff *-*-pe *-*-wince *-*-*aout* *-*-netbsd *-*-riscix*
#as:
# stderr: blx-local-thumb.l

.*: +file format .*arm.*
Disassembly of section .text:
0+00 <[^>]*> f7ff fffe 	bl	00+18 <[^>]*>	0: R_ARM_THM_CALL	foo2
0+1c <[^>]*> d004      	beq.n	00+28 <[^>]*>
0+1e <[^>]*> e003      	b.n	00+28 <[^>]*>
0+20 <[^>]*> f000 f808 	bl	00+34 <[^>]*>
0+24 <[^>]*> f000 f802 	bl	00+2c <[^>]*>
0+28 <[^>]*> 46c0      	nop			\(mov r8, r8\)
0+2a <[^>]*> 46c0      	nop			\(mov r8, r8\)
0+2c <[^>]*> 46c0      	nop			\(mov r8, r8\)
	...
0+30 <[^>]*> e1a00000 	nop			\(mov r0,r0\)
0+34 <[^>]*> e1a00000 	nop			\(mov r0,r0\)