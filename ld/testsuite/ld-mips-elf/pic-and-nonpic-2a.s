	.abicalls
	.option	pic0
	.global	__start
	.type	__start, @function
	.ent	__start
__start:
	jal	foo
	.end	__start
