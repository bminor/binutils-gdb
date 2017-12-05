	.data
	.globl	bar
	.type	bar, @object
bar:
	.dc.a	__global_pointer$
	.size	bar, . - bar
