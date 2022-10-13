	.text
	.p2align 4,,11
#APP
	.type sharedfoo, %gnu_indirect_function
#NO_APP
	.global sharedfoo
	.type	sharedfoo, @function
sharedfoo:
	ret
	.size	sharedfoo, .-sharedfoo

	.global libfunc
	.type   libfunc,%function
libfunc:
	bl foo
	ret
	.size libfunc, .-libfunc
	.data
	.align	2
	.type	var, %object
	.size	var, 4
var:
	.chericap foo
