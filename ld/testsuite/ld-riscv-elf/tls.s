	.section	.tbss,"awT",@nobits
	.global sg1
sg1:
	.zero	4
sl1:
	.zero	4

	.text
	.globl	_start
	.type	_start,@function
_start:
	/* GD, global var */
	la.tls.gd	a0,sg1
	call  __tls_get_addr

	/* IE, global var */
	la.tls.ie	a0,sg1
	add	a0,a0,tp

	/* GD, local var */
	la.tls.gd	a0,sl1
	call  __tls_get_addr

	/* IE, local var */
	la.tls.ie	a0,sl1
	add	a0,a0,tp

	ret
