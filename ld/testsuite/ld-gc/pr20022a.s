	.data
	.globl bar
	.type bar,%object
bar:
	.dc.a	__start__foo
	.section _foo,"aw",%progbits
foo:
	.ascii "This is bar"
