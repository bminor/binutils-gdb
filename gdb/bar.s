	.stabs	"bar.c",0144,0,0,LL0
LL0:
	.data
	.stabs	"int:t(0,1)=r(0,1);-2147483648;2147483647;",0x80,0,0,0
	.stabs	"char:t(0,2)=r(0,2);0;127;",0x80,0,0,0
	.stabs	"long:t(0,3)=r(0,1);-2147483648;2147483647;",0x80,0,0,0
	.stabs	"short:t(0,4)=r(0,1);-32768;32767;",0x80,0,0,0
	.stabs	"unsigned char:t(0,5)=r(0,1);0;255;",0x80,0,0,0
	.stabs	"unsigned short:t(0,6)=r(0,1);0;65535;",0x80,0,0,0
	.stabs	"unsigned long:t(0,7)=r(0,1);0;-1;",0x80,0,0,0
	.stabs	"unsigned int:t(0,8)=r(0,1);0;-1;",0x80,0,0,0
	.stabs	"float:t(0,9)=r(0,1);4;0;",0x80,0,0,0
	.stabs	"double:t(0,10)=r(0,1);8;0;",0x80,0,0,0
	.stabs	"void:t(0,11)=(0,11)",0x80,0,0,0
	.stabs	"???:t(0,12)=(0,1)",0x80,0,0,0
	.stabs	"main:F(0,1)",0x24,0,4,_main
	.text
	.stabn	0104,0,1,LL1
LL1:
|#PROC# 04
	.globl	_main
_main:
|#PROLOGUE# 0
	link	a6,#0
	addl	#-LF12,sp
	moveml	#LS12,sp@
|#PROLOGUE# 1
	.stabn	0104,0,1,LL2
LL2:
	.stabs	"i:(0,1)",0x80,0,4,-4
	.stabn	0300,0,2,LL3
LL3:
	.stabn	0104,0,4,LL4
LL4:
	clrl	a6@(-0x4)
L16:
	tstl	a6@(-0x4)
	jlt	L15
	.stabn	0104,0,5,LL5
LL5:
	jbsr	_bar
L14:
	.stabn	0104,0,4,LL6
LL6:
	addql	#0x1,a6@(-0x4)
	jra	L16
L15:
	.stabn	0340,0,2,LL7
LL7:
	.stabn	0104,0,6,LL8
LL8:
LE12:
	unlk	a6
	rts
	LF12 = 4
	LS12 = 0x0
	LFF12 = 4
	LSS12 = 0x0
	LP12 =	0x8
	.data
	.stabs	"bar:F(0,1)",0x24,0,4,_bar
	.text
	.stabn	0104,0,9,LL9
LL9:
|#PROC# 04
	.globl	_bar
_bar:
|#PROLOGUE# 0
	link	a6,#0
	addl	#-LF18,sp
	moveml	#LS18,sp@
|#PROLOGUE# 1
	.stabn	0104,0,9,LL10
LL10:
	.stabs	"i:(0,1)",0x80,0,4,-4
	.stabn	0300,0,2,LL11
LL11:
	.stabn	0104,0,12,LL12
LL12:
	movl	#0xa,a6@(-0x4)
	.stabn	0340,0,2,LL13
LL13:
	.stabn	0104,0,13,LL14
LL14:
LE18:
	unlk	a6
	rts
	LF18 = 4
	LS18 = 0x0
	LFF18 = 4
	LSS18 = 0x0
	LP18 =	0x8
	.data
