.include "t-macros.i"

	start

test_qfsrv:
        mtsab   $0,4
        load $8 0x0000000000000000 0x0000000000000000
	load $9 0xffffffffffffffff 0xffffffffffffffff
        qfsrv   $10,$8,$9   #Expectation:
        check10 0x00000000ffffffff 0xffffffffffffffff

	exit0
