	.file	"x.c"
.globl bss
	.section	.bss,"aw",@nobits
	.align 4096
	.type	bss,@object
	.size	bss,4096
bss:
	.zero	4096
.globl tbss
	.section	.tbss,"awT",@nobits
	.align 4096
	.type	tbss,@object
	.size	tbss,4096
tbss:
	.zero	4096
