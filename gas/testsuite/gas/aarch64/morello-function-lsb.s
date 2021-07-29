        .text
        .globl  f
        .p2align        2
        .type   f,@function
f:
altlabel:       // This label does not have function type, so will not have the LSB set.
	stp	c29, c30, [csp, #-32]!
	mov	c29, csp
        b       .LBB0_1
.Ltmp0:
.LBB0_1:
	ldp	c29, c30, [csp], #32
	ret	c30
.Lfunc_end0:
	// Check that the LSB is not included in the offset when the symbol `f`
	// ends up in the relocation.  In that case it's the linkers
	// responsibility to account for the LSB.
        .word   f - operand
	// Check that if we emit a relocation that does not include the symbol
	// `f` we account for the LSB.
        .word   operand - f
	// Use `.word 1` markers so is easier to tell what's going on by a
	// human.
	.word   1
	// Ensure we account for the LSB in other valid expressions.
	// In the output test we require these to evaluate to true or false,
	// and for true to be represented as 0xffffffff
	.word	altlabel != f
	.word   altlabel >= f
	.word   altlabel <= f
	.word	altlabel == f
	.word   altlabel > f
	.word   altlabel < f
	.word	f != altlabel
	.word   f >= altlabel
	.word   f <= altlabel
	.word	f == altlabel
	.word   f > altlabel
	.word   f < altlabel
	.word	f != f
	.word   f >= f
	.word   f <= f
	.word	f == f
	.word   f > f
	.word   f < f
	.word	altlabel != altlabel
	.word   altlabel >= altlabel
	.word   altlabel <= altlabel
	.word	altlabel == altlabel
	.word   altlabel > altlabel
	.word   altlabel < altlabel
        .word   1
	// Ensure we account for the LSB in the standard .size directive.
        .size   f, .Lfunc_end0-f

        .type   f.p,@object
        .data
        .p2align        4
f.p:
	// This is a standard directive form. We need to ensure accounts for
	// the LSB in the subexpression  ((.Ltmp0+1)-f), since otherwise the
	// resulting relocation is invalid.
        .capinit f+((.Ltmp0+1)-f)
        .xword  0
        .xword  0
        .size   f.p, 16
.set operand, 100
