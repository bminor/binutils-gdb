	.globl	_start
	.text
_start:
	.=.+0x800
	.data
	.=.+0x100
	.lcomm _bss,0x100
