	.text

	.ifndef r
	.set r,1
	.endif

	.global	tlsfntprelm
	.type	tlsfntprelm,@function
	.p2align 1
tlsfntprelm:
	move.w x:TPOFF16,$r10
.Lfe:
	.size	tlsfntprelm,.Lfe-tlsfntprelm

	.globl z
	.section	.tdata,"awT",@progbits
	.p2align 2
	.type	z,@object
	.size	z,r
z:
	.fill	r,1,42

	.type	x,@object
	.size	x,1
x:
	.byte 42
