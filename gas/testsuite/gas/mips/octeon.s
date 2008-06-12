	.text
	.set noreorder

foo:
        baddu   $17,$18,$19
        baddu   $2,$3

        bbit0   $19,22,foo
        nop
        bbit032 $30,11,foo
        nop
        bbit0   $8,42,foo
        nop

        bbit1   $3,31,foo
        nop
        bbit132 $24,10,foo
        nop
        bbit1   $14,46,foo
        nop

        cins    $25,$10,22,2
        cins    $9,17,29
        cins32  $15,$2,18,8
        cins32  $22,9,22
        cins    $24,$31,32,31
        cins    $15,37,5

        dmul    $19,$24,$28
        dmul    $21,$25

        pop     $8,$19
        pop     $2
        dpop    $15,$22
        dpop    $12

        exts    $4,$28,27,15
        exts    $15,17,6
        exts32  $4,$13,10,8
        exts32  $15,11,20
        exts    $7,$4,54,9
        exts    $25,37,25

        mtm0    $26
        mtm1    $19
        mtm2    $18

        mtp0    $16
        mtp1    $25
        mtp2    $9

	synciobdma
        syncs
        syncw
        syncws

        v3mulu  $21,$10,$21
        v3mulu  $20,$10
        vmm0    $3,$19,$16
        vmm0    $31,$9
        vmulu   $29,$10,$17
        vmulu   $27,$6
