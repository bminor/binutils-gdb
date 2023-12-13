	.data
	.section	.tdata,"awT",@progbits
var:
	.word 1
	.text
	.global	fn1
	.type	gn1,@function
fn1:
	# expect IE to relax LE.
	pcalau12i       $a0,%ie_pc_hi20(var)
	ld.d    	$a0,$a0,%ie_pc_lo12(var)
