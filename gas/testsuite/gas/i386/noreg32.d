#objdump: -dw
#name: 32-bit insns not sizeable through register operands

.*: +file format .*

Disassembly of section .text:

0+ <noreg>:
 *[a-f0-9]+:	83 10 01             	adcl   \$0x1,\(%eax\)
 *[a-f0-9]+:	83 00 01             	addl   \$0x1,\(%eax\)
 *[a-f0-9]+:	83 20 01             	andl   \$0x1,\(%eax\)
 *[a-f0-9]+:	0f ba 20 01          	btl    \$0x1,\(%eax\)
 *[a-f0-9]+:	0f ba 38 01          	btcl   \$0x1,\(%eax\)
 *[a-f0-9]+:	0f ba 30 01          	btrl   \$0x1,\(%eax\)
 *[a-f0-9]+:	0f ba 28 01          	btsl   \$0x1,\(%eax\)
 *[a-f0-9]+:	ff 10                	call   \*\(%eax\)
 *[a-f0-9]+:	83 38 01             	cmpl   \$0x1,\(%eax\)
 *[a-f0-9]+:	f2 0f 2a 00          	cvtsi2sdl \(%eax\),%xmm0
 *[a-f0-9]+:	f3 0f 2a 00          	cvtsi2ssl \(%eax\),%xmm0
 *[a-f0-9]+:	d8 00                	fadds  \(%eax\)
 *[a-f0-9]+:	d8 10                	fcoms  \(%eax\)
 *[a-f0-9]+:	d8 18                	fcomps \(%eax\)
 *[a-f0-9]+:	d8 30                	fdivs  \(%eax\)
 *[a-f0-9]+:	d8 38                	fdivrs \(%eax\)
 *[a-f0-9]+:	de 00                	fiadds \(%eax\)
 *[a-f0-9]+:	de 10                	ficoms \(%eax\)
 *[a-f0-9]+:	de 18                	ficomps \(%eax\)
 *[a-f0-9]+:	de 30                	fidivs \(%eax\)
 *[a-f0-9]+:	de 38                	fidivrs \(%eax\)
 *[a-f0-9]+:	df 00                	filds  \(%eax\)
 *[a-f0-9]+:	de 08                	fimuls \(%eax\)
 *[a-f0-9]+:	df 10                	fists  \(%eax\)
 *[a-f0-9]+:	df 18                	fistps \(%eax\)
 *[a-f0-9]+:	df 08                	fisttps \(%eax\)
 *[a-f0-9]+:	de 20                	fisubs \(%eax\)
 *[a-f0-9]+:	de 28                	fisubrs \(%eax\)
 *[a-f0-9]+:	d9 00                	flds   \(%eax\)
 *[a-f0-9]+:	d8 08                	fmuls  \(%eax\)
 *[a-f0-9]+:	d9 10                	fsts   \(%eax\)
 *[a-f0-9]+:	d9 18                	fstps  \(%eax\)
 *[a-f0-9]+:	d8 20                	fsubs  \(%eax\)
 *[a-f0-9]+:	d8 28                	fsubrs \(%eax\)
 *[a-f0-9]+:	ff 20                	jmp    \*\(%eax\)
 *[a-f0-9]+:	0f 01 10             	lgdtl  \(%eax\)
 *[a-f0-9]+:	0f 01 18             	lidtl  \(%eax\)
 *[a-f0-9]+:	0f 00 10             	lldt   \(%eax\)
 *[a-f0-9]+:	0f 01 30             	lmsw   \(%eax\)
 *[a-f0-9]+:	0f 00 18             	ltr    \(%eax\)
 *[a-f0-9]+:	8c 00                	mov    %es,\(%eax\)
 *[a-f0-9]+:	8e 00                	mov    \(%eax\),%es
 *[a-f0-9]+:	0f 1f 00             	nopl   \(%eax\)
 *[a-f0-9]+:	83 08 01             	orl    \$0x1,\(%eax\)
 *[a-f0-9]+:	8f 00                	popl   \(%eax\)
 *[a-f0-9]+:	07                   	pop    %es
 *[a-f0-9]+:	f3 0f ae 20          	ptwritel \(%eax\)
 *[a-f0-9]+:	ff 30                	pushl  \(%eax\)
 *[a-f0-9]+:	06                   	push   %es
 *[a-f0-9]+:	83 18 01             	sbbl   \$0x1,\(%eax\)
 *[a-f0-9]+:	83 28 01             	subl   \$0x1,\(%eax\)
 *[a-f0-9]+:	c5 fb 2a 00          	vcvtsi2sdl \(%eax\),%xmm0,%xmm0
 *[a-f0-9]+:	62 f1 7f 08 2a 00    	vcvtsi2sdl \(%eax\),%xmm0,%xmm0
 *[a-f0-9]+:	c5 fa 2a 00          	vcvtsi2ssl \(%eax\),%xmm0,%xmm0
 *[a-f0-9]+:	62 f1 7e 08 2a 00    	vcvtsi2ssl \(%eax\),%xmm0,%xmm0
 *[a-f0-9]+:	62 f1 7f 08 7b 00    	vcvtusi2sdl \(%eax\),%xmm0,%xmm0
 *[a-f0-9]+:	62 f1 7e 08 7b 00    	vcvtusi2ssl \(%eax\),%xmm0,%xmm0
 *[a-f0-9]+:	83 30 01             	xorl   \$0x1,\(%eax\)
#pass
