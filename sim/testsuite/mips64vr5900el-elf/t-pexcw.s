.include "t-macros.i"

	start

test_pexcw:
        load $8 0x1111111133333333 0x2222222244444444
        pexcw   $10,$8   #Expectation:
	check10 0x1111111122222222 0x3333333344444444

	exit0
