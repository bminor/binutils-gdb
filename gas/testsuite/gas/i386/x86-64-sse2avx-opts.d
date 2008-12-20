#as: -msse2avx
#objdump: -drwMsuffix
#name: x86-64 encoding option with -msse2avx
#source: x86-64-opts.s

.*: +file format .*


Disassembly of section .text:

0+ <_start>:
[ 	]*[a-f0-9]+:	88 d1                	movb   %dl,%cl
[ 	]*[a-f0-9]+:	8a ca                	movb.s %dl,%cl
[ 	]*[a-f0-9]+:	66 89 d1             	movw   %dx,%cx
[ 	]*[a-f0-9]+:	66 8b ca             	movw.s %dx,%cx
[ 	]*[a-f0-9]+:	89 d1                	movl   %edx,%ecx
[ 	]*[a-f0-9]+:	8b ca                	movl.s %edx,%ecx
[ 	]*[a-f0-9]+:	88 d1                	movb   %dl,%cl
[ 	]*[a-f0-9]+:	8a ca                	movb.s %dl,%cl
[ 	]*[a-f0-9]+:	66 89 d1             	movw   %dx,%cx
[ 	]*[a-f0-9]+:	66 8b ca             	movw.s %dx,%cx
[ 	]*[a-f0-9]+:	89 d1                	movl   %edx,%ecx
[ 	]*[a-f0-9]+:	8b ca                	movl.s %edx,%ecx
[ 	]*[a-f0-9]+:	48 89 d1             	movq   %rdx,%rcx
[ 	]*[a-f0-9]+:	48 8b ca             	movq.s %rdx,%rcx
[ 	]*[a-f0-9]+:	48 89 d1             	movq   %rdx,%rcx
[ 	]*[a-f0-9]+:	48 8b ca             	movq.s %rdx,%rcx
[ 	]*[a-f0-9]+:	c5 fd 28 f4          	vmovapd %ymm4,%ymm6
[ 	]*[a-f0-9]+:	c5 fd 29 e6          	vmovapd.s %ymm4,%ymm6
[ 	]*[a-f0-9]+:	c5 fc 28 f4          	vmovaps %ymm4,%ymm6
[ 	]*[a-f0-9]+:	c5 fc 29 e6          	vmovaps.s %ymm4,%ymm6
[ 	]*[a-f0-9]+:	c5 fd 6f f4          	vmovdqa %ymm4,%ymm6
[ 	]*[a-f0-9]+:	c5 fd 7f e6          	vmovdqa.s %ymm4,%ymm6
[ 	]*[a-f0-9]+:	c5 fe 6f f4          	vmovdqu %ymm4,%ymm6
[ 	]*[a-f0-9]+:	c5 fe 7f e6          	vmovdqu.s %ymm4,%ymm6
[ 	]*[a-f0-9]+:	c5 fd 10 f4          	vmovupd %ymm4,%ymm6
[ 	]*[a-f0-9]+:	c5 fd 11 e6          	vmovupd.s %ymm4,%ymm6
[ 	]*[a-f0-9]+:	c5 fc 10 f4          	vmovups %ymm4,%ymm6
[ 	]*[a-f0-9]+:	c5 fc 11 e6          	vmovups.s %ymm4,%ymm6
[ 	]*[a-f0-9]+:	c5 f9 28 f4          	vmovapd %xmm4,%xmm6
[ 	]*[a-f0-9]+:	c5 f9 29 e6          	vmovapd.s %xmm4,%xmm6
[ 	]*[a-f0-9]+:	c5 f8 28 f4          	vmovaps %xmm4,%xmm6
[ 	]*[a-f0-9]+:	c5 f8 29 e6          	vmovaps.s %xmm4,%xmm6
[ 	]*[a-f0-9]+:	c5 f9 6f f4          	vmovdqa %xmm4,%xmm6
[ 	]*[a-f0-9]+:	c5 f9 7f e6          	vmovdqa.s %xmm4,%xmm6
[ 	]*[a-f0-9]+:	c5 fa 6f f4          	vmovdqu %xmm4,%xmm6
[ 	]*[a-f0-9]+:	c5 fa 7f e6          	vmovdqu.s %xmm4,%xmm6
[ 	]*[a-f0-9]+:	c5 fa 7e f4          	vmovq  %xmm4,%xmm6
[ 	]*[a-f0-9]+:	c5 f9 d6 e6          	vmovq.s %xmm4,%xmm6
[ 	]*[a-f0-9]+:	c5 f9 10 f4          	vmovupd %xmm4,%xmm6
[ 	]*[a-f0-9]+:	c5 f9 11 e6          	vmovupd.s %xmm4,%xmm6
[ 	]*[a-f0-9]+:	c5 f8 10 f4          	vmovups %xmm4,%xmm6
[ 	]*[a-f0-9]+:	c5 f8 11 e6          	vmovups.s %xmm4,%xmm6
[ 	]*[a-f0-9]+:	c5 f9 28 f4          	vmovapd %xmm4,%xmm6
[ 	]*[a-f0-9]+:	c5 f9 29 e6          	vmovapd.s %xmm4,%xmm6
[ 	]*[a-f0-9]+:	c5 f8 28 f4          	vmovaps %xmm4,%xmm6
[ 	]*[a-f0-9]+:	c5 f8 29 e6          	vmovaps.s %xmm4,%xmm6
[ 	]*[a-f0-9]+:	c5 f9 6f f4          	vmovdqa %xmm4,%xmm6
[ 	]*[a-f0-9]+:	c5 f9 7f e6          	vmovdqa.s %xmm4,%xmm6
[ 	]*[a-f0-9]+:	c5 fa 6f f4          	vmovdqu %xmm4,%xmm6
[ 	]*[a-f0-9]+:	c5 fa 7f e6          	vmovdqu.s %xmm4,%xmm6
[ 	]*[a-f0-9]+:	c5 fa 7e f4          	vmovq  %xmm4,%xmm6
[ 	]*[a-f0-9]+:	c5 f9 d6 e6          	vmovq.s %xmm4,%xmm6
[ 	]*[a-f0-9]+:	c5 f9 10 f4          	vmovupd %xmm4,%xmm6
[ 	]*[a-f0-9]+:	c5 f9 11 e6          	vmovupd.s %xmm4,%xmm6
[ 	]*[a-f0-9]+:	c5 f8 10 f4          	vmovups %xmm4,%xmm6
[ 	]*[a-f0-9]+:	c5 f8 11 e6          	vmovups.s %xmm4,%xmm6
[ 	]*[a-f0-9]+:	0f 6f e0             	movq   %mm0,%mm4
[ 	]*[a-f0-9]+:	0f 7f c4             	movq.s %mm0,%mm4
[ 	]*[a-f0-9]+:	88 d1                	movb   %dl,%cl
[ 	]*[a-f0-9]+:	8a ca                	movb.s %dl,%cl
[ 	]*[a-f0-9]+:	66 89 d1             	movw   %dx,%cx
[ 	]*[a-f0-9]+:	66 8b ca             	movw.s %dx,%cx
[ 	]*[a-f0-9]+:	89 d1                	movl   %edx,%ecx
[ 	]*[a-f0-9]+:	8b ca                	movl.s %edx,%ecx
[ 	]*[a-f0-9]+:	48 89 d1             	movq   %rdx,%rcx
[ 	]*[a-f0-9]+:	48 8b ca             	movq.s %rdx,%rcx
[ 	]*[a-f0-9]+:	c5 fd 28 f4          	vmovapd %ymm4,%ymm6
[ 	]*[a-f0-9]+:	c5 fd 29 e6          	vmovapd.s %ymm4,%ymm6
[ 	]*[a-f0-9]+:	c5 fc 28 f4          	vmovaps %ymm4,%ymm6
[ 	]*[a-f0-9]+:	c5 fc 29 e6          	vmovaps.s %ymm4,%ymm6
[ 	]*[a-f0-9]+:	c5 fd 6f f4          	vmovdqa %ymm4,%ymm6
[ 	]*[a-f0-9]+:	c5 fd 7f e6          	vmovdqa.s %ymm4,%ymm6
[ 	]*[a-f0-9]+:	c5 fe 6f f4          	vmovdqu %ymm4,%ymm6
[ 	]*[a-f0-9]+:	c5 fe 7f e6          	vmovdqu.s %ymm4,%ymm6
[ 	]*[a-f0-9]+:	c5 fd 10 f4          	vmovupd %ymm4,%ymm6
[ 	]*[a-f0-9]+:	c5 fd 11 e6          	vmovupd.s %ymm4,%ymm6
[ 	]*[a-f0-9]+:	c5 fc 10 f4          	vmovups %ymm4,%ymm6
[ 	]*[a-f0-9]+:	c5 fc 11 e6          	vmovups.s %ymm4,%ymm6
[ 	]*[a-f0-9]+:	c5 f9 28 f4          	vmovapd %xmm4,%xmm6
[ 	]*[a-f0-9]+:	c5 f9 29 e6          	vmovapd.s %xmm4,%xmm6
[ 	]*[a-f0-9]+:	c5 f8 28 f4          	vmovaps %xmm4,%xmm6
[ 	]*[a-f0-9]+:	c5 f8 29 e6          	vmovaps.s %xmm4,%xmm6
[ 	]*[a-f0-9]+:	c5 f9 6f f4          	vmovdqa %xmm4,%xmm6
[ 	]*[a-f0-9]+:	c5 f9 7f e6          	vmovdqa.s %xmm4,%xmm6
[ 	]*[a-f0-9]+:	c5 fa 6f f4          	vmovdqu %xmm4,%xmm6
[ 	]*[a-f0-9]+:	c5 fa 7f e6          	vmovdqu.s %xmm4,%xmm6
[ 	]*[a-f0-9]+:	c5 fa 7e f4          	vmovq  %xmm4,%xmm6
[ 	]*[a-f0-9]+:	c5 f9 d6 e6          	vmovq.s %xmm4,%xmm6
[ 	]*[a-f0-9]+:	c5 f9 10 f4          	vmovupd %xmm4,%xmm6
[ 	]*[a-f0-9]+:	c5 f9 11 e6          	vmovupd.s %xmm4,%xmm6
[ 	]*[a-f0-9]+:	c5 f8 10 f4          	vmovups %xmm4,%xmm6
[ 	]*[a-f0-9]+:	c5 f8 11 e6          	vmovups.s %xmm4,%xmm6
[ 	]*[a-f0-9]+:	c5 f9 28 f4          	vmovapd %xmm4,%xmm6
[ 	]*[a-f0-9]+:	c5 f9 29 e6          	vmovapd.s %xmm4,%xmm6
[ 	]*[a-f0-9]+:	c5 f8 28 f4          	vmovaps %xmm4,%xmm6
[ 	]*[a-f0-9]+:	c5 f8 29 e6          	vmovaps.s %xmm4,%xmm6
[ 	]*[a-f0-9]+:	c5 f9 6f f4          	vmovdqa %xmm4,%xmm6
[ 	]*[a-f0-9]+:	c5 f9 7f e6          	vmovdqa.s %xmm4,%xmm6
[ 	]*[a-f0-9]+:	c5 fa 6f f4          	vmovdqu %xmm4,%xmm6
[ 	]*[a-f0-9]+:	c5 fa 7f e6          	vmovdqu.s %xmm4,%xmm6
[ 	]*[a-f0-9]+:	c5 fa 7e f4          	vmovq  %xmm4,%xmm6
[ 	]*[a-f0-9]+:	c5 f9 d6 e6          	vmovq.s %xmm4,%xmm6
[ 	]*[a-f0-9]+:	c5 f9 10 f4          	vmovupd %xmm4,%xmm6
[ 	]*[a-f0-9]+:	c5 f9 11 e6          	vmovupd.s %xmm4,%xmm6
[ 	]*[a-f0-9]+:	c5 f8 10 f4          	vmovups %xmm4,%xmm6
[ 	]*[a-f0-9]+:	c5 f8 11 e6          	vmovups.s %xmm4,%xmm6
[ 	]*[a-f0-9]+:	0f 6f e0             	movq   %mm0,%mm4
[ 	]*[a-f0-9]+:	0f 7f c4             	movq.s %mm0,%mm4
#pass
