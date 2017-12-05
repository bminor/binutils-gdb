	.data
	.globl	foo
	.type	foo, @object
foo:
	.dc.a	bar
	.dc.a	__global_pointer$
	.size	foo, . - foo
