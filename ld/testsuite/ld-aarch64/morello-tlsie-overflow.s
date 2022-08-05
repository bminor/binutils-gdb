	.text

	.align	2
	.p2align 4,,11
	.global _start
	.type   _start,%function
_start:
	adrp	c2, :gottprel:w1
	add	c2, c2, :gottprel_lo12:w1
	ldp	x2, x1, [c2]
	# Add some padding in the text section to ensure that the GOT section
	# is too far away for the gottprel relocation above.  Out of interest,
	# we can't add this padding in the .got section since the order of the
	# link determines that the GOT we add for relocations will be before
	# the GOT added in the file.
	.zero 0x80000000
