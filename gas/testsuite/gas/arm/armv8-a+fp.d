#name: Valid v8-a+fp
#objdump: -dr --prefix-addresses --show-raw-insn

.*: +file format .*arm.*

Disassembly of section .text:
0[0-9a-f]+ <[^>]+> fe000a00 	vseleq.f32	s0, s0, s0
0[0-9a-f]+ <[^>]+> fe500aa0 	vselvs.f32	s1, s1, s1
0[0-9a-f]+ <[^>]+> fe2ffa0f 	vselge.f32	s30, s30, s30
0[0-9a-f]+ <[^>]+> fe7ffaaf 	vselgt.f32	s31, s31, s31
0[0-9a-f]+ <[^>]+> fe000b00 	vseleq.f64	d0, d0, d0
0[0-9a-f]+ <[^>]+> fe500ba0 	vselvs.f64	d16, d16, d16
0[0-9a-f]+ <[^>]+> fe2ffb0f 	vselge.f64	d15, d15, d15
0[0-9a-f]+ <[^>]+> fe7ffbaf 	vselgt.f64	d31, d31, d31
0[0-9a-f]+ <[^>]+> fe00 0a00 	vseleq.f32	s0, s0, s0
0[0-9a-f]+ <[^>]+> fe50 0aa0 	vselvs.f32	s1, s1, s1
0[0-9a-f]+ <[^>]+> fe2f fa0f 	vselge.f32	s30, s30, s30
0[0-9a-f]+ <[^>]+> fe7f faaf 	vselgt.f32	s31, s31, s31
0[0-9a-f]+ <[^>]+> fe00 0b00 	vseleq.f64	d0, d0, d0
0[0-9a-f]+ <[^>]+> fe50 0ba0 	vselvs.f64	d16, d16, d16
0[0-9a-f]+ <[^>]+> fe2f fb0f 	vselge.f64	d15, d15, d15
0[0-9a-f]+ <[^>]+> fe7f fbaf 	vselgt.f64	d31, d31, d31
