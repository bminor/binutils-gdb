	.text
	.cpu 430x

;;;  Test for the assembler detecting spurious size modifiers.
	
	mov.z	r1, r2
	mov.abc	r1, r2
	mov.bcd	r1, r2
	mov.wcd	r1, r2
	mov.	r1, r2
	bis.a	#8, r2

;;; FIXME: Add more tests of assembler error detection here.
