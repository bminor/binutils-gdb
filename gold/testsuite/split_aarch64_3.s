# split_aarch64_3.s: Missing initial thread-pointer get instruction.

	.text

	.global	fn1
	.type	fn1,@function
fn1:
	mov     x10, 0
	movk    x10, 0x0, lsl 16
	sub     x10, sp, x10
	adrp    x12, .L2
	add     x12, x12, :lo12:.L2
	ldr     x9, [x9, -8]
	cmp     x9, x10
	blt     .Lbcond4
	b       __morestack
.Lbcond4:
.L2:
	b       fnn

	.size	fn1,. - fn1

	.section	.note.GNU-stack,"",@progbits
	.section	.note.GNU-split-stack,"",@progbits
