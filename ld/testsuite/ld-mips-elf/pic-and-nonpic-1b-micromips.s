	.abicalls
	.option	pic0
	.global	__start
	.type	__start, @function
	.set	micromips
	.ent	__start
__start:
	jal	f1
	jal	f2
	jal	f3
	.end	__start
