.include "t-macros.i"

	start


	
		;; Check carry/overflow set for 0x8000 - 1 -> 1:0x7fff
	ldi	r10,#0x8000
	ldi	r6,#0x8004
	mvtc	r6,cr0
test_subi:
	SUBI	r10,#1 || nop
	mvfc	r6,cr0
	check1  r10 0x7FFF
	check1	r6 0x8005	;<--- Carry should be ON.

	exit0
