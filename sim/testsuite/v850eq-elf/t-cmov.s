.include "t-macros.i"

	start

	load r2 2
	load r3 3

	load r4 4
	cmp r2, r3
test_cmov1:
	cmov ne, r2, r3, r4
	check1x 1 r4 2

	load r4 4
	cmp r2, r3
test_cmov2:
	cmov ne, 2, r3, r4
	check1x 2 r4 2

	load r4 4
	cmp r2, r3
test_cmov3:
	cmov e, r2, r3, r4
	check1x 3 r4 3

	load r4 4
	cmp r2, r3
test_cmov4:
	cmov e, 2, r3, r4
	check1x 4 r4 3

	exit0
