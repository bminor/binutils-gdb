	.text
	.global start	/* Used by SH targets.  */
start:
	.global _start
_start:
	.global __start
__start:
	.global main	/* Used by HPPA targets.  */
main:
	.globl	_main	/* Used by LynxOS targets.  */
_main:
	.dc.a 0
	.section	.bss, "aw", %nobits
	.p2align 16
	.type	bss, %object
	.size	bss, 0xb60000
bss:
	.zero	0xb60000
	.section	.note.GNU-stack,"",%progbits
