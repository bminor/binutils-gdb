.include "t-macros.i"

	start

	.align 3
	.data
byteaddr:	.word bytes
	.align 7
bytes:	
	.byte 0xb0
	.byte 0xb1
	.byte 0xb2
	.byte 0xb3
	.byte 0xb4
	.byte 0xb5
	.byte 0xb6
	.byte 0xb7
	.byte 0xb8
	.byte 0xb9
	.byte 0xba
	.byte 0xbb
	.byte 0xbc
	.byte 0xbd
	.byte 0xbe
	.byte 0xbf

	.text
	ld $8, byteaddr

	
test_ldl_0:
	load $10 0xdeadbeefdeadbeef 0xcccccccccccccccc
	ldl $10, 0($8)
	check10  0xdeadbeefdeadbeef 0xb0cccccccccccccc

test_ldl_1:
	load $10 0xdeadbeefdeadbeef 0xcccccccccccccccc
	ldl $10, 1($8)
	check10  0xdeadbeefdeadbeef 0xb1b0cccccccccccc

test_ldl_2:
	load $10 0xdeadbeefdeadbeef 0xcccccccccccccccc
	ldl $10, 2($8)
	check10  0xdeadbeefdeadbeef 0xb2b1b0cccccccccc

test_ldl_3:
	load $10 0xdeadbeefdeadbeef 0xcccccccccccccccc
	ldl $10, 3($8)
	check10  0xdeadbeefdeadbeef 0xb3b2b1b0cccccccc

test_ldl_4:
	load $10 0xdeadbeefdeadbeef 0xcccccccccccccccc
	ldl $10, 4($8)
	check10  0xdeadbeefdeadbeef 0xb4b3b2b1b0cccccc

test_ldl_5:
	load $10 0xdeadbeefdeadbeef 0xcccccccccccccccc
	ldl $10, 5($8)
	check10  0xdeadbeefdeadbeef 0xb5b4b3b2b1b0cccc

test_ldl_6:
	load $10 0xdeadbeefdeadbeef 0xcccccccccccccccc
	ldl $10, 6($8)
	check10  0xdeadbeefdeadbeef 0xb6b5b4b3b2b1b0cc

test_ldl_7:
	load $10 0xdeadbeefdeadbeef 0xcccccccccccccccc
	ldl $10, 7($8)
	check10  0xdeadbeefdeadbeef 0xb7b6b5b4b3b2b1b0

	
test_ldl_8:
	load $10 0xdeadbeefdeadbeef 0xcccccccccccccccc
	ldl $10, 8($8)
	check10  0xdeadbeefdeadbeef 0xb8cccccccccccccc

test_ldl_9:
	load $10 0xdeadbeefdeadbeef 0xcccccccccccccccc
	ldl $10, 9($8)
	check10  0xdeadbeefdeadbeef 0xb9b8cccccccccccc

test_ldl_10:
	load $10 0xdeadbeefdeadbeef 0xcccccccccccccccc
	ldl $10, 10($8)
	check10  0xdeadbeefdeadbeef 0xbab9b8cccccccccc

test_ldl_11:
	load $10 0xdeadbeefdeadbeef 0xcccccccccccccccc
	ldl $10, 11($8)
	check10  0xdeadbeefdeadbeef 0xbbbab9b8cccccccc

test_ldl_12:
	load $10 0xdeadbeefdeadbeef 0xcccccccccccccccc
	ldl $10, 12($8)
	check10  0xdeadbeefdeadbeef 0xbcbbbab9b8cccccc

test_ldl_13:
	load $10 0xdeadbeefdeadbeef 0xcccccccccccccccc
	ldl $10, 13($8)
	check10  0xdeadbeefdeadbeef 0xbdbcbbbab9b8cccc

test_ldl_14:
	load $10 0xdeadbeefdeadbeef 0xcccccccccccccccc
	ldl $10, 14($8)
	check10  0xdeadbeefdeadbeef 0xbebdbcbbbab9b8cc

test_ldl_15:
	load $10 0xdeadbeefdeadbeef 0xcccccccccccccccc
	ldl $10, 15($8)
	check10  0xdeadbeefdeadbeef 0xbfbebdbcbbbab9b8

	exit0
