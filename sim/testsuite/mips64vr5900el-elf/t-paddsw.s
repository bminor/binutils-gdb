.include "t-macros.i"

	start

test_paddsw:
	load $8 0x0123456680000007 0x89abcded00000006
        load $9 0x00000001fffffff9 0x000000027ffffffa
        paddsw  $10,$9,$8
	check10 0x0123456780000000 0x89abcdef7fffffff

	exit0
