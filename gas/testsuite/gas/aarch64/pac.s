	/* ARMv8.3 Pointer authentication instructions.  */
	.text

	/* Basic instructions.  */
	pacia x3, x4
	pacia x5, sp
	pacib x3, x4
	pacib x5, sp
	pacda x3, x4
	pacda x5, sp
	pacdb x3, x4
	pacdb x5, sp

	autia x3, x4
	autia x5, sp
	autib x3, x4
	autib x5, sp
	autda x3, x4
	autda x5, sp
	autdb x3, x4
	autdb x5, sp

	paciza x5
	pacizb x5
	pacdza x5
	pacdzb x5

	autiza x5
	autizb x5
	autdza x5
	autdzb x5

	xpaci x5
	xpacd x5

	pacga x1, x2, x3
	pacga x1, x2, sp

	/* Combined instructions.  */
	braa x1, x2
	braa x3, sp
	brab x1, x2
	brab x3, sp
	blraa x1, x2
	blraa x3, sp
	blrab x1, x2
	blrab x3, sp
	braaz x5
	brabz x5
	blraaz x5
	blrabz x5

	retaa
	retab
	eretaa
	eretab
