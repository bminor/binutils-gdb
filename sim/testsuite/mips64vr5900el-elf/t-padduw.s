.include "t-macros.i"

	start

test_padduw:
	load $8 0x0123456600000007 0x89abcded00000006
	load $9 0x00000001fffffff9 0x00000002fffffffa
        padduw  $10,$9,$8
	check10 0x01234567ffffffff 0x89abcdefffffffff

	exit0
