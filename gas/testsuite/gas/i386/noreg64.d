#objdump: -dw
#name: 64-bit insns not sizeable through register operands

.*: +file format .*

Disassembly of section .text:

0+ <noreg>:
 *[a-f0-9]+:	83 10 01             	adcl   \$0x1,\(%rax\)
 *[a-f0-9]+:	83 00 01             	addl   \$0x1,\(%rax\)
 *[a-f0-9]+:	83 20 01             	andl   \$0x1,\(%rax\)
 *[a-f0-9]+:	0f ba 20 01          	btl    \$0x1,\(%rax\)
 *[a-f0-9]+:	0f ba 38 01          	btcl   \$0x1,\(%rax\)
 *[a-f0-9]+:	0f ba 30 01          	btrl   \$0x1,\(%rax\)
 *[a-f0-9]+:	0f ba 28 01          	btsl   \$0x1,\(%rax\)
 *[a-f0-9]+:	ff 10                	callq  \*\(%rax\)
 *[a-f0-9]+:	83 38 01             	cmpl   \$0x1,\(%rax\)
 *[a-f0-9]+:	f2 0f 2a 00          	cvtsi2sdl \(%rax\),%xmm0
 *[a-f0-9]+:	f3 0f 2a 00          	cvtsi2ssl \(%rax\),%xmm0
 *[a-f0-9]+:	d8 00                	fadds  \(%rax\)
 *[a-f0-9]+:	d8 10                	fcoms  \(%rax\)
 *[a-f0-9]+:	d8 18                	fcomps \(%rax\)
 *[a-f0-9]+:	d8 30                	fdivs  \(%rax\)
 *[a-f0-9]+:	d8 38                	fdivrs \(%rax\)
 *[a-f0-9]+:	de 00                	fiadds \(%rax\)
 *[a-f0-9]+:	de 10                	ficoms \(%rax\)
 *[a-f0-9]+:	de 18                	ficomps \(%rax\)
 *[a-f0-9]+:	de 30                	fidivs \(%rax\)
 *[a-f0-9]+:	de 38                	fidivrs \(%rax\)
 *[a-f0-9]+:	df 00                	filds  \(%rax\)
 *[a-f0-9]+:	de 08                	fimuls \(%rax\)
 *[a-f0-9]+:	df 10                	fists  \(%rax\)
 *[a-f0-9]+:	df 18                	fistps \(%rax\)
 *[a-f0-9]+:	df 08                	fisttps \(%rax\)
 *[a-f0-9]+:	de 20                	fisubs \(%rax\)
 *[a-f0-9]+:	de 28                	fisubrs \(%rax\)
 *[a-f0-9]+:	d9 00                	flds   \(%rax\)
 *[a-f0-9]+:	d8 08                	fmuls  \(%rax\)
 *[a-f0-9]+:	d9 10                	fsts   \(%rax\)
 *[a-f0-9]+:	d9 18                	fstps  \(%rax\)
 *[a-f0-9]+:	d8 20                	fsubs  \(%rax\)
 *[a-f0-9]+:	d8 28                	fsubrs \(%rax\)
 *[a-f0-9]+:	ff 20                	jmpq   \*\(%rax\)
 *[a-f0-9]+:	0f 01 10             	lgdt   \(%rax\)
 *[a-f0-9]+:	0f 01 18             	lidt   \(%rax\)
 *[a-f0-9]+:	0f 00 10             	lldt   \(%rax\)
 *[a-f0-9]+:	0f 01 30             	lmsw   \(%rax\)
 *[a-f0-9]+:	0f 00 18             	ltr    \(%rax\)
 *[a-f0-9]+:	8c 00                	mov    %es,\(%rax\)
 *[a-f0-9]+:	8e 00                	mov    \(%rax\),%es
 *[a-f0-9]+:	0f 1f 00             	nopl   \(%rax\)
 *[a-f0-9]+:	83 08 01             	orl    \$0x1,\(%rax\)
 *[a-f0-9]+:	8f 00                	popq   \(%rax\)
 *[a-f0-9]+:	0f a1                	popq   %fs
 *[a-f0-9]+:	f3 0f ae 20          	ptwritel \(%rax\)
 *[a-f0-9]+:	ff 30                	pushq  \(%rax\)
 *[a-f0-9]+:	0f a0                	pushq  %fs
 *[a-f0-9]+:	83 18 01             	sbbl   \$0x1,\(%rax\)
 *[a-f0-9]+:	83 28 01             	subl   \$0x1,\(%rax\)
 *[a-f0-9]+:	c5 fb 2a 00          	vcvtsi2sdl \(%rax\),%xmm0,%xmm0
 *[a-f0-9]+:	62 61 7f 08 2a 38    	vcvtsi2sdl \(%rax\),%xmm0,%xmm31
 *[a-f0-9]+:	c5 fa 2a 00          	vcvtsi2ssl \(%rax\),%xmm0,%xmm0
 *[a-f0-9]+:	62 61 7e 08 2a 38    	vcvtsi2ssl \(%rax\),%xmm0,%xmm31
 *[a-f0-9]+:	62 f1 7f 08 7b 00    	vcvtusi2sdl \(%rax\),%xmm0,%xmm0
 *[a-f0-9]+:	62 f1 7e 08 7b 00    	vcvtusi2ssl \(%rax\),%xmm0,%xmm0
 *[a-f0-9]+:	83 30 01             	xorl   \$0x1,\(%rax\)
#pass
