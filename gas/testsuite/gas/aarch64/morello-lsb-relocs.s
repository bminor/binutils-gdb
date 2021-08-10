        .text
	// Just to make it a bit clearer to humans reading the tests that we
	// use the address of `f` and `altlabel` in some places.  Otherwise
	// when reading the disassembly you just see `0` and it's not 100%
	// clear that we're using the address vs using a dummy value for later.
	.zero 12
        .p2align        2
        .globl  f
        .type   f,@function
        .type   a,@function
f:		// This function is global, so relocations will remain in the
		// object file.
a:		// This function is local, so the relocation on the ADR
		// instruction will be relaxed to a value.
altlabel:	// This label does not have function type, so will not have the
		// LSB set.
	b	x
	b	f
	b	a
	b	altlabel
	b.le	x
	b.le	f
	b.le	a
	b.le	altlabel
	tbz	x1, 1, x
	tbz	x1, 1, f
	tbz	x1, 1, a
	tbz	x1, 1, altlabel
	bl	x
	bl	f
	bl	a
	bl	altlabel
