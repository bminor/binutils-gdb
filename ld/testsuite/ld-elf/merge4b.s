/* Check that pointing into padding zeros of string sections works. */
	.section	.rodata.str1.1,"aMS",@progbits,1
	.string ""
.LC0:
	.string	"foobar"
1:
	.string ""
	.string "whatever"
	.section .data
	.globl addr_of_str
	.type addr_of_str, @object
addr_of_str:
	.dc.a str
	.globl addr_of_str2
	.type addr_of_str2, @object
addr_of_str2:
	.dc.a 1b
	.section	.rodata
	.type	str, @object
	.size	str, 7
str:
	.string	"foobar"
	.section	.note.GNU-stack,"",@progbits
