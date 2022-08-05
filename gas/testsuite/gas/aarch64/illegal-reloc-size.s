# Very simple check to see that we're disallowing the :size: relocationos on
# non-mov[kz] instructions.  We only check `add` and plain `mov` here.
#
# N.b. we follow the lead of the error messages that are emitted for the
# `abs_*` relocations in that we emit "bad expression" complaints for the `mov`
# instruction and "this relocation modifier is not allowed on this instruction"
# for the `add` instruction.

    .data
    .globl sym1
    .size sym1, 0xa0a0
sym1:
    .xword 0
    .text
    .globl _start
    .type _start, STT_FUNC
_start:
    mov x12, #:size_g0:sym1
    mov x12, #:size_g0_nc:sym1
    add x12, x12, #:size_g0:sym1
    add x12, x12, #:size_g0_nc:sym1

    mov x12, #:size_g1:sym1
    mov x12, #:size_g1_nc:sym1
    add x12, x12, #:size_g1:sym1
    add x12, x12, #:size_g1_nc:sym1

    mov x12, #:size_g2:sym1
    mov x12, #:size_g2_nc:sym1
    add x12, x12, #:size_g2:sym1
    add x12, x12, #:size_g2_nc:sym1

    mov x12, #:size_g3:sym1
    add x12, x12, #:size_g3:sym1
