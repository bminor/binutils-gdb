.include "t-macros.i"

	start

test_pcpyh:
	load $7 0x0000000000001111 0x000000000000ffff
        pcpyh   $10,$7           #Expectation:
	check10 0x1111111111111111 0xffffffffffffffff 

	exit0
