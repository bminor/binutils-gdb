.include "t-macros.i"

	start

test_div:

	load $7 0xdeadbeefdeadbeef 0x8000000000000000
	load $8 0xdeadbeefdeadbeef 0xffffffffffffffff
	# NB:	 That $0 is needed!
        ddiv $0, $7, $8
        checkHI 0x0000000000000000 0x0000000000000000
        checkLO 0x0000000000000000 0x8000000000000000

test_div_0:
	load $7 0xdeadbeefdeadbeef 0x8000000000000000
	load $8 0xdeadbeefdeadbeef 0x0000000000000000
	# NB:	 That $0 is needed!
        ddiv $0, $7, $8
	# don't crash!
        checkHI 0x0000000000000000 0x0000000000000000
        checkLO 0x0000000000000000 0x8000000000000000
	
	exit0
