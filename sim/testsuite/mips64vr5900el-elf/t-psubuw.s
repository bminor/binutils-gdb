.include "t-macros.i"

	start

test_psubuw:	
        load $8 0x0123456800000000 0x89abcdf100000001
        load $9 0x0000000100000001 0x0000000200000002
        psubuw  $10,$8,$9
	check10 0x0123456700000000 0x89abcdef00000000

	exit0
