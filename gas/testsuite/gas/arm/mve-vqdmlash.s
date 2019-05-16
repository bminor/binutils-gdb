.syntax unified
.thumb

.irp data, s8, u8, s16, u16, s32, u32
.irp op1, q0, q1, q2, q4, q7
.irp op2, q0, q1, q2, q4, q7
.irp op3, r0, r1, r2, r4, r7, r8, r10, r12, r14
vqdmlash.\data \op1, \op2, \op3
vqrdmlash.\data \op1, \op2, \op3
.endr
.endr
.endr
.endr
vpstete
vqdmlasht.s8 q0, q1, r2
vqdmlashe.u16 q7, q7, r14
vqrdmlasht.s32 q0, q0, r0
vqrdmlashe.u8 q7, q7, r14
