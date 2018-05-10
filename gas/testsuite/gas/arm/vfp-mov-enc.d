# name: VFP check vmov supports integer immediates
# as: -mcpu=cortex-a8 -mfpu=vfpv3
# objdump: -dr --prefix-addresses --show-raw-insn

.*: +file format .*arm.*

Disassembly of section .text:
0[0-9a-f]+ <[^>]+> 4ef2da06 	vmovmi.f32	s27, #38	; 0x41300000  11.0
0[0-9a-f]+ <[^>]+> 4ef2da06 	vmovmi.f32	s27, #38	; 0x41300000  11.0
0[0-9a-f]+ <[^>]+> 4ef7da00 	vmovmi.f32	s27, #112	; 0x3f800000  1.0
0[0-9a-f]+ <[^>]+> 4ef7da00 	vmovmi.f32	s27, #112	; 0x3f800000  1.0
0[0-9a-f]+ <[^>]+> cebb1b04 	vmovgt.f64	d1, #180	; 0xc1a00000 -20.0
0[0-9a-f]+ <[^>]+> ceb81b00 	vmovgt.f64	d1, #128	; 0xc0000000 -2.0
