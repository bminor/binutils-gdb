.include "t-macros.i"

	start

	# check that a ldsr/stsr instruction clears reserved bits
	mov -1, r1
	ldsr r1, psw
	stsr psw, r2
	sub r1, r2
	be bad
	exit0
bad:	
	exit47
