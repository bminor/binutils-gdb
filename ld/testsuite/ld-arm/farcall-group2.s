
@ Test to ensure that ARM calls exceeding 32Mb generate stubs.

@ We will place the section .foo at 0x2000.

	.text
myfunc:	
	bl bar3
	bl bar4
	bl bar5
