	.text

stuff:
	.ent stuff
	di
	ei
	mfbpc	$10
	mfdab	$11
	mfdabm	$12
	mfdvb	$13
	mfdvbm	$14
	mfiab	$15
	mfiabm	$16
	mfpc	$4,1
	mfps	$4,1
	mtbpc	$10
	mtdab	$11
	mtdabm	$12
	mtdvb	$13
	mtdvbm	$14
	mtiab	$15
	mtiabm	$16
	mtpc	$4,1
	mtps	$4,1


	adda.s  $f12,$f14
	suba.s  $f10,$f12
	msuba.s $f10,$f12
	mula.s  $f10,$f12
	madda.s $f10,$f12
	max.s   $f10,$f12,$f14
	min.s   $f10,$f12,$f14

	mult1	$3,$4,$0
	mult1	$4,$3
	multu1	$3,$4,$0
	multu1	$4,$3
	div1	$0,$3,$4
	div1	$4,$3
	divu1	$0,$3,$4
	divu1	$4,$3
# Nop just to fill out ot a 16byte boundary
	nop
