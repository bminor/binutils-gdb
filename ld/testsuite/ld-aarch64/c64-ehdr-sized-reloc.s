	.data
	.global val
val:
	.chericap __ehdr_start
	.size val, .-val

	.align 4
	.text
	.global _start
_start:
	ldr	c0, [c0, :got_lo12:val]
