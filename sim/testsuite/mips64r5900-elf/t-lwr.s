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


	.text
	ld $8, byteaddr
	
test_lwr_0:
	load $10 0xdeadbeefdeadbeef 0xcccccccccccccccc
	lwr $10, 0($8)
	check10  0xdeadbeefdeadbeef 0xffffffffb3b2b1b0

test_lwr_1:
	load $10 0xdeadbeefdeadbeef 0xcccccccccccccccc
	lwr $10, 1($8)
	check10  0xdeadbeefdeadbeef 0xffffffffccb3b2b1

test_lwr_2:
	load $10 0xdeadbeefdeadbeef 0xcccccccccccccccc
	lwr $10, 2($8)
	check10  0xdeadbeefdeadbeef 0xffffffffccccb3b2

test_lwr_3:
	load $10 0xdeadbeefdeadbeef 0xcccccccccccccccc
	lwr $10, 3($8)
	check10  0xdeadbeefdeadbeef 0xffffffffccccccb3

	
test_lwr_4:
	load $10 0xdeadbeefdeadbeef 0xcccccccccccccccc
	lwr $10, 4($8)
	check10  0xdeadbeefdeadbeef 0xffffffffb7b6b5b4

test_lwr_5:
	load $10 0xdeadbeefdeadbeef 0xcccccccccccccccc
	lwr $10, 5($8)
	check10  0xdeadbeefdeadbeef 0xffffffffccb7b6b5

test_lwr_6:
	load $10 0xdeadbeefdeadbeef 0xffffffffcccccccc
	lwr $10, 6($8)
	check10  0xdeadbeefdeadbeef 0xffffffffccccb7b6

test_lwr_7:
	load $10 0xdeadbeefdeadbeef 0xcccccccccccccccc
	lwr $10, 7($8)
	check10  0xdeadbeefdeadbeef 0xffffffffccccccb7

	exit0
