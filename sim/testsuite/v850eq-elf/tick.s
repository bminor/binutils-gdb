.include "t-macros.i"

	start

	# Establish `interrupt..einterrupt' at the NMI interrupt address
	load r1 interrupt
	load r2 einterrupt
	load r3 INT_NMI
copy:	ld.b 0[r1], r4
	st.b r4, 0[r3]
	add 1, r1
	add 1, r3
	cmp r1, r2
	bge copy

	# Keep looping until r1 becomes non zero - a tick
	mov 0, r1
	mov 0, r2
loop:	cmp 0, r1
	be loop

	# Print the message indicating that a tick was encountered
	mov 4, r6
	mov 1, r7                 # FID
	load r8 tick              # string
	load r9 etick-tick        # size
	trap 31
	mov 0, r1
	add 1, r2
	cmp 10, r2
	blt loop

	exit0

interrupt:
	mov 1, r1
	reti
einterrupt:	

	.data
tick:	.ascii "Tick\n"
etick:
