	.syntax unified
	.text
	.thumb
.Lstart:
	bti
	pacbti	r11, lr, sp
	pacbti	r12, r10, sp
	pacbti	r12, lr, r10
