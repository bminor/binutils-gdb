#objdump: -dw
#name: ix86 no extended registers

.*:     file format .*

Disassembly of section .text:

0+ <ix86>:
[ 	]*[a-f0-9]+:	c4 e2 78 f2 00       	andn   \(%eax\),%eax,%eax
[ 	]*[a-f0-9]+:	c4 e2 f8 f2 00       	andn   \(%eax\),%eax,%eax
[ 	]*[a-f0-9]+:	8f e9 78 01 20       	tzmsk  \(%eax\),%eax
[ 	]*[a-f0-9]+:	8f e9 f8 01 20       	tzmsk  \(%eax\),%eax
[ 	]*[a-f0-9]+:	8f e9 78 12 c0       	llwpcb %eax
[ 	]*[a-f0-9]+:	8f e9 f8 12 c0       	llwpcb %eax
[ 	]*[a-f0-9]+:	c4 e3 79 68 00 00    	vfmaddps %xmm0,\(%eax\),%xmm0,%xmm0
[ 	]*[a-f0-9]+:	c4 e3 79 68 00 0f    	vfmaddps %xmm0,\(%eax\),%xmm0,%xmm0
[ 	]*[a-f0-9]+:	c3                   	ret[ 	]*
#pass
