	# In order to trigger the problem this testcase was added for, we need
	# to have a TLS section in the current executable.  We already had a
	# check for a TLS relocation despite not having a TLS section, but did
	# not have a check for a TLS LE relocation against a symbol that is
	# external.
	.global b
	.section	.tbss,"awT",%nobits
b:
	.zero   65540
	.zero	52
	.size b,100

	.text
	.type a,STT_TLS
	.weak a
	.align	2
	.p2align 4,,11
	.global _start
	.type   _start,%function
_start:
	# Some TLS LE relocation against a symbol not defined in the current
	# executable.
	movz	x0, #:tprel_g0:w1
