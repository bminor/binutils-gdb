	.section .text,"axG",%progbits,foo_group,comdat
	.secondary foo
	.global bar
foo:
	.byte 0
bar:
	.byte 0
	.data
	.dc.a foo
