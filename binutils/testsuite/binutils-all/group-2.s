	.section .text.foo,"axG",%progbits,.text.foo,comdat
	.global foo
foo:
	.word 0
	.section .data.bar,"awG",%progbits,.text.foo,comdat
	.global bar
bar:
	.word 0
