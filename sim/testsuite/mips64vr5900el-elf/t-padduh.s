.include "t-macros.i"

	start

test_padduh:
        load $8 0x0122000745650006 0x89a80005cdeb0004
	load $9 0x0001fff90002fffa 0x0003fffb0004fffc
        padduh  $10,$9,$8
	check10 0x0123ffff4567ffff 0x89abffffcdefffff

	exit0
