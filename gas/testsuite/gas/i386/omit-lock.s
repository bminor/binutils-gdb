        .code32
.globl main
        .type   main, @function
main:
	lock
        lock addl $0x1,(%eax)
