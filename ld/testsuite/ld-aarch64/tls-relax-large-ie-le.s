	.section	.tdata,"awT"
x:
	.word	2
y:
	.word	4

	.text
test:
	mrs	x1, tpidr_el0

	movz	x6, #:gottprel_g1:x
	movk	x6, #:gottprel_g0_nc:x
	ldr	x6, [x2, x6]
	add	x6, x6, x1

	movz	x7, #:gottprel_g1:y
	movk	x7, #:gottprel_g0_nc:y
	ldr	x7, [x2, x7]
	add	x7, x7, x1
