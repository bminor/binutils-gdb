.text
.option pic
.option norvc
.p2align 3
.global _start
_start:
	nop

sym_local:
	nop

.global sym_global
sym_global:
	nop

.section .discard.got_local, "ax"
	la	x1, sym_local

.section .discard.got_global, "ax"
	la	x1, sym_global
