        jeq %r1,%r2,2147483647
        jlt %r3,%r4,2147483648  ; Overflows.
        jge %r5,10,-2147483648
        ja -2147483649          ; Overflows.
