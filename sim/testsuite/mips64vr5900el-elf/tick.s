	# The main body of code loops on the variable FLAG, waiting
	# for it to be set.  When set, the message `Tick' is displayed
	# and the flag cleared.

	# The interrupt handler, sets the global FLAG variable and
	# then resumes normal execution.

	.globl _start
	.ent _start
	.set noreorder
_start:

	# Patch the interrupt handler, setting it to an instruction
	# sequence that jumps to this programs interrupt code.
	la $4, handler
	la $5, +0xffffffffBFC00200 + 0x180
	lw $6, 0($4)  # BR $8
	sw $6, 0($5)
	lw $6, 4($4)  # NOP
	sw $6, 4($5)

	# establish two global pointer registers:	
	la $8, flag         # r8 contains the address of FLAG
	la $9, real_handler # r9 contains the address of the
			    # real handler

	# enable interrupt delivery
	mfc0 $4, $12
	nop ; nop ; nop
	or $4, $4, 1
	or $4, $4, 4 ; xor $4, $4, 4 # clear error level
	mtc0 $4, $12
	nop ; nop ; nop

loop:	
	lw $5, 0($8)
	beq $5, $0, loop
	nop
	
	# FLAG set, print the message Tick by calling
	# write (1, str, 6)
	sw $0, 0($8)	
	li	$4, 1
	la	$5, str
	li	$6, 6
	la	$2,+0xffffffffbfc00504
	lw	$2, 0($2)
	jal	$2
	nop
	j	loop
	nop


	# Instruction sequence that jumps to
	# the real handler (address in r9).
handler:	j $9
		nop

	# local interrupt handler, set FLAG and return
real_handler:
	li $10, 1
	sw $10, 0($8) # flag = 1
	eret
	nop


	.end	_start

flag:
	.word 0
str:	.asciiz "Tick\r\n"
