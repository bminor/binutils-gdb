	.text
	.global	w1
	.global	w2
	.global	w3
	.section	.tbss,"awT",@nobits
	.align	2
	.type	w1, %object
	.size	w1, 4
w1:
	.zero	4

	.type	w2, %object
	.size	w2, 4
w2:
	.zero	4

	.type	w3, %object
	.size	w3, 4
w3:
	.zero	4
