	@ Three-argument forms of Thumb arithmetic instructions.
	.text
	.global m
	.thumb_func
m:
	adc	r0,r0,r1
	and	r0,r0,r1
	bic	r0,r0,r1
	eor	r0,r0,r1
	mul	r0,r1,r0
	orr	r0,r0,r1
	sbc	r0,r0,r1
	nop
