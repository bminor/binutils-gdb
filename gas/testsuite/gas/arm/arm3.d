# name: ARM 3 instructions
# as: -mcpu=arm3
# objdump: -dr --prefix-addresses --show-raw-insn

.*: +file format .*arm.*

Disassembly of section .text:
0+0 <[^>]*> e1080091 ?	swp	r0, r1, \[r8\]
0+4 <[^>]*> e1432093 ?	swpb	r2, r3, \[r3\]
0+8 <[^>]*> a1444091 ?	swpgeb	r4, r1, \[r4\]
0+c <[^>]*> e1a00000 ?	nop			\(mov r0,r0\)
