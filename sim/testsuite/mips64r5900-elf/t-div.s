.include "t-macros.i"

	start

test_div:
	load $7 0xdeadbeefdeadbeef 0xdeadbeef80000000
	load $8 0xdeadbeefdeadbeef 0xdeadbeefffffffff
	# NB:	 That $0 is needed!
        div $0, $7,$8
        checkHI 0x0000000000000000 0x0000000000000000
        checkLO 0x0000000000000000 0xffffffff80000000

test_div_0:
	load $7 0xdeadbeefdeadbeef 0xdeadbeef80000000
	load $8 0xdeadbeefdeadbeef 0xdeadbeef00000000
	# NB:	 That $0 is needed!
        div $0, $7,$8
	# don't crash!
        checkHI 0x0000000000000000 0x0000000000000000
        checkLO 0x0000000000000000 0xffffffff80000000
	
	exit0
