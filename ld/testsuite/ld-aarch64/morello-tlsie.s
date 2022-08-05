	.global	p
	.global	a
	.global x
	.hidden x
	.global y
	.protected y
	.section	.tbss,"awT",%nobits
p:
	.zero   65540
a:
	.zero   65540
	.zero	52
	.size a,65592
b:
	.zero	100
	.size b,100
x:
	.zero	10
	.size x, 10
y:
	.zero	15
	.size y, 15

	.section .rodata
	.string "Hello world\n"
	.text

# Compute the address of an integer within structure a, padded
# by an array of size 48

	.align	2
	.p2align 4,,11
	.global _start
	.type   _start,%function
_start:
	adrp	c2, :gottprel:a
	add	c2, c2, :gottprel_lo12:a
	ldp	x2, x1, [c2]

	adrp	c2, :gottprel:b
	add	c2, c2, :gottprel_lo12:b
	ldp	x2, x1, [c2]

	adrp	c2, :gottprel:x
	add	c2, c2, :gottprel_lo12:x
	ldp	x2, x1, [c2]

	adrp	c2, :gottprel:y
	add	c2, c2, :gottprel_lo12:y
	ldp	x2, x1, [c2]
