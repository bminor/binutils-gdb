.include "t-macros.i"

	start

	load r2 0xffff8000
test_zxh:
	zxh r2
	check r2 0x80

	load r2 0xffff7fff
test_zxh2:
	zxh r2
	check r2 0x7fff

	exit0
