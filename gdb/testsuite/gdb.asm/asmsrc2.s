; Second file in assembly source debugging testcase.

	.global foo2
foo2:
	st r13,@-sp

; Call someplace else

	bl foo3

; All done, return.

	ld r13,@sp+
	jmp r13
