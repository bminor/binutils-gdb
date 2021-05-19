	.syntax unified
	.text
	.thumb
.Lstart:
	bti
	pacbti	r11, lr, sp
	pacbti	r12, r10, sp
	pacbti	r12, lr, r10
	aut	r11, lr, sp
	aut	r12, r10, sp
	aut	r12, lr, r10
