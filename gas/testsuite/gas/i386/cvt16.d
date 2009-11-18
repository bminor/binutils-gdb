#objdump: -dw
#name: i386 CVT16

.*: +file format .*

Disassembly of section .text:

0+ <_start>:
[ 	]*[a-f0-9]+:	8f e8 78 a0 ff 00[ 	]+vcvtph2ps \$0x0,%xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e8 78 a0 3b 00[ 	]+vcvtph2ps \$0x0,\(%ebx\),%xmm7
[ 	]*[a-f0-9]+:	8f e8 78 a0 e8 00[ 	]+vcvtph2ps \$0x0,%xmm0,%xmm5
[ 	]*[a-f0-9]+:	8f e8 78 a0 c5 ff[ 	]+vcvtph2ps \$0xff,%xmm5,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 a0 c0 03[ 	]+vcvtph2ps \$0x3,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 a0 c7 03[ 	]+vcvtph2ps \$0x3,%xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 a0 ed 00[ 	]+vcvtph2ps \$0x0,%xmm5,%xmm5
[ 	]*[a-f0-9]+:	8f e8 78 a0 f8 00[ 	]+vcvtph2ps \$0x0,%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e8 78 a0 00 03[ 	]+vcvtph2ps \$0x3,\(%eax\),%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 a0 03 ff[ 	]+vcvtph2ps \$0xff,\(%ebx\),%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 a0 38 00[ 	]+vcvtph2ps \$0x0,\(%eax\),%xmm7
[ 	]*[a-f0-9]+:	8f e8 78 a0 ff ff[ 	]+vcvtph2ps \$0xff,%xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e8 78 a0 ed ff[ 	]+vcvtph2ps \$0xff,%xmm5,%xmm5
[ 	]*[a-f0-9]+:	8f e8 78 a0 2b ff[ 	]+vcvtph2ps \$0xff,\(%ebx\),%xmm5
[ 	]*[a-f0-9]+:	8f e8 78 a0 c7 ff[ 	]+vcvtph2ps \$0xff,%xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 a0 38 03[ 	]+vcvtph2ps \$0x3,\(%eax\),%xmm7
[ 	]*[a-f0-9]+:	8f e8 7c a0 ff 00[ 	]+vcvtph2ps \$0x0,%xmm7,%ymm7
[ 	]*[a-f0-9]+:	8f e8 7c a0 3b 00[ 	]+vcvtph2ps \$0x0,\(%ebx\),%ymm7
[ 	]*[a-f0-9]+:	8f e8 7c a0 e8 00[ 	]+vcvtph2ps \$0x0,%xmm0,%ymm5
[ 	]*[a-f0-9]+:	8f e8 7c a0 c5 ff[ 	]+vcvtph2ps \$0xff,%xmm5,%ymm0
[ 	]*[a-f0-9]+:	8f e8 7c a0 c0 03[ 	]+vcvtph2ps \$0x3,%xmm0,%ymm0
[ 	]*[a-f0-9]+:	8f e8 7c a0 c7 03[ 	]+vcvtph2ps \$0x3,%xmm7,%ymm0
[ 	]*[a-f0-9]+:	8f e8 7c a0 ed 00[ 	]+vcvtph2ps \$0x0,%xmm5,%ymm5
[ 	]*[a-f0-9]+:	8f e8 7c a0 f8 00[ 	]+vcvtph2ps \$0x0,%xmm0,%ymm7
[ 	]*[a-f0-9]+:	8f e8 7c a0 00 03[ 	]+vcvtph2ps \$0x3,\(%eax\),%ymm0
[ 	]*[a-f0-9]+:	8f e8 7c a0 03 ff[ 	]+vcvtph2ps \$0xff,\(%ebx\),%ymm0
[ 	]*[a-f0-9]+:	8f e8 7c a0 38 00[ 	]+vcvtph2ps \$0x0,\(%eax\),%ymm7
[ 	]*[a-f0-9]+:	8f e8 7c a0 ff ff[ 	]+vcvtph2ps \$0xff,%xmm7,%ymm7
[ 	]*[a-f0-9]+:	8f e8 7c a0 ed ff[ 	]+vcvtph2ps \$0xff,%xmm5,%ymm5
[ 	]*[a-f0-9]+:	8f e8 7c a0 2b ff[ 	]+vcvtph2ps \$0xff,\(%ebx\),%ymm5
[ 	]*[a-f0-9]+:	8f e8 7c a0 c7 ff[ 	]+vcvtph2ps \$0xff,%xmm7,%ymm0
[ 	]*[a-f0-9]+:	8f e8 7c a0 38 03[ 	]+vcvtph2ps \$0x3,\(%eax\),%ymm7
[ 	]*[a-f0-9]+:	8f e8 78 a1 2b 00[ 	]+vcvtps2ph \$0x0,%xmm5,\(%ebx\)
[ 	]*[a-f0-9]+:	8f e8 78 a1 3e 00[ 	]+vcvtps2ph \$0x0,%xmm7,\(%esi\)
[ 	]*[a-f0-9]+:	8f e8 78 a1 00 00[ 	]+vcvtps2ph \$0x0,%xmm0,\(%eax\)
[ 	]*[a-f0-9]+:	8f e8 78 a1 ea ff[ 	]+vcvtps2ph \$0xff,%xmm5,%xmm2
[ 	]*[a-f0-9]+:	8f e8 78 a1 c2 03[ 	]+vcvtps2ph \$0x3,%xmm0,%xmm2
[ 	]*[a-f0-9]+:	8f e8 78 a1 ea 03[ 	]+vcvtps2ph \$0x3,%xmm5,%xmm2
[ 	]*[a-f0-9]+:	8f e8 78 a1 c7 00[ 	]+vcvtps2ph \$0x0,%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e8 78 a1 06 00[ 	]+vcvtps2ph \$0x0,%xmm0,\(%esi\)
[ 	]*[a-f0-9]+:	8f e8 78 a1 f8 ff[ 	]+vcvtps2ph \$0xff,%xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 a1 3b 00[ 	]+vcvtps2ph \$0x0,%xmm7,\(%ebx\)
[ 	]*[a-f0-9]+:	8f e8 78 a1 2b ff[ 	]+vcvtps2ph \$0xff,%xmm5,\(%ebx\)
[ 	]*[a-f0-9]+:	8f e8 78 a1 00 ff[ 	]+vcvtps2ph \$0xff,%xmm0,\(%eax\)
[ 	]*[a-f0-9]+:	8f e8 78 a1 38 ff[ 	]+vcvtps2ph \$0xff,%xmm7,\(%eax\)
[ 	]*[a-f0-9]+:	8f e8 78 a1 3b 03[ 	]+vcvtps2ph \$0x3,%xmm7,\(%ebx\)
[ 	]*[a-f0-9]+:	8f e8 78 a1 28 03[ 	]+vcvtps2ph \$0x3,%xmm5,\(%eax\)
[ 	]*[a-f0-9]+:	8f e8 78 a1 ef ff[ 	]+vcvtps2ph \$0xff,%xmm5,%xmm7
[ 	]*[a-f0-9]+:	8f e8 7c a1 2b 00[ 	]+vcvtps2ph \$0x0,%ymm5,\(%ebx\)
[ 	]*[a-f0-9]+:	8f e8 7c a1 3e 00[ 	]+vcvtps2ph \$0x0,%ymm7,\(%esi\)
[ 	]*[a-f0-9]+:	8f e8 7c a1 00 00[ 	]+vcvtps2ph \$0x0,%ymm0,\(%eax\)
[ 	]*[a-f0-9]+:	8f e8 7c a1 ea ff[ 	]+vcvtps2ph \$0xff,%ymm5,%xmm2
[ 	]*[a-f0-9]+:	8f e8 7c a1 c2 03[ 	]+vcvtps2ph \$0x3,%ymm0,%xmm2
[ 	]*[a-f0-9]+:	8f e8 7c a1 ea 03[ 	]+vcvtps2ph \$0x3,%ymm5,%xmm2
[ 	]*[a-f0-9]+:	8f e8 7c a1 c7 00[ 	]+vcvtps2ph \$0x0,%ymm0,%xmm7
[ 	]*[a-f0-9]+:	8f e8 7c a1 06 00[ 	]+vcvtps2ph \$0x0,%ymm0,\(%esi\)
[ 	]*[a-f0-9]+:	8f e8 7c a1 f8 ff[ 	]+vcvtps2ph \$0xff,%ymm7,%xmm0
[ 	]*[a-f0-9]+:	8f e8 7c a1 3b 00[ 	]+vcvtps2ph \$0x0,%ymm7,\(%ebx\)
[ 	]*[a-f0-9]+:	8f e8 7c a1 2b ff[ 	]+vcvtps2ph \$0xff,%ymm5,\(%ebx\)
[ 	]*[a-f0-9]+:	8f e8 7c a1 00 ff[ 	]+vcvtps2ph \$0xff,%ymm0,\(%eax\)
[ 	]*[a-f0-9]+:	8f e8 7c a1 38 ff[ 	]+vcvtps2ph \$0xff,%ymm7,\(%eax\)
[ 	]*[a-f0-9]+:	8f e8 7c a1 3b 03[ 	]+vcvtps2ph \$0x3,%ymm7,\(%ebx\)
[ 	]*[a-f0-9]+:	8f e8 7c a1 28 03[ 	]+vcvtps2ph \$0x3,%ymm5,\(%eax\)
[ 	]*[a-f0-9]+:	8f e8 7c a1 ef ff[ 	]+vcvtps2ph \$0xff,%ymm5,%xmm7
#pass
