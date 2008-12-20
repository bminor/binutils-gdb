#as: -msse2avx
#objdump: -drwMintel,suffix
#name: encoding option with -msse2avx (Intel mode)
#source: opts.s

.*: +file format .*


Disassembly of section .text:

0+ <_start>:
[ 	]*[a-f0-9]+:	88 d1                	mov    cl,dl
[ 	]*[a-f0-9]+:	8a ca                	mov.s cl,dl
[ 	]*[a-f0-9]+:	66 89 d1             	mov    cx,dx
[ 	]*[a-f0-9]+:	66 8b ca             	mov.s cx,dx
[ 	]*[a-f0-9]+:	89 d1                	mov    ecx,edx
[ 	]*[a-f0-9]+:	8b ca                	mov.s ecx,edx
[ 	]*[a-f0-9]+:	88 d1                	mov    cl,dl
[ 	]*[a-f0-9]+:	8a ca                	mov.s cl,dl
[ 	]*[a-f0-9]+:	66 89 d1             	mov    cx,dx
[ 	]*[a-f0-9]+:	66 8b ca             	mov.s cx,dx
[ 	]*[a-f0-9]+:	89 d1                	mov    ecx,edx
[ 	]*[a-f0-9]+:	8b ca                	mov.s ecx,edx
[ 	]*[a-f0-9]+:	c5 fd 28 f4          	vmovapd ymm6,ymm4
[ 	]*[a-f0-9]+:	c5 fd 29 e6          	vmovapd.s ymm6,ymm4
[ 	]*[a-f0-9]+:	c5 fc 28 f4          	vmovaps ymm6,ymm4
[ 	]*[a-f0-9]+:	c5 fc 29 e6          	vmovaps.s ymm6,ymm4
[ 	]*[a-f0-9]+:	c5 fd 6f f4          	vmovdqa ymm6,ymm4
[ 	]*[a-f0-9]+:	c5 fd 7f e6          	vmovdqa.s ymm6,ymm4
[ 	]*[a-f0-9]+:	c5 fe 6f f4          	vmovdqu ymm6,ymm4
[ 	]*[a-f0-9]+:	c5 fe 7f e6          	vmovdqu.s ymm6,ymm4
[ 	]*[a-f0-9]+:	c5 fd 10 f4          	vmovupd ymm6,ymm4
[ 	]*[a-f0-9]+:	c5 fd 11 e6          	vmovupd.s ymm6,ymm4
[ 	]*[a-f0-9]+:	c5 fc 10 f4          	vmovups ymm6,ymm4
[ 	]*[a-f0-9]+:	c5 fc 11 e6          	vmovups.s ymm6,ymm4
[ 	]*[a-f0-9]+:	c5 f9 28 f4          	vmovapd xmm6,xmm4
[ 	]*[a-f0-9]+:	c5 f9 29 e6          	vmovapd.s xmm6,xmm4
[ 	]*[a-f0-9]+:	c5 f8 28 f4          	vmovaps xmm6,xmm4
[ 	]*[a-f0-9]+:	c5 f8 29 e6          	vmovaps.s xmm6,xmm4
[ 	]*[a-f0-9]+:	c5 f9 6f f4          	vmovdqa xmm6,xmm4
[ 	]*[a-f0-9]+:	c5 f9 7f e6          	vmovdqa.s xmm6,xmm4
[ 	]*[a-f0-9]+:	c5 fa 6f f4          	vmovdqu xmm6,xmm4
[ 	]*[a-f0-9]+:	c5 fa 7f e6          	vmovdqu.s xmm6,xmm4
[ 	]*[a-f0-9]+:	c5 fa 7e f4          	vmovq  xmm6,xmm4
[ 	]*[a-f0-9]+:	c5 f9 d6 e6          	vmovq.s xmm6,xmm4
[ 	]*[a-f0-9]+:	c5 f9 10 f4          	vmovupd xmm6,xmm4
[ 	]*[a-f0-9]+:	c5 f9 11 e6          	vmovupd.s xmm6,xmm4
[ 	]*[a-f0-9]+:	c5 f8 10 f4          	vmovups xmm6,xmm4
[ 	]*[a-f0-9]+:	c5 f8 11 e6          	vmovups.s xmm6,xmm4
[ 	]*[a-f0-9]+:	c5 f9 28 f4          	vmovapd xmm6,xmm4
[ 	]*[a-f0-9]+:	c5 f9 29 e6          	vmovapd.s xmm6,xmm4
[ 	]*[a-f0-9]+:	c5 f8 28 f4          	vmovaps xmm6,xmm4
[ 	]*[a-f0-9]+:	c5 f8 29 e6          	vmovaps.s xmm6,xmm4
[ 	]*[a-f0-9]+:	c5 f9 6f f4          	vmovdqa xmm6,xmm4
[ 	]*[a-f0-9]+:	c5 f9 7f e6          	vmovdqa.s xmm6,xmm4
[ 	]*[a-f0-9]+:	c5 fa 6f f4          	vmovdqu xmm6,xmm4
[ 	]*[a-f0-9]+:	c5 fa 7f e6          	vmovdqu.s xmm6,xmm4
[ 	]*[a-f0-9]+:	c5 fa 7e f4          	vmovq  xmm6,xmm4
[ 	]*[a-f0-9]+:	c5 f9 d6 e6          	vmovq.s xmm6,xmm4
[ 	]*[a-f0-9]+:	c5 f9 10 f4          	vmovupd xmm6,xmm4
[ 	]*[a-f0-9]+:	c5 f9 11 e6          	vmovupd.s xmm6,xmm4
[ 	]*[a-f0-9]+:	c5 f8 10 f4          	vmovups xmm6,xmm4
[ 	]*[a-f0-9]+:	c5 f8 11 e6          	vmovups.s xmm6,xmm4
[ 	]*[a-f0-9]+:	0f 6f e0             	movq   mm4,mm0
[ 	]*[a-f0-9]+:	0f 7f c4             	movq.s mm4,mm0
[ 	]*[a-f0-9]+:	88 d1                	mov    cl,dl
[ 	]*[a-f0-9]+:	8a ca                	mov.s cl,dl
[ 	]*[a-f0-9]+:	66 89 d1             	mov    cx,dx
[ 	]*[a-f0-9]+:	66 8b ca             	mov.s cx,dx
[ 	]*[a-f0-9]+:	89 d1                	mov    ecx,edx
[ 	]*[a-f0-9]+:	8b ca                	mov.s ecx,edx
[ 	]*[a-f0-9]+:	c5 fd 28 f4          	vmovapd ymm6,ymm4
[ 	]*[a-f0-9]+:	c5 fd 29 e6          	vmovapd.s ymm6,ymm4
[ 	]*[a-f0-9]+:	c5 fc 28 f4          	vmovaps ymm6,ymm4
[ 	]*[a-f0-9]+:	c5 fc 29 e6          	vmovaps.s ymm6,ymm4
[ 	]*[a-f0-9]+:	c5 fd 6f f4          	vmovdqa ymm6,ymm4
[ 	]*[a-f0-9]+:	c5 fd 7f e6          	vmovdqa.s ymm6,ymm4
[ 	]*[a-f0-9]+:	c5 fe 6f f4          	vmovdqu ymm6,ymm4
[ 	]*[a-f0-9]+:	c5 fe 7f e6          	vmovdqu.s ymm6,ymm4
[ 	]*[a-f0-9]+:	c5 fd 10 f4          	vmovupd ymm6,ymm4
[ 	]*[a-f0-9]+:	c5 fd 11 e6          	vmovupd.s ymm6,ymm4
[ 	]*[a-f0-9]+:	c5 fc 10 f4          	vmovups ymm6,ymm4
[ 	]*[a-f0-9]+:	c5 fc 11 e6          	vmovups.s ymm6,ymm4
[ 	]*[a-f0-9]+:	c5 f9 28 f4          	vmovapd xmm6,xmm4
[ 	]*[a-f0-9]+:	c5 f9 29 e6          	vmovapd.s xmm6,xmm4
[ 	]*[a-f0-9]+:	c5 f8 28 f4          	vmovaps xmm6,xmm4
[ 	]*[a-f0-9]+:	c5 f8 29 e6          	vmovaps.s xmm6,xmm4
[ 	]*[a-f0-9]+:	c5 f9 6f f4          	vmovdqa xmm6,xmm4
[ 	]*[a-f0-9]+:	c5 f9 7f e6          	vmovdqa.s xmm6,xmm4
[ 	]*[a-f0-9]+:	c5 fa 6f f4          	vmovdqu xmm6,xmm4
[ 	]*[a-f0-9]+:	c5 fa 7f e6          	vmovdqu.s xmm6,xmm4
[ 	]*[a-f0-9]+:	c5 fa 7e f4          	vmovq  xmm6,xmm4
[ 	]*[a-f0-9]+:	c5 f9 d6 e6          	vmovq.s xmm6,xmm4
[ 	]*[a-f0-9]+:	c5 f9 10 f4          	vmovupd xmm6,xmm4
[ 	]*[a-f0-9]+:	c5 f9 11 e6          	vmovupd.s xmm6,xmm4
[ 	]*[a-f0-9]+:	c5 f8 10 f4          	vmovups xmm6,xmm4
[ 	]*[a-f0-9]+:	c5 f8 11 e6          	vmovups.s xmm6,xmm4
[ 	]*[a-f0-9]+:	c5 f9 28 f4          	vmovapd xmm6,xmm4
[ 	]*[a-f0-9]+:	c5 f9 29 e6          	vmovapd.s xmm6,xmm4
[ 	]*[a-f0-9]+:	c5 f8 28 f4          	vmovaps xmm6,xmm4
[ 	]*[a-f0-9]+:	c5 f8 29 e6          	vmovaps.s xmm6,xmm4
[ 	]*[a-f0-9]+:	c5 f9 6f f4          	vmovdqa xmm6,xmm4
[ 	]*[a-f0-9]+:	c5 f9 7f e6          	vmovdqa.s xmm6,xmm4
[ 	]*[a-f0-9]+:	c5 fa 6f f4          	vmovdqu xmm6,xmm4
[ 	]*[a-f0-9]+:	c5 fa 7f e6          	vmovdqu.s xmm6,xmm4
[ 	]*[a-f0-9]+:	c5 fa 7e f4          	vmovq  xmm6,xmm4
[ 	]*[a-f0-9]+:	c5 f9 d6 e6          	vmovq.s xmm6,xmm4
[ 	]*[a-f0-9]+:	c5 f9 10 f4          	vmovupd xmm6,xmm4
[ 	]*[a-f0-9]+:	c5 f9 11 e6          	vmovupd.s xmm6,xmm4
[ 	]*[a-f0-9]+:	c5 f8 10 f4          	vmovups xmm6,xmm4
[ 	]*[a-f0-9]+:	c5 f8 11 e6          	vmovups.s xmm6,xmm4
[ 	]*[a-f0-9]+:	0f 6f e0             	movq   mm4,mm0
[ 	]*[a-f0-9]+:	0f 7f c4             	movq.s mm4,mm0
#pass
