.include "t-macros.i"

	start

	load r2 0x11223344
test_bsh:
	bsh r2, r3
	check1 r3 0x22114433

	exit0
