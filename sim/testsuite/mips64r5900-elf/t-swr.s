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
	
test_swr_0:
	load $10  0xdeadbeefdeadbeef 0xffffffffb3b2b1b0
	swr $10, 0($8)
	ld $10, 0($8)
	check10 0xdeadbeefdeadbeef 0xb7b6b5b4b3b2b1b0

test_swr_1:
	load $10  0xdeadbeefdeadbeef 0xffffffffccb3b2b1
	swr $10, 1($8)
	ld $10, 0($8)
	check10 0xdeadbeefdeadbeef 0xb7b6b5b4b3b2b1b0

test_swr_2:
	load $10  0xdeadbeefdeadbeef 0xffffffffccccb3b2
	swr $10, 2($8)
	ld $10, 0($8)
	check10 0xdeadbeefdeadbeef 0xb7b6b5b4b3b2b1b0

test_swr_3:
	load $10  0xdeadbeefdeadbeef 0xffffffffccccccb3
	swr $10, 3($8)
	ld $10, 0($8)
	check10 0xdeadbeefdeadbeef 0xb7b6b5b4b3b2b1b0

	
test_swr_4:
	load $10  0xdeadbeefdeadbeef 0xffffffffb7b6b5b4
	swr $10, 4($8)
	ld $10, 0($8)
	check10 0xdeadbeefdeadbeef 0xb7b6b5b4b3b2b1b0

test_swr_5:
	load $10  0xdeadbeefdeadbeef 0xffffffffccb7b6b5
	swr $10, 5($8)
	ld $10, 0($8)
	check10 0xdeadbeefdeadbeef 0xb7b6b5b4b3b2b1b0

test_swr_6:
	load $10  0xdeadbeefdeadbeef 0xffffffffccccb7b6
	swr $10, 6($8)
	ld $10, 0($8)
	check10 0xdeadbeefdeadbeef 0xb7b6b5b4b3b2b1b0

test_swr_7:
	load $10  0xdeadbeefdeadbeef 0xffffffffccccccb7
	swr $10, 7($8)
	ld $10, 0($8)
	check10 0xdeadbeefdeadbeef 0xb7b6b5b4b3b2b1b0

	exit0
