	.am33_2
	
	.section .rlcb, "ax"
	.global relax_long_cond_branch
relax_long_cond_branch:
	clr d0
	clr d1
.L2:
	add d1,d0
	inc d1

	.fill 32764, 1, 0xcb

	cmp 9,d1
	ble .L2
	rets
	

	.section .rsflb, "ax"
	.global relax_long_float_cond_branch
relax_long_float_cond_branch:
	clr d0
	clr d1
.L4:
	add d1,d0
	inc d1

	.fill 32764, 1, 0xcb

	cmp 9,d1
	fble .L4
	rets
