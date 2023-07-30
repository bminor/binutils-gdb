        add %r1, 2147483647
        or %r2, 2147483648         ; This overflows.
        xor %r3, -2147483648
        sub %r4, -2147483649       ; This overflows.
