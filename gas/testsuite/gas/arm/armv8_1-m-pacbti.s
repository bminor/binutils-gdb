	.syntax unified
	.text
	.thumb
.Lstart:
	bti
	pacbti	r12, lr, sp
	aut	r12, lr, sp
