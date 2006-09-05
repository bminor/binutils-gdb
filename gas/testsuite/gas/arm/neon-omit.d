# name: Neon optional register operands
# as: -mfpu=neon
# objdump: -dr --prefix-addresses --show-raw-insn

.*: +file format .*arm.*

Disassembly of section .text:
0[0-9a-f]+ <[^>]+> f3022746 	vabd\.u8	q1, q1, q3
0[0-9a-f]+ <[^>]+> f26cc0c6 	vhadd\.s32	q14, q14, q3
0[0-9a-f]+ <[^>]+> f2222144 	vrhadd\.s32	q1, q1, q2
0[0-9a-f]+ <[^>]+> f22aa24e 	vhsub\.s32	q5, q5, q7
0[0-9a-f]+ <[^>]+> f3166448 	vshl\.u16	q3, q3, q4
0[0-9a-f]+ <[^>]+> f32aa45c 	vqshl\.u32	q5, q5, q6
0[0-9a-f]+ <[^>]+> f20ee170 	vand	q7, q7, q8
0[0-9a-f]+ <[^>]+> f30ee170 	veor	q7, q7, q8
0[0-9a-f]+ <[^>]+> f3b5a14a 	vceq\.i16	q5, q5, #0
0[0-9a-f]+ <[^>]+> f31aa85a 	vceq\.i16	q5, q5, q5
0[0-9a-f]+ <[^>]+> f3b5a24a 	vclt\.s16	q5, q5, #0
0[0-9a-f]+ <[^>]+> f3b5a34c 	vabs\.s16	q5, q6
0[0-9a-f]+ <[^>]+> f3b57388 	vneg\.s16	d7, d8
0[0-9a-f]+ <[^>]+> f3b97708 	vabs\.f32	d7, d8
0[0-9a-f]+ <[^>]+> f3f927e4 	vneg\.f32	q9, q10
0[0-9a-f]+ <[^>]+> f2211a03 	vpmax\.s32	d1, d1, d3
0[0-9a-f]+ <[^>]+> f2255a17 	vpmin\.s32	d5, d5, d7
0[0-9a-f]+ <[^>]+> f3011f03 	vpmax\.f32	d1, d1, d3
0[0-9a-f]+ <[^>]+> f3255f07 	vpmin\.f32	d5, d5, d7
0[0-9a-f]+ <[^>]+> f2122b46 	vqdmulh\.s16	q1, q1, q3
0[0-9a-f]+ <[^>]+> f3255b07 	vqrdmulh\.s32	d5, d5, d7
0[0-9a-f]+ <[^>]+> f3922c6d 	vqdmulh\.s16	q1, q1, d5\[3\]
0[0-9a-f]+ <[^>]+> f2122056 	vqadd\.s16	q1, q1, q3
0[0-9a-f]+ <[^>]+> f2255017 	vqadd\.s32	d5, d5, d7
0[0-9a-f]+ <[^>]+> f2222944 	vmla\.i32	q1, q1, q2
0[0-9a-f]+ <[^>]+> f2133b14 	vpadd\.i16	d3, d3, d4
0[0-9a-f]+ <[^>]+> f3266948 	vmls\.i32	q3, q3, q4
0[0-9a-f]+ <[^>]+> f3022e54 	vacge\.f32	q1, q1, q2
0[0-9a-f]+ <[^>]+> f3266e58 	vacgt\.f32	q3, q3, q4
0[0-9a-f]+ <[^>]+> f30cae5a 	vacge\.f32	q5, q6, q5
0[0-9a-f]+ <[^>]+> f320eede 	vacgt\.f32	q7, q8, q7
0[0-9a-f]+ <[^>]+> f32ee370 	vcge\.u32	q7, q7, q8
0[0-9a-f]+ <[^>]+> f32ee360 	vcgt\.u32	q7, q7, q8
0[0-9a-f]+ <[^>]+> f320e3de 	vcge\.u32	q7, q8, q7
0[0-9a-f]+ <[^>]+> f320e3ce 	vcgt\.u32	q7, q8, q7
0[0-9a-f]+ <[^>]+> f3a22102 	vaddw\.u32	q1, q1, d2
0[0-9a-f]+ <[^>]+> f2a66304 	vsubw\.s32	q3, q3, d4
0[0-9a-f]+ <[^>]+> f2244856 	vtst\.32	q2, q2, q3
0[0-9a-f]+ <[^>]+> f2011f12 	vrecps\.f32	d1, d1, d2
0[0-9a-f]+ <[^>]+> f29c2052 	vshr\.s16	q1, q1, #4
0[0-9a-f]+ <[^>]+> f28b4254 	vrshr\.s8	q2, q2, #5
0[0-9a-f]+ <[^>]+> f39a6156 	vsra\.u16	q3, q3, #6
0[0-9a-f]+ <[^>]+> f39a8358 	vrsra\.u16	q4, q4, #6
0[0-9a-f]+ <[^>]+> f3954554 	vsli\.16	q2, q2, #5
0[0-9a-f]+ <[^>]+> f3bff69f 	vqshlu\.s64	d15, d15, #63
0[0-9a-f]+ <[^>]+> f2b55306 	vext\.8	d5, d5, d6, #3
