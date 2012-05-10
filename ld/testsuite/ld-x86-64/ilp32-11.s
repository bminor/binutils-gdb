	.section	.data.rel.local,"aw",@progbits
	.align 8
.Ljmp:
	.quad	func + 0x7fffffff

	.text
	.type	func, @function
func:
	ret
