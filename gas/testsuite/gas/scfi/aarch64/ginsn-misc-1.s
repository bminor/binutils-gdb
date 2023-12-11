# Testcase for a variety of misc instructions
# Ensure graceful handling, irrespective of ginsn generation
symbol:
        .string "w"

	.type   foo, %function
foo:
	mov     x16, 4384
	add     sp, sp, x16
        adrp    x0, symbol
	add     sp, x0, :lo12:symbol
	sub     sp, sp, 3
	add     sp, sp, 3
	ret
	.size   foo,.-foo
