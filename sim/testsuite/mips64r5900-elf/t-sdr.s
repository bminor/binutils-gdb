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

test_sdr_0:
	load $10  0xdeadbeefdeadbeef 0xb7b6b5b4b3b2b1b0
	sdr $10, 0($8)
	ld $10, 0($8)
	check10 0xdeadbeefdeadbeef 0xb7b6b5b4b3b2b1b0
	ld $10, 8($8)
	check10 0xdeadbeefdeadbeef 0xbfbebdbcbbbab9b8

test_sdr_1:
	load $10  0xdeadbeefdeadbeef 0xccb7b6b5b4b3b2b1
	sdr $10, 1($8)
	ld $10, 0($8)
	check10 0xdeadbeefdeadbeef 0xb7b6b5b4b3b2b1b0
	ld $10, 8($8)
	check10 0xdeadbeefdeadbeef 0xbfbebdbcbbbab9b8

test_sdr_2:
	load $10  0xdeadbeefdeadbeef 0xccccb7b6b5b4b3b2
	sdr $10, 2($8)
	ld $10, 0($8)
	check10 0xdeadbeefdeadbeef 0xb7b6b5b4b3b2b1b0
	ld $10, 8($8)
	check10 0xdeadbeefdeadbeef 0xbfbebdbcbbbab9b8

test_sdr_3:
	load $10  0xdeadbeefdeadbeef 0xccccccb7b6b5b4b3
	sdr $10, 3($8)
	ld $10, 0($8)
	check10 0xdeadbeefdeadbeef 0xb7b6b5b4b3b2b1b0
	ld $10, 8($8)
	check10 0xdeadbeefdeadbeef 0xbfbebdbcbbbab9b8

test_sdr_4:
	load $10  0xdeadbeefdeadbeef 0xccccccccb7b6b5b4
	sdr $10, 4($8)
	ld $10, 0($8)
	check10 0xdeadbeefdeadbeef 0xb7b6b5b4b3b2b1b0
	ld $10, 8($8)
	check10 0xdeadbeefdeadbeef 0xbfbebdbcbbbab9b8

test_sdr_5:
	load $10  0xdeadbeefdeadbeef 0xccccccccccb7b6b5
	sdr $10, 5($8)
	ld $10, 0($8)
	check10 0xdeadbeefdeadbeef 0xb7b6b5b4b3b2b1b0
	ld $10, 8($8)
	check10 0xdeadbeefdeadbeef 0xbfbebdbcbbbab9b8

test_sdr_6:
	load $10  0xdeadbeefdeadbeef 0xccccccccccccb7b6
	sdr $10, 6($8)
	ld $10, 0($8)
	check10 0xdeadbeefdeadbeef 0xb7b6b5b4b3b2b1b0
	ld $10, 8($8)
	check10 0xdeadbeefdeadbeef 0xbfbebdbcbbbab9b8

test_sdr_7:
	load $10  0xdeadbeefdeadbeef 0xccccccccccccccb7
	sdr $10, 7($8)
	ld $10, 0($8)
	check10 0xdeadbeefdeadbeef 0xb7b6b5b4b3b2b1b0
	ld $10, 8($8)
	check10 0xdeadbeefdeadbeef 0xbfbebdbcbbbab9b8

	
test_sdr_8:
	load $10  0xdeadbeefdeadbeef 0xbfbebdbcbbbab9b8
	sdr $10, 8($8)
	ld $10, 0($8)
	check10 0xdeadbeefdeadbeef 0xb7b6b5b4b3b2b1b0
	ld $10, 8($8)
	check10 0xdeadbeefdeadbeef 0xbfbebdbcbbbab9b8

test_sdr_9:
	load $10  0xdeadbeefdeadbeef 0xccbfbebdbcbbbab9
	sdr $10, 9($8)
	ld $10, 0($8)
	check10 0xdeadbeefdeadbeef 0xb7b6b5b4b3b2b1b0
	ld $10, 8($8)
	check10 0xdeadbeefdeadbeef 0xbfbebdbcbbbab9b8

test_sdr_10:
	load $10  0xdeadbeefdeadbeef 0xccccbfbebdbcbbba
	sdr $10, 10($8)
	ld $10, 0($8)
	check10 0xdeadbeefdeadbeef 0xb7b6b5b4b3b2b1b0
	ld $10, 8($8)
	check10 0xdeadbeefdeadbeef 0xbfbebdbcbbbab9b8

test_sdr_11:
	load $10  0xdeadbeefdeadbeef 0xccccccbfbebdbcbb
	sdr $10, 11($8)
	ld $10, 0($8)
	check10 0xdeadbeefdeadbeef 0xb7b6b5b4b3b2b1b0
	ld $10, 8($8)
	check10 0xdeadbeefdeadbeef 0xbfbebdbcbbbab9b8

test_sdr_12:
	load $10  0xdeadbeefdeadbeef 0xccccccccbfbebdbc
	sdr $10, 12($8)
	ld $10, 0($8)
	check10 0xdeadbeefdeadbeef 0xb7b6b5b4b3b2b1b0
	ld $10, 8($8)
	check10 0xdeadbeefdeadbeef 0xbfbebdbcbbbab9b8

test_sdr_13:
	load $10  0xdeadbeefdeadbeef 0xccccccccccbfbebd
	sdr $10, 13($8)
	ld $10, 0($8)
	check10 0xdeadbeefdeadbeef 0xb7b6b5b4b3b2b1b0
	ld $10, 8($8)
	check10 0xdeadbeefdeadbeef 0xbfbebdbcbbbab9b8

test_sdr_14:
	load $10  0xdeadbeefdeadbeef 0xccccccccccccbfbe
	sdr $10, 14($8)
	ld $10, 0($8)
	check10 0xdeadbeefdeadbeef 0xb7b6b5b4b3b2b1b0
	ld $10, 8($8)
	check10 0xdeadbeefdeadbeef 0xbfbebdbcbbbab9b8

test_sdr_15:
	load $10  0xdeadbeefdeadbeef 0xccccccccccccccbf
	sdr $10, 15($8)
	ld $10, 0($8)
	check10 0xdeadbeefdeadbeef 0xb7b6b5b4b3b2b1b0
	ld $10, 8($8)
	check10 0xdeadbeefdeadbeef 0xbfbebdbcbbbab9b8

	exit0
