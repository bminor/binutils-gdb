	.space $TEXT$
	.subspa $CODE$
	.align 4
	.export divu,entry
	.proc
	.callinfo
divu:	stws,ma		%r4,4(%r5)		; save registers on stack
