	.section .text,"axG",%progbits,foo,comdat
	.global foo
foo:
	.word 0
	.section .data,"awG",%progbits,foo,comdat
	.global bar
bar:
	.word 0
