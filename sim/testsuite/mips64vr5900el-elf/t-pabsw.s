.include "t-macros.i"

	start

	load $8 0x8000000080000000 0xffffffffffffffff
test_pabsw:
	pabsw $10, $8
	check10 0x7fffffff7fffffff 0x0000000100000001

	exit0
