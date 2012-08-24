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
0[0-9a-f]+ <[^>]+> fe800a00 	vmaxnm.f32	s0, s0, s0
0[0-9a-f]+ <[^>]+> fec00aa0 	vmaxnm.f32	s1, s1, s1
0[0-9a-f]+ <[^>]+> fe8ffa0f 	vmaxnm.f32	s30, s30, s30
0[0-9a-f]+ <[^>]+> fecffaaf 	vmaxnm.f32	s31, s31, s31
0[0-9a-f]+ <[^>]+> fe800b00 	vmaxnm.f64	d0, d0, d0
0[0-9a-f]+ <[^>]+> fec00ba0 	vmaxnm.f64	d16, d16, d16
0[0-9a-f]+ <[^>]+> fe8ffb0f 	vmaxnm.f64	d15, d15, d15
0[0-9a-f]+ <[^>]+> fecffbaf 	vmaxnm.f64	d31, d31, d31
0[0-9a-f]+ <[^>]+> fe800a40 	vminnm.f32	s0, s0, s0
0[0-9a-f]+ <[^>]+> fec00ae0 	vminnm.f32	s1, s1, s1
0[0-9a-f]+ <[^>]+> fe8ffa4f 	vminnm.f32	s30, s30, s30
0[0-9a-f]+ <[^>]+> fecffaef 	vminnm.f32	s31, s31, s31
0[0-9a-f]+ <[^>]+> fe800b40 	vminnm.f64	d0, d0, d0
0[0-9a-f]+ <[^>]+> fec00be0 	vminnm.f64	d16, d16, d16
0[0-9a-f]+ <[^>]+> fe8ffb4f 	vminnm.f64	d15, d15, d15
0[0-9a-f]+ <[^>]+> fecffbef 	vminnm.f64	d31, d31, d31
0[0-9a-f]+ <[^>]+> febc0ac0 	vcvta.s32.f32	s0, s0
0[0-9a-f]+ <[^>]+> fefd0ae0 	vcvtn.s32.f32	s1, s1
0[0-9a-f]+ <[^>]+> febefa4f 	vcvtp.u32.f32	s30, s30
0[0-9a-f]+ <[^>]+> fefffa6f 	vcvtm.u32.f32	s31, s31
0[0-9a-f]+ <[^>]+> febc0b40 	vcvta.u32.f64	s0, d0
0[0-9a-f]+ <[^>]+> fefd0b60 	vcvtn.u32.f64	s1, d16
0[0-9a-f]+ <[^>]+> febefb4f 	vcvtp.u32.f64	s30, d15
0[0-9a-f]+ <[^>]+> fefffb6f 	vcvtm.u32.f64	s31, d31
0[0-9a-f]+ <[^>]+> fe00 0a00 	vseleq.f32	s0, s0, s0
0[0-9a-f]+ <[^>]+> fe50 0aa0 	vselvs.f32	s1, s1, s1
0[0-9a-f]+ <[^>]+> fe2f fa0f 	vselge.f32	s30, s30, s30
0[0-9a-f]+ <[^>]+> fe7f faaf 	vselgt.f32	s31, s31, s31
0[0-9a-f]+ <[^>]+> fe00 0b00 	vseleq.f64	d0, d0, d0
0[0-9a-f]+ <[^>]+> fe50 0ba0 	vselvs.f64	d16, d16, d16
0[0-9a-f]+ <[^>]+> fe2f fb0f 	vselge.f64	d15, d15, d15
0[0-9a-f]+ <[^>]+> fe7f fbaf 	vselgt.f64	d31, d31, d31
0[0-9a-f]+ <[^>]+> fe80 0a00 	vmaxnm.f32	s0, s0, s0
0[0-9a-f]+ <[^>]+> fec0 0aa0 	vmaxnm.f32	s1, s1, s1
0[0-9a-f]+ <[^>]+> fe8f fa0f 	vmaxnm.f32	s30, s30, s30
0[0-9a-f]+ <[^>]+> fecf faaf 	vmaxnm.f32	s31, s31, s31
0[0-9a-f]+ <[^>]+> fe80 0b00 	vmaxnm.f64	d0, d0, d0
0[0-9a-f]+ <[^>]+> fec0 0ba0 	vmaxnm.f64	d16, d16, d16
0[0-9a-f]+ <[^>]+> fe8f fb0f 	vmaxnm.f64	d15, d15, d15
0[0-9a-f]+ <[^>]+> fecf fbaf 	vmaxnm.f64	d31, d31, d31
0[0-9a-f]+ <[^>]+> fe80 0a40 	vminnm.f32	s0, s0, s0
0[0-9a-f]+ <[^>]+> fec0 0ae0 	vminnm.f32	s1, s1, s1
0[0-9a-f]+ <[^>]+> fe8f fa4f 	vminnm.f32	s30, s30, s30
0[0-9a-f]+ <[^>]+> fecf faef 	vminnm.f32	s31, s31, s31
0[0-9a-f]+ <[^>]+> fe80 0b40 	vminnm.f64	d0, d0, d0
0[0-9a-f]+ <[^>]+> fec0 0be0 	vminnm.f64	d16, d16, d16
0[0-9a-f]+ <[^>]+> fe8f fb4f 	vminnm.f64	d15, d15, d15
0[0-9a-f]+ <[^>]+> fecf fbef 	vminnm.f64	d31, d31, d31
0[0-9a-f]+ <[^>]+> febc 0ac0 	vcvta.s32.f32	s0, s0
0[0-9a-f]+ <[^>]+> fefd 0ae0 	vcvtn.s32.f32	s1, s1
0[0-9a-f]+ <[^>]+> febe fa4f 	vcvtp.u32.f32	s30, s30
0[0-9a-f]+ <[^>]+> feff fa6f 	vcvtm.u32.f32	s31, s31
0[0-9a-f]+ <[^>]+> febc 0b40 	vcvta.u32.f64	s0, d0
0[0-9a-f]+ <[^>]+> fefd 0b60 	vcvtn.u32.f64	s1, d16
0[0-9a-f]+ <[^>]+> febe fb4f 	vcvtp.u32.f64	s30, d15
0[0-9a-f]+ <[^>]+> feff fb6f 	vcvtm.u32.f64	s31, d31
