	.global	p
	.global	a
	.section	.tbss,"awT",%nobits
p:
	.zero   65540
a:
	.zero   65540
	.zero	52
	.size a,65592
b:
	.zero	100
	.size b,100
	.text

# Compute the address of an integer within structure a, padded
# by an array of size 48

	.align	2
	.p2align 4,,11
	.global _start
	.type   _start,%function
_start:
	movz	x2, #:tprel_g1:a
	movk	x2, #:tprel_g0_nc:a
	movz	x1, #:size_g1:a
	movk	x1, #:size_g0_nc:a

	movz	x2, #:tprel_g1:b
	movk	x2, #:tprel_g0_nc:b
	movz	x1, #:size_g1:b
	movk	x1, #:size_g0_nc:b

	mrs	c0, ctpidr_el0
	add	c0, c0, x2
	scbnds	c0, c0, x1
	ldr	w0, [c0]
	ret
