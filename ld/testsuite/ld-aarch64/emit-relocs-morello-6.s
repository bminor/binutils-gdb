# Checking
#     - LD128 relocation has been resolved to GOT location.
#     - Relocation at that GOT location is introduced.
#     - GOT fragment contains address required.
#     - GOT fragment has LSB set if relocation is a function symbol.
.arch morello+c64

  .data
  .global data_obj
data_obj:
  .word 10
  .size data_obj, .-data_obj

  # Very odd to see, but still is nice to check we have internal consistency.
  .type data_func,@function
  .global data_func
data_func:
  .word 10
  .size data_func, 4

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
  ldr     c0, [c0, :got_lo12:data_obj]
  ldr     c0, [c0, :got_lo12:data_func]
