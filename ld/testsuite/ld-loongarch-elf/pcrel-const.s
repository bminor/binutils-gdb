	.text
	.align	2
	.globl	foo
	.type	foo, @function
foo:
	nop
	la.global	$r4,__sec_size
	ldptr.w	$r4,$r4,0
	jr	$r1
	.size	foo, .-foo
	.data
	.word 1
