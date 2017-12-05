	.data
	.globl	blah
	.type	blah, @object
blah:
	.dc.a	foo
	.dc.a	__global_pointer$
	.size	blah, . - blah
