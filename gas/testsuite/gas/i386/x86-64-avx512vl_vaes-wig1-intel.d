#as: -mevexwig=1
#objdump: -dw -Mintel
#name: x86_64 AVX512VL/VAES wig insns (Intel disassembly)
#source: x86-64-avx512vl_vaes-wig.s

.*: +file format .*


Disassembly of section \.text:

0+ <_start>:
[ 	]*[a-f0-9]+:[ 	]*62 02 95 00 de f4[ 	]*vaesdec xmm30,xmm29,xmm28
[ 	]*[a-f0-9]+:[ 	]*62 22 95 00 de b4 f0 23 01 00 00[ 	]*vaesdec xmm30,xmm29,XMMWORD PTR \[rax\+r14\*8\+0x123\]
[ 	]*[a-f0-9]+:[ 	]*62 02 95 20 de f4[ 	]*vaesdec ymm30,ymm29,ymm28
[ 	]*[a-f0-9]+:[ 	]*62 22 95 20 de b4 f0 23 01 00 00[ 	]*vaesdec ymm30,ymm29,YMMWORD PTR \[rax\+r14\*8\+0x123\]
[ 	]*[a-f0-9]+:[ 	]*62 02 95 00 df f4[ 	]*vaesdeclast xmm30,xmm29,xmm28
[ 	]*[a-f0-9]+:[ 	]*62 22 95 00 df b4 f0 23 01 00 00[ 	]*vaesdeclast xmm30,xmm29,XMMWORD PTR \[rax\+r14\*8\+0x123\]
[ 	]*[a-f0-9]+:[ 	]*62 02 95 20 df f4[ 	]*vaesdeclast ymm30,ymm29,ymm28
[ 	]*[a-f0-9]+:[ 	]*62 22 95 20 df b4 f0 23 01 00 00[ 	]*vaesdeclast ymm30,ymm29,YMMWORD PTR \[rax\+r14\*8\+0x123\]
[ 	]*[a-f0-9]+:[ 	]*62 02 95 00 dc f4[ 	]*vaesenc xmm30,xmm29,xmm28
[ 	]*[a-f0-9]+:[ 	]*62 22 95 00 dc b4 f0 23 01 00 00[ 	]*vaesenc xmm30,xmm29,XMMWORD PTR \[rax\+r14\*8\+0x123\]
[ 	]*[a-f0-9]+:[ 	]*62 02 95 20 dc f4[ 	]*vaesenc ymm30,ymm29,ymm28
[ 	]*[a-f0-9]+:[ 	]*62 22 95 20 dc b4 f0 23 01 00 00[ 	]*vaesenc ymm30,ymm29,YMMWORD PTR \[rax\+r14\*8\+0x123\]
[ 	]*[a-f0-9]+:[ 	]*62 02 95 00 dd f4[ 	]*vaesenclast xmm30,xmm29,xmm28
[ 	]*[a-f0-9]+:[ 	]*62 22 95 00 dd b4 f0 23 01 00 00[ 	]*vaesenclast xmm30,xmm29,XMMWORD PTR \[rax\+r14\*8\+0x123\]
[ 	]*[a-f0-9]+:[ 	]*62 02 95 20 dd f4[ 	]*vaesenclast ymm30,ymm29,ymm28
[ 	]*[a-f0-9]+:[ 	]*62 22 95 20 dd b4 f0 23 01 00 00[ 	]*vaesenclast ymm30,ymm29,YMMWORD PTR \[rax\+r14\*8\+0x123\]
[ 	]*[a-f0-9]+:[ 	]*62 02 95 00 de f4[ 	]*vaesdec xmm30,xmm29,xmm28
[ 	]*[a-f0-9]+:[ 	]*62 22 95 00 de b4 f0 34 12 00 00[ 	]*vaesdec xmm30,xmm29,XMMWORD PTR \[rax\+r14\*8\+0x1234\]
[ 	]*[a-f0-9]+:[ 	]*62 02 95 20 de f4[ 	]*vaesdec ymm30,ymm29,ymm28
[ 	]*[a-f0-9]+:[ 	]*62 22 95 20 de b4 f0 34 12 00 00[ 	]*vaesdec ymm30,ymm29,YMMWORD PTR \[rax\+r14\*8\+0x1234\]
[ 	]*[a-f0-9]+:[ 	]*62 02 95 00 df f4[ 	]*vaesdeclast xmm30,xmm29,xmm28
[ 	]*[a-f0-9]+:[ 	]*62 22 95 00 df b4 f0 34 12 00 00[ 	]*vaesdeclast xmm30,xmm29,XMMWORD PTR \[rax\+r14\*8\+0x1234\]
[ 	]*[a-f0-9]+:[ 	]*62 02 95 20 df f4[ 	]*vaesdeclast ymm30,ymm29,ymm28
[ 	]*[a-f0-9]+:[ 	]*62 22 95 20 df b4 f0 34 12 00 00[ 	]*vaesdeclast ymm30,ymm29,YMMWORD PTR \[rax\+r14\*8\+0x1234\]
[ 	]*[a-f0-9]+:[ 	]*62 02 95 00 dc f4[ 	]*vaesenc xmm30,xmm29,xmm28
[ 	]*[a-f0-9]+:[ 	]*62 22 95 00 dc b4 f0 34 12 00 00[ 	]*vaesenc xmm30,xmm29,XMMWORD PTR \[rax\+r14\*8\+0x1234\]
[ 	]*[a-f0-9]+:[ 	]*62 02 95 20 dc f4[ 	]*vaesenc ymm30,ymm29,ymm28
[ 	]*[a-f0-9]+:[ 	]*62 22 95 20 dc b4 f0 34 12 00 00[ 	]*vaesenc ymm30,ymm29,YMMWORD PTR \[rax\+r14\*8\+0x1234\]
[ 	]*[a-f0-9]+:[ 	]*62 02 95 00 dd f4[ 	]*vaesenclast xmm30,xmm29,xmm28
[ 	]*[a-f0-9]+:[ 	]*62 22 95 00 dd b4 f0 34 12 00 00[ 	]*vaesenclast xmm30,xmm29,XMMWORD PTR \[rax\+r14\*8\+0x1234\]
[ 	]*[a-f0-9]+:[ 	]*62 02 95 20 dd f4[ 	]*vaesenclast ymm30,ymm29,ymm28
[ 	]*[a-f0-9]+:[ 	]*62 22 95 20 dd b4 f0 34 12 00 00[ 	]*vaesenclast ymm30,ymm29,YMMWORD PTR \[rax\+r14\*8\+0x1234\]
#pass
