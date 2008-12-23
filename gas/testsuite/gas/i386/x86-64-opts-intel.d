#objdump: -drwMintel,suffix
#name: x86-64 encoding option (Intel mode)
#source: x86-64-opts.s

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
[ 	]*[a-f0-9]+:	48 89 d1             	mov    rcx,rdx
[ 	]*[a-f0-9]+:	48 8b ca             	mov.s rcx,rdx
[ 	]*[a-f0-9]+:	48 89 d1             	mov    rcx,rdx
[ 	]*[a-f0-9]+:	48 8b ca             	mov.s rcx,rdx
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
[ 	]*[a-f0-9]+:	66 0f 28 f4          	movapd xmm6,xmm4
[ 	]*[a-f0-9]+:	66 0f 29 e6          	movapd.s xmm6,xmm4
[ 	]*[a-f0-9]+:	0f 28 f4             	movaps xmm6,xmm4
[ 	]*[a-f0-9]+:	0f 29 e6             	movaps.s xmm6,xmm4
[ 	]*[a-f0-9]+:	66 0f 6f f4          	movdqa xmm6,xmm4
[ 	]*[a-f0-9]+:	66 0f 7f e6          	movdqa.s xmm6,xmm4
[ 	]*[a-f0-9]+:	f3 0f 6f f4          	movdqu xmm6,xmm4
[ 	]*[a-f0-9]+:	f3 0f 7f e6          	movdqu.s xmm6,xmm4
[ 	]*[a-f0-9]+:	f3 0f 7e f4          	movq   xmm6,xmm4
[ 	]*[a-f0-9]+:	66 0f d6 e6          	movq.s xmm6,xmm4
[ 	]*[a-f0-9]+:	f2 0f 10 f4          	movsd  xmm6,xmm4
[ 	]*[a-f0-9]+:	f2 0f 11 e6          	movsd.s xmm6,xmm4
[ 	]*[a-f0-9]+:	f3 0f 10 f4          	movss  xmm6,xmm4
[ 	]*[a-f0-9]+:	f3 0f 11 e6          	movss.s xmm6,xmm4
[ 	]*[a-f0-9]+:	66 0f 10 f4          	movupd xmm6,xmm4
[ 	]*[a-f0-9]+:	66 0f 11 e6          	movupd.s xmm6,xmm4
[ 	]*[a-f0-9]+:	0f 10 f4             	movups xmm6,xmm4
[ 	]*[a-f0-9]+:	0f 11 e6             	movups.s xmm6,xmm4
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
[ 	]*[a-f0-9]+:	c5 cb 10 d4          	vmovsd xmm2,xmm6,xmm4
[ 	]*[a-f0-9]+:	c5 cb 11 e2          	vmovsd.s xmm2,xmm6,xmm4
[ 	]*[a-f0-9]+:	c5 ca 10 d4          	vmovss xmm2,xmm6,xmm4
[ 	]*[a-f0-9]+:	c5 ca 11 e2          	vmovss.s xmm2,xmm6,xmm4
[ 	]*[a-f0-9]+:	0f 6f e0             	movq   mm4,mm0
[ 	]*[a-f0-9]+:	0f 7f c4             	movq.s mm4,mm0
[ 	]*[a-f0-9]+:	88 d1                	mov    cl,dl
[ 	]*[a-f0-9]+:	8a ca                	mov.s cl,dl
[ 	]*[a-f0-9]+:	66 89 d1             	mov    cx,dx
[ 	]*[a-f0-9]+:	66 8b ca             	mov.s cx,dx
[ 	]*[a-f0-9]+:	89 d1                	mov    ecx,edx
[ 	]*[a-f0-9]+:	8b ca                	mov.s ecx,edx
[ 	]*[a-f0-9]+:	48 89 d1             	mov    rcx,rdx
[ 	]*[a-f0-9]+:	48 8b ca             	mov.s rcx,rdx
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
[ 	]*[a-f0-9]+:	66 0f 28 f4          	movapd xmm6,xmm4
[ 	]*[a-f0-9]+:	66 0f 29 e6          	movapd.s xmm6,xmm4
[ 	]*[a-f0-9]+:	0f 28 f4             	movaps xmm6,xmm4
[ 	]*[a-f0-9]+:	0f 29 e6             	movaps.s xmm6,xmm4
[ 	]*[a-f0-9]+:	66 0f 6f f4          	movdqa xmm6,xmm4
[ 	]*[a-f0-9]+:	66 0f 7f e6          	movdqa.s xmm6,xmm4
[ 	]*[a-f0-9]+:	f3 0f 6f f4          	movdqu xmm6,xmm4
[ 	]*[a-f0-9]+:	f3 0f 7f e6          	movdqu.s xmm6,xmm4
[ 	]*[a-f0-9]+:	f3 0f 7e f4          	movq   xmm6,xmm4
[ 	]*[a-f0-9]+:	66 0f d6 e6          	movq.s xmm6,xmm4
[ 	]*[a-f0-9]+:	f2 0f 10 f4          	movsd  xmm6,xmm4
[ 	]*[a-f0-9]+:	f2 0f 11 e6          	movsd.s xmm6,xmm4
[ 	]*[a-f0-9]+:	f3 0f 10 f4          	movss  xmm6,xmm4
[ 	]*[a-f0-9]+:	f3 0f 11 e6          	movss.s xmm6,xmm4
[ 	]*[a-f0-9]+:	66 0f 10 f4          	movupd xmm6,xmm4
[ 	]*[a-f0-9]+:	66 0f 11 e6          	movupd.s xmm6,xmm4
[ 	]*[a-f0-9]+:	0f 10 f4             	movups xmm6,xmm4
[ 	]*[a-f0-9]+:	0f 11 e6             	movups.s xmm6,xmm4
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
[ 	]*[a-f0-9]+:	c5 cb 10 d4          	vmovsd xmm2,xmm6,xmm4
[ 	]*[a-f0-9]+:	c5 cb 11 e2          	vmovsd.s xmm2,xmm6,xmm4
[ 	]*[a-f0-9]+:	c5 ca 10 d4          	vmovss xmm2,xmm6,xmm4
[ 	]*[a-f0-9]+:	c5 ca 11 e2          	vmovss.s xmm2,xmm6,xmm4
[ 	]*[a-f0-9]+:	0f 6f e0             	movq   mm4,mm0
[ 	]*[a-f0-9]+:	0f 7f c4             	movq.s mm4,mm0
#pass
