.include "t-macros.i"

	start

	load r2 0x11223344
test_hsw:
	hsw r2, r3
	check1 r3 0x33441122

	exit0
