@ test omitted optional arguments

	.text
	.arm
	.syntax unified

	vabd.u8 q1,q3
	vhadd.s32 q14, q3
	vrhadd.s32 q1,q2
	vhsub.s32 q5,q7
	vshl.u16 q3,q4
	vqshl.u32 q5,q6
	vand.64 q7,q8
	veor.64 q7,q8
	vceq.i16 q5,#0
	vceq.i16 q5,q5
	vclt.s16 q5,#0
	vabs.s16 q5,q6
	vneg.s16 d7,d8
	vabs.f d7,d8
	vneg.f q9,q10
	vpmax.s32 d1,d3
	vpmin.s32 d5,d7
	vpmax.f32 d1,d3
	vpmin.f32 d5,d7
	vqdmulh.s16 q1,q3
	vqrdmulh.s32 d5,d7
	vqdmulh.s16 q1,d5[3]
	vqadd.s16 q1,q3
	vqadd.s32 d5,d7
	vmla.i32 q1,q2
	vpadd.i16 d3,d4
	vmls.s32 q3,q4
	vacge.f q1,q2
	vacgt.f q3,q4
	vacle.f q5,q6
	vaclt.f q7,q8
	vcge.u32 q7,q8
	vcgt.u32 q7,q8
	vcle.u32 q7,q8
	vclt.u32 q7,q8
	vaddw.u32 q1,d2
	vsubw.s32 q3,d4
	vtst.i32 q2,q3
	vrecps.f d1,d2
	vshr.s16 q1,#4
        vrshr.s8 q2,#5
	vsra.u16 q3,#6
        vrsra.u16 q4,#6
	vsli.16 q2,#5
	vqshlu.s64 d15,#63
	vext.8 d5,d6,#3
