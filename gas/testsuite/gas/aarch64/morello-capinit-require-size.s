.data
.align 4
a:
	.8byte 0

d:
	.capinit a
	.8byte 0
	.size d, .-d

// Need to switch section so that the capinit relocation does not have enough
// space in the relevant chunk.
.section .data.rel.ro
.align 4
e:
	.8byte 0
	.size e, .-e
