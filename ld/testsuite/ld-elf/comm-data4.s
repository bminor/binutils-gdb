 .ifdef HPUX
i .comm 4
 .else
  .comm i,4,4
 .endif

 .data
 .dc.a i

 .globl main
 .globl _main
 .globl start
 .globl _start
 .globl __start
 .text
main:
_main:
start:
_start:
__start:
 .dc.a 0
