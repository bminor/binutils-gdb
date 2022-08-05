# Ensure that the size relocations are correctly handled.
# Here we just test that each of the valid kind of size relocations is handled
# well.
#
# Description of the testcase:
#   Set of two chunks.  Fist tests `movk`, second tests `movz`.
#
#   Each chunk has 4 subparts.  Those subparts check all viable relocations for
#   a size which fits into the bottom 16, 32, 48, and 64 bits respectively.
#
# We use .hidden in order to allow the -shared test to test that things work
# fine there too (since this relocation is only allowed on a symbol which is
# known to resolve locally).
    .data
    .globl sym1
    .hidden sym1
    .size sym1, 0xa0a0
sym1:
    .xword 0
    .globl sym2
    .hidden sym2
    .size sym2, 0xa0a0b0b0
sym2:
    .xword 0
    .globl sym3
    .hidden sym3
    .size sym3, 0xa0a0b0b0c0c0
sym3:
    .xword 0
    .globl sym4
    .hidden sym4
    .size sym4, 0xa0a0b0b0c0c0d0d0
sym4:
    .xword 0
    .text
    .globl _start
    .type _start, STT_FUNC
_start:
  movk x12, #:size_g0:sym1
  movk x12, #:size_g0_nc:sym1
  movk x12, #:size_g1:sym1
  movk x12, #:size_g1_nc:sym1
  movk x12, #:size_g2:sym1
  movk x12, #:size_g2_nc:sym1
  movk x12, #:size_g3:sym1

  # movk x13, #:size_g0:sym2
  movk x13, #:size_g0_nc:sym2
  movk x13, #:size_g1:sym2
  movk x13, #:size_g1_nc:sym2
  movk x13, #:size_g2:sym2
  movk x13, #:size_g2_nc:sym2
  movk x13, #:size_g3:sym2

  # movk x13, #:size_g0:sym3
  movk x13, #:size_g0_nc:sym3
  # movk x13, #:size_g1:sym3
  movk x13, #:size_g1_nc:sym3
  movk x13, #:size_g2:sym3
  movk x13, #:size_g2_nc:sym3
  movk x13, #:size_g3:sym3

  # movk x13, #:size_g0:sym4
  movk x13, #:size_g0_nc:sym4
  # movk x13, #:size_g1:sym4
  movk x13, #:size_g1_nc:sym4
  # movk x13, #:size_g2:sym4
  movk x13, #:size_g2_nc:sym4
  movk x13, #:size_g3:sym4


  movz x12, #:size_g0:sym1
  movz x12, #:size_g0_nc:sym1
  movz x12, #:size_g1:sym1
  movz x12, #:size_g1_nc:sym1
  movz x12, #:size_g2:sym1
  movz x12, #:size_g2_nc:sym1
  movz x12, #:size_g3:sym1

  # movz x13, #:size_g0:sym2
  movz x13, #:size_g0_nc:sym2
  movz x13, #:size_g1:sym2
  movz x13, #:size_g1_nc:sym2
  movz x13, #:size_g2:sym2
  movz x13, #:size_g2_nc:sym2
  movz x13, #:size_g3:sym2

  # movz x13, #:size_g0:sym3
  movz x13, #:size_g0_nc:sym3
  # movz x13, #:size_g1:sym3
  movz x13, #:size_g1_nc:sym3
  movz x13, #:size_g2:sym3
  movz x13, #:size_g2_nc:sym3
  movz x13, #:size_g3:sym3

  # movz x13, #:size_g0:sym4
  movz x13, #:size_g0_nc:sym4
  # movz x13, #:size_g1:sym4
  movz x13, #:size_g1_nc:sym4
  # movz x13, #:size_g2:sym4
  movz x13, #:size_g2_nc:sym4
  movz x13, #:size_g3:sym4

