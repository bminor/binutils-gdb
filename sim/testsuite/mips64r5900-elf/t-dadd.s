.include "t-macros.i"

	start

	# don't overflow

test_dadd:

	load $7 0xdeadbeefdeadbeef 0x7fffffffffffffff
	load $8 0xdeadbeefdeadbeef 0xffffffffffffffff
        dadd $10, $7, $8
        check10 0x0000000000000000 0x7ffffffffffffffe

test_dadd_1:
	load $7 0xdeadbeefdeadbeef 0x8000000000000000
	load $8 0xdeadbeefdeadbeef 0x7fffffffffffffff
        dadd $10, $7, $8
        check10 0x0000000000000000 0xffffffffffffffff

	exit0
