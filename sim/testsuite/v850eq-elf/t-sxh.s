.include "t-macros.i"

	start

	load r2 0x8000
test_sxh:
	sxh r2
	check1 r2 0xffff8000

	load r2 0x7fff
test_sxh2:
	sxh r2
	check1 r2 0x7fff

	exit0
