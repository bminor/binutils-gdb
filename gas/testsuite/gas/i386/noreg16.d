#objdump: -dwMi8086
#name: 16-bit insns not sizeable through register operands

.*: +file format .*

Disassembly of section .text:

0+ <noreg>:
 *[a-f0-9]+:	83 17 01             	adcw   \$0x1,\(%bx\)
 *[a-f0-9]+:	83 07 01             	addw   \$0x1,\(%bx\)
 *[a-f0-9]+:	83 27 01             	andw   \$0x1,\(%bx\)
 *[a-f0-9]+:	0f ba 27 01          	btw    \$0x1,\(%bx\)
 *[a-f0-9]+:	0f ba 3f 01          	btcw   \$0x1,\(%bx\)
 *[a-f0-9]+:	0f ba 37 01          	btrw   \$0x1,\(%bx\)
 *[a-f0-9]+:	0f ba 2f 01          	btsw   \$0x1,\(%bx\)
 *[a-f0-9]+:	ff 17                	call   \*\(%bx\)
 *[a-f0-9]+:	83 3f 01             	cmpw   \$0x1,\(%bx\)
 *[a-f0-9]+:	f2 0f 2a 07          	cvtsi2sdl \(%bx\),%xmm0
 *[a-f0-9]+:	f3 0f 2a 07          	cvtsi2ssl \(%bx\),%xmm0
 *[a-f0-9]+:	d8 07                	fadds  \(%bx\)
 *[a-f0-9]+:	d8 17                	fcoms  \(%bx\)
 *[a-f0-9]+:	d8 1f                	fcomps \(%bx\)
 *[a-f0-9]+:	d8 37                	fdivs  \(%bx\)
 *[a-f0-9]+:	d8 3f                	fdivrs \(%bx\)
 *[a-f0-9]+:	de 07                	fiadds \(%bx\)
 *[a-f0-9]+:	de 17                	ficoms \(%bx\)
 *[a-f0-9]+:	de 1f                	ficomps \(%bx\)
 *[a-f0-9]+:	de 37                	fidivs \(%bx\)
 *[a-f0-9]+:	de 3f                	fidivrs \(%bx\)
 *[a-f0-9]+:	df 07                	filds  \(%bx\)
 *[a-f0-9]+:	de 0f                	fimuls \(%bx\)
 *[a-f0-9]+:	df 17                	fists  \(%bx\)
 *[a-f0-9]+:	df 1f                	fistps \(%bx\)
 *[a-f0-9]+:	df 0f                	fisttps \(%bx\)
 *[a-f0-9]+:	de 27                	fisubs \(%bx\)
 *[a-f0-9]+:	de 2f                	fisubrs \(%bx\)
 *[a-f0-9]+:	d9 07                	flds   \(%bx\)
 *[a-f0-9]+:	d8 0f                	fmuls  \(%bx\)
 *[a-f0-9]+:	d9 17                	fsts   \(%bx\)
 *[a-f0-9]+:	d9 1f                	fstps  \(%bx\)
 *[a-f0-9]+:	d8 27                	fsubs  \(%bx\)
 *[a-f0-9]+:	d8 2f                	fsubrs \(%bx\)
 *[a-f0-9]+:	ff 27                	jmp    \*\(%bx\)
 *[a-f0-9]+:	0f 01 17             	lgdtw  \(%bx\)
 *[a-f0-9]+:	0f 01 1f             	lidtw  \(%bx\)
 *[a-f0-9]+:	0f 00 17             	lldt   \(%bx\)
 *[a-f0-9]+:	0f 01 37             	lmsw   \(%bx\)
 *[a-f0-9]+:	0f 00 1f             	ltr    \(%bx\)
 *[a-f0-9]+:	8c 07                	mov    %es,\(%bx\)
 *[a-f0-9]+:	8e 07                	mov    \(%bx\),%es
 *[a-f0-9]+:	0f 1f 07             	nopw   \(%bx\)
 *[a-f0-9]+:	83 0f 01             	orw    \$0x1,\(%bx\)
 *[a-f0-9]+:	8f 07                	popw   \(%bx\)
 *[a-f0-9]+:	07                   	pop    %es
 *[a-f0-9]+:	f3 0f ae 27          	ptwritel \(%bx\)
 *[a-f0-9]+:	ff 37                	pushw  \(%bx\)
 *[a-f0-9]+:	06                   	push   %es
 *[a-f0-9]+:	83 1f 01             	sbbw   \$0x1,\(%bx\)
 *[a-f0-9]+:	83 2f 01             	subw   \$0x1,\(%bx\)
 *[a-f0-9]+:	c5 fb 2a 07          	vcvtsi2sdl \(%bx\),%xmm0,%xmm0
 *[a-f0-9]+:	62 f1 7f 08 2a 07    	vcvtsi2sdl \(%bx\),%xmm0,%xmm0
 *[a-f0-9]+:	c5 fa 2a 07          	vcvtsi2ssl \(%bx\),%xmm0,%xmm0
 *[a-f0-9]+:	62 f1 7e 08 2a 07    	vcvtsi2ssl \(%bx\),%xmm0,%xmm0
 *[a-f0-9]+:	62 f1 7f 08 7b 07    	vcvtusi2sdl \(%bx\),%xmm0,%xmm0
 *[a-f0-9]+:	62 f1 7e 08 7b 07    	vcvtusi2ssl \(%bx\),%xmm0,%xmm0
 *[a-f0-9]+:	83 37 01             	xorw   \$0x1,\(%bx\)
#pass
