        .section        scnfoo,"aw",%progbits
        .zero 0x10

        .globl  bar
        .data
        .align 8
        .type   bar, %object
        .size   bar, 8
bar:
        .dc.a   __start_scnfoo
        .dc.a  .startof. (scnfoo)
