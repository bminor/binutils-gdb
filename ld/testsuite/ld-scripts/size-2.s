	.section .text,"ax",@progbits
	.long 1,2

	# thread local storage sections
	.section .tdata,"awT",@progbits
	.long 6,7,8,9,10
	
	.section .tbss,"awT",@nobits
	.long 0,0,0,0,0,0
