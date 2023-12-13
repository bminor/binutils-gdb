	.global v1
	.section .tdata,"awT",@progbits
v1:
	.word 1
	.text
	.global	fn1
	.type	fn1,@function
fn1:

	# Use DESC and IE to access the same symbol,
	# DESC will relax to IE.
	pcalau12i       $a0,%desc_pc_hi20(var)
	addi.d  $a0,$a0,%desc_pc_lo12(var)
	ld.d    $ra,$a0,%desc_ld(var)
	jirl    $ra,$ra,%desc_call(var)

	pcalau12i       $a0,%ie_pc_hi20(var)
	ld.d		$ra,$a0,%ie_pc_lo12(var)
