	.global var
	.section .tdata,"awT",@progbits
var:
	.word 1
	.text
	.global	fn1
	.type	fn1,@function
fn1:

	# DESC will relax to LE.
	pcalau12i       $a0,%desc_pc_hi20(var)
	addi.d  $a0,$a0,%desc_pc_lo12(var)
	ld.d    $ra,$a0,%desc_ld(var)
	jirl    $ra,$ra,%desc_call(var)
