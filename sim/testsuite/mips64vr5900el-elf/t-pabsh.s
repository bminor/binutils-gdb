.include "t-macros.i"

	start

	load $8 0x8000800080008000 0xffffffffffffffff
test_pabsh:
	pabsh $10, $8
	check10 0x7fff7fff7fff7fff 0x0001000100010001

	exit0
