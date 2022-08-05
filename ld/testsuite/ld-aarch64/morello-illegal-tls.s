	.global	a
	.section	.tbss,"awT",%nobits
a:
	.zero   65540
	.zero	52
	.size a,65592

# Compute the address of an integer within structure a, padded
# by an array of size 48

	.text
	.align	2
	.p2align 4,,11
	.global _start
	.type   _start,%function
_start:
# Should not allow a TLS relocation with an addend.
# This particular case is tricky because it can get relaxed to a non-TLS
# relocation against the data stubs (when in an executable).
# That means that the place where we check in binutils should be before this
# relaxation (which is not the usual place to check such things).
	adrp	c2, :gottprel:a+10
	add	c2, c2, :gottprel_lo12:a+10
	adrp	c0, :tlsdesc:a+10
	ldr	c1, [c0, #:tlsdesc_lo12:a+10]
	add	c0, c0, #:tlsdesc_lo12:a+10
	.tlsdesccall a+10
	blr	c1
	# N.b. the below (Local-Exec relocations with an addend) are accepted.
	# We check this by observing that we don't get an error message produced
	# for them.
	movz	x2, #:tprel_g1:a+10
	movk	x2, #:tprel_g0_nc:a+10

	movz    x2, #:tprel_g1:undefinedsym
