
	.type foo, %function
	.set foo, 0x100000

arm:	bl	0x100000
	b	0x100000
	bl	foo

