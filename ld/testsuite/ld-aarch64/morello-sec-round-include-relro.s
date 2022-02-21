.arch morello+c64

  .section .data.rel.ro.local,"aw",@progbits
data_relro_start:
  .asciz "Hello there ;-)"

  .data
data_start:
  .chericap obj
  .text

obj:

  ldr     c0, [c0]
  ldr     c0, [c0]
  ldr     c0, [c0]
  ldr     c0, [c0]
