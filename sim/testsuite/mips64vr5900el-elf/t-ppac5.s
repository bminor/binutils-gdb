.include "t-macros.i"

	start

test_ppac5:	
	load $8 0x80a8a8a880a8a8a8 0x80a8a8a880a8a8a8
        ppac5   $10, $8
	check10 0x0000D6B50000D6B5 0x0000D6B50000D6B5

	exit0
