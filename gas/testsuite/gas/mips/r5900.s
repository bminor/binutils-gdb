	.text

stuff:
	.ent stuff
	/* Integer instructions.  */

	/* Coprocessor 0 instructions, minus standard ISA 3 ones.
	   That leaves just the performance monitoring registers.  */

	di
	ei
	mfbpc	$10
	mfdab	$11
	mfdabm	$12
	mfdvb	$13
	mfdvbm	$14
	mfiab	$15
	mfiabm	$16
	mfpc	$4,$1
	mfps	$4,$1
	mtbpc	$10
	mtdab	$11
	mtdabm	$12
	mtdvb	$13
	mtdvbm	$14
	mtiab	$15
	mtiabm	$16
	mtpc	$4,$1
	mtps	$4,$1

