	.syntax unified	
	.arch	armv5te
	.text
	.align	2
	.global	answer
	.type	answer, %function
answer:
	.fnstart
	mov	r0, #42
	bx	lr
	.fnend
	.size	answer, .-answer
