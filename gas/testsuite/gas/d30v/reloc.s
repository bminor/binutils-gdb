	.text
start:
	add	r2, r0, hello
	add	r4, r0, bar
	add	r4, r0, bar2
	add	r4, r0, unk	
	bra	cont
hello:	.ascii "Hello World\n"
	.align 3
cont:	jmp	cont2
	abs	r2,r3
cont2:
	bra	start	||	nop
	jmp	start	||	nop
	bra	start
	jmp	start
	jmp	start
	bra	start
	bra	unknown
	jmp	unknown
	
	.data
bar:	.asciz	"XYZZY"
bar2:	.long	0xdeadbeef
	