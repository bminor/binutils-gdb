.include "t-macros.i"

	start

test_psubsw:
        load $8 0x0123456800000000 0x89abcdf1ffffffff
        load $9 0x0000000180000000 0x000000027fffffff
        psubsw  $10,$8,$9
	check10 0x012345677fffffff 0x89abcdef80000000

	exit0
