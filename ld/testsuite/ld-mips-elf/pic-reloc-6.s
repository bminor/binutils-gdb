	.text
	.globl	foo
	.ent	foo
foo:
$LCL:
	li	$2, %hi($LCL)
	addiu	$2, %lo($LCL)
	li	$2, %hi(bar)
	addiu	$2, %lo(bar)
	.end	foo
