WORDSIZE=4

	.text
	.globl _doworm

_doworm:
	moveml %a2-%a4,%sp@-
	movl _memsize,%d0
	subl &CODESIZE,%d0
	addl  &0x0,%d0
	movl %d0,%a3
	movl %a3,%a4
	addl &WORMSIZE,%a4
	movl &CODESIZE,%sp@-
	movl %a3,%sp@-
	movl &worm,%sp@-
	jsr _bcopy
	addl &12,%sp
	movl %a4,%a0
	subl &WORDSIZE,%a0
	movl %sp@(16),%a0@
	jsr %a3@
	moveml %sp@+,%a2-%a4
	rts


worm:
	jsr _t_disable
	movl &LONGWORMSIZE,%d0
	movl %a3,%a0
	movl &worm,%a2
	lea %a3@(-WORDSIZE),%a1
crawl:
	movl %a0@+,%a1@
	cmpml %a1@+,%a2@+
	dbne %d0,crawl
	jsr _t_enable
	subl &WORDSIZE,%a3
	jmp %a4@
	nop
	.long 0
WORMSIZE=.-worm
LONGWORMSIZE=WORMSIZE/4


manager:
	tstw %d0
	bgt manerr
	cmpl _baseaddr,%a3
	beq manfin
	jmp %a3@
manerr:
	cmpw &1,_noiselevel
	blt manerr1
	movl %a1,%d1
	subl &4,%d1
	movl %d1,%sp@-
	movl %d0,%sp@-
	pea errmsg
	jsr _printf
	addl &12,%sp
manerr1:
	moveq &0,%d0
	bra manret
manfin:
	moveq &1,%d0
manret:
	rts
	nop
CODESIZE=.-worm

errmsg: .asciz " Premature termination (%d) at %#x"
