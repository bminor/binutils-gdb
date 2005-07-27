	.text
	.global _start
	.type _start, "function"
_start:
	.byte 0
	.size _start, 1

	.weak foo
	.type foo, "function"
foo:
	.byte 0
	.size foo, 1
