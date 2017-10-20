#as: -mevexwig=1
#objdump: -dw -Mintel
#name: x86_64 AVX512F/VAES wig insns (Intel disassembly)
#source: x86-64-avx512f_vaes-wig.s

.*: +file format .*


Disassembly of section \.text:

0+ <_start>:
[ 	]*[a-f0-9]+:[ 	]*62 02 95 40 de f4[ 	]*vaesdec zmm30,zmm29,zmm28
[ 	]*[a-f0-9]+:[ 	]*62 22 95 40 de b4 f0 23 01 00 00[ 	]*vaesdec zmm30,zmm29,ZMMWORD PTR \[rax\+r14\*8\+0x123\]
[ 	]*[a-f0-9]+:[ 	]*62 02 95 40 df f4[ 	]*vaesdeclast zmm30,zmm29,zmm28
[ 	]*[a-f0-9]+:[ 	]*62 22 95 40 df b4 f0 23 01 00 00[ 	]*vaesdeclast zmm30,zmm29,ZMMWORD PTR \[rax\+r14\*8\+0x123\]
[ 	]*[a-f0-9]+:[ 	]*62 02 95 40 dc f4[ 	]*vaesenc zmm30,zmm29,zmm28
[ 	]*[a-f0-9]+:[ 	]*62 22 95 40 dc b4 f0 23 01 00 00[ 	]*vaesenc zmm30,zmm29,ZMMWORD PTR \[rax\+r14\*8\+0x123\]
[ 	]*[a-f0-9]+:[ 	]*62 02 95 40 dd f4[ 	]*vaesenclast zmm30,zmm29,zmm28
[ 	]*[a-f0-9]+:[ 	]*62 22 95 40 dd b4 f0 23 01 00 00[ 	]*vaesenclast zmm30,zmm29,ZMMWORD PTR \[rax\+r14\*8\+0x123\]
[ 	]*[a-f0-9]+:[ 	]*62 02 95 40 de f4[ 	]*vaesdec zmm30,zmm29,zmm28
[ 	]*[a-f0-9]+:[ 	]*62 22 95 40 de b4 f0 34 12 00 00[ 	]*vaesdec zmm30,zmm29,ZMMWORD PTR \[rax\+r14\*8\+0x1234\]
[ 	]*[a-f0-9]+:[ 	]*62 02 95 40 df f4[ 	]*vaesdeclast zmm30,zmm29,zmm28
[ 	]*[a-f0-9]+:[ 	]*62 22 95 40 df b4 f0 34 12 00 00[ 	]*vaesdeclast zmm30,zmm29,ZMMWORD PTR \[rax\+r14\*8\+0x1234\]
[ 	]*[a-f0-9]+:[ 	]*62 02 95 40 dc f4[ 	]*vaesenc zmm30,zmm29,zmm28
[ 	]*[a-f0-9]+:[ 	]*62 22 95 40 dc b4 f0 34 12 00 00[ 	]*vaesenc zmm30,zmm29,ZMMWORD PTR \[rax\+r14\*8\+0x1234\]
[ 	]*[a-f0-9]+:[ 	]*62 02 95 40 dd f4[ 	]*vaesenclast zmm30,zmm29,zmm28
[ 	]*[a-f0-9]+:[ 	]*62 22 95 40 dd b4 f0 34 12 00 00[ 	]*vaesenclast zmm30,zmm29,ZMMWORD PTR \[rax\+r14\*8\+0x1234\]
#pass
