.include "t-macros.i"

	start

	jarl l1, r1
d1:
	
	exit47
	exit47
	exit47
	exit47
	exit47
	exit47
	exit47
	exit47
	exit47
	exit47
	exit47
	
l5:	jarl l6, r6
d6:

l6:	# check
	check1x 1 r1 d1
	check1x 2 r2 d2
	check1x 3 r3 d3
	check1x 4 r4 d4
	check1x 5 r5 d5
	check1x 6 r6 d6
	exit0

	exit47
	exit47
	
l2:	jarl l3, r3
d3:
	
	exit47
	exit47
	exit47
	exit47
	exit47
	exit47
	exit47
	exit47
	exit47
	exit47
	exit47

l1:	jarl l2, r2
d2:	

	exit47
	exit47
	
l4:	jarl l5, r5
d5:		
	exit47
	exit47

l3:	jarl l4, r4
d4:	
	exit47
	exit47
	exit47
	exit47
	exit47
	exit47
	exit47
	exit47
