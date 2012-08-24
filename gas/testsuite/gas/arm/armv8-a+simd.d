#name: Valid v8-a+simdv3
#objdump: -dr --prefix-addresses --show-raw-insn

.*: +file format .*arm.*

Disassembly of section .text:
0[0-9a-f]+ <[^>]+> f3000f10 	vmaxnm.f32	d0, d0, d0
0[0-9a-f]+ <[^>]+> f3400fb0 	vmaxnm.f32	d16, d16, d16
0[0-9a-f]+ <[^>]+> f30fff1f 	vmaxnm.f32	d15, d15, d15
0[0-9a-f]+ <[^>]+> f34fffbf 	vmaxnm.f32	d31, d31, d31
0[0-9a-f]+ <[^>]+> f3000f50 	vmaxnm.f32	q0, q0, q0
0[0-9a-f]+ <[^>]+> f3400ff0 	vmaxnm.f32	q8, q8, q8
0[0-9a-f]+ <[^>]+> f30eef5e 	vmaxnm.f32	q7, q7, q7
0[0-9a-f]+ <[^>]+> f34eeffe 	vmaxnm.f32	q15, q15, q15
0[0-9a-f]+ <[^>]+> f3200f10 	vminnm.f32	d0, d0, d0
0[0-9a-f]+ <[^>]+> f3600fb0 	vminnm.f32	d16, d16, d16
0[0-9a-f]+ <[^>]+> f32fff1f 	vminnm.f32	d15, d15, d15
0[0-9a-f]+ <[^>]+> f36fffbf 	vminnm.f32	d31, d31, d31
0[0-9a-f]+ <[^>]+> f3200f50 	vminnm.f32	q0, q0, q0
0[0-9a-f]+ <[^>]+> f3600ff0 	vminnm.f32	q8, q8, q8
0[0-9a-f]+ <[^>]+> f32eef5e 	vminnm.f32	q7, q7, q7
0[0-9a-f]+ <[^>]+> f36eeffe 	vminnm.f32	q15, q15, q15
0[0-9a-f]+ <[^>]+> ff00 0f10 	vmaxnm.f32	d0, d0, d0
0[0-9a-f]+ <[^>]+> ff40 0fb0 	vmaxnm.f32	d16, d16, d16
0[0-9a-f]+ <[^>]+> ff0f ff1f 	vmaxnm.f32	d15, d15, d15
0[0-9a-f]+ <[^>]+> ff4f ffbf 	vmaxnm.f32	d31, d31, d31
0[0-9a-f]+ <[^>]+> ff00 0f50 	vmaxnm.f32	q0, q0, q0
0[0-9a-f]+ <[^>]+> ff40 0ff0 	vmaxnm.f32	q8, q8, q8
0[0-9a-f]+ <[^>]+> ff0e ef5e 	vmaxnm.f32	q7, q7, q7
0[0-9a-f]+ <[^>]+> ff4e effe 	vmaxnm.f32	q15, q15, q15
0[0-9a-f]+ <[^>]+> ff20 0f10 	vminnm.f32	d0, d0, d0
0[0-9a-f]+ <[^>]+> ff60 0fb0 	vminnm.f32	d16, d16, d16
0[0-9a-f]+ <[^>]+> ff2f ff1f 	vminnm.f32	d15, d15, d15
0[0-9a-f]+ <[^>]+> ff6f ffbf 	vminnm.f32	d31, d31, d31
0[0-9a-f]+ <[^>]+> ff20 0f50 	vminnm.f32	q0, q0, q0
0[0-9a-f]+ <[^>]+> ff60 0ff0 	vminnm.f32	q8, q8, q8
0[0-9a-f]+ <[^>]+> ff2e ef5e 	vminnm.f32	q7, q7, q7
0[0-9a-f]+ <[^>]+> ff6e effe 	vminnm.f32	q15, q15, q15
