	.section .text.foo,"ax",%progbits
	.globl	foo
	.type	foo, %function
foo:
	.byte 0
	.section .text.opt_out,"ax",%progbits
	.type	opt_out, %function
opt_out:
	.dc.a bar
