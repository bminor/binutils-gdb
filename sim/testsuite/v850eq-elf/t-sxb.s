.include "t-macros.i"

	start

	load r2 0x80
test_sxb:
	sxb r2
	check r2 0xffffff80

	load r2 0x7f
test_sxb2:
	sxb r2
	check r2 0x7f

	exit0
