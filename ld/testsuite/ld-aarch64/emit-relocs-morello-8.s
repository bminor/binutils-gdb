	.arch morello+crc+c64
	.text
	.align	2
	.global	_start
_start:
	adrp	c0, .LC1
	add	c0, c0, :lo12:.LC1
	ldr	c0, [c0]

	.section	.data.rel.ro.local,"aw"
	.align	4
	.type	.LC1, %object
	.size	.LC1, 16
.LC1:
	.chericap	absolute_sym
	.ident	"GCC: (unknown) 11.0.0 20200826 (experimental)"
