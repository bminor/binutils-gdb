	.globl __start
__start:
# write (hello world)
	addu 4, r0, r15
	addu 1, r0, r2
	addu hello, r0, r4
	ld length(r0), r6
#	addu 12, r0, r5
	trap 72
# exit (0)
	addu 1, r0, r15
	addu 0, r0, r2
	trap 72

length:	.long 14
hello:	.ascii "Hello World!\r\n"
