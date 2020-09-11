	.ifdef C64MODE
		REG .req c0
	.else
		REG .req x0
	.endif

	.type foo, %gnu_indirect_function
	.set __GI_foo, foo
	.text
	.type	foo, @function
foo:
	ret
	.size	foo, .-foo
.globl bar
	.type	bar, @function
bar:
	bl	__GI_foo
        adrp    REG, __GI_foo
        add     REG, REG, :lo12:__GI_foo
	ret
	.size	bar, .-bar
