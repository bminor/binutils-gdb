.include "t-macros.i"

	start

test_mult1:
	load $8 0xffffffff7fffffff 0x0000000080000000
	load $9 0x10000000ffffffff 0x00000000ffffffff
	mult1 $10, $8, $9
	check10 0x0000000000000000 0xffffffff80000000 
 	checkHI 0x000000007fffffff 0x0000000000000000
 	checkLO 0xffffffff80000000 0x0000000000000000

	exit0
