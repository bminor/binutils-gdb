.include "t-macros.i"

	start

test_multuw:
	load $8 0xffffffff7fffffff 0x0000000080000000
	load $9 0x00000000ffffffff 0x00000000ffffffff
        pmultuw    $10,$8,$9
	check10 0x7ffffffe80000001 0x7fffffff80000000
        checkHI 0x000000007ffffffe 0x000000007fffffff
        checkLO 0xffffffff80000001 0xffffffff80000000

	exit0
