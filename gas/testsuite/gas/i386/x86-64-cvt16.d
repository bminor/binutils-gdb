#objdump: -dw
#name: x86-64 CVT16

.*: +file format .*

Disassembly of section .text:

0+ <_start>:
[ 	]*[a-f0-9]+:	8f 48 78 a0 ff 00[ 	]+vcvtph2ps \$0x0,%xmm15,%xmm15
[ 	]*[a-f0-9]+:	8f 68 78 a0 3e 00[ 	]+vcvtph2ps \$0x0,\(%rsi\),%xmm15
[ 	]*[a-f0-9]+:	8f 68 78 a0 d8 00[ 	]+vcvtph2ps \$0x0,%xmm0,%xmm11
[ 	]*[a-f0-9]+:	8f c8 78 a0 c7 ff[ 	]+vcvtph2ps \$0xff,%xmm15,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 a0 c0 03[ 	]+vcvtph2ps \$0x3,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f c8 78 a0 c7 03[ 	]+vcvtph2ps \$0x3,%xmm15,%xmm0
[ 	]*[a-f0-9]+:	8f 48 78 a0 db 00[ 	]+vcvtph2ps \$0x0,%xmm11,%xmm11
[ 	]*[a-f0-9]+:	8f 68 78 a0 f8 00[ 	]+vcvtph2ps \$0x0,%xmm0,%xmm15
[ 	]*[a-f0-9]+:	8f e8 78 a0 01 03[ 	]+vcvtph2ps \$0x3,\(%rcx\),%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 a0 06 ff[ 	]+vcvtph2ps \$0xff,\(%rsi\),%xmm0
[ 	]*[a-f0-9]+:	8f 68 78 a0 3f 00[ 	]+vcvtph2ps \$0x0,\(%rdi\),%xmm15
[ 	]*[a-f0-9]+:	8f 48 78 a0 ff ff[ 	]+vcvtph2ps \$0xff,%xmm15,%xmm15
[ 	]*[a-f0-9]+:	8f 48 78 a0 db ff[ 	]+vcvtph2ps \$0xff,%xmm11,%xmm11
[ 	]*[a-f0-9]+:	8f 68 78 a0 1e ff[ 	]+vcvtph2ps \$0xff,\(%rsi\),%xmm11
[ 	]*[a-f0-9]+:	8f 68 78 a0 3f 03[ 	]+vcvtph2ps \$0x3,\(%rdi\),%xmm15
[ 	]*[a-f0-9]+:	8f 48 78 a0 df 03[ 	]+vcvtph2ps \$0x3,%xmm15,%xmm11
[ 	]*[a-f0-9]+:	8f 48 7c a0 ff 00[ 	]+vcvtph2ps \$0x0,%xmm15,%ymm15
[ 	]*[a-f0-9]+:	8f 68 7c a0 3e 00[ 	]+vcvtph2ps \$0x0,\(%rsi\),%ymm15
[ 	]*[a-f0-9]+:	8f 68 7c a0 d8 00[ 	]+vcvtph2ps \$0x0,%xmm0,%ymm11
[ 	]*[a-f0-9]+:	8f c8 7c a0 c7 ff[ 	]+vcvtph2ps \$0xff,%xmm15,%ymm0
[ 	]*[a-f0-9]+:	8f e8 7c a0 c0 03[ 	]+vcvtph2ps \$0x3,%xmm0,%ymm0
[ 	]*[a-f0-9]+:	8f c8 7c a0 c7 03[ 	]+vcvtph2ps \$0x3,%xmm15,%ymm0
[ 	]*[a-f0-9]+:	8f 48 7c a0 db 00[ 	]+vcvtph2ps \$0x0,%xmm11,%ymm11
[ 	]*[a-f0-9]+:	8f 68 7c a0 f8 00[ 	]+vcvtph2ps \$0x0,%xmm0,%ymm15
[ 	]*[a-f0-9]+:	8f e8 7c a0 01 03[ 	]+vcvtph2ps \$0x3,\(%rcx\),%ymm0
[ 	]*[a-f0-9]+:	8f e8 7c a0 06 ff[ 	]+vcvtph2ps \$0xff,\(%rsi\),%ymm0
[ 	]*[a-f0-9]+:	8f 68 7c a0 3f 00[ 	]+vcvtph2ps \$0x0,\(%rdi\),%ymm15
[ 	]*[a-f0-9]+:	8f 48 7c a0 ff ff[ 	]+vcvtph2ps \$0xff,%xmm15,%ymm15
[ 	]*[a-f0-9]+:	8f 48 7c a0 db ff[ 	]+vcvtph2ps \$0xff,%xmm11,%ymm11
[ 	]*[a-f0-9]+:	8f 68 7c a0 1e ff[ 	]+vcvtph2ps \$0xff,\(%rsi\),%ymm11
[ 	]*[a-f0-9]+:	8f 68 7c a0 3f 03[ 	]+vcvtph2ps \$0x3,\(%rdi\),%ymm15
[ 	]*[a-f0-9]+:	8f 48 7c a0 df 03[ 	]+vcvtph2ps \$0x3,%xmm15,%ymm11
[ 	]*[a-f0-9]+:	8f 68 78 a1 18 00[ 	]+vcvtps2ph \$0x0,%xmm11,\(%rax\)
[ 	]*[a-f0-9]+:	8f 68 78 a1 3f 00[ 	]+vcvtps2ph \$0x0,%xmm15,\(%rdi\)
[ 	]*[a-f0-9]+:	8f c8 78 a1 04 24 00[ 	]+vcvtps2ph \$0x0,%xmm0,\(%r12\)
[ 	]*[a-f0-9]+:	8f 48 78 a1 df ff[ 	]+vcvtps2ph \$0xff,%xmm11,%xmm15
[ 	]*[a-f0-9]+:	8f c8 78 a1 c7 03[ 	]+vcvtps2ph \$0x3,%xmm0,%xmm15
[ 	]*[a-f0-9]+:	8f 48 78 a1 df 03[ 	]+vcvtps2ph \$0x3,%xmm11,%xmm15
[ 	]*[a-f0-9]+:	8f e8 78 a1 c4 00[ 	]+vcvtps2ph \$0x0,%xmm0,%xmm4
[ 	]*[a-f0-9]+:	8f e8 78 a1 07 00[ 	]+vcvtps2ph \$0x0,%xmm0,\(%rdi\)
[ 	]*[a-f0-9]+:	8f 68 78 a1 f8 ff[ 	]+vcvtps2ph \$0xff,%xmm15,%xmm0
[ 	]*[a-f0-9]+:	8f 68 78 a1 38 00[ 	]+vcvtps2ph \$0x0,%xmm15,\(%rax\)
[ 	]*[a-f0-9]+:	8f 68 78 a1 18 ff[ 	]+vcvtps2ph \$0xff,%xmm11,\(%rax\)
[ 	]*[a-f0-9]+:	8f c8 78 a1 04 24 ff[ 	]+vcvtps2ph \$0xff,%xmm0,\(%r12\)
[ 	]*[a-f0-9]+:	8f 48 78 a1 3c 24 ff[ 	]+vcvtps2ph \$0xff,%xmm15,\(%r12\)
[ 	]*[a-f0-9]+:	8f 68 78 a1 38 03[ 	]+vcvtps2ph \$0x3,%xmm15,\(%rax\)
[ 	]*[a-f0-9]+:	8f 48 78 a1 1c 24 03[ 	]+vcvtps2ph \$0x3,%xmm11,\(%r12\)
[ 	]*[a-f0-9]+:	8f 68 78 a1 dc ff[ 	]+vcvtps2ph \$0xff,%xmm11,%xmm4
[ 	]*[a-f0-9]+:	8f 68 7c a1 18 00[ 	]+vcvtps2ph \$0x0,%ymm11,\(%rax\)
[ 	]*[a-f0-9]+:	8f 68 7c a1 3f 00[ 	]+vcvtps2ph \$0x0,%ymm15,\(%rdi\)
[ 	]*[a-f0-9]+:	8f c8 7c a1 04 24 00[ 	]+vcvtps2ph \$0x0,%ymm0,\(%r12\)
[ 	]*[a-f0-9]+:	8f 48 7c a1 df ff[ 	]+vcvtps2ph \$0xff,%ymm11,%xmm15
[ 	]*[a-f0-9]+:	8f c8 7c a1 c7 03[ 	]+vcvtps2ph \$0x3,%ymm0,%xmm15
[ 	]*[a-f0-9]+:	8f 48 7c a1 df 03[ 	]+vcvtps2ph \$0x3,%ymm11,%xmm15
[ 	]*[a-f0-9]+:	8f e8 7c a1 c4 00[ 	]+vcvtps2ph \$0x0,%ymm0,%xmm4
[ 	]*[a-f0-9]+:	8f e8 7c a1 07 00[ 	]+vcvtps2ph \$0x0,%ymm0,\(%rdi\)
[ 	]*[a-f0-9]+:	8f 68 7c a1 f8 ff[ 	]+vcvtps2ph \$0xff,%ymm15,%xmm0
[ 	]*[a-f0-9]+:	8f 68 7c a1 38 00[ 	]+vcvtps2ph \$0x0,%ymm15,\(%rax\)
[ 	]*[a-f0-9]+:	8f 68 7c a1 18 ff[ 	]+vcvtps2ph \$0xff,%ymm11,\(%rax\)
[ 	]*[a-f0-9]+:	8f c8 7c a1 04 24 ff[ 	]+vcvtps2ph \$0xff,%ymm0,\(%r12\)
[ 	]*[a-f0-9]+:	8f 48 7c a1 3c 24 ff[ 	]+vcvtps2ph \$0xff,%ymm15,\(%r12\)
[ 	]*[a-f0-9]+:	8f 68 7c a1 38 03[ 	]+vcvtps2ph \$0x3,%ymm15,\(%rax\)
[ 	]*[a-f0-9]+:	8f 48 7c a1 1c 24 03[ 	]+vcvtps2ph \$0x3,%ymm11,\(%r12\)
[ 	]*[a-f0-9]+:	8f 68 7c a1 dc ff[ 	]+vcvtps2ph \$0xff,%ymm11,%xmm4
#pass
