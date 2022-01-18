# Checking
#     - LD128 relocation has been resolved to GOT location.
#     - Relocation at that GOT location is introduced.
#     - GOT fragment contains address required.
#     - GOT fragment has LSB set if relocation is a function symbol.
.arch morello+c64
  .text
  .global _start

  .type _start,@function
_start:
  .size _start,12

  .type obj,@object
  .global obj
  .size obj,1
obj:

  ldr     c0, [c0, :got_lo12:_start]
  ldr     c0, [c0, :got_lo12:obj]
