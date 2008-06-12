	.text
	.set noreorder

foo:
        bbit032 $23,51,foo
        nop
        bbit0   $23,71,foo
        nop

        bbit132 $23,49,foo
        nop
        bbit1   $23,74,foo
        nop

        cins    $2,0,37

        cins32  $19,$31,39,12
        cins32  $17,$20,7,25

        cins    $24,$10,64,8
        cins    $21,$30,50,14

        exts    $26,26,32

        exts32  $7,$21,32,10
        exts32  $31,$13,3,29

        exts    $14,$29,70,14
        exts    $20,$16,39,25
