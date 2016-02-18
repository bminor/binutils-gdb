.text
	ldr x0, .LC0
	ldr x0, .LC1
	ret
.LC0:
	.word dummy
.LC1:
	.word .LC0
