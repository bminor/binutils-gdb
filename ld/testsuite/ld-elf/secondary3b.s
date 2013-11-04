	.section .text,"axG",%progbits,foo_group,comdat
	.secondary foo
	.global bar
foo:
	.byte 0
bar:
	.byte 0
	.data
	.dc.a foo

	.text
	.global start	/* Used by SH targets.  */
start:
	.global _start
_start:
	.global __start
__start:
	.global main	/* Used by HPPA targets.  */
main:
	.dc.a bar
