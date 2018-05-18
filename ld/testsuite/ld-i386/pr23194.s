	.text
	.symver foo,foo@FOO
	.globl	foo
	.type	foo, @function
foo:
	ret
	.size	foo, .-foo
	.globl	bar
	.type	bar, @function
bar:
	jmp	*foo@GOT(%eax)
	.size	bar, .-bar
