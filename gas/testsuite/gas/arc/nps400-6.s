        .text

        .macro  addb_like_test mnem
        \mnem		r0,r0,r1,0,8,2
        \mnem\().f	r0,r0,r1,16,8,2
        \mnem\().f.sx	r0,r0,r1,8,24,6
        .endm

        .macro  andb_like_test mnem, size
        \mnem		r0,r0,r1,0,8,\size
        \mnem\().f	r0,r0,r1,16,8,\size
        .endm

        .macro  notb_like_test mnem
        \mnem		r0,r1,0,8
        \mnem\().f	r0,r1,16,16
        .endm

        .macro  div_like_test mnem
        \mnem		r0,r0,r2,8,8
        \mnem\().f	r0,r0,0xf,8
        .endm

        .macro  qcmp_test mnem
        \mnem           r2,r2,r0,8,8,0,1,3
        \mnem           r2,r2,r0,8,8,1,1
        \mnem           r2,r2,r0,8,8,1
        \mnem           r2,r2,r0,8,8
        .endm

        .macro  calcsxd_test mnem
        \mnem\()	r0, r0, r0, 1
        \mnem\().f	r1, r1, r0, 2
        \mnem\()	r2, r2, r3, 4
        \mnem\().f	r0, r0, r0, 8
        \mnem\()	r3, r3, r0, 16
        \mnem\().f	r0, r0, r0, 32
        \mnem\()	r0, r0, r1, 64
        \mnem\().f	r2, r2, r0, 128
        \mnem\()	r1, r1, r2, 256
        .endm

        .macro calcbsxdkey_test mnem
        \mnem\()        r0, r0, r0
        \mnem\().f      r1, r1, r0
        \mnem\()        r1, r1, r2
        \mnem\().f      r3, r3, r2
        .endm

        .macro mxb_like_test mnem
        \mnem\()        r0, r1, 0, 1, 2
        \mnem\()        r0, r1, 7, 8, 7
        \mnem\()        r0, r1, 7, 7, 8
        \mnem\().s      r2, r3, 1, 4, 3, 2
        \mnem\().s      r2, r3, 7, 8, 7, 7
        \mnem\().s      r2, r3, 7, 7, 8, 7
        \mnem\().s      r2, r3, 7, 8, 7, 8
        .endm

        .macro addsubl_test mnem
        \mnem\()        r0, r0, 0
        \mnem\()        r2, r2, 32767
        \mnem\()        r5, r5, -32768
        \mnem\().f      r10, r10, 0
        \mnem\().f      r11, r11, 1
        \mnem\().f      r12, r12, -1
        .endm

        .macro andorxorl_test mnem
        \mnem\()        r0, r0, 0
        \mnem\()        r1, r1, 1
        \mnem\()        r2, r2, 65535
        \mnem\().f      r10, r10, 0
        \mnem\().f      r11, r11, 1
        \mnem\().f      r12, r12, 65535
        .endm

        .macro andorab_test mnem
        \mnem\()        r0, r0, 0, 1
        \mnem\()        r1, r2, 15, 16
        \mnem\().f      r2, r3, 0, 1
        \mnem\().f      r12, r13, 16, 16
        \mnem\()        r0, r0, r0, 0, 1
        \mnem\()        r1, r1, r2, 15, 16
        \mnem\().f      r2, r2, r3, 0, 1
        \mnem\().f      r12, r12, r13, 16, 16
        .endm

        addb_like_test addb
        addb_like_test subb
        addb_like_test adcb
        addb_like_test sbcb

        andb_like_test andb, 2
        andb_like_test xorb, 2
        andb_like_test orb, 2
        andb_like_test shlb, 2
        andb_like_test shrb, 2
        andb_like_test fxorb, 8
        andb_like_test wxorb, 16

        notb_like_test notb
        notb_like_test cntbb

        div_like_test div
        div_like_test mod
        div_like_test divm

        qcmp_test qcmp.ar
        qcmp_test qcmp.al

        calcsxd_test    calcsd
        calcsxd_test    calcxd

        calcbsxdkey_test calcbsd
        calcbsxdkey_test calcbxd
        calcbsxdkey_test calckey
        calcbsxdkey_test calcxkey

        mxb_like_test mxb
        mxb_like_test imxb

        addsubl_test addl
        addsubl_test subl

        andorxorl_test andl
        andorxorl_test orl
        andorxorl_test xorl

        andorab_test andab
        andorab_test orab

        lbdsize         r0, r1
        lbdsize         r2, r3
        lbdsize.f       r0, r1
        lbdsize.f       r2, r3

        bdlen           r0, r1, 1
        bdlen           r1, r3, 256
        bdlen           r1, r2, 240
        bdlen           r12, r13
        bdlen.f         r0, r1, 1
        bdlen.f         r1, r3, 256
        bdlen.f         r12, r13

        .macro cscb_test mnem
        \mnem\()        r10,r12,r20
        \mnem\()        r0,0x12345678,r20
        \mnem\()        r6,r7,0xffffffff
        \mnem\()        r8,0xffffffff,0xffffffff
        \mnem\()        0,r14,r13
        \mnem\()        0,0xffffffff,r10
        \mnem\()        0,r12,0xffffffff
        \mnem\()        r4,r5,0x1
        \mnem\()        r3,0x12345678,0x2
        \mnem\()        0,r1,0x4
        \mnem\()        0,0xffffffff,0x1
        .endm

        cscb_test csma
        cscb_test csms
        cscb_test cbba
        cscb_test cbba.f

        .macro zncv_test mnem
        \mnem\()        r10,r12,r20
        \mnem\()        r0,0x12345678,r20
        \mnem\()        r6,r7,0xffffffff
        \mnem\()        r8,0xffffffff,0xffffffff
        \mnem\()        0,r14,r13
        \mnem\()        0,0xffffffff,r10
        \mnem\()        0,r12,0xffffffff
        \mnem\()        r4,r5,0x1
        \mnem\()        r3,0x12345678,0x2
        \mnem\()        0,r1,0x4
        \mnem\()        0,0xffffffff,0x1
        \mnem\()        r1,r1,-1
        \mnem\()        0,0xffffffff,-1
        .endm

        zncv_test zncv.rd
        zncv_test zncv.wr

        hofs            r1, r2, r3
        hofs.f          r4, r5, r6
        hofs            r7, r8, 240, 0
        hofs.f          r7, r8, 0, 1

