.include "t-macros.i"

	start

	load r2 0x7FFF8000
	mv2wtac r2, a0
test_rachi_1:
	rachi r4, a0, 0
	check1 r4 0x7FFF


	load r2 0x80001000
	mv2wtac r2, a0
test_rachi_2:
	rachi r4, a0, 0
	check1 r4 0x8000


	load r2 0x1000A000
	mv2wtac r2, a0
test_rachi_3:
	rachi r4, a0, 0
	check1 r4 0x1001


	load r2 0xA0007FFF
	mv2wtac r2, a0
test_rachi_4:
	rachi r4, a0, 0
	check1 r4 0xa000

	exit0
