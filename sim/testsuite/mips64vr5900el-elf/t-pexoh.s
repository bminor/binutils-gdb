.include "t-macros.i"

	start

test_pexoh:
        load $8 0x1111444433332222 0x5555888877776666
        pexoh  $10,$8   #Expectation:
        check10 0x1111222233334444 0x5555666677778888

	exit0
