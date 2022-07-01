	.arch morello+crc+c64
	.text
	.p2align 4,,11
	.global	_start
	.type	_start, %function
_start:
	.cfi_startproc purecap
	ret
	.cfi_endproc
	.size	_start, .-_start
	.global	p
	.section	.data.rel,"aw"
	.align	4
	.type	p, %object
	.size	p, 16
p:
	.chericap	memcpy

