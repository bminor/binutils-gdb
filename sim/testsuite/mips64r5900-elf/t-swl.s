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
	
test_swl_0:
	load $10  0xdeadbeefdeadbeef 0xffffffffb0cccccc
	swl $10, 0($8)
	ld $10, 0($8)
	check10   0xdeadbeefdeadbeef 0xb7b6b5b4b3b2b1b0

test_swl_1:
	load $10  0xdeadbeefdeadbeef 0xffffffffb1b0cccc
	swl $10, 1($8)
	ld $10, 0($8)
	check10   0xdeadbeefdeadbeef 0xb7b6b5b4b3b2b1b0

test_swl_2:
	load $10  0xdeadbeefdeadbeef 0xffffffffb2b1b0cc
	swl $10, 2($8)
	ld $10, 0($8)
	check10   0xdeadbeefdeadbeef 0xb7b6b5b4b3b2b1b0

test_swl_3:
	load $10  0xdeadbeefdeadbeef 0xffffffffb3b2b1b0
	swl $10, 3($8)
	ld $10, 0($8)
	check10   0xdeadbeefdeadbeef 0xb7b6b5b4b3b2b1b0

	
test_swl_4:
	load $10  0xdeadbeefdeadbeef 0xffffffffb4cccccc
	swl $10, 4($8)
	ld $10, 0($8)
	check10   0xdeadbeefdeadbeef 0xb7b6b5b4b3b2b1b0

test_swl_5:
	load $10  0xdeadbeefdeadbeef 0xffffffffb5b4cccc
	swl $10, 5($8)
	ld $10, 0($8)
	check10   0xdeadbeefdeadbeef 0xb7b6b5b4b3b2b1b0

test_swl_6:
	load $10  0xdeadbeefdeadbeef 0xffffffffb6b5b4cc
	swl $10, 6($8)
	ld $10, 0($8)
	check10 0xdeadbeefdeadbeef 0xb7b6b5b4b3b2b1b0

test_swl_7:
	load $10  0xdeadbeefdeadbeef 0xffffffffb7b6b5b4
	swl $10, 7($8)
	ld $10, 0($8)
	check10   0xdeadbeefdeadbeef 0xb7b6b5b4b3b2b1b0

	exit0
