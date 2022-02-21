# Here we want to make sure that if we have a set of sections which do not need
# to be adjusted for morello precise bounds, but the total PCC range *does*
# need to be adjusted for Morello precise bounds then we adjust that PCC range.
.text
      .global _start
_start:
      ret
      .zero 0x2ffc
.section .alt,"ax",@progbits
      .zero 0x3000
