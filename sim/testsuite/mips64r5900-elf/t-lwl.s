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
	
test_lwl0:
	load $10 0xdeadbeefdeadbeef 0xcccccccccccccccc
	lwl $10, 0($8)
	check10  0xdeadbeefdeadbeef 0xffffffffb0cccccc

test_lwl1:
	load $10 0xdeadbeefdeadbeef 0xcccccccccccccccc
	lwl $10, 1($8)
	check10  0xdeadbeefdeadbeef 0xffffffffb1b0cccc

test_lwl2:
	load $10 0xdeadbeefdeadbeef 0xcccccccccccccccc
	lwl $10, 2($8)
	check10  0xdeadbeefdeadbeef 0xffffffffb2b1b0cc

test_lwl3:
	load $10 0xdeadbeefdeadbeef 0xcccccccccccccccc
	lwl $10, 3($8)
	check10  0xdeadbeefdeadbeef 0xffffffffb3b2b1b0

	
test_lwl4:
	load $10 0xdeadbeefdeadbeef 0xcccccccccccccccc
	lwl $10, 4($8)
	check10  0xdeadbeefdeadbeef 0xffffffffb4cccccc

test_lwl5:
	load $10 0xdeadbeefdeadbeef 0xcccccccccccccccc
	lwl $10, 5($8)
	check10  0xdeadbeefdeadbeef 0xffffffffb5b4cccc

test_lwl6:
	load $10 0xdeadbeefdeadbeef 0xcccccccccccccccc
	lwl $10, 6($8)
	check10  0xdeadbeefdeadbeef 0xffffffffb6b5b4cc

test_lwl7:
	load $10 0xdeadbeefdeadbeef 0xcccccccccccccccc
	lwl $10, 7($8)
	check10  0xdeadbeefdeadbeef 0xffffffffb7b6b5b4

	exit0
