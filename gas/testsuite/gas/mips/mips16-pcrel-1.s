	.text

bar:
	.space	0x10000

	.ent	foo
	.set	mips16
foo:
	dla	$2, . - 1
	nop
	ld	$2, . - 5
	nop
	dla	$2, . - 1 + 0x7c
	nop
	ld	$2, . - 5 + 0xf8
	nop
	dla	$2, . - 1 + 0x80
	ld	$2, . - 5 + 0x100
	dla	$2, . - 1 - 0x4
	ld	$2, . - 5 - 0x4
	dla	$2, . - 1 + 0x7fff
	ld	$2, . - 5 + 0x7fff
	dla	$2, . - 1 - 0x8000
	ld	$2, . - 5 - 0x8000
	dla	$2, . - 1 + 0x8000
	ld	$2, . - 5 + 0x8000
	dla	$2, . - 1 - 0x8001
	ld	$2, . - 5 - 0x8001
	nop
	.set	nomips16
	.end	foo

baz:
	.align	7, 0
	.space	0xff00

# Force some (non-delay-slot) zero bytes, to make 'objdump' print ...
	.align	4, 0
	.space	16
