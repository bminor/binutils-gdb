# Testcase with a variety of "change of flow instructions"
#
# Must be run with -W so it remains warning free.
#
# This test does not have much going on wrt synthesis of CFI;
# it just aims to ensure x8_64 -> ginsn decoding behaves
# gracefully for these "change of flow instructions"
	.text
	.globl  foo
	.type   foo, @function
foo:
	.cfi_startproc
	addq    %rdx, %rax
	loop    foo
	notrack jmp     *%rax
	call    *%r8
	jecxz   .L179
	jmp     *48(%rdi)
	jo      .L179
.L179:
	ret
	.cfi_endproc
.LFE0:
	.size   foo, .-foo
