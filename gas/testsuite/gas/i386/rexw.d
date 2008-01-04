#objdump: -dw
#name: x86-64 REX.W optimization

.*: +file format .*

Disassembly of section .text:

0+ <foo>:
[ 	]*[a-f0-9]+:	66 0f 3a 17 c1 00    	extractps \$0x0,%xmm0,%ecx
[ 	]*[a-f0-9]+:	66 0f 50 ca          	movmskpd %xmm2,%ecx
[ 	]*[a-f0-9]+:	0f 50 ca             	movmskps %xmm2,%ecx
[ 	]*[a-f0-9]+:	66 0f 3a 14 c1 00    	pextrb \$0x0,%xmm0,%ecx
[ 	]*[a-f0-9]+:	0f c5 c8 00          	pextrw \$0x0,%mm0,%ecx
[ 	]*[a-f0-9]+:	66 0f c5 c8 00       	pextrw \$0x0,%xmm0,%ecx
[ 	]*[a-f0-9]+:	66 0f 3a 20 c1 00    	pinsrb \$0x0,%ecx,%xmm0
[ 	]*[a-f0-9]+:	66 0f c4 c1 00       	pinsrw \$0x0,%ecx,%xmm0
[ 	]*[a-f0-9]+:	0f c4 c1 00          	pinsrw \$0x0,%ecx,%mm0
[ 	]*[a-f0-9]+:	0f d7 c5             	pmovmskb %mm5,%eax
[ 	]*[a-f0-9]+:	66 0f d7 c5          	pmovmskb %xmm5,%eax
[ 	]*[a-f0-9]+:	66 0f 3a 17 c1 00    	extractps \$0x0,%xmm0,%ecx
[ 	]*[a-f0-9]+:	66 0f 50 ca          	movmskpd %xmm2,%ecx
[ 	]*[a-f0-9]+:	0f 50 ca             	movmskps %xmm2,%ecx
[ 	]*[a-f0-9]+:	66 0f 3a 14 c1 00    	pextrb \$0x0,%xmm0,%ecx
[ 	]*[a-f0-9]+:	0f c5 c8 00          	pextrw \$0x0,%mm0,%ecx
[ 	]*[a-f0-9]+:	66 0f c5 c8 00       	pextrw \$0x0,%xmm0,%ecx
[ 	]*[a-f0-9]+:	66 0f 3a 20 c1 00    	pinsrb \$0x0,%ecx,%xmm0
[ 	]*[a-f0-9]+:	0f c4 c1 00          	pinsrw \$0x0,%ecx,%mm0
[ 	]*[a-f0-9]+:	66 0f c4 c1 00       	pinsrw \$0x0,%ecx,%xmm0
[ 	]*[a-f0-9]+:	0f d7 cd             	pmovmskb %mm5,%ecx
[ 	]*[a-f0-9]+:	66 0f d7 cd          	pmovmskb %xmm5,%ecx
#pass
