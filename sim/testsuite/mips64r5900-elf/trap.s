	.include "t-macros.i"

	start
	
        mtc0    $0,$12
        mtc0    $0,$11
        mtc0    $0,$13

	# install our jump table
	la	$24, jump_table
	li	$25, 0x80000180
	li	$26, 0xbfc00380
	lw	$27, 0($24)
	sw	$27, 0($25)
	sw	$27, 0($26)
	lw	$27, 4($24)
	sw	$27, 4($25)
	sw	$27, 4($26)
	lw	$27, 8($24)
	sw	$27, 8($25)
	sw	$27, 8($26)

        la      $25, LocalHandler
        la      $27, address_trap
        la      $31, address_return
        li      $2, 1
        li      $3, 1
address_trap:
        tge     $2, $3
        b       fail

address_return:
	exit0

fail:
	exit47

jump_table:
	.align 7
	la	$24, LocalHandler
	jr	$24


        .align 7
LocalHandler:

        mfc0    $25,$12
        andi    $7,$25,0x002        # check that exl bit was set
        beq     $7,$0,fail

        mfc0    $25,$14
        bne     $27,$25,fail        # compare with the exception program counter
        mfc0    $25,$13
        andi    $25,$25,0x000000ff  # extract excpt code, not checking int*[5:0]
        li      $8,(( 13 )<<2)              # expected value of cause register
        bne     $25,$8,fail
        mtc0    $31,$14           # return address from handler
        nop
        nop
        nop
        nop
        eret
        nop
