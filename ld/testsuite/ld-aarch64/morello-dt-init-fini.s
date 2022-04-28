        .section .init,"ax",%progbits
        .global _init
        .hidden _init
        .type   _init, %function
_init:
        ret

        .section .fini,"ax",%progbits
        .global _fini
        .hidden _fini
        .type   _fini, %function
_fini:
        ret
