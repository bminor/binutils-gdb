	.text
	.weak _start
	.type _start, "function"
_start:
	.byte 0
	.byte 0
	.size _start, 2

	.weak foo
	.type foo, "function"
foo:
	.byte 0
	.byte 0
	.size foo, 2
