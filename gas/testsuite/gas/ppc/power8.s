	.text
power8:
	tabort.      5
	tabortwc.    7,8,16
	tabortdc.    20,11,10
	tabortwci.   17,10,-13
	tabortdci.   29,3,-5
	tbegin.
	tcheck       7
	tend.        0
	tend.
	tend.        1
	tendall.     
	treclaim.    24
	trechkpt.    
	tsr.         0
	tsuspend.    
	tsr.         1
	tresume.     
	ori	     2,2,0
	.p2align     4,,15
