	.text
	.align 0
label:	
	bxj     r0
	bxj	r1
	bxj	r14
	bxjeq	r0
	bxjmi	r0
	bxjpl	r7
	
	# Add two nop instructions to ensure that the output
	# is aligned as will automatically be done for arm-aout.
	nop
	nop
