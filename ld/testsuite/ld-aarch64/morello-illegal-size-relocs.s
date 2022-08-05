# This is illegal for a shared object since this symbol is interposable.
    .data
    .globl sym1
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
    .size localsym, 0xa0a0
localsym:
    .xword 0

    .weak othersym

    .text
    .globl _start
    .type _start, STT_FUNC
_start:
  movk x12, #:size_g0:sym1

  # Too large symbols, n.b. we include some relocations which are not too large
  # since they will simply not get an error on them.
  movk x12, #:size_g0:sym2
  movk x12, #:size_g1:sym2
  movk x12, #:size_g2:sym2
  movk x12, #:size_g3:sym2

  movk x12, #:size_g0:sym3
  movk x12, #:size_g1:sym3
  movk x12, #:size_g2:sym3
  movk x12, #:size_g3:sym3

  movk x12, #:size_g0:sym4
  movk x12, #:size_g1:sym4
  movk x12, #:size_g2:sym4
  movk x12, #:size_g3:sym4

  # Relocation with an addend disallowed.
  movk x12, #:size_g0:sym1+10
  # N.b. this currently complains about a relocation against `.data` with an
  # addend.  That is due to a GAS bug that we need to fix.  Once that GAS bug
  # has been fixed (i.e. we should not emit relocations against a section symbol
  # plus addend) the error message that this testcase should check against should
  # be changed.  At the same time it would probably be nice to put extra tests
  # into morello-size-relocs.s checking that local symbols work just fine.
  #
  # Luckily, the transformation from `localsym` to `.data` isn't problematic
  # since the section symbol is local and the point of including this line is
  # to check that our obtaining of local symbols names is correct.
  movk x12, #:size_g0:localsym+10

  # Relocation againts undefweak symbol disallowed.
  movk x12, #:size_g0:othersym
