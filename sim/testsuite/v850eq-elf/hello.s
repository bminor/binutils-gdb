.include "t-macros.i"

	start
	
	mov 4, r6
	mov 1, r7                   # FID
	load32 r8 hello             # string
	mov ehello - hello, r9      # size
	trap 31

	exit0

	.data
hello:	.ascii "Hello World!\n"
ehello:
