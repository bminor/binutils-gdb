.include "t-macros.i"

	start

	load r2 0xffff8000
test_zxh:
	zxh r2
	check1 r2 0x8000

	load r2 0xffff7fff
test_zxh2:
	zxh r2
	check1 r2 0x7fff

	exit0
