	.section .text,"ax",@progbits
	.long 1,2

	.section .data,"aw",@progbits
	.long 3,4,5

	.section .bss,"aw",@nobits
	.long 0,0,0,0
	
	# thread local storage sections
	.section .tdata,"awT",@progbits
	.long 6,7,8,9,10
	
	.section .tbss,"awT",@nobits
	.long 0,0,0,0,0,0
