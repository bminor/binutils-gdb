.include "t-macros.i"

	start

	load r2 0xffffff80
test_zxb:
	zxb r2
	check1 r2 0x80

	load r2 0xffffff7f
test_zxb2:
	zxb r2
	check1 r2 0x7f

	exit0
