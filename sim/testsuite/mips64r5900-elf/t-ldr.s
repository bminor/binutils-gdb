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

	
test_ldr_0:
	load $10 0xdeadbeefdeadbeef 0xcccccccccccccccc
	ldr $10, 0($8)
	check10  0xdeadbeefdeadbeef 0xb7b6b5b4b3b2b1b0

test_ldr_1:
	load $10 0xdeadbeefdeadbeef 0xcccccccccccccccc
	ldr $10, 1($8)
	check10  0xdeadbeefdeadbeef 0xccb7b6b5b4b3b2b1

test_ldr_2:
	load $10 0xdeadbeefdeadbeef 0xcccccccccccccccc
	ldr $10, 2($8)
	check10  0xdeadbeefdeadbeef 0xccccb7b6b5b4b3b2

test_ldr_3:
	load $10 0xdeadbeefdeadbeef 0xcccccccccccccccc
	ldr $10, 3($8)
	check10  0xdeadbeefdeadbeef 0xccccccb7b6b5b4b3

test_ldr_4:
	load $10 0xdeadbeefdeadbeef 0xcccccccccccccccc
	ldr $10, 4($8)
	check10  0xdeadbeefdeadbeef 0xccccccccb7b6b5b4

test_ldr_5:
	load $10 0xdeadbeefdeadbeef 0xcccccccccccccccc
	ldr $10, 5($8)
	check10  0xdeadbeefdeadbeef 0xccccccccccb7b6b5

test_ldr_6:
	load $10 0xdeadbeefdeadbeef 0xcccccccccccccccc
	ldr $10, 6($8)
	check10  0xdeadbeefdeadbeef 0xccccccccccccb7b6

test_ldr_7:
	load $10 0xdeadbeefdeadbeef 0xcccccccccccccccc
	ldr $10, 7($8)
	check10  0xdeadbeefdeadbeef 0xccccccccccccccb7

	
test_ldr_8:
	load $10 0xdeadbeefdeadbeef 0xcccccccccccccccc
	ldr $10, 8($8)
	check10  0xdeadbeefdeadbeef 0xbfbebdbcbbbab9b8

test_ldr_9:
	load $10 0xdeadbeefdeadbeef 0xcccccccccccccccc
	ldr $10, 9($8)
	check10  0xdeadbeefdeadbeef 0xccbfbebdbcbbbab9

test_ldr_10:
	load $10 0xdeadbeefdeadbeef 0xcccccccccccccccc
	ldr $10, 10($8)
	check10  0xdeadbeefdeadbeef 0xccccbfbebdbcbbba

test_ldr_11:
	load $10 0xdeadbeefdeadbeef 0xcccccccccccccccc
	ldr $10, 11($8)
	check10  0xdeadbeefdeadbeef 0xccccccbfbebdbcbb

test_ldr_12:
	load $10 0xdeadbeefdeadbeef 0xcccccccccccccccc
	ldr $10, 12($8)
	check10  0xdeadbeefdeadbeef 0xccccccccbfbebdbc

test_ldr_13:
	load $10 0xdeadbeefdeadbeef 0xcccccccccccccccc
	ldr $10, 13($8)
	check10  0xdeadbeefdeadbeef 0xccccccccccbfbebd

test_ldr_14:
	load $10 0xdeadbeefdeadbeef 0xcccccccccccccccc
	ldr $10, 14($8)
	check10  0xdeadbeefdeadbeef 0xccccccccccccbfbe

test_ldr_15:
	load $10 0xdeadbeefdeadbeef 0xcccccccccccccccc
	ldr $10, 15($8)
	check10  0xdeadbeefdeadbeef 0xccccccccccccccbf

	exit0
