	.section .text,"axG",%progbits,foo,comdat
foo:
	.word 0
	.section .data,"awG",%progbits,foo,comdat
bar:
	.word 0
