# split_aarch64_n.s: AArch64 specific, -fsplit-stack calling non-split

	.text

	.global	fnn
	.type	fnn,@function
fnn:
	ret

	.size	fnn,. - fnn

	.section	.note.GNU-stack,"",@progbits
