.arch morello+c64

  .section .data.rel.ro.local,"aw",@progbits
  .asciz "Hello there ;-)"
  .zero 0x10000

  .data
data_start:
  .chericap __data_relro_start
  .text

obj:

  adrp    c0, data_start
  add     c0, c0, :lo12:data_start
  ldr     c0, [c0]
