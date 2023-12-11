# Testcase for a variety of misc instructions
# Ensure graceful handling, irrespective of ginsn generation
symbol:
	.string "w"

	.type   foo, %function
foo:
	adrp    x0, symbol
	add     sp, x0, :lo12:symbol
	ret
	.size   foo,.-foo
