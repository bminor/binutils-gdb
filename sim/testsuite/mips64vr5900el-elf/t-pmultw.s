.include "t-macros.i"

	start

test_multw:
	load $8 0xffffffff7fffffff 0x0000000080000000
	load $9 0x10000000ffffffff 0x00000000ffffffff
	pmultw $10, $8, $9
	check10 0xffffffff80000001 0x0000000080000000
	checkHI 0xffffffffffffffff 0x0000000000000000
	checkLO 0xffffffff80000001 0xffffffff80000000

	exit0
