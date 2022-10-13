	.text
	.p2align 4,,11
#APP
	.type foo, %gnu_indirect_function
#NO_APP
	.global foo
	.type	foo, @function
foo:
	ret
	.size	foo, .-foo

	.global _start
	.type   _start,%function
_start:
	bl sharedfoo
	ret
	.size _start, .-_start
	.data
	.align	2
	.type	var, %object
	.size	var, 4
var:
	.chericap sharedfoo
