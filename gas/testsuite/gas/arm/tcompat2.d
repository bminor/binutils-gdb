#name: Thumb ARM-compat pseudos
#objdump: -dr --prefix-addresses --show-raw-insn -M force-thumb
#as:

# Test the Thumb pseudo instructions that exist for ARM source compatibility

.*: +file format .*arm.*

Disassembly of section .text:

0+00 <[^>]*> 4148 *	adcs	r0, r1
0+02 <[^>]*> 4008 *	ands	r0, r1
0+04 <[^>]*> 4388 *	bics	r0, r1
0+06 <[^>]*> 4048 *	eors	r0, r1
0+08 <[^>]*> 4348 *	muls	r0, r1
0+0a <[^>]*> 4308 *	orrs	r0, r1
0+0c <[^>]*> 4188 *	sbcs	r0, r1
0+0e <[^>]*> 46c0 *	nop			\(mov r8, r8\)
