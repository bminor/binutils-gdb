	.section .foo,"ax",@progbits
	.global foo
foo:
	.word 0x12345678

	.section .bar,"ax",@progbits
	.global bar
bar:
	.word 0x87654321

	.section .overlay1,"ax",@progbits;
	.global jim
jim:
	.word 0xdeadbeef

	.section .overlay2,"ax",@progbits
	.global harry
harry:
	.word 0x99119911

	