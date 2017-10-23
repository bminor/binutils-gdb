#as: -mevexwig=1
#objdump: -dw
#name: x86_64 AVX512VL/VAES wig insns
#source: x86-64-avx512vl_vaes-wig.s

.*: +file format .*


Disassembly of section \.text:

0+ <_start>:
[ 	]*[a-f0-9]+:[ 	]*62 02 95 00 de f4[ 	]*vaesdec %xmm28,%xmm29,%xmm30
[ 	]*[a-f0-9]+:[ 	]*62 22 95 00 de b4 f0 23 01 00 00[ 	]*vaesdec 0x123\(%rax,%r14,8\),%xmm29,%xmm30
[ 	]*[a-f0-9]+:[ 	]*62 02 95 20 de f4[ 	]*vaesdec %ymm28,%ymm29,%ymm30
[ 	]*[a-f0-9]+:[ 	]*62 22 95 20 de b4 f0 23 01 00 00[ 	]*vaesdec 0x123\(%rax,%r14,8\),%ymm29,%ymm30
[ 	]*[a-f0-9]+:[ 	]*62 02 95 00 df f4[ 	]*vaesdeclast %xmm28,%xmm29,%xmm30
[ 	]*[a-f0-9]+:[ 	]*62 22 95 00 df b4 f0 23 01 00 00[ 	]*vaesdeclast 0x123\(%rax,%r14,8\),%xmm29,%xmm30
[ 	]*[a-f0-9]+:[ 	]*62 02 95 20 df f4[ 	]*vaesdeclast %ymm28,%ymm29,%ymm30
[ 	]*[a-f0-9]+:[ 	]*62 22 95 20 df b4 f0 23 01 00 00[ 	]*vaesdeclast 0x123\(%rax,%r14,8\),%ymm29,%ymm30
[ 	]*[a-f0-9]+:[ 	]*62 02 95 00 dc f4[ 	]*vaesenc %xmm28,%xmm29,%xmm30
[ 	]*[a-f0-9]+:[ 	]*62 22 95 00 dc b4 f0 23 01 00 00[ 	]*vaesenc 0x123\(%rax,%r14,8\),%xmm29,%xmm30
[ 	]*[a-f0-9]+:[ 	]*62 02 95 20 dc f4[ 	]*vaesenc %ymm28,%ymm29,%ymm30
[ 	]*[a-f0-9]+:[ 	]*62 22 95 20 dc b4 f0 23 01 00 00[ 	]*vaesenc 0x123\(%rax,%r14,8\),%ymm29,%ymm30
[ 	]*[a-f0-9]+:[ 	]*62 02 95 00 dd f4[ 	]*vaesenclast %xmm28,%xmm29,%xmm30
[ 	]*[a-f0-9]+:[ 	]*62 22 95 00 dd b4 f0 23 01 00 00[ 	]*vaesenclast 0x123\(%rax,%r14,8\),%xmm29,%xmm30
[ 	]*[a-f0-9]+:[ 	]*62 02 95 20 dd f4[ 	]*vaesenclast %ymm28,%ymm29,%ymm30
[ 	]*[a-f0-9]+:[ 	]*62 22 95 20 dd b4 f0 23 01 00 00[ 	]*vaesenclast 0x123\(%rax,%r14,8\),%ymm29,%ymm30
[ 	]*[a-f0-9]+:[ 	]*62 02 95 00 de f4[ 	]*vaesdec %xmm28,%xmm29,%xmm30
[ 	]*[a-f0-9]+:[ 	]*62 22 95 00 de b4 f0 34 12 00 00[ 	]*vaesdec 0x1234\(%rax,%r14,8\),%xmm29,%xmm30
[ 	]*[a-f0-9]+:[ 	]*62 02 95 20 de f4[ 	]*vaesdec %ymm28,%ymm29,%ymm30
[ 	]*[a-f0-9]+:[ 	]*62 22 95 20 de b4 f0 34 12 00 00[ 	]*vaesdec 0x1234\(%rax,%r14,8\),%ymm29,%ymm30
[ 	]*[a-f0-9]+:[ 	]*62 02 95 00 df f4[ 	]*vaesdeclast %xmm28,%xmm29,%xmm30
[ 	]*[a-f0-9]+:[ 	]*62 22 95 00 df b4 f0 34 12 00 00[ 	]*vaesdeclast 0x1234\(%rax,%r14,8\),%xmm29,%xmm30
[ 	]*[a-f0-9]+:[ 	]*62 02 95 20 df f4[ 	]*vaesdeclast %ymm28,%ymm29,%ymm30
[ 	]*[a-f0-9]+:[ 	]*62 22 95 20 df b4 f0 34 12 00 00[ 	]*vaesdeclast 0x1234\(%rax,%r14,8\),%ymm29,%ymm30
[ 	]*[a-f0-9]+:[ 	]*62 02 95 00 dc f4[ 	]*vaesenc %xmm28,%xmm29,%xmm30
[ 	]*[a-f0-9]+:[ 	]*62 22 95 00 dc b4 f0 34 12 00 00[ 	]*vaesenc 0x1234\(%rax,%r14,8\),%xmm29,%xmm30
[ 	]*[a-f0-9]+:[ 	]*62 02 95 20 dc f4[ 	]*vaesenc %ymm28,%ymm29,%ymm30
[ 	]*[a-f0-9]+:[ 	]*62 22 95 20 dc b4 f0 34 12 00 00[ 	]*vaesenc 0x1234\(%rax,%r14,8\),%ymm29,%ymm30
[ 	]*[a-f0-9]+:[ 	]*62 02 95 00 dd f4[ 	]*vaesenclast %xmm28,%xmm29,%xmm30
[ 	]*[a-f0-9]+:[ 	]*62 22 95 00 dd b4 f0 34 12 00 00[ 	]*vaesenclast 0x1234\(%rax,%r14,8\),%xmm29,%xmm30
[ 	]*[a-f0-9]+:[ 	]*62 02 95 20 dd f4[ 	]*vaesenclast %ymm28,%ymm29,%ymm30
[ 	]*[a-f0-9]+:[ 	]*62 22 95 20 dd b4 f0 34 12 00 00[ 	]*vaesenclast 0x1234\(%rax,%r14,8\),%ymm29,%ymm30
#pass
