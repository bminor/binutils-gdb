  .comm foo,4
  .comm bar,4

  .text
  .align 8

  .global __start
__start:
  nop_s                          # messing with the alignment a bit
  ldl r14, [pcl, @foo@gotpc]
  ldl r15, [pcl, @bar@gotpc]
