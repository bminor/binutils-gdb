.include "t-macros.i"

	start

	load r2 0x11223344
test_bsw:
	bsw r2
	check1 r2 0x44332211

	exit0
