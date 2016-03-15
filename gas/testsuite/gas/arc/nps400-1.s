        .text
        movb		r0, r0, r1, 4, 5, 6
        movb		r0, r0, r12, 4, 5, 6
        movb		r15, r15, r12, 4, 5, 6
        movb.cl		r0, r1, 4, 5, 6
        movb.cl		r0, r14, 4, 5, 6
        movb.cl		r13, r1, 4, 5, 6

        movh		r0, r0, 1234
        movh		r3, r3, 0xffff
        movh.cl		r0, 1234
        movh.cl		r3, 0xffff
