# split_aarch64_1.s: Split aware function calling each other, no need to
# adjust stack or change the morestack symbol call.

	.text

	.global	fn1
	.type	fn1,@function
fn1:
	mrs     x9, tpidr_el0
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
	b       fn2
	.size	fn1,. - fn1

	.global	fn2
	.type	fn2,@function
fn2:
	mrs     x9, tpidr_el0
	mov     x10, 528
	movk    x10, 0x0, lsl 16
	sub     x10, sp, x10
	adrp    x12, .L3
	add     x12, x12, :lo12:.L3
	ldr     x9, [x9, -8]
	cmp     x9, x10
	blt     .Lbcond5
	b       __morestack
.Lbcond5:
.L3:
	bl      fn1
	ret

	.size	fn2,. - fn2

	.section	.note.GNU-stack,"",@progbits
	.section	.note.GNU-split-stack,"",@progbits
