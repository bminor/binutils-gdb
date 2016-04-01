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
