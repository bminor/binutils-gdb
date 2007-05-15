#objdump: -dr --prefix-addresses --show-raw-insn
#name: Backslash-at for ARM

.*:     file format .*arm.*

Disassembly of section .text:
00000000 <.text> 615c      	.short	0x615c
00000002 <foo> e3a00000 	mov	r0, #0	; 0x0
00000006 <foo\+0x4> e3a00000 	mov	r0, #0	; 0x0
0000000a <foo\+0x8> e3a00000 	mov	r0, #0	; 0x0
0000000e <foo\+0xc> e3a00001 	mov	r0, #1	; 0x1
00000012 <foo\+0x10> e3a00001 	mov	r0, #1	; 0x1
00000016 <foo\+0x14> e3a00001 	mov	r0, #1	; 0x1
0000001a <foo\+0x18> e3a00002 	mov	r0, #2	; 0x2
0000001e <foo\+0x1c> e3a00002 	mov	r0, #2	; 0x2
00000022 <foo\+0x20> e3a00002 	mov	r0, #2	; 0x2
