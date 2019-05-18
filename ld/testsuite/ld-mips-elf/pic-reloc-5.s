	.text
	.globl	foo
	.ent	foo
foo:
$LCL:
	lui	$2, %hi($LCL)
	addiu	$2, %lo($LCL)
	lui	$2, %hi(bar)
	addiu	$2, %lo(bar)
	.end	foo
