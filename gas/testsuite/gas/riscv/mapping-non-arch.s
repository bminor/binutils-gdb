.attribute arch, "rv32i"
.option arch, +c
.text
addi	a0, zero, 1
.align 3
.word 0x1
addi	a0, zero, 2

.section text.A, "ax"
addi	a0, zero, 3
.word 0x2
