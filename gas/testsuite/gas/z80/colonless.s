	.text
	;; colon less labels test
start
	ld	a,0
.L_next
	jr	z,finish
	ld	a,1
.L_xx	ld	a,2
	ld	a,3
	jr	start
	jr	.L_next
	jr	.L_xx
finish	ret
	.end
