bar:
	adrp	c0, :tlsdesc:foo
	ldr	c1, [c0, #:tlsdesc_lo12:foo]
	.tlsdesccall foo
	blr c1
