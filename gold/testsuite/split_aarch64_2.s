# split_aarch64_2.s: Split aware function calling non-split.

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
	b       fnn
	.size	fn1,. - fn1

	.global	fn2
	.type	fn2,@function
fn2:
	mrs     x9, tpidr_el0
	mov     x10, 528
	movk    x10, 0x0, lsl 16
	sub     x10, sp, x10
	adrp    x12, .L6
	add     x12, x12, :lo12:.L6
	ldr     x9, [x9, -8]
	cmp     x9, x10
	blt     .Lbcond8
	b       __morestack
.Lbcond8:
.L6:
	bl      fnn
	ret
	.size	fn2,. - fn2

	.global fn3
	.type	fn3,@function
fn3:
        mrs     x9, tpidr_el0
        mov     x10, 272
        movk    x10, 0x1, lsl 16
        sub     x10, sp, x10
        adrp    x12, .L10
        add     x12, x12, :lo12:.L10
        ldr     x9, [x9, -8]
        cmp     x9, x10
        blt     .Lbcond12
        b       __morestack
.Lbcond12:
.L10:
        bl      fnn
        ret
        .size   fn3, .-fn3

	.global fn4
	.type	fn4,@function
fn4:
        mrs     x9, tpidr_el0
        mov     x10, 65520
        movk    x10, 0x1, lsl 16
        sub     x10, sp, x10
        adrp    x12, .L14
        add     x12, x12, :lo12:.L14
        ldr     x9, [x9, -8]
        cmp     x9, x10
        blt     .Lbcond16
        b       __morestack
.Lbcond16:
.L14:
        bl      fnn
        ret
        .size   fn4, .-fn4

	.section	.note.GNU-stack,"",@progbits
	.section	.note.GNU-split-stack,"",@progbits
