.include "t-macros.i"

	start

test_mtsa:	
        load $9 0x0000000000000000 0x0000000000000020 # 32
        mtsa $9
        mfsa $10      #Expectation rd:0x20
        check10 0x0000000000000000 0x0000000000000020

	exit0
