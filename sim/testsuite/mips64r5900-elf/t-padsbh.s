.include "t-macros.i"

	start

test_padsbh:
        load $8 0x0122800745658006 0x89a80005cdeb0004
        load $9 0x0001fff90002fffa 0x00037ffb00047ffc
        padsbh  $10,$9,$8
	check10 0x0123800045678000 0x765b7ff632197ff8

	exit0
