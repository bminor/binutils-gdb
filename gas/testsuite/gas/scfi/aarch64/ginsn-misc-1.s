# Testcase for a variety of misc instructions
# Ensure graceful handling, irrespective of ginsn generation
symbol:
	.string "w"

	.type   foo, %function
foo:
	adrp    x0, symbol
	add     sp, x0, :lo12:symbol
	movk    x0, 0x1, lsl 16
	prfm    PLDL1STRM, [sp]
	ret
	.size   foo,.-foo
