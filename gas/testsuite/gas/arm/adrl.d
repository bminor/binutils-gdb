#objdump: -dr --prefix-addresses --show-raw-insn
#name: ADRL

# Test the `ADRL' pseudo-op

.*: +file format .*arm.*

Disassembly of section .text:
	...
0x00002000 e24f0008 	sub	r0, pc, #8	; 0x8
0x00002004 e2400c20 	sub	r0, r0, #8192	; 0x2000
0x00002008 e28f0018 	add	r0, pc, #24	; 0x18
0x0000200c e2800c20 	add	r0, r0, #8192	; 0x2000
0x00002010 e24f0018 	sub	r0, pc, #24	; 0x18
0x00002014 e1a00000 	nop			\(mov r0,r0\)
0x00002018 e28f0000 	add	r0, pc, #0	; 0x0
0x0000201c e1a00000 	nop			\(mov r0,r0\)
	...
