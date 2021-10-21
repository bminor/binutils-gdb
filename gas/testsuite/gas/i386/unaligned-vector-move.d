#as: -muse-unaligned-vector-move
#objdump: -dw
#name: i386 (Encode aligned vector move as unaligned vector move)

.*: +file format .*


Disassembly of section .text:

0+ <_start>:
 +[a-f0-9]+:	0f 10 d1             	movups %xmm1,%xmm2
 +[a-f0-9]+:	66 0f 10 d1          	movupd %xmm1,%xmm2
 +[a-f0-9]+:	f3 0f 6f d1          	movdqu %xmm1,%xmm2
 +[a-f0-9]+:	c5 f8 10 d1          	vmovups %xmm1,%xmm2
 +[a-f0-9]+:	c5 f9 10 d1          	vmovupd %xmm1,%xmm2
 +[a-f0-9]+:	c5 fa 6f d1          	vmovdqu %xmm1,%xmm2
 +[a-f0-9]+:	c5 f8 10 d1          	vmovups %xmm1,%xmm2
 +[a-f0-9]+:	62 f1 fd 09 10 d1    	vmovupd %xmm1,%xmm2\{%k1\}
 +[a-f0-9]+:	62 f1 7c 09 10 d1    	vmovups %xmm1,%xmm2\{%k1\}
 +[a-f0-9]+:	62 f1 7e 09 6f d1    	vmovdqu32 %xmm1,%xmm2\{%k1\}
 +[a-f0-9]+:	62 f1 fe 09 6f d1    	vmovdqu64 %xmm1,%xmm2\{%k1\}
#pass
