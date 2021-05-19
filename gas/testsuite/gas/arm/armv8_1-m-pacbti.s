	.syntax unified
	.text
	.thumb
.Lstart:
	bti
	pacbti	r12, lr, sp
	aut	r12, lr, sp
	pac	r12, lr, sp
	bxaut	r3, r4, r5
