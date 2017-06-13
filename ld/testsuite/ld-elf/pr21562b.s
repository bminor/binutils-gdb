        .section        scnfoo,"aw",%progbits
        .zero 0x10

        .globl  bar
        .data
        .type   bar, %object
bar:
        .dc.a   __stop_scnfoo
