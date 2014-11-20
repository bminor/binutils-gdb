# relocs against undefined weak symbols should not be treated as
# overflowing


	.globl	start
	.weak	foo
start:
	.set mips64r6
	beqzc	$2, foo
	bnezc	$2, foo
	lwpc	$2, foo
	ldpc	$2, foo
	bc	foo
	lui	$4, %pcrel_hi(foo)
	addiu	$4, $4, %pcrel_lo(foo)

	.set mips32r2
	b      foo
	bal    foo
	lui    $4, %gp_rel(foo)

	.set micromips
	beqz16 $4, foo
	b16    foo
	b      foo
