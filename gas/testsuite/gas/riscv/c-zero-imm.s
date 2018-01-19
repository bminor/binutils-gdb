	.option rvc
	# These are valid instructions.
	li a0,0
	c.li a1,0
	andi a2,a2,0
	c.andi a3,0
	# Don't let this compress to a hint.
	addi a4,a4,0
	# These are hints.
	c.addi a5,0
