.include "t-macros.i"

	start
	
	mov 4, r6
	mov 1, r7                   # FID
	load r8 hello               # string
	load r9 ehello-hello        # size
	trap 31

	exit0

	.data
hello:	.ascii "Hello World!\n"
ehello:
