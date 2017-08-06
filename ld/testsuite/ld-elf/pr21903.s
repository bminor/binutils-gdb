	.text
	.global start	/* Used by SH targets.  */
start:
	.global _start
_start:
	.global __start
__start:
	.global main	/* Used by HPPA targets.  */
main:
	.dc.a foo
	.ifdef	HPUX
foo	.comm	4
	.else
	.comm	foo, 4, 4
	.endif
