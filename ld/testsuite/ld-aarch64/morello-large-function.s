.data
   .chericap _start
.text
  .globl _start
  .type _start,@function
_start:
  ret
  .zero 0x8000
  .size _start, .-_start
