.include "t-macros.i"

	start

test_prot3w:	
        load $8 0x3333333311111111 0x2222222244444444
        prot3w  $10,$8       #Expectation:

	# Prior to PR 17362, this was:
        # check10 0x1111111122222222 0x3333333344444444

        check10 0x1111111144444444 0x2222222233333333

	exit0
