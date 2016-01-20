        .syntax unified
        .cpu cortex-m3
        .fpu softvfp
        .eabi_attribute 20, 1
        .eabi_attribute 21, 1
        .eabi_attribute 23, 3
        .eabi_attribute 24, 1
        .eabi_attribute 25, 1
        .eabi_attribute 26, 1
        .eabi_attribute 30, 2
        .eabi_attribute 34, 1
        .eabi_attribute 18, 4
        .thumb
        .section        .text.foo,"axy",%progbits
        .align  2
        .global foo
        .thumb
        .thumb_func
        .type   foo, %function
foo:
        @ args = 0, pretend = 0, frame = 0
        @ frame_needed = 0, uses_anonymous_args = 0
        @ link register save eliminated.
        movs    r0, #1
        movs    r1, #1
        movw    r2, #257
        movs    r3, #1
        b       madd
        .size   foo, .-foo

